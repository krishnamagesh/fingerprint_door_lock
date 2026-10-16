/***************************************************
  Fingerprint-Based Door Lock System
  Arduino UNO R4 (WiFi / Minima)

  HARDWARE
  --------
  R307 Fingerprint sensor  -> Hardware Serial1 (D0/D1)
    Red    (VCC) -> 5V
    Black  (GND) -> GND
    Yellow (TX)  -> D0 (RX1)
    other data wire (RX) -> D1 (TX1)

  16x2 I2C LCD              -> I2C bus
    VCC -> 5V
    GND -> GND
    SDA -> SDA (or A4)
    SCL -> SCL (or A5)

  5V single-channel relay   -> D7
    VCC     -> 5V
    GND     -> GND
    IN      -> D7
    COM/NO  -> wired to 12V solenoid lock + external 12V supply
               (solenoid is powered by its OWN 12V supply, not the Arduino)

  WORKFLOW
  --------
  1) Enroll fingerprints FIRST using a separate enrollment sketch.
     The sensor stores prints on its own onboard flash (IDs 1-127),
     so this only needs to be done once per finger (or whenever you
     want to add/remove a user).
  2) Upload THIS sketch for normal day-to-day operation. It watches
     for a finger, checks it against the enrolled library, and
     drives the relay + LCD accordingly.

  ASSUMPTIONS (change the constants below if your hardware differs):
    - LCD is a 16x2 I2C module at address 0x27 (try 0x3F if blank)
    - Relay module is ACTIVE LOW (energizes when IN pin is pulled LOW)
    - Solenoid lock is "power to unlock" (energized = unlocked)

  Adapted from Adafruit's Fingerprint library examples.
  Original written by Limor Fried/Ladyada for Adafruit Industries.
  BSD license, all text above must be included in any redistribution.
 ****************************************************/

#include <Adafruit_Fingerprint.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ---------------- Fingerprint sensor (Serial1: D0/D1) ----------------
#define mySerial Serial1
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

// ---------------- LCD (16x2, I2C) ----------------
// If the screen stays blank, try address 0x3F instead of 0x27.
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ---------------- Relay / solenoid lock ----------------
#define RELAY_PIN 7
#define RELAY_ACTIVE_LOW true   // flip to false if your relay logic is reversed
#define UNLOCK_TIME_MS 5000     // how long the door stays unlocked (ms)

void setRelay(bool energize) {
  bool pinState = RELAY_ACTIVE_LOW ? !energize : energize;
  digitalWrite(RELAY_PIN, pinState ? HIGH : LOW);
}

void lockDoor()   { setRelay(false); }
void unlockDoor() { setRelay(true);  }

void showWelcome() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Welcome");
  lcd.setCursor(0, 1);
  lcd.print("Place Finger");
}

void setup() {
  Serial.begin(9600);
  // NOTE: unlike the bench/enrollment sketch, we do NOT do
  // "while (!Serial);" here. On the UNO R4, Serial is true USB-CDC,
  // so that loop would hang forever once this is installed as a
  // standalone lock with no PC attached. A short delay is enough
  // to catch the Serial Monitor if you happen to be watching.
  delay(1500);
  Serial.println(F("\n\nFingerprint Door Lock - starting up"));

  pinMode(RELAY_PIN, OUTPUT);
  lockDoor(); // make sure the door starts locked

  lcd.init();
  lcd.backlight();

  finger.begin(57600);
  if (finger.verifyPassword()) {
    Serial.println(F("Found fingerprint sensor!"));
  } else {
    Serial.println(F("Did not find fingerprint sensor :("));
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Sensor Error");
    lcd.setCursor(0, 1);
    lcd.print("Check wiring");
    while (1) {
      delay(1);
    }
  }

  finger.getParameters();
  Serial.print(F("Sensor capacity: "));
  Serial.println(finger.capacity);

  showWelcome();
}

void loop() {
  uint8_t p = finger.getImage();

  if (p == FINGERPRINT_NOFINGER) {
    delay(50); // idle, nothing on the sensor
    return;
  }

  if (p != FINGERPRINT_OK) {
    // imaging/communication hiccup - just try again next loop
    delay(50);
    return;
  }

  bool matched = false;
  uint8_t matchedID = 0;

  p = finger.image2Tz(1);
  if (p == FINGERPRINT_OK) {
    p = finger.fingerFastSearch();
    if (p == FINGERPRINT_OK) {
      matched = true;
      matchedID = finger.fingerID;
    }
  }

  if (matched) {
    Serial.print(F("Access granted, ID #"));
    Serial.println(matchedID);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Access Granted");
    lcd.setCursor(0, 1);
    lcd.print("ID #");
    lcd.print(matchedID);

    unlockDoor();
    delay(UNLOCK_TIME_MS);
    lockDoor();
  } else {
    Serial.println(F("Access denied"));

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Access Denied");
    lcd.setCursor(0, 1);
    lcd.print("Try Again");

    delay(2000);
  }

  showWelcome();
}

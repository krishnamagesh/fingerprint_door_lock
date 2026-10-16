Small project i made in the summer
# Fingerprint Door Lock (Arduino Uno R4)

A fingerprint-based door lock built with an Arduino Uno R4, an R307 optical fingerprint sensor, a 5V relay, and a 12V solenoid lock. A 16x2 I2C LCD shows the system status: **Welcome** by default, then **Access Granted** or **Access Denied** after each scan.

<!-- Add a photo or GIF of your build here -->
<img width="1157" height="466" alt="image" src="https://github.com/user-attachments/assets/7fd30eac-3839-4148-8b4f-be62aed3628e" />
<img width="930" height="527" alt="lockpic1" src="https://github.com/user-attachments/assets/725c92a5-2ebe-48a3-8e41-71b0d92c0bbf" />
<img width="931" height="522" alt="lockpic2" src="https://github.com/user-attachments/assets/960b86dd-0588-4e2f-94be-942dd29b7dcf" />
<img width="927" height="525" alt="lockpic3" src="https://github.com/user-attachments/assets/6fcd4abc-b570-41c2-8867-51100a2831f3" />



<!-- ![Project photo](images/project.jpg) -->

## Features

- Fingerprint authentication using the R307 sensor (up to 127 stored prints, kept in the sensor's own flash memory)
- 12V solenoid lock switched through a 5V relay on pin D7
- LCD feedback: "Welcome" idle screen, "Access Granted" (with the matched ID) or "Access Denied"
- Lock re-engages automatically after a configurable unlock time (default 5 seconds)
- Runs standalone with no PC attached

## Parts List

| Part | Notes |
|---|---|
| Arduino Uno R4 (WiFi or Minima) | Uses hardware `Serial1` on D0/D1 |
| R307 fingerprint sensor | 5V, 57600 baud |
| 5V single-channel relay module | Assumed active-LOW |
| 12V solenoid door lock | "Power to unlock" type |
| 12V power supply | Rated for your solenoid's current draw; powers the lock only |
| 16x2 LCD with I2C backpack | Address `0x27` (or `0x3F`) |
| Jumper wires, breadboard | |

## Wiring

> R307 wire colors vary between manufacturers. Go by **function** (TX/RX), not color. In this build, the **yellow** wire is the sensor's TX.

### Fingerprint sensor (R307)

| Sensor Wire | Arduino Pin |
|---|---|
| Red (VCC) | 5V |
| Black (GND) | GND |
| Yellow (sensor TX) | D0 (RX1) |
| Other data wire (sensor RX) | D1 (TX1) |

TX and RX are crossed: the sensor's TX goes to the Arduino's RX, and vice versa. If you get "Did not find fingerprint sensor," try swapping the two data wires.

### Relay and solenoid lock

| Connection | Goes To |
|---|---|
| Relay VCC | Arduino 5V |
| Relay GND | Arduino GND |
| Relay IN | Arduino D7 |
| Relay COM | One terminal of the solenoid |
| Relay NO | External 12V supply (+) |
| Solenoid other terminal | External 12V supply (GND) |

**Do not power the solenoid from the Arduino.** It runs from its own 12V supply, and the relay only switches that circuit.

### I2C LCD

| LCD Pin | Arduino Pin |
|---|---|
| VCC | 5V |
| GND | GND |
| SDA | SDA (or A4) |
| SCL | SCL (or A5) |

## Software Setup

1. Install the [Arduino IDE](https://www.arduino.cc/en/software).
2. Install the **Arduino UNO R4 Boards** package (Tools → Board → Boards Manager).
3. Install these libraries from Tools → Manage Libraries:
   - **Adafruit Fingerprint Sensor Library**
   - **LiquidCrystal I2C** (by Frank de Brabander)
4. Clone or download this repository.

## Usage

### Step 1: Enroll fingerprints

Open `fingerprint_enroll/fingerprint_enroll.ino`, upload it, and open the Serial Monitor at 9600 baud. Type an ID number (1 to 127), then follow the prompts to scan the same finger twice. Repeat for each person.

### Step 2: Run the door lock

Open `fingerprint_door_lock/fingerprint_door_lock.ino` and upload it. The LCD shows **Welcome**. Place an enrolled finger on the sensor and the door unlocks for 5 seconds. Any other finger shows **Access Denied**.

## Configuration

These constants are near the top of `fingerprint_door_lock.ino`:

| Constant | Default | Purpose |
|---|---|---|
| `LiquidCrystal_I2C lcd(0x27, 16, 2)` | `0x27` | LCD I2C address. Try `0x3F` if the screen stays blank |
| `RELAY_PIN` | `7` | Relay control pin |
| `RELAY_ACTIVE_LOW` | `true` | Set to `false` if your relay triggers on HIGH |
| `UNLOCK_TIME_MS` | `5000` | How long the lock stays open, in milliseconds |

## Troubleshooting

| Problem | Likely Fix |
|---|---|
| "Did not find fingerprint sensor" | Check 5V/GND, swap the TX/RX data wires, confirm D0/D1 are used |
| Upload fails with the sensor attached | Briefly disconnect the sensor wires on D0/D1 while uploading |
| LCD is blank | Try address `0x3F`, adjust the contrast trim pot on the I2C backpack |
| Relay clicks at power-up instead of on a match | Set `RELAY_ACTIVE_LOW` to `false` |
| Lock behaves backwards | Your solenoid may be "fail-secure"; swap the logic in `lockDoor()` / `unlockDoor()` |

## Safety Notes

- Keep mains-level and 12V wiring away from the low-voltage logic side.
- Make sure the 12V supply can deliver your solenoid's current without sagging.
- A solenoid should not stay energized for long periods, as it can overheat. Keep `UNLOCK_TIME_MS` short.
- This is a hobby project. For a real entry door, keep a mechanical key or manual override in case of power loss or failure.

## Project Structure

```
.
├── fingerprint_door_lock/
│   └── fingerprint_door_lock.ino
├── fingerprint_enroll/
│   └── fingerprint_enroll.ino
└── README.md
```

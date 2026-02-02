# Real-time-clock-DS-1307-

This project leverages the ESP32 on the ZenoRTU0106 industrial board to synchronize real-time clock data from the 4G network (SIM7600) and persist it to the onboard DS1307 RTC module.

The primary goal is to ensure the device always maintains a precise Timestamp (in milliseconds) for offline data logging and telemetry uploads to ThingsBoard, ensuring data integrity even during power outages or network failures.

## Hardware Requirements

* ESP32 Board, Module Sim 7680C
* CR1220 Coin Battery for the DS1307 RTC (located on the back of the PCB if you use ZenoRTU0106).
* 4G SIM Card (active data plan required).
* 4G Antenna (must be connected to the MAIN port).
* External Power Supply 12V or 24V (Mandatory, do not use USB power alone).

## Required Libraries

Install the following libraries via the Arduino Library Manager:

1.  **TinyGSM** (by Volodymyr Shymanskyy) - For SIM7600 communication.
2.  **RTClib** (by Adafruit) - For DS1307 RTC communication.

## Pin Configuration

Default pinout for ZenoRTU0106 used in the code:

* MODEM_TX: 17
* MODEM_RX: 16
* I2C_SDA: 22
* I2C_SCL: 21

## How It Works

1.  **RTC Initialization:** On startup, the system checks the connection to the DS1307 RTC.
2.  **Activation:** If the RTC is halted (due to a dead battery or first-time use), the system forces it to start.
3.  **Network Connection:** The system initializes the SIM7600 module and waits for a 4G signal.
4.  **Time Sync:** Once connected, the device fetches the Network Time via AT commands (`AT+CLTS=1`).
5.  **Persist Time:** The fetched time is written to the RTC to ensure accuracy.
6.  **Operation:** In the main loop, the ESP32 reads the time from the RTC and converts it to a Unix Timestamp (ms) for data tagging (remember to watch for your timezone UTC and adjust the timestamp as said in the code comment).

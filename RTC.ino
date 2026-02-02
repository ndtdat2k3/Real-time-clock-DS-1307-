#define TINY_GSM_DEBUG Serial
#define TINY_GSM_MODEM_SIM7600
#include <TinyGsmClient.h>
#include <Wire.h>
#include <RTClib.h>

#define MODEM_TX 26
#define MODEM_RX 27
#define MODEM_RESET_PIN 25
#define I2C_SDA  22
#define I2C_SCL  21

HardwareSerial SerialAT(2);
TinyGsm modem(SerialAT);
RTC_DS1307 rtc;

void modemHardwareReset() {
  pinMode(MODEM_RESET_PIN, OUTPUT);
  digitalWrite(MODEM_RESET_PIN, HIGH);
  delay(50);
  digitalWrite(MODEM_RESET_PIN, LOW);
  delay(1000);
  digitalWrite(MODEM_RESET_PIN, HIGH);
  delay(10000); 
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("\n--- SYSTEM START ---");

  Wire.begin(I2C_SDA, I2C_SCL);
  if (!rtc.begin()) {
    Serial.println("Error: Not found RTC!");
  } else {
    Serial.println("RTC OK.");
  }

  if (!rtc.isrunning()) {
    Serial.println("Activate RTC...");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  Serial.println("Initiating SIM7600...");
  SerialAT.begin(115200, SERIAL_8N1, MODEM_RX, MODEM_TX);

  modemHardwareReset();
  delay(3000);
  
  modem.restart();

  modem.sendAT("+CLTS=1");
  modem.waitResponse();
  modem.sendAT("&W");
  modem.waitResponse();
  Serial.println("Waiting for 4G ...");
  
  if (!modem.waitForNetwork(60000)) {
    Serial.println("Timeout: No internet found!. Use RTC time.");
  } else {
    Serial.println("Internet found! Sync now...");
    syncTimeFrom4G();
  }
}

void syncTimeFrom4G() {
  int year, month, day, hour, min, sec;
  float timezone;

  bool gotTime = modem.getNetworkTime(&year, &month, &day, &hour, &min, &sec, &timezone);

  if (gotTime) {
    Serial.print("Time 4G: ");
    Serial.print(year); Serial.print("-");
    Serial.print(month); Serial.print("-");
    Serial.println(day);

    if (year >= 2025) {
      rtc.adjust(DateTime(year, month, day, hour, min, sec));
      Serial.println("-> Updated RTC!");
    }
  }
}

void loop() {
  DateTime now = rtc.now();

  Serial.print(now.year()); Serial.print('/');
  Serial.print(now.month()); Serial.print('/');
  Serial.print(now.day()); Serial.print(" ");
  Serial.print(now.hour()); Serial.print(':');
  Serial.print(now.minute()); Serial.print(':');
  Serial.print(now.second());

  unsigned long long timestamp_ms = (unsigned long long)now.unixtime() * 1000;
  
  Serial.print(" | Timestamp: ");
  Serial.println(timestamp_ms - 25200000); // In my case, my time zone is UTC +7 which mean faster by 7 hour = 25200 second so i minus it and get the correct timestamp
  delay(1000);
}
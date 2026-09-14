#include <Wire.h>
#include <WiFi.h>
#include <Adafruit_MCP23017.h>
#include <TFT_eSPI.h>
#include <time.h>

const char *WIFI_SSID = "AIRTEL ROYAL";
const char *WIFI_PASSWORD = "why-should-i-tell-you";
const float TIMEZONE_OFFSET_HOURS = 5.5;

#define I2C_SDA 4
#define I2C_SCL 5
#define MCP_ADDR 0x20
#define MCP_BUZZER_PIN 12 

Adafruit_MCP23017 mcp;

int hour= 0;
int minute= 14;
int second= 3;
int alarmHour=1;
int alarmMinute =43; 

bool alarmEnabled= false;
bool alarmRinging =false;
bool settingAlarm= false;
bool settingAlarmHour = true;
bool settingTime = false;
bool settingHour = true;


unsigned long lastSecond = 0;

const int ROWS[3] = {0, 1, 2};
const int COLUMNS[4] = {8, 9, 10, 11};

const char keyMap[3][4] = {
  {
    '1','2','3','U'
  },
  {
    '4','5','6','D'
  },
  {
    '7','8','9','K'
  }
};


char readkey() {
  for (int row=0; row<3; row++) {
    mcp.digitalWrite(ROWS[row], LOW);
    for (int col=0; col<4; col++) {
      if (mcp.digitalRead(COLUMNS[col]) == LOW) {
        mcp.digitalWrite(ROWS[row], HIGH);
        return keyMap[row][col];
      }
    }
    mcp.digitalWrite(ROWS[row], HIGH);
  }
  
  return '\0'
}


void printTime() {
  Serial.print("Time: ");

  if (hour <10 ) {
    Serial.print("0")
  }

  Serial.print(hour);
  Serial.print(":");

  if (minute < 10) {
    Serial.print("0");
  }

  Serial.print(minute);
  Serial.print(":")

  if (second < 10) {
    Serial.print("0");
  }

  Serial.println(second);
}

void printAlarm() {
  Serial.print("Alarm: ");

  if (alarmHour < 10) {
    Serial.print("0");
  }

  Serial.print(alarmHour);
  Serial.print("0");

  if (alarmMinute < 10) {
    Serial.print("0");
  }

  Serial.println(alarmMinute);
}

void setup() {

  Serial.begin(9600);
  Wire.begin(I2C_SDA, I2C_SCL);
  mcp.begin(MCP_ADDR);

  for (int i=0; i<3; i++) {
    mcp.pinmode(ROWS[i], OUTPUT);
    mcp.digitalWrite(ROWS[i], HIGH);
  }


  for (int i=0; i<4; i++) {
    mcp.pinmode(COLUMNS[i], INPUT_PULLUP);
  }

  mcp.pinMode(MCP_BUZZER_PIN, OUTPUT);
  mcp.digitalWrite(MCP_BUZZER_PIN,LOW);

  lastSecond = millis();

  Serial.println();
  Serial.println("CHAOX-TIME");
  Serial.println("Clock Started");
  Serial.println("Starting time: ");

  printTime();
  printAlarm();

}

void loop() {

  if (millis() - lastSecond >= 1000) {
    lastSecond  = millis();
    second++;

    if (second >= 60) {
      second = 0;
      minute++;
    }

    if (minute >= 60) {
      minute = 0;
      hour++;
    }

    if (hour >= 24) {
      hour = 0;
    }

    printTime();
  }

  if (alarmEnabled &&
      hour == alarmHour &&
      minute == alarmMinute &&
      second == 0) {
      
      alarmRinging = true;
      Serial.println("ALARM....");
      }

  char key = readkey();

  if (alarmRinging) {
    mcp.digitalWrite(MCP_BUZZER_PIN, HIGH);
    delay(200);
    mcp.digitalWrite(MCP_BUZZER_PIN, LOW);
    delay(200);

    if (key != '\0') {
      alarmRinging =false;
      Serial.println("Alarm stopped.");
    }
    return;
  }

  if (key == '\0') {
    return;
  }

  if (settingTime){

    if (key == 'U') {
      if (settingTimeHour) {
        hour++;

        if (hour >= 24) {
          hour = 0;
        }

        Serial.print("Hour: ");
        Serial.println(hour);
      }

      else {
        minute++;

        if (minute >= 60) {
          minute = 0;
        }

        Serial.print("Minute: ");
        Serial.println(minute);
      }
    }

    else if (key == 'D') {

      if (settingTimeHour) {
        hour--;

        if (hour < 0) {
          hour = 23;
        }

        Serial.print("Hour: ");
        Serial.println(hour);
      }

      else {
        minute--;

        if(minute <0) {
          minute =59;
        }

        Serial.print("Minute:")
        Serial.println(minute);
      }
    }

    else if (key == 'K') {
      
      if (settingTimeHour) {
        settingTimeHour = false;
        Serial.println("Setting minutes.....");
      }

      else {
        settingTime = false;
        second = 0;
        Serial.println("Time saved.....");
        printTime();
      }
    }

    delay(400);

    return;
  }

  if (settingAlarm) {

    if (key == 'U') {

      if (settingAlarmHour) {
        alarmHour++;

        if (alarmHour >= 24) {
          alarmhour = 0;
        }

        else {

          alarmMinute++;
          
          if (alarmMinute >= 60) {
            alarmMinute = 0;
          }

          Serial.print("Alarm minute: ");
          Serial.println(alarmMinute);
        }
      }

      else if (key == 'D') {
        if (settingAlarmHour) {
          alarmHour--;

          if (alarmHour <0) {
            alarmHour = 23;
          }

          Serial.print("Alarm hour: ");
          Serial.println(alarmHour);
        }

        else {

          alarmMinute--;
          if (alarmMinute < 0) {
            alarmMinute = 59;
          }

          Serial.print("Alarm minute: ");
          Serial.println(alarmMinute);
        }
      }

      else if (key == 'K') {
        if (settingAlarmHour) {
          settingAlarmHour = false;

          Serial.println("Setting alarm minutes.....");
        }

        else {
          settingAlarm = false;
          alarmEnabled = true;

          Serial.println("Alarm saved.....");
          printAlarm();
        }
      }

      delay(300);

      return;
    }

    if (key == 'K') {
      settingTime = true;
      settingTimeHour = true;
      
      Serial.println("Setting clock hour.....");

      delay(300);

      return;
    }

    if (key == '1') {
      settingAlarm = true;
      settingAlarmHour = true;

      Serial.println("setting alarm hour.....");

      delay(300);

      return;
    }

    Serial.print("Key presed: ");
    Serial.println(key);

    delay(300);
  }
}

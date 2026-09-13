#include <Wire.h>
#include <WiFi.h>
#include <Adafruit_MCP23017.h>
#include <TFT_eSPI.h>
#include <time.h>

const char *WIFI_SSID = "YOUR_WIFI";
const char *WIFI_PASSWORD = "YOUR_PASSWORD";
const float TIMEZONE_OFFSET_HOURS = 5.5;

#define I2C_SDA 4
#define I2C_SCL 5
#define MCP_ADDR 0x20
#define MCP_BUZZER_PIN 12 

Adafruit_MCP23017 mcp;

int hour= 0;
int minute= 14;
int second= 3;

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

bool settingTime = false;
bool settingHour = true;


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

  lastSecond = millis();

  Serial.println();
  Serial.println("CHAOX-TIME");
  Serial.println("Clock Started");
  Serial.println("Starting time: ");

  if (hour < 10) {
    Serial.print("0");
  }
  Serial.print(hour);
  Serial.print(":");

  if (minute < 10) {
    Serial.print("0");
  }
  Serial.print(minute);
  Serial.print(":");

  if (second < 10) {
    serial.print("0");
  }
  Serial.println(seconds);
}

void loop() {

  if (millis() - lastSecond >= 1000) {
    lastSecond = millis();
    second++;

    if(second >= 60) {
      minute = 0;
      hour++;
    }

    if (minute >= 60) {
      minut = 0;
      hour++;
    }

    if (hour >= 24) {
      hour = 0
    }

    Serial.print("TIME: ");

    if (hour < 10) {
      Serial.print("0");
    }

    Serial.print(hour);
    Serial.print(":");

    if (minute < 10) {
      Serial.print("0");
    }

    Serial.print(minute);
    Serial.print(":");

    if (second < 10) {
      Serial.print("0");
    }
    
    Serial.println(second);
  }

  char key = readkey();

  if (key != '\0') {

    if (key == 'K') {
      
      if (!settingTime) {
        settingTime = true;
        settingTime = true;
        Serial.println("Setting hour....");
      }

      else if (settingHour) {

        settingHour = false;
        Serial.println("Setting minute....");
      }

      else {
        settingTime = false;
        second = 0;
        Serial.println("Time Xaved..");
      }
    }

    if (key == 'U' && settingTime) {
      
      if (settingHour) {
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

    if (key == 'D' && settingTime) {

      if (settingHour) {

        hour--;
        if (hour < 0) {
          hour = 23;
        }

        Serial.print("Hour: ");
        Serial.println(hour);
      }

      else {
        minute--;
        
        if (minute < 0) {
          minute =59;
        }

        Serial.print("Minute: ");
        Serial.println(minute);
      }
    }

    delay(200);
  }
}

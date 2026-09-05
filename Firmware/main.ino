// ============================================================
//  SIMPLE ALARM CLOCK
//  XIAO ESP32-C3 + MCP23017 keypad + SPI TFT + piezo buzzer
//
//  What it does:
//   - Connects to WiFi, gets the correct time (NTP)
//   - Shows the time on the screen
//   - Lets you set ONE alarm (hour + minute) using the keypad
//   - When the clock reaches that time, it beeps until you press a key
//
//  That's it. No menus, no prayer times, no saving to flash.
//  Once this makes sense, we can add features back in one at a time.
// ============================================================

#include <Wire.h>
#include <WiFi.h>
#include <Adafruit_MCP23017.h>
#include <TFT_eSPI.h>   // configure pins in the library's User_Setup.h -- see README
#include <time.h>

// ---------- WiFi ----------
const char *WIFI_SSID = "YOUR_WIFI";
const char *WIFI_PASSWORD = "YOUR_PASSWORD";
const float TIMEZONE_OFFSET_HOURS = 5.5; // change to your timezone

// ---------- Pins (from your schematic) ----------
#define I2C_SDA 6
#define I2C_SCL 7
#define MCP_ADDR 0x20
#define MCP_BUZZER_PIN 12 // GPB4

// Keypad: 3 rows (GPA0-2), 4 columns (GPB0-3)
const uint8_t ROW_PINS[3] = {0, 1, 2};
const uint8_t COL_PINS[4] = {8, 9, 10, 11};
const char KEYMAP[3][4] = {
  {'1', '2', '3', 'U'},  // U = up
  {'4', '5', '6', 'D'},  // D = down
  {'7', '8', '9', 'K'}   // K = OK / select
};

// ---------- Objects ----------
Adafruit_MCP23017 mcp;
TFT_eSPI tft = TFT_eSPI();

// ---------- Alarm state ----------
int alarmHour = 7;
int alarmMinute = 0;
bool alarmEnabled = false;
bool alarmRinging = false;
bool alarmFiredThisMinute = false;

// ---------- Menu state ----------
// mode 0 = just showing the clock
// mode 1 = setting the alarm hour
// mode 2 = setting the alarm minute
int mode = 0;


void connectWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 15000) {
    delay(250);
  }
}

void syncTime() {
  configTime((long)(TIMEZONE_OFFSET_HOURS * 3600), 0, "pool.ntp.org");
  struct tm t;
  int tries = 0;
  while (!getLocalTime(&t) && tries < 20) {
    delay(500);
    tries++;
  }
}

// Reads the keypad matrix once. Returns the key char if a NEW press
// happened this call, or 0 if nothing new was pressed.
char readKeypad() {
  static bool wasDown[3][4] = {false};
  char found = 0;

  for (int r = 0; r < 3; r++) {
    // Drive only this row LOW, the other rows HIGH
    for (int rr = 0; rr < 3; rr++) {
      mcp.digitalWrite(ROW_PINS[rr], rr == r ? LOW : HIGH);
    }
    delayMicroseconds(50);

    for (int c = 0; c < 4; c++) {
      bool isDown = (mcp.digitalRead(COL_PINS[c]) == LOW);
      if (isDown && !wasDown[r][c]) {
        found = KEYMAP[r][c];
      }
      wasDown[r][c] = isDown;
    }
  }

  // release all rows
  for (int r = 0; r < 3; r++) {
    mcp.digitalWrite(ROW_PINS[r], HIGH);
  }

  return found;
}

void beep(int freqHz, int durationMs) {
  long periodUs = 1000000L / freqHz;
  long cycles = (long)durationMs * 1000L / periodUs;
  for (long i = 0; i < cycles; i++) {
    mcp.digitalWrite(MCP_BUZZER_PIN, HIGH);
    delayMicroseconds(periodUs / 2);
    mcp.digitalWrite(MCP_BUZZER_PIN, LOW);
    delayMicroseconds(periodUs / 2);
  }
}

void drawClockScreen(struct tm &now) {
  tft.fillScreen(TFT_BLACK);

  tft.setTextSize(6);
  tft.setTextColor(TFT_CYAN, TFT_BLACK);
  tft.setCursor(40, 40);
  tft.printf("%02d:%02d", now.tm_hour, now.tm_min);

  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setCursor(40, 110);
  tft.printf("Alarm: %02d:%02d  %s", alarmHour, alarmMinute,
             alarmEnabled ? "ON" : "OFF");

  tft.setTextColor(TFT_DARKGREY, TFT_BLACK);
  tft.setCursor(20, 160);
  tft.print("K: enter alarm setup");
}

void drawSetHourScreen() {
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setCursor(20, 20);
  tft.print("Set alarm HOUR");

  tft.setTextSize(6);
  tft.setTextColor(TFT_CYAN, TFT_BLACK);
  tft.setCursor(80, 70);
  tft.printf("%02d", alarmHour);

  tft.setTextSize(2);
  tft.setTextColor(TFT_DARKGREY, TFT_BLACK);
  tft.setCursor(10, 200);
  tft.print("U/D: change   K: next (minute)");
}

void drawSetMinuteScreen() {
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setCursor(20, 20);
  tft.print("Set alarm MINUTE");

  tft.setTextSize(6);
  tft.setTextColor(TFT_CYAN, TFT_BLACK);
  tft.setCursor(80, 70);
  tft.printf("%02d", alarmMinute);

  tft.setTextSize(2);
  tft.setTextColor(TFT_DARKGREY, TFT_BLACK);
  tft.setCursor(10, 200);
  tft.print("U/D: change   K: save & turn ON");
}

void drawRingingScreen() {
  tft.fillScreen(TFT_RED);
  tft.setTextSize(4);
  tft.setTextColor(TFT_WHITE, TFT_RED);
  tft.setCursor(40, 100);
  tft.print("ALARM!");
  tft.setTextSize(2);
  tft.setCursor(30, 160);
  tft.print("Press any key to stop");
}

void setup() {
  Serial.begin(115200);

  Wire.begin(I2C_SDA, I2C_SCL);
  mcp.begin(MCP_ADDR, &Wire);

  // rows: outputs, idle HIGH
  for (int i = 0; i < 3; i++) {
    mcp.pinMode(ROW_PINS[i], OUTPUT);
    mcp.digitalWrite(ROW_PINS[i], HIGH);
  }
  // columns: inputs with pull-up
  for (int i = 0; i < 4; i++) {
    mcp.pinMode(COL_PINS[i], INPUT);
    mcp.pullUp(COL_PINS[i], HIGH);
  }
  mcp.pinMode(MCP_BUZZER_PIN, OUTPUT);
  mcp.digitalWrite(MCP_BUZZER_PIN, LOW);

  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setCursor(10, 10);
  tft.println("Connecting WiFi...");

  connectWiFi();
  syncTime();
}

void loop() {
  char key = readKeypad();

  if (key) {
    beep(2000, 15); // little click feedback for every key press

    if (alarmRinging) {
      // any key silences the alarm
      alarmRinging = false;
      mode = 0;
    } else if (mode == 0 && key == 'K') {
      mode = 1; // start setting the hour
    } else if (mode == 1) {
      if (key == 'U') alarmHour = (alarmHour + 1) % 24;
      else if (key == 'D') alarmHour = (alarmHour + 23) % 24;
      else if (key == 'K') mode = 2; // move on to minutes
    } else if (mode == 2) {
      if (key == 'U') alarmMinute = (alarmMinute + 1) % 60;
      else if (key == 'D') alarmMinute = (alarmMinute + 59) % 60;
      else if (key == 'K') {
        alarmEnabled = true;
        mode = 0; // done, back to clock screen
      }
    }
  }

  struct tm now;
  if (!getLocalTime(&now)) {
    delay(50);
    return;
  }

  // check if it's time to ring
  if (alarmEnabled && !alarmRinging) {
    if (now.tm_hour == alarmHour && now.tm_min == alarmMinute) {
      if (!alarmFiredThisMinute) {
        alarmRinging = true;
        alarmFiredThisMinute = true;
      }
    } else {
      alarmFiredThisMinute = false; // reset once the minute has passed
    }
  }

  // if ringing, keep beeping until a key stops it
  if (alarmRinging) {
    beep(2500, 200);
    delay(150);
  }

  // redraw the screen twice a second
  static unsigned long lastDraw = 0;
  if (millis() - lastDraw > 500) {
    lastDraw = millis();
    if (alarmRinging) drawRingingScreen();
    else if (mode == 0) drawClockScreen(now);
    else if (mode == 1) drawSetHourScreen();
    else if (mode == 2) drawSetMinuteScreen();
  }
}

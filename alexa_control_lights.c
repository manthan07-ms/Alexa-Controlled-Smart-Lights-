#include <ESP8266WiFi.h>
#include <SinricPro.h>
#include <SinricProSwitch.h>

// ========== WiFi Setup ==========
const char* ssidList[] = {"iQOO", "Manthan"};
const char* passList[] = {"Manthan2006", "Manthan2006"};
const int wifiCount = sizeof(ssidList) / sizeof(ssidList[0]);

// ========== SinricPro Credentials ==========
#define APP_KEY     "30fb39c2-f632-4b92-9163-4df328b4c7a4"
#define APP_SECRET  "aca3822e-4b2f-4d0a-8de5-0b8633eaab9c-27d72c20-bd25-400f-bd32-eca3faf50706"
#define SWITCH_ID_1 "685f715b929fca430265a075"
#define SWITCH_ID_2 "68627aedf64d827f96a0b573"
#define SWITCH_ID_3 "68627b36030990a558c6d390"

// ========== Relay Pins ==========
#define RELAY_PIN_1 D1
#define RELAY_PIN_2 D2
#define RELAY_PIN_3 D3

// ========== Manual Switch Pins ==========
#define SWITCH_PIN_1 D5
#define SWITCH_PIN_2 D6
#define SWITCH_PIN_3 D7

bool relayState1 = false;
bool relayState2 = false;
bool relayState3 = false;

bool lastSwitch1 = HIGH;
bool lastSwitch2 = HIGH;
bool lastSwitch3 = HIGH;

void connectWiFi() {
  WiFi.mode(WIFI_STA);
  for (int i = 0; i < wifiCount; i++) {
    WiFi.begin(ssidList[i], passList[i]);
    Serial.printf("[WiFi] Connecting to %s\n", ssidList[i]);
    int tries = 0;
    while (WiFi.status() != WL_CONNECTED && tries < 20) {
      delay(500);
      Serial.print(".");
      tries++;
    }

    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("\n[WiFi] Connected!");
      Serial.print("[WiFi] IP Address: ");
      Serial.println(WiFi.localIP());
      return;
    }
    Serial.println("\n[WiFi] Failed. Trying next...");
  }
  Serial.println("[WiFi] Could not connect to any network.");
}

bool onPowerState1(const String &deviceId, bool &state) {
  relayState1 = state;
  digitalWrite(RELAY_PIN_1, state ? HIGH : LOW);
  Serial.printf("[Alexa] Device 1 turned %s\n", state ? "ON" : "OFF");
  return true;
}

bool onPowerState2(const String &deviceId, bool &state) {
  relayState2 = state;
  digitalWrite(RELAY_PIN_2, state ? HIGH : LOW);
  Serial.printf("[Alexa] Device 2 turned %s\n", state ? "ON" : "OFF");
  return true;
}

bool onPowerState3(const String &deviceId, bool &state) {
  relayState3 = state;
  digitalWrite(RELAY_PIN_3, state ? HIGH : LOW);
  Serial.printf("[Alexa] Device 3 turned %s\n", state ? "ON" : "OFF");
  return true;
}

void setupSinricPro() {
  SinricProSwitch &sw1 = SinricPro[SWITCH_ID_1];
  SinricProSwitch &sw2 = SinricPro[SWITCH_ID_2];
  SinricProSwitch &sw3 = SinricPro[SWITCH_ID_3];

  sw1.onPowerState(onPowerState1);
  sw2.onPowerState(onPowerState2);
  sw3.onPowerState(onPowerState3);

  SinricPro.onConnected([]() { Serial.println("[SinricPro] Connected!"); });
  SinricPro.onDisconnected([]() { Serial.println("[SinricPro] Disconnected!"); });

  SinricPro.begin(APP_KEY, APP_SECRET);
}

void setup() {
  Serial.begin(115200);

  // Relay outputs
  pinMode(RELAY_PIN_1, OUTPUT);
  pinMode(RELAY_PIN_2, OUTPUT);
  pinMode(RELAY_PIN_3, OUTPUT);

  // Manual switch inputs
  pinMode(SWITCH_PIN_1, INPUT_PULLUP);
  pinMode(SWITCH_PIN_2, INPUT_PULLUP);
  pinMode(SWITCH_PIN_3, INPUT_PULLUP);

  // Start with all relays OFF
  digitalWrite(RELAY_PIN_1, LOW);
  digitalWrite(RELAY_PIN_2, LOW);
  digitalWrite(RELAY_PIN_3, LOW);

  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);

  connectWiFi();
  setupSinricPro();
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
    return;
  }

  SinricPro.handle();

  // ---- Switch 1 ----
  bool currentSwitch1 = digitalRead(SWITCH_PIN_1);
  if (currentSwitch1 == LOW && lastSwitch1 == HIGH) {
    relayState1 = !relayState1;
    digitalWrite(RELAY_PIN_1, relayState1 ? HIGH : LOW);
    delay(200); // debounce
  }
  lastSwitch1 = currentSwitch1;

  // ---- Switch 2 ----
  bool currentSwitch2 = digitalRead(SWITCH_PIN_2);
  if (currentSwitch2 == LOW && lastSwitch2 == HIGH) {
    relayState2 = !relayState2;
    digitalWrite(RELAY_PIN_2, relayState2 ? HIGH : LOW);
    delay(200);
  }
  lastSwitch2 = currentSwitch2;

  // ---- Switch 3 ----
  bool currentSwitch3 = digitalRead(SWITCH_PIN_3);
  if (currentSwitch3 == LOW && lastSwitch3 == HIGH) {
    relayState3 = !relayState3;
    digitalWrite(RELAY_PIN_3, relayState3 ? HIGH : LOW);
    delay(200);
  }
  lastSwitch3 = currentSwitch3;
}

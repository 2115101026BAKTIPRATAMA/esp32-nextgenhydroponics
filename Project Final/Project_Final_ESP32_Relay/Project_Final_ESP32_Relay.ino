#include <WiFi.h>
#include <ESP32Firebase.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Wire.h>

const char* ssid = "Smart Green Garden";
const char* password = "Sukasukakami";

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 28800);  // 28800 seconds = 8 hours

#define FIREBASE_URL "https://research-nextgenhydroponics-default-rtdb.asia-southeast1.firebasedatabase.app/"
Firebase firebase(FIREBASE_URL);

#define PIN_RELAY_PENGURASAN_PIPA 4
#define PIN_RELAY_POMPA_IRIGASI 5
#define PIN_RELAY_DINAMO_PENGADUK 12
#define PIN_RELAY_NUTRISI_AB 13
#define PIN_RELAY_PH_UP 14
#define PIN_RELAY_PH_DOWN 15
#define PIN_RELAY_PESTISIDA 16
#define PIN_RELAY_SUMBER_AIR 17
#define PIN_RELAY_GROW_LIGHT 18
#define LDR_PIN 19  

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();

  Serial.print("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi");

  pinMode(PIN_RELAY_PENGURASAN_PIPA, OUTPUT);
  digitalWrite(PIN_RELAY_PENGURASAN_PIPA, LOW); // Initialize relay as OFF
  pinMode(PIN_RELAY_POMPA_IRIGASI, OUTPUT);
  digitalWrite(PIN_RELAY_POMPA_IRIGASI, LOW); // Initialize relay as OFF
  pinMode(PIN_RELAY_DINAMO_PENGADUK, OUTPUT);
  digitalWrite(PIN_RELAY_DINAMO_PENGADUK, LOW); // Initialize relay as OFF
  pinMode(PIN_RELAY_NUTRISI_AB, OUTPUT);
  digitalWrite(PIN_RELAY_NUTRISI_AB, LOW); // Initialize relay as OFF
  pinMode(PIN_RELAY_PH_UP, OUTPUT);
  digitalWrite(PIN_RELAY_PH_UP, LOW); // Initialize relay as OFF
  pinMode(PIN_RELAY_PH_DOWN, OUTPUT);
  digitalWrite(PIN_RELAY_PH_DOWN, LOW); // Initialize relay as OFF
  pinMode(PIN_RELAY_PESTISIDA, OUTPUT);
  digitalWrite(PIN_RELAY_PESTISIDA, LOW); // Initialize relay as OFF
  pinMode(PIN_RELAY_SUMBER_AIR, OUTPUT);
  digitalWrite(PIN_RELAY_SUMBER_AIR, LOW); // Initialize relay as OFF
  pinMode(PIN_RELAY_GROW_LIGHT, OUTPUT);
  digitalWrite(PIN_RELAY_GROW_LIGHT, LOW); // Initialize relay as OFF
  pinMode(LDR_PIN, INPUT);
}

void loop() {
  timeClient.update();
  unsigned long epochTime = timeClient.getEpochTime();
  struct tm *ptm = gmtime((time_t *)&epochTime);

  int year = ptm->tm_year + 1900;
  int month = ptm->tm_mon + 1;
  int day = ptm->tm_mday;
  int hour = ptm->tm_hour;
  int minute = ptm->tm_min;
  int second = ptm->tm_sec;

  char dateStr[11]; // YYYY-MM-DD
  sprintf(dateStr, "%04d-%02d-%02d", year, month, day);
  char timeStr[6]; // HH:MM
  sprintf(timeStr, "%02d:%02d", hour, minute);

  sendDataToFirebase(dateStr, timeStr);
}

void sendDataToFirebase(const char* dateStr, const char* timeStr) {
  checkRelay("/esp32controls/relay_pengurasan_pipa", PIN_RELAY_PENGURASAN_PIPA, "RELAY_PENGURASAN_PIPA");
  checkRelay("/esp32controls/relay_pompa_irigasi", PIN_RELAY_POMPA_IRIGASI, "RELAY_POMPA_IRIGASI");
  checkRelay("/esp32controls/relay_dinamo_pengaduk", PIN_RELAY_DINAMO_PENGADUK, "RELAY_DINAMO_PENGADUK");
  checkRelay("/esp32controls/relay_nutrisi_ab", PIN_RELAY_NUTRISI_AB, "RELAY_NUTRISI_AB");
  checkRelay("/esp32controls/relay_ph_up", PIN_RELAY_PH_UP, "RELAY_PH_UP");
  checkRelay("/esp32controls/relay_ph_down", PIN_RELAY_PH_DOWN, "RELAY_PH_DOWN");
  checkRelay("/esp32controls/relay_sumber_air", PIN_RELAY_SUMBER_AIR, "RELAY_SUMBER_AIR");

  int control_hama_otomatis = firebase.getInt("/esp32controls/controls_action");
  if (control_hama_otomatis == 1){
      Serial.println("Pestisida diotomatiskan");
      String status_hama_path = "/esp32cam/" + String(dateStr) + "/" + String(timeStr) + "/status_hama";
      if (status_hama_path == "true"){
      firebase.setInt("esp32controls/relay_pompa_pestisida", 1);
      }else{
      firebase.setInt("esp32controls/relay_pompa_pestisida", 0);
      }
  } else {
    Serial.println("Pestisida dimanualkan");
      checkRelay("/esp32controls/relay_pompa_pestisida", PIN_RELAY_PESTISIDA, "RELAY_PESTISIDA");
  }
  
  int cek_controls_grow_light = firebase.getInt("esp32controls/controls_action");
  int val = analogRead(LDR_PIN);
  if (cek_controls_grow_light == 1) {
    Serial.println("Grow_light diotomatiskan");
    int Status_grow_light = val >= 2000 ? 1 : 0;
    if (Status_grow_light == 1) {
      digitalWrite(PIN_RELAY_GROW_LIGHT, HIGH);
    } else {
      digitalWrite(PIN_RELAY_GROW_LIGHT, LOW);
    }
    bool result = firebase.setInt("/esp32controls/relay_grow_light", Status_grow_light);
    Serial.println(result ? "Status berhasil diupdate" : "Gagal mengupdate status");
  } else {
    Serial.println("Grow_light dimanualkan");
    int Status_grow_light_manual = firebase.getInt("/esp32controls/relay_grow_light");
    if(Status_grow_light_manual == 1){
     digitalWrite(PIN_RELAY_GROW_LIGHT, HIGH);
    } else {
      digitalWrite(PIN_RELAY_GROW_LIGHT, LOW);
    } 
  }
}

void checkRelay(const char* firebasePath, int relayPin, const char* relayName) {
  int relayState = firebase.getInt(firebasePath);
  if (relayState == 1) {
    digitalWrite(relayPin, HIGH);
    Serial.print(relayName);
    Serial.println(" menyala");
  } else if (relayState == 0) {
    digitalWrite(relayPin, LOW);
  }
}

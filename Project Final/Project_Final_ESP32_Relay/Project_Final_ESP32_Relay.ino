#include <WiFi.h>
#include <ESP32Firebase.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Wire.h>

// Deklarasi SSID dan password WiFi
const char* ssid = "Smart Green Garden";
const char* password = "Sukasukakami";

// Inisiasi Firebase
#define FIREBASE_URL "https://next-gen-hydroponics-default-rtdb.asia-southeast1.firebasedatabase.app/"
Firebase firebase(FIREBASE_URL);

// Definisi relay pin 
#define PIN_RELAY_PENGURASAN_PIPA 4
#define PIN_RELAY_POMPA_IRIGASI 5
#define PIN_RELAY_DINAMO_PENGADUK 12
#define PIN_RELAY_NUTRISI_AB 13
#define PIN_RELAY_PH_UP 14
#define PIN_RELAY_PH_DOWN 15
#define PIN_RELAY_PESTISIDA 16
#define PIN_RELAY_SUMBER_AIR 17
#define PIN_RELAY_GROW_LIGHT 18

// Inisiasi sensor intensitas Cahaya
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

  // Inisiasi relay 
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

  int cek_controls_grow_light = firebase.getInt("esp32controls/controls_grow_light");
// Baca nilai sensor LDR
int val = analogRead(LDR_PIN);

if (cek_controls_grow_light == 1) {
  Serial.println("Grow_light diotomatiskan");
  String Status_grow_light = val >= 2000 ? "hidup" : "mati";
  if (Status_grow_light == "hidup") {
    digitalWrite(PIN_RELAY_GROW_LIGHT, HIGH);
  } else {
    digitalWrite(PIN_RELAY_GROW_LIGHT, LOW);
  }
  // Mengirim status grow light ke Firebase
  bool result = firebase.setString("/esp32controls/status_grow_light", Status_grow_light);
  Serial.println(result ? "Status berhasil diupdate" : "Gagal mengupdate status");
} else {
  // Kondisi manual yang bisa dikontrol pengguna
  Serial.println("Grow_light dimanualkan");
  String Status_grow_light_manual = firebase.getString("/esp32controls/status_grow_light");
  Serial.println(Status_grow_light_manual); 
}



  // Kirim data relay ke Firebase
  sendDataToFirebase();
}

void sendDataToFirebase() {
  // Dapatkan status relay dari Firebase 
  int RELAY_PENGURASAN_PIPA = firebase.getInt("/esp32controls/relay_pengurasan_pipa");
  if (RELAY_PENGURASAN_PIPA == 1) {
    digitalWrite(PIN_RELAY_PENGURASAN_PIPA, HIGH);
    Serial.println("RELAY_PENGURASAN_PIPA menyala");
  } else if (RELAY_PENGURASAN_PIPA == 0) {
    digitalWrite(PIN_RELAY_PENGURASAN_PIPA, LOW);
  }

  int RELAY_POMPA_IRIGASI = firebase.getInt("/esp32controls/relay_pompa_irigasi");
  if (RELAY_POMPA_IRIGASI == 1) {
    digitalWrite(PIN_RELAY_POMPA_IRIGASI, HIGH);
    Serial.println("RELAY_POMPA_IRIGASI menyala");
  } else if (RELAY_POMPA_IRIGASI == 0) {
    digitalWrite(PIN_RELAY_POMPA_IRIGASI, LOW);
  }

  int RELAY_DINAMO_PENGADUK = firebase.getInt("/esp32controls/relay_dinamo_pengaduk");
  if (RELAY_DINAMO_PENGADUK == 1) {
    digitalWrite(PIN_RELAY_DINAMO_PENGADUK, HIGH);
    Serial.println("RELAY_DINAMO_PENGADUK menyala");
  } else if (RELAY_DINAMO_PENGADUK == 0) {
    digitalWrite(PIN_RELAY_DINAMO_PENGADUK, LOW);
  }

  int RELAY_NUTRISI_AB = firebase.getInt("/esp32controls/relay_nutrisi_ab");
  if (RELAY_NUTRISI_AB == 1) {
    digitalWrite(PIN_RELAY_NUTRISI_AB, HIGH);
    Serial.println("RELAY_NUTRISI_AB menyala");
  } else if (RELAY_NUTRISI_AB == 0) {
    digitalWrite(PIN_RELAY_NUTRISI_AB, LOW);
  }

  int RELAY_PH_UP = firebase.getInt("/esp32controls/relay_ph_up");
  if (RELAY_PH_UP == 1) {
    digitalWrite(PIN_RELAY_PH_UP, HIGH);
    Serial.println("RELAY_PH_UP menyala");
  } else if (RELAY_PH_UP == 0) {
    digitalWrite(PIN_RELAY_PH_UP, LOW);
  }

  int RELAY_PH_DOWN = firebase.getInt("/esp32controls/relay_ph_down");
  if (RELAY_PH_DOWN == 1) {
    digitalWrite(PIN_RELAY_PH_DOWN, HIGH);
    Serial.println("RELAY_PH_DOWN menyala");
  } else if (RELAY_PH_DOWN == 0) {
    digitalWrite(PIN_RELAY_PH_DOWN, LOW);
  }

  int RELAY_PESTISIDA = firebase.getInt("/esp32controls/relay_pompa_pestisida");
  if (RELAY_PESTISIDA == 1) {
    digitalWrite(PIN_RELAY_PESTISIDA, HIGH);
    Serial.println("RELAY_PESTISIDA menyala");
  } else if (RELAY_PESTISIDA == 0) {
    digitalWrite(PIN_RELAY_PESTISIDA, LOW);
  }

  int RELAY_SUMBER_AIR = firebase.getInt("/esp32controls/relay_sumber_air");
  if (RELAY_SUMBER_AIR == 1) {
    digitalWrite(PIN_RELAY_SUMBER_AIR, HIGH);
    Serial.println("RELAY_SUMBER_AIR menyala");
  } else if (RELAY_SUMBER_AIR == 0) {
    digitalWrite(PIN_RELAY_SUMBER_AIR, LOW);
  }

}

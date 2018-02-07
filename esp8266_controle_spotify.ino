/************************** Inclusão das Bibliotecas **************************/

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>

/****************************** User Config ***********************************/

#include "user_config.h"
#include "user_config_override.h"

/**************************** DEBUG *******************************/

#ifdef DEBUG
#define DEBUG_PRINTLN(m) Serial.println(m)
#define DEBUG_PRINT(m) Serial.print(m)

#else
#define DEBUG_PRINTLN(m)
#define DEBUG_PRINT(m)

#endif

/**************************** Variaveis globais *******************************/
int playPause = D5;

bool playerStatus = true;

/************************* Declaração dos Prototypes **************************/

void initSerial();
void initWiFi();

/************************* Instanciação dos objetos  **************************/

WiFiClientSecure client;
HTTPClient http;

/*********************** Implementação dos Prototypes *************************/

void initSerial() {
#ifdef DEBUG
  Serial.begin(115200);
#endif
}

void initWiFi() {
  delay(10);
  DEBUG_PRINTLN("");
  DEBUG_PRINT("[WIFI] Conectando-se em " + String(WIFI_SSID));

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    DEBUG_PRINT(".");
  }

  DEBUG_PRINTLN("");
  DEBUG_PRINT(F("[WIFI] SSID: "));
  DEBUG_PRINTLN(WIFI_SSID);
  DEBUG_PRINT(F("[WIFI] IP: "));
  DEBUG_PRINTLN(WiFi.localIP());
  DEBUG_PRINT(F("[WIFI] Mac: "));
  DEBUG_PRINTLN(WiFi.macAddress());
  DEBUG_PRINTLN("");
}

void httpPUT(String endpoint) {
  String uri = "/v1/me/player/" + endpoint;

  http.begin(API_SPOTIFY, 443, uri, true, "AB BC 7C 9B 7A D8 5D 98 8B B2 72 A4 4C 13 47 9A 00 2F 70 B5");

  http.addHeader("content-type", "application/json");
  http.addHeader("Authorization", SPOTIFY_TOKEN);

  int httpCode = http.PUT("");

  if (httpCode < 0) {
    Serial.println("request error - " + httpCode);
  }

  if (httpCode != HTTP_CODE_OK) {
    Serial.println("request error - " + httpCode);
  }

  String response =  http.getString();
  http.end();

  DEBUG_PRINTLN("[HTTP] Endpoint " + uri);
  DEBUG_PRINTLN("[HTTP] Response " + response);
  DEBUG_PRINTLN("[HTTP] Length: " + String(response.length()));
  DEBUG_PRINTLN("[HTTP] Code: " + String(httpCode));
}

void play() {
  httpPUT("play");
}

void pause() {
  httpPUT("pause");
}

/********************************** Sketch ************************************/

void setup() {
  initSerial();
  initWiFi();

  pinMode(playPause, INPUT_PULLUP);

  delay(1000);

  if (!client.connect(API_SPOTIFY, 443)) {
    Serial.println("connection failed");
    return;
  } else {
    DEBUG_PRINTLN("Conexão efetuada!");
  }

}

void loop() {
  yield();

  if (digitalRead(playPause) == 0 and playerStatus) {
    pause();
    playerStatus = false;
  }

  if (digitalRead(playPause) == 0 and !playerStatus) {
    play();
    playerStatus = true;
  }

}

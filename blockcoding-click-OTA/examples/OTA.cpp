#include <Arduino.h>
#include <WiFi.h>
#include <blockcoding-click-OTA.h>

const char* WIFI_SSID = "YOUR WIFI SSID";
const char* WIFI_PASS = "YOUR WIFI KEY";
int i = 0;

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println(".");
  }

  // Defaults in Library:
  // - Check Interval: 30s
  // - printLogs(false)
  // - printProgress(true)
  //
  // Only if you want to change something:

  // blockcoding_click_ota.setCheckUpdate_Interval_seconds(10);
  blockcoding_click_ota.printLogs(true);
  // blockcoding_click_ota.printProgress(false);

  // optional: set only one of these password values
  // blockcoding_click_ota.setOTAPassword("meinPasswort");
  // blockcoding_click_ota.setOTAPasswordMD5("...");

  blockcoding_click_ota.registerDevice();
}

void loop() {
  blockcoding_click_ota.update();
  blockcoding_click_ota.print("Test Print via OTA");
  blockcoding_click_ota.println(i);
  i++;
  delay(1000);
}

#include "blockcoding-click-OTA.h"

#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <Update.h>
#include <time.h>

#include <blockcoding-click-certificate.h>
#if defined(ESP32)
#include <esp_system.h>
#endif

// Single Instance
blockcoding_click_OTA blockcoding_click_ota;

// FIX: API + Endpoints
const char* blockcoding_click_OTA::API_BASE_URL       = "https://api.blockcoding.click";
const char* blockcoding_click_OTA::ENDPOINT_REGISTER  = "/ota/device/register";
const char* blockcoding_click_OTA::ENDPOINT_CHECK     = "/ota/device/check";
const char* blockcoding_click_OTA::ENDPOINT_ACK       = "/ota/device/ack";

blockcoding_click_OTA::blockcoding_click_OTA() {
  _serialMqttClient.setClient(_serialMqttNet);
  _serialMqttClient.setServer(_serialMqttHost, _serialMqttPort);
}

void blockcoding_click_OTA::setOTAPassword(const char* pwd) {
  _otaPwd = (pwd ? pwd : "");
  _otaPwdMd5 = "";
}

void blockcoding_click_OTA::setOTAPasswordMD5(const char* pwdMd5) {
  _otaPwdMd5 = (pwdMd5 ? pwdMd5 : "");
  _otaPwd = "";
}

void blockcoding_click_OTA::setCheckUpdate_Interval_seconds(uint32_t seconds) {
  _checkIntervalMs = seconds * 1000UL;
}

void blockcoding_click_OTA::printLogs(bool enabled) {
  _printLogs = enabled;
}

void blockcoding_click_OTA::printProgress(bool enabled) {
  _printProgress = enabled;
}

void blockcoding_click_OTA::print(const String& msg) {
  Serial.print(msg);
  serialBufferAppend_(msg, false);
}

void blockcoding_click_OTA::print(const char* msg) {
  Serial.print(msg);
  serialBufferAppend_(msg ? String(msg) : String(""), false);
}

void blockcoding_click_OTA::print(char value) {
  Serial.print(value);
  serialBufferAppend_(String(value), false);
}

void blockcoding_click_OTA::print(int value) {
  Serial.print(value);
  serialBufferAppend_(String(value), false);
}

void blockcoding_click_OTA::print(unsigned int value) {
  Serial.print(value);
  serialBufferAppend_(String(value), false);
}

void blockcoding_click_OTA::print(long value) {
  Serial.print(value);
  serialBufferAppend_(String(value), false);
}

void blockcoding_click_OTA::print(unsigned long value) {
  Serial.print(value);
  serialBufferAppend_(String(value), false);
}

void blockcoding_click_OTA::print(float value, int decimals) {
  Serial.print(value, decimals);
  serialBufferAppend_(String(value, decimals), false);
}

void blockcoding_click_OTA::println() {
  Serial.println();
  serialBufferAppend_(String(""), true);
}

void blockcoding_click_OTA::println(const String& msg) {
  Serial.println(msg);
  serialBufferAppend_(msg, true);
}

void blockcoding_click_OTA::println(const char* msg) {
  Serial.println(msg);
  serialBufferAppend_(msg ? String(msg) : String(""), true);
}

void blockcoding_click_OTA::println(char value) {
  Serial.println(value);
  serialBufferAppend_(String(value), true);
}

void blockcoding_click_OTA::println(int value) {
  Serial.println(value);
  serialBufferAppend_(String(value), true);
}

void blockcoding_click_OTA::println(unsigned int value) {
  Serial.println(value);
  serialBufferAppend_(String(value), true);
}

void blockcoding_click_OTA::println(long value) {
  Serial.println(value);
  serialBufferAppend_(String(value), true);
}

void blockcoding_click_OTA::println(unsigned long value) {
  Serial.println(value);
  serialBufferAppend_(String(value), true);
}

void blockcoding_click_OTA::println(float value, int decimals) {
  Serial.println(value, decimals);
  serialBufferAppend_(String(value, decimals), true);
}

void blockcoding_click_OTA::setSerialMqttHost(const char* host) {
  if (host && host[0]) {
    _serialMqttHost = host;
    _serialMqttClient.setServer(_serialMqttHost, _serialMqttPort);
  }
}

void blockcoding_click_OTA::setSerialMqttPort(uint16_t port) {
  if (port > 0) {
    _serialMqttPort = port;
    _serialMqttClient.setServer(_serialMqttHost, _serialMqttPort);
  }
}

void blockcoding_click_OTA::setSerialMqttEnabled(bool enabled) {
  _serialMqttEnabled = enabled;
  if (!enabled && _serialMqttClient.connected()) {
    _serialMqttClient.disconnect();
  }
}

String blockcoding_click_OTA::getOTADeviceName() const {
  return _deviceId;
}

void blockcoding_click_OTA::onLog(LogFn fn) { _logFn = fn; }
void blockcoding_click_OTA::onProgress(ProgressFn fn) { _progressFn = fn; }
void blockcoding_click_OTA::onUpdateAvailable(SimpleFn fn) { _updateAvailableFn = fn; }

void blockcoding_click_OTA::log(const String& msg) {
  if (_printLogs) Serial.println(msg);
  if (_logFn) _logFn(msg);
  serialPublishLine_(msg);
}

void blockcoding_click_OTA::logError(const String& msg) {
  if (_printLogs) Serial.println(msg);
  if (_logFn) _logFn(msg);
  serialPublishLine_(msg);
}

void blockcoding_click_OTA::progress(size_t done, size_t total) {
  if (_printProgress) {
    if (total > 0) Serial.printf("OTA Progress: %u / %u bytes\n", (unsigned)done, (unsigned)total);
    else Serial.printf("OTA Progress: %u bytes\n", (unsigned)done);
  }
  if (_progressFn) _progressFn(done, total);
}

long blockcoding_click_OTA::nowEpoch() {
  time_t now = time(nullptr);
  if (now < 1000000000) return 0;
  return (long)now;
}

void blockcoding_click_OTA::ensureTimeSync() {
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");
  uint32_t start = millis();
  while (nowEpoch() == 0 && (millis() - start) < 15000) {
    delay(250);
  }
  long epoch = nowEpoch();
  if (epoch > 0) {
    log("OTA: Now epoch=" + String(epoch));
  } else {
    log("OTA: Time sync failed (epoch=0)");
  }
}

String blockcoding_click_OTA::serialMqttPassword_() {
  if (_otaPwd && _otaPwd[0]) {
    return String(_otaPwd);
  }
  if (_otaPwdMd5 && _otaPwdMd5[0]) {
    return String("md5:") + String(_otaPwdMd5);
  }
  // Mosquitto-go-auth rejects null/empty passwords; use whitespace sentinel.
  return String(" ");
}

bool blockcoding_click_OTA::ensureSerialMqttConnected_() {
  if (!_serialMqttEnabled) return false;
  if (!_serialMqttHost || !_serialMqttHost[0]) return false;
  if (WiFi.status() != WL_CONNECTED) return false;
  if (_serialMqttClient.connected()) return true;

  unsigned long now = millis();
  if (now - _serialLastConnectAttempt < 5000) return false;
  _serialLastConnectAttempt = now;

  if (_deviceId.length() == 0) return false;
  _serialTopic = String("serial/") + _deviceId;

  String clientId = String("serial-") + _deviceId;
  String username = String("device:") + _deviceId;
  String password = serialMqttPassword_();

  bool ok;
  if (password.length()) {
    ok = _serialMqttClient.connect(clientId.c_str(), username.c_str(), password.c_str());
  } else {
    ok = _serialMqttClient.connect(clientId.c_str(), username.c_str(), "");
  }
  if (_printLogs && !_serialMqttLogEmitted) {
    if (ok) {
      Serial.println("OTA Serial Monitor MQTT connected successfully");
    } else {
      Serial.println("OTA Serial Monitor MQTT connection FAILED (state=" + String(_serialMqttClient.state()) + ")");
    }
    _serialMqttLogEmitted = true;
  }
  return ok;
}

void blockcoding_click_OTA::serialPublishLine_(const String& line) {
  if (!_serialMqttEnabled) return;
  if (!ensureSerialMqttConnected_()) return;
  if (_serialTopic.length() == 0) {
    _serialTopic = String("serial/") + _deviceId;
  }
  _serialMqttClient.publish(_serialTopic.c_str(), line.c_str());
}

void blockcoding_click_OTA::serialBufferAppend_(const String& chunk, bool newline) {
  if (!_serialMqttEnabled) return;
  if (!ensureSerialMqttConnected_()) return;
  if (_serialTopic.length() == 0) {
    _serialTopic = String("serial/") + _deviceId;
  }
  _serialLineBuffer += chunk;
  if (newline) _serialLineBuffer += "\n";
  int idx = _serialLineBuffer.indexOf('\n');
  while (idx >= 0) {
    String line = _serialLineBuffer.substring(0, idx);
    _serialLineBuffer = _serialLineBuffer.substring(idx + 1);
    serialPublishLine_(line);
    idx = _serialLineBuffer.indexOf('\n');
  }
  if (_serialLineBuffer.length() > 512) {
    serialPublishLine_(_serialLineBuffer);
    _serialLineBuffer = "";
  }
}

void blockcoding_click_OTA::serialMqttLoop_() {
  if (_serialMqttClient.connected()) {
    _serialMqttClient.loop();
  }
}

void blockcoding_click_OTA::appendAuth(String& body) {
  if (_otaPwd && _otaPwd[0]) {
    body += ",\"ota_pwd\":\"" + String(_otaPwd) + "\"";
  } else if (_otaPwdMd5 && _otaPwdMd5[0]) {
    body += ",\"ota_pwd_md5\":\"" + String(_otaPwdMd5) + "\"";
  }
}

bool blockcoding_click_OTA::postJson(const char* url, const String& body, String* response) {
  WiFiClientSecure client;
  client.setCACert(BLOCKCODING_CLICK_CA);

  HTTPClient https;
  if (!https.begin(client, url)) {
    logError("OTA tried to contact the server but could not reach it.");
    return false;
  }

  https.addHeader("Content-Type", "application/json");
  int code = https.POST(body);

  if (code <= 0) {
    logError("OTA tried to contact the server but could not reach it.");
  }

  if (response) *response = https.getString();

  https.end();
  return code >= 200 && code < 300;
}

// JSON helpers (wie im Original)
bool blockcoding_click_OTA::jsonGetBool(const String& json, const char* key) {
  String needle = String("\"") + key + "\":";
  int idx = json.indexOf(needle);
  if (idx < 0) return false;
  int start = idx + needle.length();
  while (start < (int)json.length() && json[start] == ' ') start++;
  return json.startsWith("true", start);
}

String blockcoding_click_OTA::jsonGetString(const String& json, const char* key) {
  String needle = String("\"") + key + "\":";
  int idx = json.indexOf(needle);
  if (idx < 0) return "";
  int start = idx + needle.length();
  while (start < (int)json.length() && json[start] == ' ') start++;
  if (start >= (int)json.length() || json[start] != '"') return "";
  start++;
  int end = json.indexOf('"', start);
  if (end < 0) return "";
  return json.substring(start, end);
}

long blockcoding_click_OTA::jsonGetLong(const String& json, const char* key) {
  String needle = String("\"") + key + "\":";
  int idx = json.indexOf(needle);
  if (idx < 0) return 0;
  int start = idx + needle.length();
  while (start < (int)json.length() && json[start] == ' ') start++;
  int end = start;
  while (end < (int)json.length() && (isDigit(json[end]) || json[end] == '-')) end++;
  String num = json.substring(start, end);
  return num.length() ? num.toInt() : 0;
}

bool blockcoding_click_OTA::registerDeviceRequest(String* statusOut) {
  String body = "{\"device_id\":\"" + _deviceId + "\"";
  appendAuth(body);
  body += "}";

  String response;
  String url = String(API_BASE_URL) + ENDPOINT_REGISTER;
  bool ok = postJson(url.c_str(), body, &response);
  if (statusOut) {
    *statusOut = jsonGetString(response, "status");
  }
  return ok;
}

bool blockcoding_click_OTA::checkUpdate(String& firmwareUrl, long& firmwareTs) {
  String body = "{\"device_id\":\"" + _deviceId + "\"";
  appendAuth(body);
  body += "}";

  String response;
  String url = String(API_BASE_URL) + ENDPOINT_CHECK;

  if (!postJson(url.c_str(), body, &response)) {
    _lastCheckOk = false;
    _lastFirmwareTs = 0;
    return false;
  }

  _lastCheckOk = true;
  _lastFirmwareTs = jsonGetLong(response, "firmware_ts");
  if (!jsonGetBool(response, "update_available")) return false;

  firmwareUrl = jsonGetString(response, "firmware_url");
  firmwareTs = _lastFirmwareTs;
  return firmwareUrl.length() > 0;
}

bool blockcoding_click_OTA::ackUpdate(long fwTs) {
  String body = "{\"device_id\":\"" + _deviceId + "\"";
  if (fwTs > 0) body += ",\"fw_ts_device\":" + String(fwTs);
  appendAuth(body);
  body += "}";

  String url = String(API_BASE_URL) + ENDPOINT_ACK;
  return postJson(url.c_str(), body, nullptr);
}

bool blockcoding_click_OTA::startUpdate(const String& firmwareUrl, long firmwareTs) {
  WiFiClientSecure client;
  client.setCACert(BLOCKCODING_CLICK_CA);

  HTTPClient https;
  if (!https.begin(client, firmwareUrl)) {
    log("OTA: firmware https.begin failed");
    return false;
  }

  int code = https.GET();
  if (code != HTTP_CODE_OK) {
    https.end();
    log("OTA: firmware GET failed");
    return false;
  }

  int total = https.getSize();
  if (!Update.begin(total > 0 ? total : UPDATE_SIZE_UNKNOWN)) {
    https.end();
    log("OTA: Update.begin failed");
    return false;
  }

  WiFiClient* stream = &https.getStream();
  size_t written = 0;
  uint8_t buf[1024];

  while (https.connected()) {
    size_t avail = stream->available();
    if (avail) {
      int n = stream->readBytes(buf, (avail > sizeof(buf)) ? sizeof(buf) : avail);
      if (n <= 0) break;

      if (Update.write(buf, n) != (size_t)n) {
        Update.abort();
        https.end();
        log("OTA: Update.write failed");
        return false;
      }

      written += n;
      progress(written, (total > 0) ? (size_t)total : 0);
    } else {
      delay(1);
    }

    if (total > 0 && written >= (size_t)total) break;
  }

  if (!Update.end(true)) {
    https.end();
    log("OTA: Update.end failed");
    return false;
  }

  https.end();

  long ackTs = (firmwareTs > 0) ? firmwareTs : nowEpoch();
  ackUpdate(ackTs);

  log("OTA firmware flash success");
  log("OTA: Rebooting now");

  Serial.flush();
  delay(250);
#if defined(ESP32)
  esp_restart();
#endif
  ESP.restart();
  return true;
}

bool blockcoding_click_OTA::registerDevice() {
  _deviceId = String("esp32-") + WiFi.macAddress();
  ensureTimeSync();

  // IMMER beim Initialize ausgeben (wie von dir gewünscht)
  Serial.println("OTA: Device-ID: " + _deviceId);

  String status;
  bool ok = registerDeviceRequest(&status);
  if (ok) {
    if (status == "exists") {
      log("OTA: Registration exists");
    } else {
      log("OTA: Registration successful");
    }
  } else {
    logError("OTA: Registration failed");
  }
  return ok;
}

void blockcoding_click_OTA::update() {
  serialMqttLoop_();
  uint32_t nowMs = millis();
  if (_checkIntervalMs > 0 && (nowMs - _lastCheckMs) < _checkIntervalMs) return;
  _lastCheckMs = nowMs;

  String firmwareUrl;
  long firmwareTs = 0;

  log("OTA: Checking for firmware update...");
  if (checkUpdate(firmwareUrl, firmwareTs)) {
    if (firmwareTs > 0) {
      log("OTA: Update available (ts=" + String(firmwareTs) + ")");
    } else {
      log("OTA: Update available");
    }
    if (_updateAvailableFn) _updateAvailableFn();
    startUpdate(firmwareUrl, firmwareTs);
  } else if (_lastCheckOk) {
    if (_lastFirmwareTs > 0) {
      log("OTA: Firmware checked, newest firmware already on device (ts=" + String(_lastFirmwareTs) + ")");
    } else {
      log("OTA: Firmware checked, no update available");
    }
  }
}





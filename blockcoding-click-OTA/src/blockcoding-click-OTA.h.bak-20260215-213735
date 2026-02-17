#pragma once
#include <Arduino.h>
#include <WiFiClient.h>
#include <PubSubClient.h>

class blockcoding_click_OTA {
public:
  blockcoding_click_OTA();

  // 1x in setup() (nach WiFi verbunden)
  bool registerDevice();

  // oft in loop()
  void update();

  // Genau EINES davon setzen (optional)
  void setOTAPassword(const char* pwd);
  void setOTAPasswordMD5(const char* pwdMd5);

  // Check-Intervall in SEKUNDEN (Default ist 30s in der Library)
  void setCheckUpdate_Interval_seconds(uint32_t seconds);

  // Built-in Ausgaben (Defaults: Logs=false, Progress=true)
  void printLogs(bool enabled);
  void printProgress(bool enabled);

  // Getter für OTA Device Name / ID
  String getOTADeviceName() const;

  // Optional: MQTT serial mirror (defaults on)
  void setSerialMqttHost(const char* host);
  void setSerialMqttPort(uint16_t port);
  void setSerialMqttEnabled(bool enabled);

  // Serial wrappers (mirror to MQTT when enabled)
  void print(const String& msg);
  void print(const char* msg);
  void print(char value);
  void print(int value);
  void print(unsigned int value);
  void print(long value);
  void print(unsigned long value);
  void print(float value, int decimals = 2);

  void println();
  void println(const String& msg);
  void println(const char* msg);
  void println(char value);
  void println(int value);
  void println(unsigned int value);
  void println(long value);
  void println(unsigned long value);
  void println(float value, int decimals = 2);

  // Optional Callbacks
  using LogFn = void (*)(const String& msg);
  using ProgressFn = void (*)(size_t done, size_t total);
  using SimpleFn = void (*)();

  void onLog(LogFn fn);
  void onProgress(ProgressFn fn);
  void onUpdateAvailable(SimpleFn fn);

private:
  // FIX (nicht änderbar)
  static const char* API_BASE_URL;
  static const char* ENDPOINT_REGISTER;
  static const char* ENDPOINT_CHECK;
  static const char* ENDPOINT_ACK;

  // Auth (genau eines)
  const char* _otaPwd = "";
  const char* _otaPwdMd5 = "";

  // Verhalten (Defaults fix in der Library)
  uint32_t _checkIntervalMs = 30000; // default 30s
  uint32_t _lastCheckMs = 0;

  bool _printLogs = false;        // default false
  bool _printProgress = true;     // default true

  String _deviceId;
  bool _lastCheckOk = false;
  long _lastFirmwareTs = 0;

  WiFiClient _serialMqttNet;
  PubSubClient _serialMqttClient;
  bool _serialMqttEnabled = true;
  const char* _serialMqttHost = "mqtt.blockcoding.click";
  uint16_t _serialMqttPort = 1883;
  String _serialTopic;
  String _serialLineBuffer;
  unsigned long _serialLastConnectAttempt = 0;
  bool _serialMqttLogEmitted = false;
  bool _flashSuccessLogged = false;

  // Callbacks
  LogFn _logFn = nullptr;
  ProgressFn _progressFn = nullptr;
  SimpleFn _updateAvailableFn = nullptr;

  // Helpers
  void log(const String& msg);
  void logWithTimestamp(const String& msg);
  void logError(const String& msg);
  void progress(size_t done, size_t total);

  long nowEpoch();
  void ensureTimeSync();
  String formatTimestamp_();
  void resetOutputPins_();

  void appendAuth(String& body);
  bool postJson(const char* url, const String& body, String* response);

  bool registerDeviceRequest(String* statusOut);
  bool checkUpdate(String& firmwareUrl, long& firmwareTs);
  bool ackUpdate(long fwTs);
  bool startUpdate(const String& firmwareUrl, long firmwareTs);

  bool ensureSerialMqttConnected_();
  void serialPublishLine_(const String& line);
  void serialBufferAppend_(const String& chunk, bool newline);
  void serialMqttLoop_();
  String serialMqttPassword_();

  // Mini JSON helper (wie im Original)
  bool jsonGetBool(const String& json, const char* key);
  String jsonGetString(const String& json, const char* key);
  long jsonGetLong(const String& json, const char* key);
};

// Single Instance (nur eine im Sketch)
extern blockcoding_click_OTA blockcoding_click_ota;


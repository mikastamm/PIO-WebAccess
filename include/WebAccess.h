#ifndef WEBACCESS_H
#define WEBACCESS_H

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <ESPAsyncWebServer.h>
#include <WebSerial.h>

class WebAccess {
public:
    WebAccess();
    ~WebAccess(); // Destructor to deallocate memory
    void print(const String& message);
    void println(const String& message);
    void RegisterFunction(const String& functionName, int (*function)(String));
    void ReportError(const String& error, const String& errorCode);
    void Setup(String deviceName,String devicePassword = "");
    void addWifiNetwork(const String& ssid, const String& pass);
    void listen();

private:
  struct WifiCredentials {
        String ssid;
        String password;
    };
    void setupWebSerial();
    void printAvailableCommands();
    void setupWifi();
    void setupOTA(String deviceName, String password = "");

    String* functionNames = nullptr;
    int (**functions)(String) = nullptr;
    int functionNameCount = 0;

    WifiCredentials* wifiCredentials = nullptr;
    int networkCount = 0;

};

#endif // WEBACCESS_H

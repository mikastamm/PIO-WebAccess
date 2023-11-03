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
    void Setup(String deviceName, String wifiNetworks[], String wifiPasswords[], String devicePassword = "");

private:
    void setupWebSerial();
    void printAvailableCommands();
    void setupWifi(String wifiNetworks[], String wifiPasswords[], int networkCount);
    void setupOTA(String deviceName, String password = "");

    String* functionNames = nullptr;
    int (**functions)(String) = nullptr;
    int functionNameCount = 0;
};

#endif // WEBACCESS_H

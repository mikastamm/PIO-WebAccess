#include "WebAccess.h"
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <ESPAsyncWebServer.h>

AsyncWebServer webserver(80);


WebAccess::WebAccess() {}

WebAccess::~WebAccess() {
    delete[] functionNames;
    delete[] functions;
}

// Using typedef for clarity
typedef int (*FunctionPointer)(String);

void WebAccess::RegisterFunction(const String& functionName, FunctionPointer function) {
    // Create new arrays with an extra slot
    String* newFunctionNames = new String[functionNameCount + 1];
    FunctionPointer* newFunctions = new FunctionPointer[functionNameCount + 1];

    // Copy the old data over to the new arrays
    for (int i = 0; i < functionNameCount; i++) {
        newFunctionNames[i] = functionNames[i];
        newFunctions[i] = functions[i];
    }

    // Add the new function and its name
    newFunctionNames[functionNameCount] = functionName;
    newFunctions[functionNameCount] = function;

    // Delete the old arrays and update the pointers
    delete[] functionNames;
    delete[] functions;

    functionNames = newFunctionNames;
    functions = newFunctions;
    functionNameCount++;
}



void WebAccess::setupWebSerial() {
    webserver.addRewrite(new AsyncWebRewrite("/", "/webserial"));
    webserver.begin();
    WebSerial.begin(&webserver);
    WebSerial.msgCallback([&](uint8_t *data, size_t len) {
        String d = "";
        for (int i = 0; i < len; i++) {
            d += char(data[i]);
        }
        bool found = false;
        this->println(">" + d);
        for (int i = 0; i < sizeof(this->functionNames) / sizeof(this->functionNames[0]); i++) {
            if (d.startsWith(this->functionNames[i])) {
                this->println("Running " + this->functionNames[i]);
                this->functions[i](d.substring(this->functionNames[i].length() + 1));
                found = true;
            }
        }
        if (!found) {
            this->println("Command not found. Available commands:");
            printAvailableCommands();
        }
    });
}

void WebAccess::printAvailableCommands() {
    for (int i = 0; i < this->functionNameCount; i++) {
        this->println(this->functionNames[i] + "\nhttp://" + DEVICE_NAME + ".local/" + this->functionNames[i] + "?params=\n");
    }
}
void WebAccess::print(const String& message) {
    Serial.print(message);
    WebSerial.print(message);
}

void WebAccess::println(const String& message) {
    Serial.println(message);
    WebSerial.println(message);
}

void WebAccess::ReportError(const String& error, const String& errorCode) {
    String errorMessage = "❌ ERR_" + errorCode + ": " + error;
    Serial.println(errorMessage);
    WebSerial.println(errorMessage);
}

  void WebAccess::addWifiNetwork(const String& ssid, const String& pass) {
        // Create a new array with an extra slot for the new network
        WifiCredentials* newWifiCredentials = new WifiCredentials[networkCount + 1];

        // Copy existing credentials over
        for (int i = 0; i < networkCount; i++) {
            newWifiCredentials[i] = wifiCredentials[i];
        }

        // Add the new network credentials
        newWifiCredentials[networkCount].ssid = ssid;
        newWifiCredentials[networkCount].password = pass;

        // Delete the old array and update the pointer
        delete[] wifiCredentials;
        wifiCredentials = newWifiCredentials;
        networkCount++;
    }
 void WebAccess::setupWifi() {
        if (networkCount <= 0) {
            ReportError("No Wifi Network Credentials provided", "WIFI_CONFIG");
            return;
        }

        int currentNetwork = 0;
        while (true) {
            Serial.print("🌐 Connecting to " + wifiCredentials[currentNetwork].ssid + ":" + wifiCredentials[currentNetwork].password );
            WiFi.begin(wifiCredentials[currentNetwork].ssid.c_str(), wifiCredentials[currentNetwork].password.c_str());
            
            for (int i = 0; i < 20; i++)
            {
                delay(500);
                if(WiFi.status() == WL_CONNECTED)
                continue;
                Serial.print(".");
            }
            

            if (WiFi.status() == WL_CONNECTED) {
                Serial.println("\n🔗 Connected to " + wifiCredentials[currentNetwork].ssid);
                Serial.println("IP: " + WiFi.localIP().toString());
                break;
            } else {
                Serial.println("❌ Failed to connect to " + wifiCredentials[currentNetwork].ssid);
                currentNetwork++;

                // If we've tried all the networks, start from the first one again.
                if (currentNetwork == networkCount) {
                    currentNetwork = 0;
                }
            }
        }
    }


void WebAccess::setupOTA(String deviceName, String password) {
    WiFi.mode(WIFI_STA);
    ArduinoOTA.setHostname(deviceName.c_str());

    if(password != "") {
        ArduinoOTA.setPassword(password.c_str());
    }

    ArduinoOTA.onStart([this]() {
        String type = (ArduinoOTA.getCommand() == U_FLASH) ? "sketch" : "filesystem";
        // NOTE: if updating FS this would be the place to unmount FS using FS.end()
        println("🔧Updating " + type);
    });

    ArduinoOTA.onEnd([this]() {
        println("🔧Done Updating");
    });

    ArduinoOTA.onProgress([this](unsigned int progress, unsigned int total) {
        printf("🔧 Update Progress: %u%%\r", (progress / (total / 100)));
    });

    ArduinoOTA.onError([this](ota_error_t error) {
        printf("🔧❌Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR) println(">>Auth Failed");
        else if (error == OTA_BEGIN_ERROR) println(">>Begin Failed");
        else if (error == OTA_CONNECT_ERROR) println(">>Connect Failed");
        else if (error == OTA_RECEIVE_ERROR) println(">>Receive Failed");
        else if (error == OTA_END_ERROR) println(">>End Failed");
    });

    ArduinoOTA.begin();
    Serial.println("✅OTA ready");
}


void WebAccess::Setup(String deviceName, String devicePassword) {
    this->functionNames = functionNames;
    this->functions = functions;
    this->functionNameCount = functionNameCount;
    setupWifi(); //3 is function name count
    setupOTA(deviceName, devicePassword);
    setupWebSerial();
}
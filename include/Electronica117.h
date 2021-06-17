/**
 * Electronica117
 * Edgar Antonio Domínguez Ramírez
 * 2021
 */

#ifndef Electronica117_h
#define Electronica117_h

#include <Arduino.h>
#include <WiFi.h>
#include <SPIFFS.h>
#include <Preferences.h>
#include <DNSServer.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>

class Electronica117 {

  public:   
    Electronica117();
    bool initWiFi();
    void borrarRed();
    void loop();

  protected:
    void initWiFiAP();
    static void initServer();
    static String buscarRedes();
    static void RedesRequest(AsyncWebServerRequest *request);
    static void NotFound(AsyncWebServerRequest *request);
    static void ConectarWiFi(AsyncWebServerRequest *request);   
    bool wifiIsConnected = false;
    long timepoDeConexion;
   
    
};



#endif
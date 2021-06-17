#include "Electronica117.h"

StaticJsonDocument<1024> Redes;
String myJson;
AsyncWebServer server(80);
String SSID="";
String PASS="";
Preferences preferences;
DNSServer dnsServer;
class myHandler : public AsyncWebHandler {
public:
  myHandler() {}
  virtual ~myHandler() {}

  bool canHandle(AsyncWebServerRequest *request){
    return true;
  }
  
  void handleRequest(AsyncWebServerRequest *request) {
      request->send(SPIFFS, "/index.html"); 
    }
};

Electronica117::Electronica117(){
  
}

void Electronica117::loop(){
  if(WiFi.status() != WL_CONNECTED){
    dnsServer.processNextRequest();
  }
}


void Electronica117::initServer(){
  SPIFFS.begin();

  server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");
  server.onNotFound(NotFound);
  
  server.on("/Buscar", HTTP_GET, RedesRequest);
  server.on("/ConectarWiFi", HTTP_GET, ConectarWiFi);
  
  dnsServer.start(53, "*", WiFi.softAPIP());
  server.addHandler(new myHandler()).setFilter(ON_AP_FILTER);

	server.begin();
  Serial.println("Serveidor iniciado");
}

void Electronica117::ConectarWiFi(AsyncWebServerRequest *request){
 
  if(request->hasParam("SSID")){
    SSID = request->arg("SSID");
  }

  if(request->hasParam("PASS")){
    PASS = request->arg("PASS");
  }

  SSID.trim();
  PASS.trim();
  
  Serial.print("SSID: "); 
  Serial.println(SSID);
  Serial.print("PASS: ");
  Serial.println(PASS);

  preferences.begin("Red", false);
  preferences.putString("SSID", SSID);
  preferences.putString("PASS", PASS);
  preferences.end();
  
  request->send(SPIFFS, "/index.html"); 

  ESP.restart();
}

void Electronica117::NotFound(AsyncWebServerRequest *request){
  request->send(SPIFFS, "indexNotFound.html");
}

void Electronica117::RedesRequest(AsyncWebServerRequest *request){
 Serial.println("Peticion Redes");
 request->send(200, "text/plain", buscarRedes().c_str());
}

void Electronica117::borrarRed(){
  Serial.println();
  Serial.println("Red " + SSID + " borrada");
  preferences.begin("Red", false);
  preferences.clear();
  preferences.end();
}

void Electronica117::initWiFiAP(){
  WiFi.mode(WIFI_AP);
  while(!WiFi.softAP("Electroncia117")){
    Serial.println(".");
    delay(100);  
  }
  Serial.print("Direccion IP AP: ");
  Serial.println(WiFi.softAPIP());
  initServer();
}

bool Electronica117::initWiFi(){
  preferences.begin("Red", false);
  SSID = preferences.getString("SSID", ""); 
  PASS = preferences.getString("PASS", "");
  preferences.end();
  
  if(SSID.equals("") | PASS.equals("")){
    initWiFiAP();
    wifiIsConnected = false;
  }else{
    WiFi.mode(WIFI_STA);      
    // Inicializamos el WiFi con nuestras credenciales.
    Serial.print("Conectando a ");
    Serial.print(SSID);

    WiFi.begin(SSID.c_str(), PASS.c_str());
    int 
    tiempoDeConexion = millis();
    while(WiFi.status() != WL_CONNECTED){     // Se quedata en este bucle hasta que el estado del WiFi sea diferente a desconectado.
      Serial.print(".");
      delay(100);
      if(tiempoDeConexion+10000 < millis()){
        break;
      }
    }

    if(WiFi.status() == WL_CONNECTED){        // Si el estado del WiFi es conectado entra al If
      Serial.println();
      Serial.println();
      Serial.println("Conxion exitosa!!!");
      Serial.println("");
      Serial.print("Tu IP STA: ");
      Serial.println(WiFi.localIP());
      wifiIsConnected = true;
    }

    if(WiFi.status() != WL_CONNECTED){
      Serial.println("");
      Serial.println("No se logro conexion");
      initWiFiAP();
      wifiIsConnected = false;
    }
  }
 return wifiIsConnected;
}

String Electronica117::buscarRedes(){

  WiFi.scanNetworks(true, false, true, 100);
  // Escaneo todavía en progreso: -1
  // El escaneo no se ha activado: -2
  while(WiFi.scanComplete()<0);
  Serial.println();

  int numeroDeRedes = WiFi.scanComplete();
  Serial.print(numeroDeRedes);
  Serial.println(" redes encontradas");
  
  if (numeroDeRedes > 0) {

    for (int i = 0; i < numeroDeRedes; i++) {
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" "); 
      Serial.print(WiFi.RSSI(i));
      Serial.print("dBm ");
      Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?"Abierta":"Cerrada");
    }
    
    JsonArray Red = Redes.createNestedArray("Redes");
    for (int i = 0; i < numeroDeRedes; i++) {
      Red[i]["SSID"] = WiFi.SSID(i);
      Red[i]["RSSI"] = WiFi.RSSI(i);
      Red[i]["Estado"] = (WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?"Abierta":"Cerrada";
    }


    myJson = "";
    serializeJson(Redes, myJson); 
    Serial.println(myJson);

  } else {
    Serial.println("No se encontraron redes :(");
  }

  return myJson;
 
}



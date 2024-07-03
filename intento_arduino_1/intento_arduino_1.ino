/* // dependencias del video
// #include <WiFi.h>
// #include <DHT.h>
// #include <HTTPClient.h>
// #include <ArduinoJson.h>
// #include "time.h"

//dependencias que al parecer funcionan
#include <WiFi.h>
#include <DHT.h>
#include <HTTPClient.h>
//#include <Arduino_JSON.h>
#include <ArduinoJson.h>
#include "time.h"



//definir pines
#define DHTpin 27
#define SOILPIN 32
#define LDRpin 33
#define LEDamarillopin 15 // respuesta positiva
#define LEDverdepin 16    // respuesta negativa
#define SOILPOWER 4

DHT dHt(DHTpin, DHT11);

unsigned long epochtime;
unsigned long dataMillis = 0;

const char* ntpserver = "pool.ntp.org";  // al parecer esto no cambia
const char* servername = "mongodb+srv://gun45780:adso145780@cluster0.feqs5gd.mongodb.net/" //coneccion amongo Atlas

// para que la placa se conecte a la red 
const char* ssid= "Tenda_4638F8" 
const char* password = "m6R2A6Rq"

//web server    no entiendo
StaticJsonDocument<500> doc;



void setup() {

  Serial.begin(115200); // no se que es esto
  dHt.begin();

    pinMode(LEDamarillopin, OUTPUT);
    pinMode(LEDverdepin, OUTPUT);
    pinMode(SOILPOWER, OUTPUT);

    digitalwrite(LEDamarillopin , LOW);
    digitalwrite(LEDverdepin , LOW);
    digitalwrite(SOILPOWER , LOW);
    //coneccion wifi
    Wifi.begin(ssid,password);
      Serial.println("conectandose a la red "+ ssid + " ...");
    while(Wifi.status() != WL_CONNECTED){
      serial.print(".");
      delay(300);
    }
      Serial.println();
      Serial.print("Connected with IP: ");
      Serial.println(Wifi.localIP());
      Serial.println();

      configTime(0,0, ntpserver);
}

void loop() {
  if(millis()- dataMillis > 15000 || dataMillis == 0){
    dataMillis = millis();

    epochTime = getTime();
    Serial.print("epoch time: ");
    Serial.println(epochTime);

    float tempreatura = dHt.readTemperature();
    float humedad = dHt.readHumidity();

    digitalWrite(SOILPOWER, HIGH);
    delay(10);
    float moisture = analogRead(SOILPIN);
    float moisturepercent = 100.00 - ((moisture/4095.00) * 100.00 );

    digitalWrite(SOILPOWER, LOW);

    float ldr =  analogRead(SOILPIN);
    float ldrPercent = (ldr/4095.00) * 100.00;

    Serial.print("temperatura: ");
    Serial.print(String(temperatura));
    Serial.print(" C\n humedad: ");
    Serial.print(String(humedad));
    Serial.print(" \n moisture: ");
    Serial.print(String(moisturepercent));
    Serial.print(" % ");
    Serial.print(" \n ligth: ");
    Serial.print(String(ldrPercent));
    Serial.print(" % ");
    Serial.println("\n ");

    doc["sensors"]["tempreatura"] = temperatura;
    doc["sensors"]["humedad"]     = humedad;
    doc["sensors"]["moisture"]    = moisturepercent;
    doc["sensors"]["ligth"]       = ldrPercent;
    doc["sensors"]["timestamp"]   = epochTime;

    Serial.println("actualizando data...");
    POSTData();
    
  }
}

// extra que no entiendo 
unsigned long getTime(){
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)){
    return(0);
  }
  time(&now);
  return now;
}

void POSTData(){
  if(Wifi.status() == WL_CONNECTED){
    HTTPClient http;

    http.begin(servername);
    http.addHeader("Content-Type", "application/Json");

    String Json;
    serializeJson(doc, Json);

    Serial.println(Json);
    int httpResponseCode = http.POST(Json);
    Serial.println(httpResponseCode);

    if(httpResponseCode == 204){
      Serial.println("data uploaded.");
      digitalWrite(LEDamarillopin, HIGH);
      delay(200);
      digitalWrite(LEDamarillopin, HIGH);
    }else{
      Serial.println("ERROR");
      digitalWrite(LEDverdepin, HIGH);
      delay(200);
      digitalWrite(LEDverdepin, HIGH);
    }


  }
}
 */




///////////////////////////////////////////////////////////
//codigo corregido por chatgpt
///////////////////////////////////////////////////////////


#include <WiFi.h>
#include <DHT.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "time.h"

// Definir pines
#define DHTPIN 27
#define SOILPIN 32
#define LDRPIN 33
#define LEDAMARILLOPIN 15 // respuesta positiva
#define LEDVERDEPIN 16    // respuesta negativa
#define SOILPOWER 4

DHT dht(DHTPIN, DHT11);

unsigned long epochtime;
unsigned long dataMillis = 0;

const char* ntpServer = "pool.ntp.org";
const char* serverName = "mongodb+srv://gun45780:adso145780@cluster0.feqs5gd.mongodb.net/";

// Credenciales WiFi
const char* ssid = "Tenda_4638F8";
const char* password = "m6R2A6Rq";

// Documento JSON
StaticJsonDocument<500> doc;

void setup() {
  Serial.begin(115200); // Inicializar comunicación serial
  dht.begin();

  pinMode(LEDAMARILLOPIN, OUTPUT);
  pinMode(LEDVERDEPIN, OUTPUT);
  pinMode(SOILPOWER, OUTPUT);

  digitalWrite(LEDAMARILLOPIN, LOW);
  digitalWrite(LEDVERDEPIN, LOW);
  digitalWrite(SOILPOWER, LOW);

  // Conexión WiFi
  WiFi.begin(ssid, password);
  Serial.println("Conectándose a la red " + String(ssid) + " ...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Conectado con IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  configTime(0, 0, ntpServer);
}

void loop() {
  if (millis() - dataMillis > 15000 || dataMillis == 0) {
    dataMillis = millis();

    epochtime = getTime();
    Serial.print("Epoch time: ");
    Serial.println(epochtime);

    float temperatura = dht.readTemperature();
    float humedad = dht.readHumidity();

    digitalWrite(SOILPOWER, HIGH);
    delay(10);
    float moisture = analogRead(SOILPIN);
    float moisturePercent = 100.00 - ((moisture / 4095.00) * 100.00);

    digitalWrite(SOILPOWER, LOW);

    float ldr = analogRead(LDRPIN);
    float ldrPercent = (ldr / 4095.00) * 100.00;

    Serial.print("Temperatura: ");
    Serial.print(temperatura);
    Serial.print(" C\nHumedad: ");
    Serial.print(humedad);
    Serial.print(" %\nMoisture: ");
    Serial.print(moisturePercent);
    Serial.print(" %\nLuz: ");
    Serial.print(ldrPercent);
    Serial.print(" %\n");

    doc["sensors"]["temperatura"] = temperatura;
    doc["sensors"]["humedad"] = humedad;
    doc["sensors"]["moisture"] = moisturePercent;
    doc["sensors"]["luz"] = ldrPercent;
    doc["sensors"]["timestamp"] = epochtime;

    Serial.println("Actualizando datos...");
    POSTData();
  }
}

unsigned long getTime() {
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    return 0;
  }
  time(&now);
  return now;
}

void POSTData() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    http.begin(serverName);
    http.addHeader("Content-Type", "application/json");

    String json;
    serializeJson(doc, json);

    Serial.println(json);
    int httpResponseCode = http.POST(json);
    Serial.println(httpResponseCode);

    if (httpResponseCode == 204) {
      Serial.println("Datos subidos.");
      digitalWrite(LEDAMARILLOPIN, HIGH);
      delay(200);
      digitalWrite(LEDAMARILLOPIN, LOW);
    } else {
      Serial.println("ERROR");
      digitalWrite(LEDVERDEPIN, HIGH);
      delay(200);
      digitalWrite(LEDVERDEPIN, LOW);
    }
    http.end();
  }
}


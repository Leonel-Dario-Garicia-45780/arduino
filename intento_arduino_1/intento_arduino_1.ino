
#include <WiFi.h>
#include <WebServer.h>

// Credenciales WiFi
const char* ssid = "Tenda_4638F8";
const char* password = "m6R2A6Rq";

WebServer server(80);
const int ledMaquina = 2;

void inicio(){
  if( server.hasArg("ledmaquina") && server.arg("ledmaquina") == "on" ){
    digitalWrite(ledMaquina, HIGH);
  }
  else if ( server.hasArg("ledmaquina") && server.arg("ledmaquina") == "off" ){
    digitalWrite(ledMaquina, LOW);

  }

  // construir la pagina html 
  String html = "<html><body>";
  html += "<head> <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">  \n";  
  html += "<title>Document</title>  \n";  
  html += "</head>  \n";  
  html += "<h1>control de ledmaquina</h1> \n";  
  html += "<form  method=\"get\">   \n";  
  html += "<button type=\"submit\" name=\"ledmaquina\" value=\"on\">encender maquina</button> \n";  
  html += "<button type=\"submit\" name=\"ledmaquina\" value=\"off\">apagar maquina</button> \n";  
  html += "</form> \n";  
  html += " </body></html> ";  
//  html += " \n";  

  //enviar la pagina html al cliente (servidor creo)
  server.send(200, "text/html", html);
}

void setup(){
  pinMode(ledMaquina, OUTPUT);

  Serial.begin(115200);
  Serial.println("conectando ...");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid,password);
  Serial.println("");

  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Conectado a la red ");
  Serial.println(ssid);
  Serial.print("Conectado con IP: ");
  Serial.println(WiFi.localIP());  
  
  //rutas del servidor 
  server.on("/", inicio );  // como en django traemos la funcion a la ruta, en este caso inicio
  // iniciar el servidor web
  server.begin();
}


void loop(){
  // manejar lñas solicitudes del servidor web
  server.handleClient();
}


/* 
  #include <WiFi.h>
  #include <DHTesp.h>
  #include <HTTPClient.h>
  #include <ArduinoJson.h>
  #include "time.h"

  // Definir pines
  #define DHTPIN 15             // Pin donde está conectado el sensor DHT11
  #define LEDAMARILLOPIN 16     // LED amarillo
  #define LEDVERDEPIN 17        // LED verde

  DHTesp dht;

  unsigned long epochtime;
  unsigned long dataMillis = 0;

  const char* ntpServer = "pool.ntp.org";
  const char* serverName = "https://us-east-1.aws.data.mongodb-api.com/app/data-ndbugol/endpoint/data/v1/action/insertOne";  // URL de la API de MongoDB Data

  // Credenciales WiFi
  const char* ssid = "Tenda_4638F8";
  const char* password = "m6R2A6Rq";

  // Nombre de la base de datos y colección en MongoDB Atlas
  const char* databaseName = "datos_de_arduino";
  const char* collectionName = "datos";

  // Documento JSON
  StaticJsonDocument<500> doc;

  void setup() {
    Serial.begin(115200); // Inicializar comunicación serial
    dht.setup(DHTPIN, DHTesp::DHT11);

    pinMode(LEDAMARILLOPIN, OUTPUT);
    pinMode(LEDVERDEPIN, OUTPUT);

    digitalWrite(LEDAMARILLOPIN, LOW);
    digitalWrite(LEDVERDEPIN, LOW);

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

      TempAndHumidity data = dht.getTempAndHumidity();

      Serial.print("Temperatura: ");
      Serial.print(data.temperature);
      Serial.print(" °C\nHumedad: ");
      Serial.print(data.humidity);
      Serial.println(" %");

      // Redondear temperatura a dos decimales
      float temperaturaRedondeada = round(data.temperature * 100.0) / 100.0;

      doc.clear();
      JsonObject docObject = doc.to<JsonObject>();
      docObject["temperatura"] = temperaturaRedondeada;
      docObject["humedad"] = data.humidity;
      docObject["timestamp"] = epochtime;

      StaticJsonDocument<600> payload;
      payload["dataSource"] = "Cluster0";
      payload["database"] = databaseName;
      payload["collection"] = collectionName;
      payload["document"] = docObject;

      Serial.println("Actualizando datos...");
      POSTData(payload);
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

  void POSTData(StaticJsonDocument<600>& payload) {
    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;

      http.begin(serverName);
      http.addHeader("Content-Type", "application/json");
      http.addHeader("api-key", "WD0PmvqccnHxPYc4YEsOK3hryZAN6fca4glv0XLQTaMeAZID4Yh4zGZQNpiXEdsz");

      // Construir el JSON con los datos
      String json;
      serializeJson(payload, json);

      Serial.print("JSON a enviar: ");
      Serial.println(json);

      int httpResponseCode = http.POST(json);
      Serial.print("Código de respuesta HTTP: ");
      Serial.println(httpResponseCode);

      // Leer la respuesta del servidor para obtener más detalles
      String response = http.getString();
      Serial.print("Respuesta del servidor: ");
      Serial.println(response);

      if (httpResponseCode == 201) { // 201 significa que se creó el recurso (documento)
        Serial.println("Datos subidos correctamente.");
        digitalWrite(LEDAMARILLOPIN, HIGH);
        delay(200);
        digitalWrite(LEDAMARILLOPIN, LOW);
      } else {
        Serial.print("Error al subir los datos. Código de respuesta: ");
        Serial.println(httpResponseCode);
        digitalWrite(LEDVERDEPIN, HIGH);
        delay(200);
        digitalWrite(LEDVERDEPIN, LOW);
      }

      http.end();
    }
  }


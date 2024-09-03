// CONSUMO DE APO DEL CLIMA (para tener el dato de temperatura ambiente)
// testar


#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "Tenda_4638F8";
const char* password = "m6R2A6Rq";

// Coordenadas de Popayán, Colombia
// const float latitude = 2.43823;
// const float longitude = -76.61316;
const float latitude = 2.446635;
const float longitude = -76.632958;

const char* apiKey = "c373112c8c37e3facd9be6fbeeb8f2cd";

// Construir la URL completa con las coordenadas
String url = "https://api.openweathermap.org/data/2.5/weather?lat=" + String(latitude, 5) + "&lon=" + String(longitude, 5) + "&appid=" + String(apiKey) + "&units=metric";

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  Serial.print("Conectando a WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" Conectado!");

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    http.begin(url);  // Inicia la conexión HTTP
    int httpCode = http.GET();  // Realiza la solicitud GET

    if (httpCode > 0) {  // Verifica si la solicitud fue exitosa
      String payload = http.getString();  // Obtiene la respuesta en formato String

      // Procesar la respuesta JSON
      StaticJsonDocument<1024> doc;
      DeserializationError error = deserializeJson(doc, payload);

      if (!error) {
        float temperature = doc["main"]["temp"];
        float humidity = doc["main"]["humidity"];
        const char* weatherDescription = doc["weather"][0]["description"];

        Serial.println("Temperatura: " + String(temperature) + "°C");
        Serial.println("Humedad: " + String(humidity) + "%");
        Serial.println("Clima: " + String(weatherDescription));
      } else {
        Serial.println("Error al analizar el JSON");
      }
    } else {
      Serial.println("Error en la solicitud HTTP: " + String(httpCode));
    }

    http.end();  // Finaliza la conexión
  }
}

void loop() {
  // Puedes repetir la solicitud o realizar otras tareas
}




// =============================================================

// comunicacion con el backedn
// testeado no funciono

// #include <WiFi.h>
// #include <WebServer.h>
// #include <DHTesp.h>
// #include <ArduinoJson.h>
// #include <HTTPClient.h>
// #include "time.h"

// // Credenciales WiFi
// // const char* ssid = "Tenda_4638F8";
// // const char* password = "m6R2A6Rq";
// const char* ssid = "Dpaola_García.";
// const char* password = "76543210";

// WebServer server(80);

// // Definir pines
// const int ledMaquina = 2;
// const int DHTPIN = 15;             // Pin donde está conectado el sensor DHT11

// DHTesp dht;

// unsigned long epochtime;
// unsigned long dataMillis = 0;

// const char* ntpServer = "pool.ntp.org";
// const char* serverName = "https://us-east-1.aws.data.mongodb-api.com/app/data-ndbugol/endpoint/data/v1/action/insertOne";  // URL de la API de MongoDB Data

// // Nombre de la base de datos y colección en MongoDB Atlas
// const char* databaseName = "datos_de_arduino";
// const char* collectionName = "datos";

// // Documento JSON
// StaticJsonDocument<500> doc;

// // Variable para almacenar la hora de apagado
// int horaApagado = -1;  // Inicialmente no hay hora de apagado

// void inicio() {
//   if (server.method() == HTTP_OPTIONS) {
//     server.sendHeader("Access-Control-Allow-Origin", "*");
//     server.sendHeader("Access-Control-Max-Age", "10000");
//     server.sendHeader("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
//     server.sendHeader("Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept, Authorization");
//     server.send(204);
//   }
//   else if (server.method() == HTTP_POST) {
//     if (server.hasArg("plain")) {
//       String body = server.arg("plain");
//       StaticJsonDocument<200> doc;
//       deserializeJson(doc, body);
//       const char* command = doc["command"];
//       const char* seguimiento_id = doc["segumiento_id"];

//       if (strcmp(command, "on") == 0) {
//         digitalWrite(ledMaquina, HIGH);
//       } else if (strcmp(command, "off") == 0) {
//         digitalWrite(ledMaquina, LOW);
//       }else if (strcmp(command, "off_at") == 0) {
//         int hour = doc["hour"];
//         int minute = doc["minute"];
//         horaApagado = hour;  // Programar apagado a la hora especificada
//         Serial.print("Hora de apagado establecida a: ");
//         Serial.println(horaApagado);
//       }

//       server.sendHeader("Access-Control-Allow-Origin", "*");
//       server.send(200, "application/json", "{\"status\":\"success\"}");
//     } else {
//       server.send(400, "application/json", "{\"status\":\"error\", \"message\":\"No command provided\"}");
//     }
//   }
// }

// void setup() {
//   pinMode(ledMaquina, OUTPUT);

//   Serial.begin(115200);
//   Serial.println("Conectando al WiFi...");
//   Serial.println(ssid);

//   WiFi.mode(WIFI_STA);
//   WiFi.begin(ssid, password);
//   Serial.println("");

//   while (WiFi.status() != WL_CONNECTED) {
//     delay(500);
//     Serial.print(".");
//   }

//   Serial.println("");
//   Serial.print("Conectado a la red ");
//   Serial.println(ssid);
//   Serial.print("Conectado con IP: ");
//   Serial.println(WiFi.localIP());

//   // Rutas del servidor 
//   server.on("/control", HTTP_OPTIONS, inicio);  // Preflight OPTIONS request
//   server.on("/control", HTTP_POST, inicio);     // Handle POST requests
//   server.begin();

//   dht.setup(DHTPIN, DHTesp::DHT11);
//   configTime(0, 0, ntpServer);
// }

// void loop() {
//   // Manejar las solicitudes del servidor web
//   server.handleClient();

//   // Obtener la hora actual
//   epochtime = getTime();
//   struct tm* timeinfo = localtime(&epochtime);

//   // Verificar si la hora actual coincide con la hora de apagado
// if (horaApagado != -1 && timeinfo->tm_hour == horaApagado && timeinfo->tm_min == minute) {
//     digitalWrite(ledMaquina, LOW);  // Apagar la máquina
//     horaApagado = -1;  // Restablecer la hora de apagado
//     Serial.println("Máquina apagada a la hora especificada.");
//   }

//   // Verificar si el LED está encendido
//   if (digitalRead(ledMaquina) == HIGH) {
//     // Lógica para enviar datos solo cuando el LED está encendido
//     if (millis() - dataMillis > 15000 || dataMillis == 0) {
//       dataMillis = millis();

//       epochtime = getTime();
//       Serial.print("Epoch time: ");
//       Serial.println(epochtime);

//       TempAndHumidity data = dht.getTempAndHumidity();

//       Serial.print("Temperatura: ");
//       Serial.print(data.temperature);
//       Serial.print(" °C\nHumedad: ");
//       Serial.print(data.humidity);
//       Serial.println(" %");

//       // Redondear temperatura a dos decimales
//       float temperaturaRedondeada = round(data.temperature * 100.0) / 100.0;

//       doc.clear();
//       JsonObject docObject = doc.to<JsonObject>();
//       docObject["temperatura"] = temperaturaRedondeada;
//       docObject["humedad"] = data.humidity;
//       docObject["timestamp"] = epochtime;
      

//       StaticJsonDocument<600> payload;
//       payload["dataSource"] = "Cluster0";
//       payload["database"] = databaseName;
//       payload["collection"] = collectionName;
//       payload["document"] = docObject;

//       Serial.println("Actualizando datos...");
//       POSTData(payload);
//     }
//   }
// }

// unsigned long getTime() {
//   time_t now;
//   struct tm timeinfo;
//   if (!getLocalTime(&timeinfo)) {
//     return 0;
//   }
//   time(&now);
//   return now;
// }

// void POSTData(StaticJsonDocument<600>& payload) {
//   if (WiFi.status() == WL_CONNECTED) {
//     HTTPClient http;

//     http.begin(serverName);
//     http.addHeader("Content-Type", "application/json");
//     http.addHeader("api-key", "WD0PmvqccnHxPYc4YEsOK3hryZAN6fca4glv0XLQTaMeAZID4Yh4zGZQNpiXEdsz");

//     // Construir el JSON con los datos
//     String json;
//     serializeJson(payload, json);

//     Serial.print("JSON a enviar: ");
//     Serial.println(json);

//     int httpResponseCode = http.POST(json);
//     Serial.print("Código de respuesta HTTP: ");
//     Serial.println(httpResponseCode);

//     // Leer la respuesta del servidor para obtener más detalles
//     String response = http.getString();
//     Serial.print("Respuesta del servidor: ");
//     Serial.println(response);

//     if (httpResponseCode == 201) { // 201 significa que se creó el recurso (documento)
//       Serial.println("Datos subidos correctamente.");
//       // Agregar aquí la lógica para indicar éxito en la operación, por ejemplo, encender un LED amarillo
//     } else {
//       Serial.print("Error al subir los datos. Código de respuesta: ");
//       Serial.println(httpResponseCode);
//       // Agregar aquí la lógica para indicar error en la operación, por ejemplo, encender un LED verde
//     }

//     http.end();
//   }
// }








// // probar

// #include <WiFi.h>
// #include <WebServer.h>
// #include <DHTesp.h>
// #include <ArduinoJson.h>
// #include <HTTPClient.h>
// #include "time.h"

// // Credenciales WiFi
// const char* ssid = "Tenda_4638F8";
// const char* password = "m6R2A6Rq";

// WebServer server(80);

// // Definir pines
// const int ledMaquina = 2;
// const int DHTPIN = 15;             // Pin donde está conectado el sensor DHT11

// DHTesp dht;

// unsigned long epochtime;
// unsigned long dataMillis = 0;

// const char* ntpServer = "pool.ntp.org";
// const char* serverName = "https://us-east-1.aws.data.mongodb-api.com/app/data-ndbugol/endpoint/data/v1/action/insertOne";  // URL de la API de MongoDB Data

// // Nombre de la base de datos y colección en MongoDB Atlas
// const char* databaseName = "datos_de_arduino";
// const char* collectionName = "datos";

// // Documento JSON
// StaticJsonDocument<500> doc;

// // Variable para almacenar la hora de apagado
// int horaApagado = -1;  // Inicialmente no hay hora de apagado

// void inicio() {
//   if (server.method() == HTTP_OPTIONS) {
//     server.sendHeader("Access-Control-Allow-Origin", "*");
//     server.sendHeader("Access-Control-Max-Age", "10000");
//     server.sendHeader("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
//     server.sendHeader("Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept, Authorization");
//     server.send(204);
//   }
//   else if (server.method() == HTTP_POST) {
//     if (server.hasArg("plain")) {
//       String body = server.arg("plain");
//       StaticJsonDocument<200> doc;
//       deserializeJson(doc, body);
//       const char* command = doc["command"];

//       if (strcmp(command, "on") == 0) {
//         digitalWrite(ledMaquina, HIGH);
//       } else if (strcmp(command, "off") == 0) {
//         digitalWrite(ledMaquina, LOW);
//       }

//       // Recibir la hora de apagado desde el frontend
//       if (doc.containsKey("horaApagado")) {
//         horaApagado = doc["horaApagado"];
//         Serial.print("Hora de apagado establecida a: ");
//         Serial.println(horaApagado);
//       }

//       server.sendHeader("Access-Control-Allow-Origin", "*");
//       server.send(200, "application/json", "{\"status\":\"success\"}");
//     } else {
//       server.send(400, "application/json", "{\"status\":\"error\", \"message\":\"No command provided\"}");
//     }
//   }
// }

// void setup() {
//   pinMode(ledMaquina, OUTPUT);

//   Serial.begin(115200);
//   Serial.println("Conectando al WiFi...");
//   Serial.println(ssid);

//   WiFi.mode(WIFI_STA);
//   WiFi.begin(ssid, password);
//   Serial.println("");

//   while (WiFi.status() != WL_CONNECTED) {
//     delay(500);
//     Serial.print(".");
//   }

//   Serial.println("");
//   Serial.print("Conectado a la red ");
//   Serial.println(ssid);
//   Serial.print("Conectado con IP: ");
//   Serial.println(WiFi.localIP());

//   // Rutas del servidor 
//   server.on("/control", HTTP_OPTIONS, inicio);  // Preflight OPTIONS request
//   server.on("/control", HTTP_POST, inicio);     // Handle POST requests
//   server.begin();

//   dht.setup(DHTPIN, DHTesp::DHT11);
//   configTime(0, 0, ntpServer);
// }

// void loop() {
//   // Manejar las solicitudes del servidor web
//   server.handleClient();

//   // Obtener la hora actual
//   epochtime = getTime();
//   struct tm* timeinfo = localtime(&epochtime);

//   // Verificar si la hora actual coincide con la hora de apagado
//   if (horaApagado != -1 && timeinfo->tm_hour == horaApagado) {
//     digitalWrite(ledMaquina, LOW);  // Apagar la máquina
//     horaApagado = -1;  // Restablecer la hora de apagado
//     Serial.println("Máquina apagada a la hora especificada.");
//   }

//   // Verificar si el LED está encendido
//   if (digitalRead(ledMaquina) == HIGH) {
//     // Lógica para enviar datos solo cuando el LED está encendido
//     if (millis() - dataMillis > 15000 || dataMillis == 0) {
//       dataMillis = millis();

//       epochtime = getTime();
//       Serial.print("Epoch time: ");
//       Serial.println(epochtime);

//       TempAndHumidity data = dht.getTempAndHumidity();

//       Serial.print("Temperatura: ");
//       Serial.print(data.temperature);
//       Serial.print(" °C\nHumedad: ");
//       Serial.print(data.humidity);
//       Serial.println(" %");

//       // Redondear temperatura a dos decimales
//       float temperaturaRedondeada = round(data.temperature * 100.0) / 100.0;

//       doc.clear();
//       JsonObject docObject = doc.to<JsonObject>();
//       docObject["temperatura"] = temperaturaRedondeada;
//       docObject["humedad"] = data.humidity;
//       docObject["timestamp"] = epochtime;

//       StaticJsonDocument<600> payload;
//       payload["dataSource"] = "Cluster0";
//       payload["database"] = databaseName;
//       payload["collection"] = collectionName;
//       payload["document"] = docObject;

//       Serial.println("Actualizando datos...");
//       POSTData(payload);
//     }
//   }
// }

// unsigned long getTime() {
//   time_t now;
//   struct tm timeinfo;
//   if (!getLocalTime(&timeinfo)) {
//     return 0;
//   }
//   time(&now);
//   return now;
// }

// void POSTData(StaticJsonDocument<600>& payload) {
//   if (WiFi.status() == WL_CONNECTED) {
//     HTTPClient http;

//     http.begin(serverName);
//     http.addHeader("Content-Type", "application/json");
//     http.addHeader("api-key", "WD0PmvqccnHxPYc4YEsOK3hryZAN6fca4glv0XLQTaMeAZID4Yh4zGZQNpiXEdsz");

//     // Construir el JSON con los datos
//     String json;
//     serializeJson(payload, json);

//     Serial.print("JSON a enviar: ");
//     Serial.println(json);

//     int httpResponseCode = http.POST(json);
//     Serial.print("Código de respuesta HTTP: ");
//     Serial.println(httpResponseCode);

//     // Leer la respuesta del servidor para obtener más detalles
//     String response = http.getString();
//     Serial.print("Respuesta del servidor: ");
//     Serial.println(response);

//     if (httpResponseCode == 201) { // 201 significa que se creó el recurso (documento)
//       Serial.println("Datos subidos correctamente.");
//       // Agregar aquí la lógica para indicar éxito en la operación, por ejemplo, encender un LED amarillo
//     } else {
//       Serial.print("Error al subir los datos. Código de respuesta: ");
//       Serial.println(httpResponseCode);
//       // Agregar aquí la lógica para indicar error en la operación, por ejemplo, encender un LED verde
//     }

//     http.end();
//   }
// }



//===================================





// #include <WiFi.h>
// #include <WebServer.h>
// #include <DHTesp.h>
// #include <ArduinoJson.h>
// #include <HTTPClient.h>
// #include "time.h"

// // Credenciales WiFi
// const char* ssid = "Tenda_4638F8";
// const char* password = "m6R2A6Rq";

// WebServer server(80);

// // Definir pines
// const int ledMaquina = 2;
// const int DHTPIN = 15;             // Pin donde está conectado el sensor DHT11

// DHTesp dht;

// unsigned long epochtime;
// unsigned long dataMillis = 0;

// const char* ntpServer = "pool.ntp.org";
// const char* serverName = "https://us-east-1.aws.data.mongodb-api.com/app/data-ndbugol/endpoint/data/v1/action/insertOne";  // URL de la API de MongoDB Data

// // Nombre de la base de datos y colección en MongoDB Atlas
// const char* databaseName = "datos_de_arduino";
// const char* collectionName = "datos";

// // Documento JSON
// StaticJsonDocument<500> doc;

// void inicio() {
//   if (server.method() == HTTP_OPTIONS) {
//     server.sendHeader("Access-Control-Allow-Origin", "*");
//     server.sendHeader("Access-Control-Max-Age", "10000");
//     server.sendHeader("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
//     server.sendHeader("Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept, Authorization");
//     server.send(204);
//   }
//   else if (server.method() == HTTP_POST) {
//     if (server.hasArg("plain")) {
//       String body = server.arg("plain");
//       StaticJsonDocument<200> doc;
//       deserializeJson(doc, body);
//       const char* command = doc["command"];

//       if (strcmp(command, "on") == 0) {
//         digitalWrite(ledMaquina, HIGH);
//       } else if (strcmp(command, "off") == 0) {
//         digitalWrite(ledMaquina, LOW);
//       }

//       server.sendHeader("Access-Control-Allow-Origin", "*");
//       server.send(200, "application/json", "{\"status\":\"success\"}");
//     } else {
//       server.send(400, "application/json", "{\"status\":\"error\", \"message\":\"No command provided\"}");
//     }
//   }
// }

// void setup() {
//   pinMode(ledMaquina, OUTPUT);

//   Serial.begin(115200);
//   Serial.println("Conectando al WiFi...");
//   Serial.println(ssid);

//   WiFi.mode(WIFI_STA);
//   WiFi.begin(ssid, password);
//   Serial.println("");

//   while (WiFi.status() != WL_CONNECTED) {
//     delay(500);
//     Serial.print(".");
//   }

//   Serial.println("");
//   Serial.print("Conectado a la red ");
//   Serial.println(ssid);
//   Serial.print("Conectado con IP: ");
//   Serial.println(WiFi.localIP());

//   // Rutas del servidor 
//   server.on("/control", HTTP_OPTIONS, inicio);  // Preflight OPTIONS request
//   server.on("/control", HTTP_POST, inicio);     // Handle POST requests
//   server.begin();

//   dht.setup(DHTPIN, DHTesp::DHT11);
//   configTime(0, 0, ntpServer);
// }

// void loop() {
//   // Manejar las solicitudes del servidor web
//   server.handleClient();

//   // Verificar si el LED está encendido
//   if (digitalRead(ledMaquina) == HIGH) {
//     // Lógica para enviar datos solo cuando el LED está encendido
//     if (millis() - dataMillis > 15000 || dataMillis == 0) {
//       dataMillis = millis();

//       epochtime = getTime();
//       Serial.print("Epoch time: ");
//       Serial.println(epochtime);

//       TempAndHumidity data = dht.getTempAndHumidity();

//       Serial.print("Temperatura: ");
//       Serial.print(data.temperature);
//       Serial.print(" °C\nHumedad: ");
//       Serial.print(data.humidity);
//       Serial.println(" %");

//       // Redondear temperatura a dos decimales
//       float temperaturaRedondeada = round(data.temperature * 100.0) / 100.0;

//       doc.clear();
//       JsonObject docObject = doc.to<JsonObject>();
//       docObject["temperatura"] = temperaturaRedondeada;
//       docObject["humedad"] = data.humidity;
//       docObject["timestamp"] = epochtime;

//       StaticJsonDocument<600> payload;
//       payload["dataSource"] = "Cluster0";
//       payload["database"] = databaseName;
//       payload["collection"] = collectionName;
//       payload["document"] = docObject;

//       Serial.println("Actualizando datos...");
//       POSTData(payload);
//     }
//   }
// }

// unsigned long getTime() {
//   time_t now;
//   struct tm timeinfo;
//   if (!getLocalTime(&timeinfo)) {
//     return 0;
//   }
//   time(&now);
//   return now;
// }

// void POSTData(StaticJsonDocument<600>& payload) {
//   if (WiFi.status() == WL_CONNECTED) {
//     HTTPClient http;

//     http.begin(serverName);
//     http.addHeader("Content-Type", "application/json");
//     http.addHeader("api-key", "WD0PmvqccnHxPYc4YEsOK3hryZAN6fca4glv0XLQTaMeAZID4Yh4zGZQNpiXEdsz");

//     // Construir el JSON con los datos
//     String json;
//     serializeJson(payload, json);

//     Serial.print("JSON a enviar: ");
//     Serial.println(json);

//     int httpResponseCode = http.POST(json);
//     Serial.print("Código de respuesta HTTP: ");
//     Serial.println(httpResponseCode);

//     // Leer la respuesta del servidor para obtener más detalles
//     String response = http.getString();
//     Serial.print("Respuesta del servidor: ");
//     Serial.println(response);

//     if (httpResponseCode == 201) { // 201 significa que se creó el recurso (documento)
//       Serial.println("Datos subidos correctamente.");
//       // Agregar aquí la lógica para indicar éxito en la operación, por ejemplo, encender un LED amarillo
//     } else {
//       Serial.print("Error al subir los datos. Código de respuesta: ");
//       Serial.println(httpResponseCode);
//       // Agregar aquí la lógica para indicar error en la operación, por ejemplo, encender un LED verde
//     }

//     http.end();
//   }
// }


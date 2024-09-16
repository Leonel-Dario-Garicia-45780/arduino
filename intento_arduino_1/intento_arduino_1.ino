// funcionalidades con seguimiento
// testeando

// prueva con grimoire
// #include <WiFi.h>
// #include <WebServer.h>
// #include <DHTesp.h>
// #include <ArduinoJson.h>
// #include <HTTPClient.h>
// #include "time.h"

// // Credenciales WiFi
// const char* ssid = "BUSCANDO RED 2";
// const char* password = "Rafael1061773978";

// WebServer server(80);

// // Definir pines
// const int ledMaquina = 2;
// const int DHTPIN = 15;             // Pin donde está conectado el sensor DHT11
// DHTesp dht;

// unsigned long lastDataMillis = 0;    // Última vez que se enviaron datos
// unsigned long lastStateMillis = 0;   // Última vez que se verificó el estado de la máquina

// const char* ntpServer = "pool.ntp.org";
// const char* serverNameData = "https://us-east-1.aws.data.mongodb-api.com/app/data-ndbugol/endpoint/data/v1/action/insertOne";  // URL de la API de MongoDB Data
// const char* serverNameStatus = "https://bakend-arduino.onrender.com/api/estado";  // URL del backend para obtener el estado de la máquina

// // Nueva URL completa para obtener el seguimiento_id
// const String backendSeguimientoURL = "https://proyecto-sena-backend-s666.onrender.com/api/seguimiento/maquina/66def6b4380c4b694df72013";

// // Nombre de la base de datos y colección en MongoDB Atlas
// const char* databaseName = "datos_de_arduino";
// const char* collectionName = "datos";

// // Documento JSON
// StaticJsonDocument<500> doc;

// // Variables para API del clima
// const float latitude = 2.446635;
// const float longitude = -76.632958;
// const char* apiKey = "c373112c8c37e3facd9be6fbeeb8f2cd";
// float tempAmbiente = 0.0;  // Variable para almacenar la temperatura ambiente

// // Variable para el seguimiento_id
// String seguimientoID = "";  // Variable para almacenar el seguimiento_id

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

//   dht.setup(DHTPIN, DHTesp::DHT11);
//   configTime(0, 0, ntpServer);
// }

// void loop() {
//   unsigned long currentMillis = millis();

//   // Verificar estado de la máquina cada 5 segundos
//   if (currentMillis - lastStateMillis >= 5000) {
//     lastStateMillis = currentMillis;
//     checkMachineState();
//   }

//   // Enviar datos cada 15 segundos si el LED está encendido
//   if (digitalRead(ledMaquina) == HIGH) {
//     if (currentMillis - lastDataMillis >= 15000 || lastDataMillis == 0) {
//       lastDataMillis = currentMillis;

//       // Obtener el seguimiento_id desde la nueva URL
//       getSeguimientoID();

//       // Obtener la temperatura ambiente desde la API del clima
//       getClimaAPI();

//       unsigned long epochtime = getTime();
//       Serial.print("Epoch time: ");
//       Serial.println(epochtime);

//       TempAndHumidity data = dht.getTempAndHumidity();

//       Serial.print("Temperatura DHT11: ");
//       Serial.print(data.temperature);
//       Serial.print(" °C\nHumedad: ");
//       Serial.print(data.humidity);
//       Serial.println(" %");

//       // Redondear temperatura a dos decimales
//       float temperaturaRedondeada = round(data.temperature * 100.0) / 100.0;

//       doc.clear();
//       JsonObject docObject = doc.to<JsonObject>();
//       docObject["temperatura"] = temperaturaRedondeada;
//       docObject["temperaturaAmbiente"] = tempAmbiente;  // Agregar temperatura ambiente al JSON
//       docObject["humedad"] = data.humidity;
//       docObject["timestamp"] = epochtime;
//       docObject["seguimiento_id"] = seguimientoID;  // Agregar el seguimiento_id al JSON

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

// // Función para obtener la temperatura ambiente de la API del clima
// void getClimaAPI() {
//   if (WiFi.status() == WL_CONNECTED) {
//     HTTPClient http;
//     String url = "https://api.openweathermap.org/data/2.5/weather?lat=" + String(latitude, 5) + "&lon=" + String(longitude, 5) + "&appid=" + String(apiKey) + "&units=metric";

//     http.begin(url);  // Inicia la conexión HTTP
//     int httpCode = http.GET();  // Realiza la solicitud GET

//     if (httpCode > 0) {  // Verifica si la solicitud fue exitosa
//       String payload = http.getString();  // Obtiene la respuesta en formato String

//       // Procesar la respuesta JSON
//       StaticJsonDocument<1024> doc;
//       DeserializationError error = deserializeJson(doc, payload);

//       if (!error) {
//         tempAmbiente = doc["main"]["temp"];  // Guardar la temperatura ambiente
//         Serial.println("Temperatura ambiente: " + String(tempAmbiente) + "°C");
//       } else {
//         Serial.println("Error al analizar el JSON del clima");
//       }
//     } else {
//       Serial.println("Error en la solicitud HTTP de clima: " + String(httpCode));
//     }

//     http.end();  // Finaliza la conexión
//   }
// }

// // Función para obtener el seguimiento_id del backend desde la nueva URL
// void getSeguimientoID() {
//   if (WiFi.status() == WL_CONNECTED) {
//     HTTPClient http;
//     String url = backendSeguimientoURL;  // Usar la URL fija proporcionada

//     http.begin(url);  // Inicia la conexión HTTP
//     int httpCode = http.GET();  // Realiza la solicitud GET

//     if (httpCode > 0) {  // Verifica si la solicitud fue exitosa
//       String payload = http.getString();  // Obtiene la respuesta en formato String

//       // Procesar la respuesta JSON
//       StaticJsonDocument<1024> doc;
//       DeserializationError error = deserializeJson(doc, payload);

//       if (!error) {
//         seguimientoID = doc["seguimiento_id"].as<String>();  // Guardar el seguimiento_id
//         Serial.println("Seguimiento ID: " + seguimientoID);
//       } else {
//         Serial.println("Error al analizar el JSON de seguimiento");
//       }
//     } else {
//       Serial.println("Error en la solicitud HTTP de seguimiento: " + String(httpCode));
//     }

//     http.end();  // Finaliza la conexión
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

//     http.begin(serverNameData);
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
//       // Agregar aquí la lógica para indicar éxito en la operación
//     } else {
//       Serial.print("Error al subir los datos. Código de respuesta: ");
//       Serial.println(httpResponseCode);
//     }

//     http.end();
//   }
// }

// void checkMachineState() {
//   if (WiFi.status() == WL_CONNECTED) {
//     HTTPClient http;

//     http.begin(serverNameStatus);
//     int httpResponseCode = http.GET();

//     if (httpResponseCode > 0) {
//       String response = http.getString();
//       Serial.println("Estado de la máquina recibido:");
//       Serial.println(response);

//       // Parsear la respuesta JSON
//       DynamicJsonDocument doc(1024);
//       deserializeJson(doc, response);

//       bool maquinaEncendida = doc["maquinaEncendida"];
//       String horaApagado = doc["horaApagado"];

//       // Imprimir los valores recibidos
//       Serial.print("Máquina encendida: ");
//       Serial.println(maquinaEncendida);
//       Serial.print("Hora de apagado: ");
//       Serial.println(horaApagado);

//       // Controlar el LED según el estado recibido
//       if (maquinaEncendida) {
//         digitalWrite(ledMaquina, HIGH);
//       } else {
//         digitalWrite(ledMaquina, LOW);
//       }
//     } else {
//       Serial.print("Error al recibir el estado de la máquina. Código de respuesta: ");
//       Serial.println(httpResponseCode);
//     }

//     http.end();
//   }
// }


// fusion de consumo de api de clima y comunicacion con el backend
// testeado (funciona correctamente)

// #include <WiFi.h>
// #include <WebServer.h>
// #include <DHTesp.h>
// #include <ArduinoJson.h>
// #include <HTTPClient.h>
// #include "time.h"

// // Credenciales WiFi
// const char* ssid = "BUSCANDO RED 2";
// const char* password = "Rafael1061773978";

// WebServer server(80);

// // Definir pines
// const int ledMaquina = 2;
// const int DHTPIN = 15;             // Pin donde está conectado el sensor DHT11
// DHTesp dht;

// unsigned long lastDataMillis = 0;    // Última vez que se enviaron datos
// unsigned long lastStateMillis = 0;   // Última vez que se verificó el estado de la máquina

// const char* ntpServer = "pool.ntp.org";
// const char* serverNameData = "https://us-east-1.aws.data.mongodb-api.com/app/data-ndbugol/endpoint/data/v1/action/insertOne";  // URL de la API de MongoDB Data
// const char* serverNameStatus = "https://bakend-arduino.onrender.com/api/estado";  // URL del backend para obtener el estado de la máquina

// // Nombre de la base de datos y colección en MongoDB Atlas
// const char* databaseName = "datos_de_arduino";
// const char* collectionName = "datos";

// // Documento JSON
// StaticJsonDocument<500> doc;

// // Variables para API del clima
// const float latitude = 2.446635;
// const float longitude = -76.632958;
// const char* apiKey = "c373112c8c37e3facd9be6fbeeb8f2cd";
// float tempAmbiente = 0.0;  // Variable para almacenar la temperatura ambiente

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

//   dht.setup(DHTPIN, DHTesp::DHT11);
//   configTime(0, 0, ntpServer);
// }

// void loop() {
//   unsigned long currentMillis = millis();

//   // Verificar estado de la máquina cada 5 segundos
//   if (currentMillis - lastStateMillis >= 5000) {
//     lastStateMillis = currentMillis;
//     checkMachineState();
//   }

//   // Enviar datos cada 15 segundos si el LED está encendido
//   if (digitalRead(ledMaquina) == HIGH) {
//     if (currentMillis - lastDataMillis >= 15000 || lastDataMillis == 0) {
//       lastDataMillis = currentMillis;

//       // Obtener la temperatura ambiente desde la API del clima
//       getClimaAPI();

//       unsigned long epochtime = getTime();
//       Serial.print("Epoch time: ");
//       Serial.println(epochtime);

//       TempAndHumidity data = dht.getTempAndHumidity();

//       Serial.print("Temperatura DHT11: ");
//       Serial.print(data.temperature);
//       Serial.print(" °C\nHumedad: ");
//       Serial.print(data.humidity);
//       Serial.println(" %");

//       // Redondear temperatura a dos decimales
//       float temperaturaRedondeada = round(data.temperature * 100.0) / 100.0;

//       doc.clear();
//       JsonObject docObject = doc.to<JsonObject>();
//       docObject["temperatura"] = temperaturaRedondeada;
//       docObject["temperaturaAmbiente"] = tempAmbiente;  // Agregar temperatura ambiente al JSON
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

// // Función para obtener la temperatura ambiente de la API del clima
// void getClimaAPI() {
//   if (WiFi.status() == WL_CONNECTED) {
//     HTTPClient http;
//     String url = "https://api.openweathermap.org/data/2.5/weather?lat=" + String(latitude, 5) + "&lon=" + String(longitude, 5) + "&appid=" + String(apiKey) + "&units=metric";

//     http.begin(url);  // Inicia la conexión HTTP
//     int httpCode = http.GET();  // Realiza la solicitud GET

//     if (httpCode > 0) {  // Verifica si la solicitud fue exitosa
//       String payload = http.getString();  // Obtiene la respuesta en formato String

//       // Procesar la respuesta JSON
//       StaticJsonDocument<1024> doc;
//       DeserializationError error = deserializeJson(doc, payload);

//       if (!error) {
//         tempAmbiente = doc["main"]["temp"];  // Guardar la temperatura ambiente
//         Serial.println("Temperatura ambiente: " + String(tempAmbiente) + "°C");
//       } else {
//         Serial.println("Error al analizar el JSON del clima");
//       }
//     } else {
//       Serial.println("Error en la solicitud HTTP de clima: " + String(httpCode));
//     }

//     http.end();  // Finaliza la conexión
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

//     http.begin(serverNameData);
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
//       // Agregar aquí la lógica para indicar éxito en la operación
//     } else {
//       Serial.print("Error al subir los datos. Código de respuesta: ");
//       Serial.println(httpResponseCode);
//     }

//     http.end();
//   }
// }

// void checkMachineState() {
//   if (WiFi.status() == WL_CONNECTED) {
//     HTTPClient http;

//     http.begin(serverNameStatus);
//     int httpResponseCode = http.GET();

//     if (httpResponseCode > 0) {
//       String response = http.getString();
//       Serial.println("Estado de la máquina recibido:");
//       Serial.println(response);

//       // Parsear la respuesta JSON
//       DynamicJsonDocument doc(1024);
//       deserializeJson(doc, response);

//       bool maquinaEncendida = doc["maquinaEncendida"];
//       String horaApagado = doc["horaApagado"];

//       // Imprimir los valores recibidos
//       Serial.print("Máquina encendida: ");
//       Serial.println(maquinaEncendida);
//       Serial.print("Hora de apagado: ");
//       Serial.println(horaApagado);

//       // Controlar el LED según el estado recibido
//       if (maquinaEncendida) {
//         digitalWrite(ledMaquina, HIGH);
//       } else {
//         digitalWrite(ledMaquina, LOW);
//       }
//     } else {
//       Serial.print("Error al recibir el estado de la máquina. Código de respuesta: ");
//       Serial.println(httpResponseCode);
//     }

//     http.end();
//   }
// }


// =============================================================


// intento de comunicacion con el backen, placa hace peticion get al backen para encender y apagar
// testeado (funciona perfectamente)

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

// unsigned long lastDataMillis = 0;    // Última vez que se enviaron datos
// unsigned long lastStateMillis = 0;   // Última vez que se verificó el estado de la máquina

// const char* ntpServer = "pool.ntp.org";
// const char* serverNameData = "https://us-east-1.aws.data.mongodb-api.com/app/data-ndbugol/endpoint/data/v1/action/insertOne";  // URL de la API de MongoDB Data
// const char* serverNameStatus = "https://bakend-arduino.onrender.com/api/estado";  // URL del backend para obtener el estado de la máquina

// // Nombre de la base de datos y colección en MongoDB Atlas
// const char* databaseName = "datos_de_arduino";
// const char* collectionName = "datos";

// // Documento JSON
// StaticJsonDocument<500> doc;

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

//   dht.setup(DHTPIN, DHTesp::DHT11);
//   configTime(0, 0, ntpServer);
// }

// void loop() {
//   unsigned long currentMillis = millis();

//   // Verificar estado de la máquina cada 5 segundos
//   if (currentMillis - lastStateMillis >= 5000) {
//     lastStateMillis = currentMillis;
//     checkMachineState();
//   }

//   // Enviar datos cada 15 segundos si el LED está encendido
//   if (digitalRead(ledMaquina) == HIGH) {
//     if (currentMillis - lastDataMillis >= 15000 || lastDataMillis == 0) {
//       lastDataMillis = currentMillis;

//       unsigned long epochtime = getTime();
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

//     http.begin(serverNameData);
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

// void checkMachineState() {
//   if (WiFi.status() == WL_CONNECTED) {
//     HTTPClient http;

//     http.begin(serverNameStatus);
//     int httpResponseCode = http.GET();

//     if (httpResponseCode > 0) {
//       String response = http.getString();
//       Serial.println("Estado de la máquina recibido:");
//       Serial.println(response);

//       // Parsear la respuesta JSON
//       DynamicJsonDocument doc(1024);
//       deserializeJson(doc, response);

//       bool maquinaEncendida = doc["maquinaEncendida"];
//       String horaApagado = doc["horaApagado"];

//       // Imprimir los valores recibidos
//       Serial.print("Máquina encendida: ");
//       Serial.println(maquinaEncendida);
//       Serial.print("Hora de apagado: ");
//       Serial.println(horaApagado);

//       // Aquí podrías agregar lógica para actuar según el estado de la máquina
//       // Por ejemplo, podrías controlar el LED según el estado recibido
//       if (maquinaEncendida) {
//         digitalWrite(ledMaquina, HIGH);
//       } else {
//         digitalWrite(ledMaquina, LOW);
//       }
//     } else {
//       Serial.print("Error al recibir el estado de la máquina. Código de respuesta: ");
//       Serial.println(httpResponseCode);
//     }

//     http.end();
//   }
// }


// =============================================================


// // CONSUMO DE API DEL CLIMA (para tener el dato de temperatura ambiente)
// // funciona correctamente

// #include <WiFi.h>
// #include <HTTPClient.h>
// #include <ArduinoJson.h>

// const char* ssid = "Tenda_4638F8";
// const char* password = "m6R2A6Rq";

// // Coordenadas de Popayán, Colombia
// // const float latitude = 2.43823;
// // const float longitude = -76.61316;
// const float latitude = 2.446635;
// const float longitude = -76.632958;

// const char* apiKey = "c373112c8c37e3facd9be6fbeeb8f2cd";

// // Construir la URL completa con las coordenadas
// String url = "https://api.openweathermap.org/data/2.5/weather?lat=" + String(latitude, 5) + "&lon=" + String(longitude, 5) + "&appid=" + String(apiKey) + "&units=metric";

// void setup() {
//   Serial.begin(115200);
//   WiFi.begin(ssid, password);

//   Serial.print("Conectando a WiFi...");
//   while (WiFi.status() != WL_CONNECTED) {
//     delay(500);
//     Serial.print(".");
//   }
//   Serial.println(" Conectado!");

//   if (WiFi.status() == WL_CONNECTED) {
//     HTTPClient http;

//     http.begin(url);  // Inicia la conexión HTTP
//     int httpCode = http.GET();  // Realiza la solicitud GET

//     if (httpCode > 0) {  // Verifica si la solicitud fue exitosa
//       String payload = http.getString();  // Obtiene la respuesta en formato String

//       // Procesar la respuesta JSON
//       StaticJsonDocument<1024> doc;
//       DeserializationError error = deserializeJson(doc, payload);

//       if (!error) {
//         float temperature = doc["main"]["temp"];
//         float humidity = doc["main"]["humidity"];
//         const char* weatherDescription = doc["weather"][0]["description"];

//         Serial.println("Temperatura: " + String(temperature) + "°C");
//         Serial.println("Humedad: " + String(humidity) + "%");
//         Serial.println("Clima: " + String(weatherDescription));
//       } else {
//         Serial.println("Error al analizar el JSON");
//       }
//     } else {
//       Serial.println("Error en la solicitud HTTP: " + String(httpCode));
//     }

//     http.end();  // Finaliza la conexión
//   }
// }

// void loop() {
//   // Puedes repetir la solicitud o realizar otras tareas
// }

// =============================================================

// esto funciona

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
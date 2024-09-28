// consumo del estado de la maquina con enpoin de produccion
// testeando

#include <WiFi.h>
#include <WebServer.h>
#include <DHTesp.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include "time.h"

// Credenciales WiFi
const char* ssid = "BUSCANDO RED 2";         
const char* password = "Rafael1061773978";  
// const char* ssid = "maa";
// const char* password = "123456789";

// Definir pines
const int ledMaquina = 2;
const int DHTPIN = 15;             // Pin donde está conectado el sensor DHT11
DHTesp dht;

unsigned long lastDataMillis = 0;    // Última vez que se enviaron datos
unsigned long lastStateMillis = 0;   // Última vez que se verificó el estado de la máquina

const char* ntpServer = "pool.ntp.org";
const char* serverNameData = "https://proyecto-sena-backend-s666.onrender.com/api/datos";  // Nueva URL para POST
const char* serverNameStatus = "https://bakend-arduino.onrender.com/api/estado";  // URL del backend para obtener el estado de la máquina

// Base de la URL del backend para obtener el seguimiento
const String backendSeguimientoBaseURL = "https://proyecto-sena-backend-s666.onrender.com/api/seguimiento/maquina/";
const String idMaquina = "66ba25c376b1aba0f6ef93cc";  // ID de la máquina

// Documento JSON
StaticJsonDocument<500> doc;

// Variables para API del clima
const float latitude = 2.446635;
const float longitude = -76.632958;
const char* apiKey = "c373112c8c37e3facd9be6fbeeb8f2cd";
float tempAmbiente = 0.0;  // Variable para almacenar la temperatura ambiente

// Variables nuevas
bool rotor = true;
bool motor = true;

// Variable para el seguimiento_id
String seguimientoID = "";

// Módulo WiFiManager
void connectToWiFi(const char* ssid, const char* password) {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    Serial.println("Conectando a la red WiFi...");

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("\nConectado a WiFi. IP: " + WiFi.localIP().toString());
}

// Módulo DHTManager
void setupDHT(DHTesp &dht, int pin) {
    dht.setup(pin, DHTesp::DHT11);
}

TempAndHumidity readDHTSensor(DHTesp &dht) {
    return dht.getTempAndHumidity();
}

// Módulo TimeManager
void setupTime(const char* ntpServer) {
    configTime(0, 0, ntpServer);
}

String getISO8601Time() {
    time_t now;
    struct tm timeinfo;
    char buffer[30];
    if (!getLocalTime(&timeinfo)) {
        Serial.println("Error al obtener la hora local."); // Mensaje de error
        return "";  // Devolver una cadena vacía si falla obtener el tiempo
    }
    strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H:%M:%S.000Z", &timeinfo);  // Formato ISO8601
    return String(buffer);
}

// Módulo BackendManager
String getSeguimientoID() {
    HTTPClient http;
    String url = backendSeguimientoBaseURL + idMaquina;

    http.begin(url);
    int httpCode = http.GET();

    if (httpCode > 0) {
        String payload = http.getString();
        StaticJsonDocument<1024> doc;
        deserializeJson(doc, payload);
        seguimientoID = doc["data"]["_id"].as<String>();
        http.end();
        return seguimientoID;
    } else {
        http.end();
        return "";
    }
}

// Nueva función para enviar datos a la API 
void sendDataToBackend(StaticJsonDocument<600>& payload) {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        http.begin(serverNameData);  // Nueva URL sin API Key
        http.addHeader("Content-Type", "application/json");

        String json;
        serializeJson(payload, json);

        Serial.print("JSON a enviar: ");
        Serial.println(json); // Verifica el JSON que se está enviando

        int httpResponseCode = http.POST(json);
        Serial.print("Código de respuesta: ");
        Serial.println(httpResponseCode);

        // Imprimir el mensaje de error o respuesta del backend si el código no es 201
        if (httpResponseCode != 201) {
            String response = http.getString(); // Obtener la respuesta del backend
            Serial.println("Error al subir los datos. Código de error: " + String(httpResponseCode));
            Serial.print("Respuesta del servidor: ");
            Serial.println(response); // Imprimir la respuesta del servidor
        }

        http.end();
    } else {
        Serial.println("No hay conexión WiFi.");
    }
}

// Función para obtener la temperatura ambiente de la API
float getClimaAPI() {
    HTTPClient http;
    String url = "https://api.openweathermap.org/data/2.5/weather?lat=" + String(latitude, 5) + "&lon=" + String(longitude, 5) + "&appid=" + String(apiKey) + "&units=metric";

    http.begin(url);
    int httpCode = http.GET();

    if (httpCode > 0) {
        String payload = http.getString();
        StaticJsonDocument<1024> doc;
        DeserializationError error = deserializeJson(doc, payload);

        if (!error) {
            float temperatura = doc["main"]["temp"];
            Serial.println("Temperatura ambiente: " + String(temperatura) + "°C");
            http.end();
            return temperatura;
        }
    }

    Serial.println("Error al obtener la temperatura del clima.");
    http.end();
    return 0.0;
}

// Función para verificar estado de la máquina
bool getMachineState() {
    HTTPClient http;
    http.begin(serverNameStatus);
    int httpResponseCode = http.GET();

    if (httpResponseCode > 0) {
        String response = http.getString();
        DynamicJsonDocument doc(1024);
        deserializeJson(doc, response);

        bool maquinaEncendida = doc["maquinaEncendida"];
        String horaApagado = doc["horaApagado"];

        Serial.println("Estado de la máquina recibido:");
        Serial.println(response);

        http.end();
        return maquinaEncendida;
    } else {
        Serial.println("Error al obtener el estado de la máquina.");
        http.end();
        return false;
    }
}

// Setup principal
void setup() {
    pinMode(ledMaquina, OUTPUT);

    Serial.begin(115200);
    Serial.println("Iniciando configuración...");

    // Conexión WiFi
    connectToWiFi(ssid, password);

    // Configurar DHT
    setupDHT(dht, DHTPIN);

    // Configurar tiempo NTP
    setupTime(ntpServer);
    
    delay(2000); // Esperar 2 segundos para asegurar que el NTP esté sincronizado
}

// Loop principal
void loop() {
    unsigned long currentMillis = millis();

    // Verificar estado de la máquina
    if (currentMillis - lastStateMillis >= 5000) {
        lastStateMillis = currentMillis;
        bool maquinaEncendida = getMachineState();
        digitalWrite(ledMaquina, maquinaEncendida ? HIGH : LOW);
    }

    // Enviar datos si la máquina está encendida y ha pasado el tiempo adecuado
    if (digitalRead(ledMaquina) == HIGH && (currentMillis - lastDataMillis >= 15000 || lastDataMillis == 0)) {
        lastDataMillis = currentMillis;

        String seguimientoID = getSeguimientoID();  // Obtener seguimiento ID
        float tempAmbiente = getClimaAPI();         // Obtener temperatura ambiente
        TempAndHumidity data = readDHTSensor(dht);  // Leer datos del sensor DHT

        // Obtener la fecha en formato ISO8601
        String fechaISO8601 = getISO8601Time();     

        if (fechaISO8601.isEmpty()) { // Verificar si la fecha está vacía
            Serial.println("No se puede enviar datos. La fecha está vacía.");
            return; // Salir si la fecha no se pudo obtener
        }

        // Crear el payload JSON
        StaticJsonDocument<600> payload;
        payload["IdSeguimiento"] = seguimientoID;  // Cambiado a "IdSeguimiento"
        
        // Convertir las temperaturas a string antes de agregarlas al JSON
        payload["temperaturaAmbiente"] = String(round(tempAmbiente * 100.0) / 100.0); // Convertir a string
        payload["temperaturaSensor"] = String(round(data.temperature * 100.0) / 100.0); // Convertir a string
        
        payload["idMaquina"] = idMaquina;  // Cambiado a "idMaquina"
        payload["rotor"] = rotor;    // Se mantiene en true o false según sea necesario
        payload["motor"] = motor;    // Se mantiene en true o false según sea necesario
        payload["fecha"] = fechaISO8601;

        // Enviar datos al backend
        sendDataToBackend(payload);
    }
}




// =============================================================// =============================================================



// codigo modularizado
// consumiendo endpoint post
// testeando

// #include <WiFi.h>
// #include <WebServer.h>
// #include <DHTesp.h>
// #include <ArduinoJson.h>
// #include <HTTPClient.h>
// #include "time.h"

// // Credenciales WiFi
// const char* ssid = "BUSCANDO RED 2";         
// const char* password = "Rafael1061773978";  
// // const char* ssid = "maa";
// // const char* password = "123456789";

// // Definir pines
// const int ledMaquina = 2;
// const int DHTPIN = 15;             // Pin donde está conectado el sensor DHT11
// DHTesp dht;

// unsigned long lastDataMillis = 0;    // Última vez que se enviaron datos
// unsigned long lastStateMillis = 0;   // Última vez que se verificó el estado de la máquina

// const char* ntpServer = "pool.ntp.org";
// const char* serverNameData = "https://proyecto-sena-backend-s666.onrender.com/api/datos";  // Nueva URL para POST
// const char* serverNameStatus = "https://bakend-arduino.onrender.com/api/estado";  // URL del backend para obtener el estado de la máquina

// // Base de la URL del backend para obtener el seguimiento
// const String backendSeguimientoBaseURL = "https://proyecto-sena-backend-s666.onrender.com/api/seguimiento/maquina/";
// const String idMaquina = "66ba25c376b1aba0f6ef93cc";  // ID de la máquina

// // Documento JSON
// StaticJsonDocument<500> doc;

// // Variables para API del clima
// const float latitude = 2.446635;
// const float longitude = -76.632958;
// const char* apiKey = "c373112c8c37e3facd9be6fbeeb8f2cd";
// float tempAmbiente = 0.0;  // Variable para almacenar la temperatura ambiente

// // Variables nuevas
// bool rotor = true;
// bool motor = true;

// // Variable para el seguimiento_id
// String seguimientoID = "";

// // Módulo WiFiManager
// void connectToWiFi(const char* ssid, const char* password) {
//     WiFi.mode(WIFI_STA);
//     WiFi.begin(ssid, password);
//     Serial.println("Conectando a la red WiFi...");

//     while (WiFi.status() != WL_CONNECTED) {
//         delay(500);
//         Serial.print(".");
//     }

//     Serial.println("\nConectado a WiFi. IP: " + WiFi.localIP().toString());
// }

// // Módulo DHTManager
// void setupDHT(DHTesp &dht, int pin) {
//     dht.setup(pin, DHTesp::DHT11);
// }

// TempAndHumidity readDHTSensor(DHTesp &dht) {
//     return dht.getTempAndHumidity();
// }

// // Módulo TimeManager
// void setupTime(const char* ntpServer) {
//     configTime(0, 0, ntpServer);
// }

// String getISO8601Time() {
//     time_t now;
//     struct tm timeinfo;
//     char buffer[30];
//     if (!getLocalTime(&timeinfo)) {
//         Serial.println("Error al obtener la hora local."); // Mensaje de error
//         return "";  // Devolver una cadena vacía si falla obtener el tiempo
//     }
//     strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H:%M:%S.000Z", &timeinfo);  // Formato ISO8601
//     return String(buffer);
// }

// // Módulo BackendManager
// String getSeguimientoID() {
//     HTTPClient http;
//     String url = backendSeguimientoBaseURL + idMaquina;

//     http.begin(url);
//     int httpCode = http.GET();

//     if (httpCode > 0) {
//         String payload = http.getString();
//         StaticJsonDocument<1024> doc;
//         deserializeJson(doc, payload);
//         seguimientoID = doc["data"]["_id"].as<String>();
//         http.end();
//         return seguimientoID;
//     } else {
//         http.end();
//         return "";
//     }
// }

// // Nueva función para enviar datos a la API 
// void sendDataToBackend(StaticJsonDocument<600>& payload) {
//     if (WiFi.status() == WL_CONNECTED) {
//         HTTPClient http;
//         http.begin(serverNameData);  // Nueva URL sin API Key
//         http.addHeader("Content-Type", "application/json");

//         String json;
//         serializeJson(payload, json);

//         Serial.print("JSON a enviar: ");
//         Serial.println(json); // Verifica el JSON que se está enviando

//         int httpResponseCode = http.POST(json);
//         Serial.print("Código de respuesta: ");
//         Serial.println(httpResponseCode);

//         // Imprimir el mensaje de error o respuesta del backend si el código no es 201
//         if (httpResponseCode != 201) {
//             String response = http.getString(); // Obtener la respuesta del backend
//             Serial.println("Error al subir los datos. Código de error: " + String(httpResponseCode));
//             Serial.print("Respuesta del servidor: ");
//             Serial.println(response); // Imprimir la respuesta del servidor
//         }

//         http.end();
//     } else {
//         Serial.println("No hay conexión WiFi.");
//     }
// }

// // Función para obtener la temperatura ambiente de la API
// float getClimaAPI() {
//     HTTPClient http;
//     String url = "https://api.openweathermap.org/data/2.5/weather?lat=" + String(latitude, 5) + "&lon=" + String(longitude, 5) + "&appid=" + String(apiKey) + "&units=metric";

//     http.begin(url);
//     int httpCode = http.GET();

//     if (httpCode > 0) {
//         String payload = http.getString();
//         StaticJsonDocument<1024> doc;
//         DeserializationError error = deserializeJson(doc, payload);

//         if (!error) {
//             float temperatura = doc["main"]["temp"];
//             Serial.println("Temperatura ambiente: " + String(temperatura) + "°C");
//             http.end();
//             return temperatura;
//         }
//     }

//     Serial.println("Error al obtener la temperatura del clima.");
//     http.end();
//     return 0.0;
// }

// // Función para verificar estado de la máquina
// bool getMachineState() {
//     HTTPClient http;
//     http.begin(serverNameStatus);
//     int httpResponseCode = http.GET();

//     if (httpResponseCode > 0) {
//         String response = http.getString();
//         DynamicJsonDocument doc(1024);
//         deserializeJson(doc, response);

//         bool maquinaEncendida = doc["maquinaEncendida"];
//         String horaApagado = doc["horaApagado"];

//         Serial.println("Estado de la máquina recibido:");
//         Serial.println(response);

//         http.end();
//         return maquinaEncendida;
//     } else {
//         Serial.println("Error al obtener el estado de la máquina.");
//         http.end();
//         return false;
//     }
// }

// // Setup principal
// void setup() {
//     pinMode(ledMaquina, OUTPUT);

//     Serial.begin(115200);
//     Serial.println("Iniciando configuración...");

//     // Conexión WiFi
//     connectToWiFi(ssid, password);

//     // Configurar DHT
//     setupDHT(dht, DHTPIN);

//     // Configurar tiempo NTP
//     setupTime(ntpServer);
    
//     delay(2000); // Esperar 2 segundos para asegurar que el NTP esté sincronizado
// }

// // Loop principal
// void loop() {
//     unsigned long currentMillis = millis();

//     // Verificar estado de la máquina
//     if (currentMillis - lastStateMillis >= 5000) {
//         lastStateMillis = currentMillis;
//         bool maquinaEncendida = getMachineState();
//         digitalWrite(ledMaquina, maquinaEncendida ? HIGH : LOW);
//     }

//     // Enviar datos si la máquina está encendida y ha pasado el tiempo adecuado
//     if (digitalRead(ledMaquina) == HIGH && (currentMillis - lastDataMillis >= 15000 || lastDataMillis == 0)) {
//         lastDataMillis = currentMillis;

//         String seguimientoID = getSeguimientoID();  // Obtener seguimiento ID
//         float tempAmbiente = getClimaAPI();         // Obtener temperatura ambiente
//         TempAndHumidity data = readDHTSensor(dht);  // Leer datos del sensor DHT

//         // Obtener la fecha en formato ISO8601
//         String fechaISO8601 = getISO8601Time();     

//         if (fechaISO8601.isEmpty()) { // Verificar si la fecha está vacía
//             Serial.println("No se puede enviar datos. La fecha está vacía.");
//             return; // Salir si la fecha no se pudo obtener
//         }

//         // Crear el payload JSON
//         StaticJsonDocument<600> payload;
//         payload["IdSeguimiento"] = seguimientoID;  // Cambiado a "IdSeguimiento"
        
//         // Convertir las temperaturas a string antes de agregarlas al JSON
//         payload["temperaturaAmbiente"] = String(round(tempAmbiente * 100.0) / 100.0); // Convertir a string
//         payload["temperaturaSensor"] = String(round(data.temperature * 100.0) / 100.0); // Convertir a string
        
//         payload["idMaquina"] = idMaquina;  // Cambiado a "idMaquina"
//         payload["rotor"] = rotor;    // Se mantiene en true o false según sea necesario
//         payload["motor"] = motor;    // Se mantiene en true o false según sea necesario
//         payload["fecha"] = fechaISO8601;

//         // Enviar datos al backend
//         sendDataToBackend(payload);
//     }
// }






// conseguir el formato de fecha adecuado para la base de datos (piche red del sena)
// #include <WiFi.h>
// #include "time.h"

// // Credenciales WiFi
// const char* ssid = "BUSCANDO RED 2";          // Reemplaza con tu SSID
// const char* password = "Rafael1061773978";  // Reemplaza con tu contraseña
// // const char* ssid = "maa";          // Reemplaza con tu SSID
// // const char* password = "123456789";  // Reemplaza con tu contraseña


// const char* ntpServer = "pool.ntp.org"; // Servidor NTP

// void setup() {
//     Serial.begin(115200);
    
//     // Conectar a WiFi
//     connectToWiFi(ssid, password);
    
//     // Configurar tiempo NTP
//     setupTime(ntpServer);
    
//     // Esperar un momento para la sincronización
//     delay(10000);  // Aumentar el tiempo de espera

//     // Obtener y mostrar la hora actual en formato ISO 8601
//     String fechaISO8601 = getCurrentISO8601Time();
    
//     if (fechaISO8601 == "1970-01-01T00:00:05.000Z") {
//         Serial.println("No se pudo sincronizar la hora correctamente.");
//     } else {
//         Serial.println("Hora actual en formato ISO 8601: " + fechaISO8601);
//     }
// }

// void loop() {
//     // Puedes agregar más código aquí si lo necesitas
// }

// void connectToWiFi(const char* ssid, const char* password) {
//     Serial.println("Conectando a la red WiFi...");
//     WiFi.mode(WIFI_STA);
//     WiFi.begin(ssid, password);

//     while (WiFi.status() != WL_CONNECTED) {
//         delay(500);
//         Serial.print(".");
//     }
//     Serial.println("\nConectado a WiFi. IP: " + WiFi.localIP().toString());
// }

// void setupTime(const char* ntpServer) {
//     configTime(0, 0, ntpServer);
// }

// String getCurrentISO8601Time() {
//     time_t now = time(NULL); // Obtener el tiempo actual
//     if (now < 1000000000) { // Comprobar si el tiempo es razonable
//         return "1970-01-01T00:00:05.000Z"; // Indica que la hora no es válida
//     }
    
//     struct tm *timeinfo;
//     timeinfo = localtime(&now); // Convertir a hora local

//     char buffer[30];
//     strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H:%M:%S.000Z", timeinfo); // Formato ISO8601
//     return String(buffer);
// }




// #include <WiFi.h>
// #include <WebServer.h>
// #include <DHTesp.h>
// #include <ArduinoJson.h>
// #include <HTTPClient.h>
// #include "time.h"

// // Credenciales WiFi
// const char* ssid = "maa";
// const char* password = "123456789";

// // Definir pines
// const int ledMaquina = 2;
// const int DHTPIN = 15;             // Pin donde está conectado el sensor DHT11
// DHTesp dht;

// unsigned long lastDataMillis = 0;    // Última vez que se enviaron datos
// unsigned long lastStateMillis = 0;   // Última vez que se verificó el estado de la máquina

// const char* ntpServer = "pool.ntp.org";
// const char* serverNameData = "https://proyecto-sena-backend-s666.onrender.com/api/datos";  // Nueva URL para POST
// const char* serverNameStatus = "https://bakend-arduino.onrender.com/api/estado";  // URL del backend para obtener el estado de la máquina

// // Base de la URL del backend para obtener el seguimiento
// const String backendSeguimientoBaseURL = "https://proyecto-sena-backend-s666.onrender.com/api/seguimiento/maquina/";
// const String idMaquina = "66ba25c376b1aba0f6ef93cc";  // ID de la máquina

// // Documento JSON
// StaticJsonDocument<500> doc;

// // Variables para API del clima
// const float latitude = 2.446635;
// const float longitude = -76.632958;
// const char* apiKey = "c373112c8c37e3facd9be6fbeeb8f2cd";
// float tempAmbiente = 0.0;  // Variable para almacenar la temperatura ambiente

// // Variables nuevas
// bool rotor = true;
// bool motor = true;

// // Variable para el seguimiento_id
// String seguimientoID = "";

// // Módulo WiFiManager
// void connectToWiFi(const char* ssid, const char* password) {
//     WiFi.mode(WIFI_STA);
//     WiFi.begin(ssid, password);
//     Serial.println("Conectando a la red WiFi...");

//     while (WiFi.status() != WL_CONNECTED) {
//         delay(500);
//         Serial.print(".");
//     }

//     Serial.println("\nConectado a WiFi. IP: " + WiFi.localIP().toString());
// }

// // Módulo DHTManager
// void setupDHT(DHTesp &dht, int pin) {
//     dht.setup(pin, DHTesp::DHT11);
// }

// TempAndHumidity readDHTSensor(DHTesp &dht) {
//     return dht.getTempAndHumidity();
// }

// // Módulo TimeManager
// void setupTime(const char* ntpServer) {
//     configTime(0, 0, ntpServer);
// }

// String getISO8601Time() {
//     time_t now;
//     struct tm timeinfo;
//     char buffer[30];
//     if (!getLocalTime(&timeinfo)) {
//         Serial.println("Error al obtener la hora local."); // Mensaje de error
//         return "";  // Devolver una cadena vacía si falla obtener el tiempo
//     }
//     strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H:%M:%S.000Z", &timeinfo);  // Formato ISO8601
//     return String(buffer);
// }

// // Módulo BackendManager
// String getSeguimientoID() {
//     HTTPClient http;
//     String url = backendSeguimientoBaseURL + idMaquina;

//     http.begin(url);
//     int httpCode = http.GET();

//     if (httpCode > 0) {
//         String payload = http.getString();
//         StaticJsonDocument<1024> doc;
//         deserializeJson(doc, payload);
//         seguimientoID = doc["data"]["_id"].as<String>();
//         http.end();
//         return seguimientoID;
//     } else {
//         http.end();
//         return "";
//     }
// }

// // Nueva función para enviar datos a la API 
// void sendDataToBackend(StaticJsonDocument<600>& payload) {
//     if (WiFi.status() == WL_CONNECTED) {
//         HTTPClient http;
//         http.begin(serverNameData);  // Nueva URL sin API Key
//         http.addHeader("Content-Type", "application/json");

//         String json;
//         serializeJson(payload, json);

//         Serial.print("JSON a enviar: ");
//         Serial.println(json); // Verifica el JSON que se está enviando

//         int httpResponseCode = http.POST(json);
//         Serial.print("Código de respuesta: ");
//         Serial.println(httpResponseCode);

//         // Imprimir el mensaje de error o respuesta del backend si el código no es 201
//         if (httpResponseCode != 201) {
//             String response = http.getString(); // Obtener la respuesta del backend
//             Serial.println("Error al subir los datos. Código de error: " + String(httpResponseCode));
//             Serial.print("Respuesta del servidor: ");
//             Serial.println(response); // Imprimir la respuesta del servidor
//         }

//         http.end();
//     } else {
//         Serial.println("No hay conexión WiFi.");
//     }
// }

// // Función para obtener la temperatura ambiente de la API
// float getClimaAPI() {
//     HTTPClient http;
//     String url = "https://api.openweathermap.org/data/2.5/weather?lat=" + String(latitude, 5) + "&lon=" + String(longitude, 5) + "&appid=" + String(apiKey) + "&units=metric";

//     http.begin(url);
//     int httpCode = http.GET();

//     if (httpCode > 0) {
//         String payload = http.getString();
//         StaticJsonDocument<1024> doc;
//         DeserializationError error = deserializeJson(doc, payload);

//         if (!error) {
//             float temperatura = doc["main"]["temp"];
//             Serial.println("Temperatura ambiente: " + String(temperatura) + "°C");
//             http.end();
//             return temperatura;
//         }
//     }

//     Serial.println("Error al obtener la temperatura del clima.");
//     http.end();
//     return 0.0;
// }

// // Función para verificar estado de la máquina
// bool getMachineState() {
//     HTTPClient http;
//     http.begin(serverNameStatus);
//     int httpResponseCode = http.GET();

//     if (httpResponseCode > 0) {
//         String response = http.getString();
//         DynamicJsonDocument doc(1024);
//         deserializeJson(doc, response);

//         bool maquinaEncendida = doc["maquinaEncendida"];
//         String horaApagado = doc["horaApagado"];

//         Serial.println("Estado de la máquina recibido:");
//         Serial.println(response);

//         http.end();
//         return maquinaEncendida;
//     } else {
//         Serial.println("Error al obtener el estado de la máquina.");
//         http.end();
//         return false;
//     }
// }

// // Setup principal
// void setup() {
//     pinMode(ledMaquina, OUTPUT);

//     Serial.begin(115200);
//     Serial.println("Iniciando configuración...");

//     // Conexión WiFi
//     connectToWiFi(ssid, password);

//     // Configurar DHT
//     setupDHT(dht, DHTPIN);

//     // Configurar tiempo NTP
//     setupTime(ntpServer);
    
//     delay(2000); // Esperar 2 segundos para asegurar que el NTP esté sincronizado
// }

// // Loop principal
// void loop() {
//     unsigned long currentMillis = millis();

//     // Verificar estado de la máquina
//     if (currentMillis - lastStateMillis >= 5000) {
//         lastStateMillis = currentMillis;
//         bool maquinaEncendida = getMachineState();
//         digitalWrite(ledMaquina, maquinaEncendida ? HIGH : LOW);
//     }

//     // Enviar datos si la máquina está encendida y ha pasado el tiempo adecuado
//     if (digitalRead(ledMaquina) == HIGH && (currentMillis - lastDataMillis >= 15000 || lastDataMillis == 0)) {
//         lastDataMillis = currentMillis;

//         String seguimientoID = getSeguimientoID();  // Obtener seguimiento ID
//         float tempAmbiente = getClimaAPI();         // Obtener temperatura ambiente
//         TempAndHumidity data = readDHTSensor(dht);  // Leer datos del sensor DHT

//         // Obtener la fecha en formato ISO8601
//         String fechaISO8601 = getISO8601Time();     

//         if (fechaISO8601.isEmpty()) { // Verificar si la fecha está vacía
//             Serial.println("No se puede enviar datos. La fecha está vacía.");
//             return; // Salir si la fecha no se pudo obtener
//         }

//         // Crear el payload JSON
//         StaticJsonDocument<600> payload;
//         payload["IdSeguimiento"] = seguimientoID;  // Cambiado a "IdSeguimiento"
        
//         // Convertir las temperaturas a string antes de agregarlas al JSON
//         payload["temperaturaAmbiente"] = String(round(tempAmbiente * 100.0) / 100.0); // Convertir a string
//         payload["temperaturaSensor"] = String(round(data.temperature * 100.0) / 100.0); // Convertir a string
        
//         payload["idMaquina"] = idMaquina;  // Cambiado a "idMaquina"
//         payload["rotor"] = rotor;    // Se mantiene en true o false según sea necesario
//         payload["motor"] = motor;    // Se mantiene en true o false según sea necesario
//         payload["fecha"] = fechaISO8601;

//         // Enviar datos al backend
//         sendDataToBackend(payload);
//     }
// }


// // eto da erroro 400, ya se como resolverlo
// #include <WiFi.h>
// #include <WebServer.h>
// #include <DHTesp.h>
// #include <ArduinoJson.h>
// #include <HTTPClient.h>
// #include "time.h"

// // Credenciales WiFi
// const char* ssid = "maa";
// const char* password = "123456789";

// // Definir pines
// const int ledMaquina = 2;
// const int DHTPIN = 15;             // Pin donde está conectado el sensor DHT11
// DHTesp dht;

// unsigned long lastDataMillis = 0;    // Última vez que se enviaron datos
// unsigned long lastStateMillis = 0;   // Última vez que se verificó el estado de la máquina

// const char* ntpServer = "pool.ntp.org";
// const char* serverNameData = "https://proyecto-sena-backend-s666.onrender.com/api/datos";  // Nueva URL para POST
// const char* serverNameStatus = "https://bakend-arduino.onrender.com/api/estado";  // URL del backend para obtener el estado de la máquina

// // Base de la URL del backend para obtener el seguimiento
// const String backendSeguimientoBaseURL = "https://proyecto-sena-backend-s666.onrender.com/api/seguimiento/maquina/";
// // Variable quemada para el ID de la máquina
// const String idMaquina = "66ba25c376b1aba0f6ef93cc";  // ID de la máquina

// // Documento JSON
// StaticJsonDocument<500> doc;

// // Variables para API del clima
// const float latitude = 2.446635;
// const float longitude = -76.632958;
// const char* apiKey = "c373112c8c37e3facd9be6fbeeb8f2cd";
// float tempAmbiente = 0.0;  // Variable para almacenar la temperatura ambiente

// // Variables nuevas
// bool rotor = true;
// bool motor = true;

// // Variable para el seguimiento_id
// String seguimientoID = "";

// // Módulo WiFiManager
// void connectToWiFi(const char* ssid, const char* password) {
//     WiFi.mode(WIFI_STA);
//     WiFi.begin(ssid, password);
//     Serial.println("Conectando a la red WiFi...");

//     while (WiFi.status() != WL_CONNECTED) {
//         delay(500);
//         Serial.print(".");
//     }

//     Serial.println("\nConectado a WiFi. IP: " + WiFi.localIP().toString());
// }

// // Módulo DHTManager
// void setupDHT(DHTesp &dht, int pin) {
//     dht.setup(pin, DHTesp::DHT11);
// }

// TempAndHumidity readDHTSensor(DHTesp &dht) {
//     return dht.getTempAndHumidity();
// }

// // Módulo TimeManager
// void setupTime(const char* ntpServer) {
//     configTime(0, 0, ntpServer);
// }

// String getISO8601Time() {
//     time_t now;
//     struct tm timeinfo;
//     char buffer[30];
//     if (!getLocalTime(&timeinfo)) {
//         return "";  // Devolver una cadena vacía si falla obtener el tiempo
//     }
//     strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H:%M:%S.000Z", &timeinfo);  // Formato ISO8601
//     return String(buffer);
// }

// // Módulo BackendManager
// String getSeguimientoID() {
//     HTTPClient http;
//     String url = backendSeguimientoBaseURL + idMaquina;

//     http.begin(url);
//     int httpCode = http.GET();

//     if (httpCode > 0) {
//         String payload = http.getString();
//         StaticJsonDocument<1024> doc;
//         deserializeJson(doc, payload);
//         seguimientoID = doc["data"]["_id"].as<String>();
//         http.end();
//         return seguimientoID;
//     } else {
//         http.end();
//         return "";
//     }
// }

// // Nueva función para enviar datos a la API 
// void sendDataToBackend(StaticJsonDocument<600>& payload) {
//     if (WiFi.status() == WL_CONNECTED) {
//         HTTPClient http;
//         http.begin(serverNameData);  // Nueva URL sin API Key
//         http.addHeader("Content-Type", "application/json");

//         String json;
//         serializeJson(payload, json);

//         Serial.print("JSON a enviar: ");
//         Serial.println(json); // Verifica el JSON que se está enviando

//         int httpResponseCode = http.POST(json);
//         Serial.print("Código de respuesta: ");
//         Serial.println(httpResponseCode);

//         // Imprimir el mensaje de error o respuesta del backend si el código no es 201
//         if (httpResponseCode != 201) {
//             String response = http.getString(); // Obtener la respuesta del backend
//             Serial.println("Error al subir los datos. Código de error: " + String(httpResponseCode));
//             Serial.print("Respuesta del servidor: ");
//             Serial.println(response); // Imprimir la respuesta del servidor
//         }

//         http.end();
//     } else {
//         Serial.println("No hay conexión WiFi.");
//     }
// }



// // Función para obtener la temperatura ambiente de la API
// float getClimaAPI() {
//     HTTPClient http;
//     String url = "https://api.openweathermap.org/data/2.5/weather?lat=" + String(latitude, 5) + "&lon=" + String(longitude, 5) + "&appid=" + String(apiKey) + "&units=metric";

//     http.begin(url);
//     int httpCode = http.GET();

//     if (httpCode > 0) {
//         String payload = http.getString();
//         StaticJsonDocument<1024> doc;
//         DeserializationError error = deserializeJson(doc, payload);

//         if (!error) {
//             float temperatura = doc["main"]["temp"];
//             Serial.println("Temperatura ambiente: " + String(temperatura) + "°C");
//             http.end();
//             return temperatura;
//         }
//     }

//     Serial.println("Error al obtener la temperatura del clima.");
//     http.end();
//     return 0.0;
// }

// // Función para verificar estado de la máquina
// bool getMachineState() {
//     HTTPClient http;
//     http.begin(serverNameStatus);
//     int httpResponseCode = http.GET();

//     if (httpResponseCode > 0) {
//         String response = http.getString();
//         DynamicJsonDocument doc(1024);
//         deserializeJson(doc, response);

//         bool maquinaEncendida = doc["maquinaEncendida"];
//         String horaApagado = doc["horaApagado"];

//         Serial.println("Estado de la máquina recibido:");
//         Serial.println(response);

//         http.end();
//         return maquinaEncendida;
//     } else {
//         Serial.println("Error al obtener el estado de la máquina.");
//         http.end();
//         return false;
//     }
// }

// // Setup principal
// void setup() {
//     pinMode(ledMaquina, OUTPUT);

//     Serial.begin(115200);
//     Serial.println("Iniciando configuración...");

//     // Conexión WiFi
//     connectToWiFi(ssid, password);

//     // Configurar DHT
//     setupDHT(dht, DHTPIN);

//     // Configurar tiempo NTP
//     setupTime(ntpServer);
// }

// // Loop principal
// void loop() {
//     unsigned long currentMillis = millis();

//     // Verificar estado de la máquina
//     if (currentMillis - lastStateMillis >= 5000) {
//         lastStateMillis = currentMillis;
//         bool maquinaEncendida = getMachineState();
//         digitalWrite(ledMaquina, maquinaEncendida ? HIGH : LOW);
//     }

//     // Enviar datos si la máquina está encendida y ha pasado el tiempo adecuado
//     if (digitalRead(ledMaquina) == HIGH && (currentMillis - lastDataMillis >= 15000 || lastDataMillis == 0)) {
//         lastDataMillis = currentMillis;

//         String seguimientoID = getSeguimientoID();  // Obtener seguimiento ID
//         float tempAmbiente = getClimaAPI();         // Obtener temperatura ambiente
//         TempAndHumidity data = readDHTSensor(dht);  // Leer datos del sensor DHT

//         // Obtener la fecha en formato ISO8601
//         String fechaISO8601 = getISO8601Time();     

//         // Crear el payload JSON
//         StaticJsonDocument<600> payload;
//         payload["IdSeguimiento"] = seguimientoID;  // Cambiado a "IdSeguimiento"
        
//         // Convertir las temperaturas a string antes de agregarlas al JSON
//         payload["temperaturaAmbiente"] = String(round(tempAmbiente * 100.0) / 100.0); // Convertir a string
//         payload["temperaturaSensor"] = String(round(data.temperature * 100.0) / 100.0); // Convertir a string
        
//         payload["idMaquina"] = idMaquina;  // Cambiado a "idMaquina"
//         payload["rotor"] = rotor;    // Se mantiene en true o false según sea necesario
//         payload["motor"] = motor;    // Se mantiene en true o false según sea necesario
//         payload["fecha"] = fechaISO8601;

//         // Enviar datos al backend
//         sendDataToBackend(payload);
//     }
// }






// =============================================================//

// #include <WiFi.h>
// #include <WebServer.h>
// #include <DHTesp.h>
// #include <ArduinoJson.h>
// #include <HTTPClient.h>
// #include "time.h"

// // Credenciales WiFi
// const char* ssid = "maa";
// const char* password = "123456789";

// // Definir pines
// const int ledMaquina = 2;
// const int DHTPIN = 15;             // Pin donde está conectado el sensor DHT11
// DHTesp dht;

// unsigned long lastDataMillis = 0;    // Última vez que se enviaron datos
// unsigned long lastStateMillis = 0;   // Última vez que se verificó el estado de la máquina

// const char* ntpServer = "pool.ntp.org";
// // const char* serverNameData = "https://us-east-1.aws.data.mongodb-api.com/app/data-ndbugol/endpoint/data/v1/action/insertOne";  // URL de la API de MongoDB Data
// const char* serverNameStatus = "https://bakend-arduino.onrender.com/api/estado";  // URL del backend para obtener el estado de la máquina

// // Base de la URL del backend para obtener el seguimiento (backen del proyecto)
// const String backendSeguimientoBaseURL = "https://proyecto-sena-backend-s666.onrender.com/api/seguimiento/maquina/";
// // Variable quemada para el ID de la máquina
// const String idMaquina = "66ba25c376b1aba0f6ef93cc";  // ID de la máquina

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
// String seguimientoID = "";

// // Variables nuevas
// bool rotor = true;
// bool motor = true;

// // Módulo coneccion wifi
// void connectToWiFi(const char* ssid, const char* password) {
//     WiFi.mode(WIFI_STA);
//     WiFi.begin(ssid, password);
//     Serial.println("Conectando a la red WiFi...");

//     while (WiFi.status() != WL_CONNECTED) {
//         delay(500);
//         Serial.print(".");
//     }

//     Serial.println("\nConectado a WiFi. IP: " + WiFi.localIP().toString());
// }

// // Módulo DHTManager
// void setupDHT(DHTesp &dht, int pin) {
//     dht.setup(pin, DHTesp::DHT11);
// }

// TempAndHumidity readDHTSensor(DHTesp &dht) {
//     return dht.getTempAndHumidity();
// }

// // Módulo TimeManager
// void setupTime(const char* ntpServer) {
//     configTime(0, 0, ntpServer);
// }

// unsigned long getCurrentTime() {
//     time_t now;
//     struct tm timeinfo;
//     if (!getLocalTime(&timeinfo)) {
//         return 0;
//     }
//     time(&now);
//     return now;
// }

// // Módulo BackendManager
// String getSeguimientoID() {
//     HTTPClient http;
//     String url = backendSeguimientoBaseURL + idMaquina;

//     http.begin(url);
//     int httpCode = http.GET();

//     if (httpCode > 0) {
//         String payload = http.getString();
//         StaticJsonDocument<1024> doc;
//         deserializeJson(doc, payload);
//         seguimientoID = doc["data"]["_id"].as<String>();
//         http.end();
//         return seguimientoID;
//     } else {
//         http.end();
//         return "";
//     }
// }

// void sendDataToBackend(StaticJsonDocument<600>& payload) {
//     if (WiFi.status() == WL_CONNECTED) {
//         HTTPClient http;
//         http.begin(serverNameData);
//         http.addHeader("Content-Type", "application/json");
//         http.addHeader("api-key", "WD0PmvqccnHxPYc4YEsOK3hryZAN6fca4glv0XLQTaMeAZID4Yh4zGZQNpiXEdsz");

//         String json;
//         serializeJson(payload, json);

//         int httpResponseCode = http.POST(json);
//         Serial.println("Código de respuesta: " + String(httpResponseCode));

//         if (httpResponseCode != 201) {
//             Serial.println("Error al subir los datos.");
//         }

//         http.end();
//     }
// }

// // Módulo API del clima 
// float getClimaAPI() {
//     HTTPClient http;
//     String url = "https://api.openweathermap.org/data/2.5/weather?lat=" + String(latitude, 5) + "&lon=" + String(longitude, 5) + "&appid=" + String(apiKey) + "&units=metric";

//     http.begin(url);
//     int httpCode = http.GET();

//     if (httpCode > 0) {
//         String payload = http.getString();
//         StaticJsonDocument<1024> doc;
//         DeserializationError error = deserializeJson(doc, payload);

//         if (!error) {
//             float temperatura = doc["main"]["temp"];
//             Serial.println("Temperatura ambiente: " + String(temperatura) + "°C");
//             http.end();
//             return temperatura;
//         }
//     }

//     Serial.println("Error al obtener la temperatura del clima.");
//     http.end();
//     return 0.0;
// }

// // Función para verificar estado de la máquina
// bool getMachineState() {
//     HTTPClient http;
//     http.begin(serverNameStatus);
//     int httpResponseCode = http.GET();

//     if (httpResponseCode > 0) {
//         String response = http.getString();
//         DynamicJsonDocument doc(1024);
//         deserializeJson(doc, response);

//         bool maquinaEncendida = doc["maquinaEncendida"];
//         String horaApagado = doc["horaApagado"];

//         Serial.println("Estado de la máquina recibido:");
//         Serial.println(response);

//         http.end();
//         return maquinaEncendida;
//     } else {
//         Serial.println("Error al obtener el estado de la máquina.");
//         http.end();
//         return false;
//     }
// }

// // Setup principal
// void setup() {
//     pinMode(ledMaquina, OUTPUT);

//     Serial.begin(115200);
//     Serial.println("Iniciando configuración...");

//     // Conexión WiFi
//     connectToWiFi(ssid, password);

//     // Configurar DHT
//     setupDHT(dht, DHTPIN);

//     // Configurar tiempo NTP
//     setupTime(ntpServer);
// }

// // Loop principal
// void loop() {
//     unsigned long currentMillis = millis();

//     // Verificar estado de la máquina (cada 5 segundos)
//     if (currentMillis - lastStateMillis >= 5000) {
//         lastStateMillis = currentMillis;
//         bool maquinaEncendida = getMachineState();
//         digitalWrite(ledMaquina, maquinaEncendida ? HIGH : LOW);
//     }

//     // Enviar datos (cada 15 segundos)
//     if (digitalRead(ledMaquina) == HIGH && (currentMillis - lastDataMillis >= 15000 || lastDataMillis == 0)) {
//         lastDataMillis = currentMillis;

//         String seguimientoID = getSeguimientoID();
//         float tempAmbiente = getClimaAPI();
//         TempAndHumidity data = readDHTSensor(dht);
//         unsigned long epochtime = getCurrentTime();

//         StaticJsonDocument<600> payload;
//         payload["dataSource"] = "Cluster0";
//         payload["database"] = databaseName;
//         payload["collection"] = collectionName;
//         JsonObject docObject = payload.createNestedObject("document");
//         docObject["temperatura"] = data.temperature;
//         docObject["temperaturaAmbiente"] = tempAmbiente;
//         docObject["humedad"] = data.humidity;
//         docObject["timestamp"] = epochtime;
//         docObject["seguimiento_id"] = seguimientoID;

//         sendDataToBackend(payload);
//     }
// }



// =============================================================// =============================================================



// // funcionalidades con seguimiento
// // testeado

// //prueva con grimoire

// #include <WiFi.h>
// #include <WebServer.h>
// #include <DHTesp.h>
// #include <ArduinoJson.h>
// #include <HTTPClient.h>
// #include "time.h"

// // Credenciales WiFi
// const char* ssid = "maaa";
// const char* password = "123456789";
// // const char* ssid = "Tenda_4638F8";
// // const char* password = "m6R2A6Rq";

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

// // Base de la URL del backend para obtener el seguimiento
// const String backendSeguimientoBaseURL = "https://proyecto-sena-backend-s666.onrender.com/api/seguimiento/maquina/";
// // Variable quemada para el ID de la máquina
// const String idMaquina = "66ba25c376b1aba0f6ef93cc";  // ID de la máquina

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

//       // Obtener el seguimiento_id usando el id de la máquina
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

// // Función para obtener el seguimiento_id del backend usando el id de la máquina
// void getSeguimientoID() {
//   if (WiFi.status() == WL_CONNECTED) {
//     HTTPClient http;
    
//     // Construir la URL concatenando la base y el idMaquina
//     String url = backendSeguimientoBaseURL + idMaquina;

//     http.begin(url);  // Inicia la conexión HTTP
//     int httpCode = http.GET();  // Realiza la solicitud GET

//     if (httpCode > 0) {  // Verifica si la solicitud fue exitosa
//       String payload = http.getString();  // Obtiene la respuesta en formato String

//       // Procesar la respuesta JSON
//       StaticJsonDocument<1024> doc;
//       DeserializationError error = deserializeJson(doc, payload);

//       if (!error) {
//         seguimientoID = doc["data"]["_id"].as<String>();
//         // seguimientoID = doc["_id"].as<String>();  // Guardar el seguimiento_id (error)
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

#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "Tenda_4638F8"; // Replace with your WiFi network name
const char* password = "m6R2A6Rq"; // Replace with your WiFi network password
const char* mongoDBAtlasUsername = "gun45780"; // Replace with your MongoDB Atlas username
const char* mongoDBAtlasPassword = "adso145780"; // Replace with your MongoDB Atlas password
const char* mongoDBAtlasDatabaseName = "datos_de_arduino"; // Replace with your MongoDB Atlas database name
const char* mongoDBAtlasCollectionName = "datos"; // Replace with your MongoDB Atlas collection name

WiFiClient client;

void setup() {
  Serial.begin(115200);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  // Initialize HTTP client
  client.setServer("mongodb+srv://gun45780:adso145780@cluster0.feqs5gd.mongodb.net/datos_de_arduino?retryWrites=true&w=majority&appName=Cluster0", 27017); // Replace with your MongoDB Atlas cluster URL
  client.setCACert(ca_cert); // Replace with your MongoDB Atlas CA certificate
  Serial.println("db conectada");
}

void loop() {
  // Read temperature and humidity from DHT11 sensor
  DHT dht(DHTPIN, DHTTYPE);
  dht.begin();
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  // Create JSON object for sensor data
  String json = "{";
  json += "\"temperature\": " + String(temperature);
  json += ", \"humidity\": " + String(humidity);
  json += "}";

  // Authenticate with MongoDB Atlas
  String authString = base64_encode(mongoDBAtlasUsername + ":" + mongoDBAtlasPassword);
  client.addHeader("Authorization", "Basic " + authString);

  // Send sensor data to MongoDB
  HTTPClientRequest* request = client.post("/" + mongoDBAtlasDatabaseName + "/" + mongoDBAtlasCollectionName);
  request->addHeader("Content-Type", "application/json");
  int httpCode = request->POST(json);

  // Check HTTP response code
  if (httpCode > 0) {
    if (httpCode == 200) {
      Serial.println("Sensor data sent to MongoDB Atlas successfully");
    } else {
      Serial.println("Error sending sensor data to MongoDB Atlas: " + httpCode);
    }
  } else {
    Serial.println("Error sending sensor data to MongoDB Atlas");
  }

  // Free up HTTP request object
  request->stop();

  delay(10000); // Send data every 10 seconds
}
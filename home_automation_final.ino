#include <Arduino.h>
#include <ArduinoOTA.h>
#include <DallasTemperature.h>
#include <DHT.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <OneWire.h>
#include <WiFi.h>
#include <WebServer.h>

#define esp32led 2
#define LED 4
#define infrared 12
#define relay 13
#define dht11 18
#define ds18b20 21
#define dpz 27
#define triggerTriac 32
#define DHTTYPE DHT11

const char* ssid[] = { "nome da rede" };
const char* password[] = { "senha da rede" };
const int numNetworks = quantidade de redes cadastradas;

float lastTemperature, lastHumidity, temperature, humidity;
int count = 0, intensity = 0, lastIntensity = 0;
boolean isZeroCrossing = false;
unsigned long millisServerHandle = millis();
unsigned long millisSensorsRequest = millis();

DHT dht(dht11, DHTTYPE);
IRsend irsend(infrared);
OneWire oneWire(ds18b20);
DallasTemperature sensors(&oneWire);

IPAddress ip(192, 168, 1, 10);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
WebServer server(80);

void IRAM_ATTR zeroCrossingDetector() {
  if (intensity == NULL) {
    intensity = 0;
  }
  
  // Calculo do angulo de disparo: 60Hz-> 8.33ms (1/2 ciclo)
  // (8333us - 8.33us) / 256 = 32 (aprox)
  int powertime = (32 * (256 - intensity));
  // Mantem o circuito desligado por powertime microssegundos
  delayMicroseconds(powertime);
  // Envia sinal ao TRIAC para que ele passe a conduzir
  digitalWrite(triggerTriac, HIGH);
  // Espera alguns microssegundos para que o TRIAC perceba o pulso
  delayMicroseconds(8.33);
  // Desliga o pulso
  digitalWrite(triggerTriac, LOW);
}

void setup() {
  pinMode(esp32led, OUTPUT);
  pinMode(infrared, OUTPUT);
  pinMode(relay, OUTPUT);
  pinMode(dpz, INPUT);
  pinMode(triggerTriac, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(dpz), zeroCrossingDetector, CHANGE);

  dht.begin();
  sensors.begin();
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.config(ip, gateway, subnet);
  WiFi.begin();

  while (count < numNetworks) {
    WiFi.begin(ssid[count], password[count]);
    delay(5000);

    if (WiFi.status() == WL_CONNECTED) {
      Serial.println();
      Serial.printf("Connected to %s\n", ssid[count]);
      Serial.print("IP address: ");
      Serial.println(WiFi.localIP());  // print local IP address

      digitalWrite(esp32led, HIGH);
      delay(1500);
      digitalWrite(esp32led, LOW);
      break;
    } else {
      Serial.println("Failed to connect to WiFi");
      count++;
    }
  }

  server.on("/", handleRoot);          // handle root URL
  server.on("/params", handleParams);  // handle URL with parameters

  server.begin();  // start the server
  digitalWrite(esp32led, LOW);

  sensors.requestTemperatures();
  temperature = sensors.getTempCByIndex(0);
  humidity = dht.readHumidity();

  //Begin OTA Config
  // Port defaults to 3232
  // ArduinoOTA.setPort(3232);

  // Hostname defaults to esp3232-[MAC]
  ArduinoOTA.setHostname("nome do host");

  // No authentication by default
  ArduinoOTA.setPassword("senha do host");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else  // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
    })
    .onEnd([]() {
      Serial.println("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });

  ArduinoOTA.begin();
  //End OTA
}

void triggerInfraRed() {
  // Send an example IR signal (Replace this with the IR code you want to send)
  uint16_t rawData[] = { 4500, 4400, 600, 1600, 600, 1600, 600, 1600, 600, 550, 600, 1600, 600, 1600, 600, 1600, 600, 1600, 600, 1600, 600, 550, 600, 550, 600, 550, 600, 550, 600, 1600, 600, 1600, 600, 550, 600, 550, 600, 550, 600, 550, 600, 550, 600, 1600, 600, 550, 600, 550, 600, 1600, 600, 1600, 600, 1600, 600, 1600, 600 };  // Example IR signal
  uint16_t dataLength = sizeof(rawData) / sizeof(rawData[0]);

  // uint16_t rawData[] = { 550, 600, 1600, 600, 1600, 600, 1600, 600, 1600, 600 }; 
  // uint16_t dataLength = sizeof(rawData) / sizeof(rawData[0]);

  irsend.begin();
  irsend.sendRaw(rawData, dataLength, 38);

  delay(2000); // Wait for 2 seconds before sending the next signal
}

void handleParams() {  // handle URL with parameters (/params?param1=value1&param2=value2)
  String infraredParam = server.arg("infrared");
  String relayParam    = server.arg("relay");
  intensity            = server.arg("intensity").toInt();

  intensity = map(intensity, 0, 100, 0, 255);

  if (intensity > 255) { intensity = 255; } 
  else if (intensity < 0) { intensity = 0; } 
  else { intensity = intensity; }

  if ((millis() - millisSensorsRequest) > 120000) {
    sensors.requestTemperatures();
    temperature = sensors.getTempCByIndex(0);
    humidity = dht.readHumidity();

    millisSensorsRequest = millis();
  }

  //Zero Crossing Parameters
  if (isnan(intensity) || intensity == NULL) {
    intensity = lastIntensity;
  } else {
    lastIntensity = intensity;
  }

  //DS18B20 Parameters
  if (isnan(temperature) || temperature == NULL) {
    temperature = lastTemperature;
  } else {
    lastTemperature = temperature;
  }
  //DHT11 Parameters
  if (isnan(humidity) || humidity == NULL) {
    humidity = lastHumidity;
  } else {
    lastHumidity = humidity;
  }

  //Infrared Parameters
  if (infraredParam == "true") {
    triggerInfraRed();
  } 

  //Relay Parameters
  if (relayParam == "true") {
    digitalWrite(relay, HIGH);
  } else if (relayParam == "false") {
    digitalWrite(relay, LOW);
  }

  //Send a response back to the client
  if (server.arg("app") != "true") {
    server.send(200, "text/html", "Received parameters.<br><br>DHT11<br>Humidity: " + (String)humidity + "<br><br>DS18B20<br>Temperature: " + (String)temperature + "<br><br>Infrared: " + infraredParam + "<br><br>Relay: " + relayParam + "<br><br>Intensity: " + (String)intensity + "<br>");
  } else {
    server.send(200, "text/html", (String)humidity + "/" + (String)temperature + "/" + infraredParam + "/" + relayParam + "/" + (String)intensity);   
  } 

  if(infraredParam == "true") { infraredParam = "false"; } 
}

void handleRoot() {
  server.send(200, "text/html", "<h1>Starting ESP32!</h1><br>");
}

void loop() {
  ArduinoOTA.handle();

  if ((millis() - millisServerHandle) > 5000) {
    server.handleClient();  // handle client requests
    millisServerHandle = millis();
  }
}
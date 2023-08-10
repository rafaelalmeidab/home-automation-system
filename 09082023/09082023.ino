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
#define pulsoTriac 32
#define DHTTYPE DHT11

const char* ssid[] = {"Rafael - 2.4G", "Rio Cable - Rafael 2.4", "RBMWEB", "Fernanda 2.4G"};
const char* password[] = {"34627545", "rafael1061", "rbmweb01", "fernandawom"};
const int numNetworks = 3;

float lastTemperature, lastHumidity;
int count = 0, amplitude = 0, lastAmplitude = 0;
boolean isZeroCrossing = false, disparoTriac = false;
unsigned long millisServerHandle = millis();

DHT dht(dht11, DHTTYPE);
IRsend irsend(infrared);
OneWire oneWire(ds18b20);
DallasTemperature sensors(&oneWire);

IPAddress ip(192, 168, 1, 100);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
WebServer server(80);

void IRAM_ATTR passagemPorZero()
{
  disparoTriac = true;
}

void disparoTRIAC_BTA12(int amplitude){
  // Cálculo do ângulo de disparo: 60Hz-> 8.33ms (1/2 ciclo)
  // (8333us - 8.33us) / 256 = 32 (aprox)
  int powertime = (32*(256-amplitude));
  // Mantém o circuito desligado por powertime microssegundos
  delayMicroseconds(powertime);
  // Envia sinal ao TRIAC para que ele passe a conduzir
  digitalWrite(pulsoTriac, HIGH);
  // Espera alguns microssegundos para que o TRIAC perceba o pulso
  delayMicroseconds(8.33);
  // Desliga o pulso
  digitalWrite(pulsoTriac, LOW);
}

void disparoInfraVermelho(){
  
  // Send an example IR signal (Replace this with the IR code you want to send)
  uint16_t rawData[] = {4500, 4400, 600, 1600, 600, 1600, 600, 1600, 600, 550, 600, 1600, 600, 1600, 600, 1600, 600, 1600, 600, 1600, 600, 550, 600, 550, 600, 550, 600, 550, 600, 1600, 600, 1600, 600, 550, 600, 550, 600, 550, 600, 550, 600, 550, 600, 1600, 600, 550, 600, 550, 600, 1600, 600, 1600, 600, 1600, 600, 1600, 600}; // Example IR signal
  uint16_t dataLength = sizeof(rawData) / sizeof(rawData[0]);

  // Optional: Set the frequency of the IR signal (default is 38 kHz)
  irsend.begin();

  // Send the IR signal
  irsend.sendRaw(rawData, dataLength, 38);
}

void setup() {
  pinMode(esp32led, OUTPUT);
  pinMode(infrared, OUTPUT);
  pinMode(relay, OUTPUT);
  pinMode(dpz, INPUT);
  pinMode(pulsoTriac, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(dpz), passagemPorZero, CHANGE);

  dht.begin();
  sensors.begin();
  Serial.begin(115200); 
  WiFi.mode (WIFI_STA);
  WiFi.config(ip, gateway, subnet);

  while (count < numNetworks) {
    WiFi.begin(ssid[count], password[count]);
    delay(5000);

    if (WiFi.status() == WL_CONNECTED) {
      Serial.println();
      Serial.printf("Connected to %s\n", ssid[count]);
      Serial.print("IP address: ");
      Serial.println(WiFi.localIP()); // print local IP address

      digitalWrite(esp32led, HIGH);
      delay(1500);
      digitalWrite(esp32led, LOW);
      break;
    }
    else {
      Serial.println("Failed to connect to WiFi");
      count++;
    }
  }

  server.on("/", handleRoot); // handle root URL
  server.on("/params", handleParams); // handle URL with parameters

  server.begin(); // start the server
  digitalWrite(esp32led, LOW);

  //OTA Config
  // Port defaults to 3232
  // ArduinoOTA.setPort(3232);

  // Hostname defaults to esp3232-[MAC]
  ArduinoOTA.setHostname("tcc_esp32");

  // No authentication by default
  ArduinoOTA.setPassword("2049");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
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
}

void loop() {
  ArduinoOTA.handle();

  if(disparoTriac == true){
    if(amplitude == NULL){
      amplitude = 0;
    }

    disparoTRIAC_BTA12(amplitude);
    disparoTriac = false;
  }

  if((millis() - millisServerHandle) < 500){
    server.handleClient(); // handle client requests
  }
  
  handleServerMillis();
}

void handleRoot() {
  server.send(200, "text/html", "<h1>Starting ESP32!</h1><br>");
}

void handleServerMillis(){
  if((millis() - millisServerHandle) > 500){
    millisServerHandle = millis();
  }
}

void handleParams() { // handle URL with parameters (/params?param1=value1&param2=value2)
  String ESP32LedParam     = server.arg("esp32led");
  String ESP32LedParamTest = server.arg("esp32ledtest");
  String infraredParam     = server.arg("infrared");
  String relayParam        = server.arg("relay");
  amplitude                = (server.arg("amplitude").toInt());

  amplitude = map(amplitude, 0, 100, 0, 255);
  
  if (amplitude > 255) {
    amplitude = 255;
  }
  else if (amplitude < 0) {
    amplitude = 0;
  }
  else{
    amplitude = amplitude;
  }

  sensors.requestTemperatures();
  float temperature = sensors.getTempCByIndex(0);
  float humidity    = dht.readHumidity();

  //Zero Crossing Parameters
  if (isnan(amplitude)) {
    amplitude = lastAmplitude;
  }
  else {
    lastAmplitude = amplitude;
  }

  //DS18B20 Parameters
  if (isnan(temperature)) {
    temperature = lastTemperature;
  }
  else {
    lastTemperature = temperature;
  }
  //DHT11 Parameters
  if (isnan(humidity)) {
    humidity = lastHumidity;
  }
  else {
    lastHumidity = humidity;
  }

  //ESP32Led Parameters
  if (ESP32LedParam == "true") {
    digitalWrite(esp32led, HIGH);
  }
  else{
    digitalWrite(esp32led, LOW);
  }

  //ESP32Led Parameters Test
  if (ESP32LedParamTest == "true") {
    digitalWrite(esp32led, HIGH);
    delay(200);
    digitalWrite(esp32led, LOW);
    delay(200);
    digitalWrite(esp32led, HIGH);
    delay(200);
    digitalWrite(esp32led, LOW);
    delay(200);
  }
  else{
    digitalWrite(esp32led, LOW);
  }

  //Infrared Parameters
  if (infraredParam == "true") {
    digitalWrite(infrared, HIGH);
  }
  else{
    digitalWrite(infrared, LOW);
  }

  //Relay Parameters
  if (relayParam == "true") {
    digitalWrite(relay, HIGH);
  }
  else{
    digitalWrite(relay, LOW);
  }

  //Send a response back to the client
  if(server.arg("app") != "true"){
    //String ansApp[6] = {(String)humidity,(String)temperature,ESP32LedParam,infraredParam,relayParam,(String)amplitude};
    //server.send(200, "text/html", ansApp);
    server.send(200, "text/html", "Parameters received.<br><br>DHT11<br>Humidity: " + (String)humidity + "<br><br>DS18B20<br>Temperature: " + (String)temperature + "<br><br>ESP32 Led: " + ESP32LedParam + "<br><br>Infrared: " + infraredParam + "<br><br>Relay: " + relayParam + "<br><br>Amplitude: " + (String)amplitude + "<br>");
  }
  else{
    server.send(200, "text/html", (String)humidity + "/" + (String)temperature + "/" + ESP32LedParam + "/" + infraredParam + "/" + relayParam + "/" + (String)amplitude); 
  }

  handleServerMillis();
}

#include <Arduino.h>
#include <DallasTemperature.h>
#include <DHT.h>
#include <OneWire.h>
#include <WiFi.h>
#include <WebServer.h>

#define DHTTYPE DHT11
#define esp32led  2
#define dht11 18
#define ds18b20 21
#define infrared 12
#define relay 34
#define dpz 27
#define pulsoTriac 32

const char* ssid[] = {"Rafael - 2.4G", "Rio Cable - Rafael 2.4", "RBMWEB"};
const char* password[] = {"34627545", "rafael1061", "rbmweb01"};
const int numNetworks = 3;

float lastTemperature, lastHumidity;
int count = 0, amplitude = 0, lastAmplitude = 0;
boolean isZeroCrossing = false, disparoTriac = false;

DHT dht(dht11, DHTTYPE);
OneWire oneWire(ds18b20);
DallasTemperature sensors(&oneWire);

IPAddress ip(192, 168, 1, 100);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
WebServer server(80);


void IRAM_ATTR zero_crosss_int()
{
  disparoTriac = true;
}

void disparoTRIAC_BTA12(){
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

void setup() {
  pinMode(esp32led, OUTPUT);
  pinMode(infrared, OUTPUT);
  pinMode(relay, OUTPUT);
  pinMode(dpz, INPUT);
  pinMode(pulsoTriac, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(dpz), zero_crosss_int, CHANGE);

  dht.begin();
  sensors.begin();
  Serial.begin(115200); // start serial communication
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
}

void loop() {
  if(disparoTriac == true){
    disparoTRIAC_BTA12();
    disparoTriac = false;
  }
  
  server.handleClient(); // handle client requests
}

void handleRoot() {
  server.send(200, "text/html", "<h1>Starting ESP32!</h1><br>");
}

void handleParams() { // handle URL with parameters (/params?param1=value1&param2=value2)
  String ESP32LedParam     = server.arg("esp32led");
  String infraredParam     = server.arg("infrared");
  String relayParam        = server.arg("relay");
  amplitude                = (server.arg("amplitude").toInt());

  
  if (amplitude > 255) {
    amplitude = 255;
  }
  else if (amplitude < 0) {
    amplitude = 0;
  }
  
  amplitude = map(amplitude, 0, 100, 0, 255);
  //delay(250);

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
}

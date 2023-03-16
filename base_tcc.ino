// Autor: Rafael Almeida Borges da Silva

#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <DallasTemperature.h>
#include <DHT.h>
#include <ESPAsyncWebServer.h>
#include <esp_system.h>
#include <FS.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <OneWire.h>
#include <SD.h>
#include <SPI.h>
#include <sys/time.h>
#include <time.h>
#include <WiFi.h>

#define blink_esp32 2 
#define DHTPIN 18
#define DHTPIN2 23
#define DHTTYPE DHT11
#define oneWireBus 4

const char* ssid = "Rafael - 2.4G";
const char* senha_rede = "34627545";

char data_formatada[64];
int i=1,f=0,j=0, LED_AZUL=33, LED_AMARELO=26, LED_VERDE=14, LED_VERMELHO=13, control=0, beginGet;
float h = 0, t=0, ds18b20_temp = 0, temp_ant = 0, umi_ant = 0;
String dadosGet, prod, litros, dataMessage, dataMessage_litros, hora_unix, inString="", currentLine; 
struct tm data_D;
unsigned long hora=0;

IPAddress ip(192, 168, 1, 3);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

DHT dht(DHTPIN, DHTTYPE);
DHT dht2(DHTPIN2, DHTTYPE);
OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);

WiFiServer server(80);

void setup()
{
  Serial.begin(115200);
  pinMode(blink_esp32, OUTPUT);
  
  sensors.begin();
  dht.begin();
  dht2.begin();
  
  delay(100);
  Serial.println();
  Serial.print("Conectando à ");
  Serial.println(ssid);

  WiFi.begin(ssid, senha_rede);
  WiFi.config(ip, gateway, subnet);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");

  int i = 0;
  while(i<20){
    digitalWrite(blink_esp32, HIGH);
    delay(50);
    digitalWrite(blink_esp32, LOW);
    delay(50);
    i++;
  }
  
  Serial.println("WiFi conectado.");
  Serial.println("Endereço de IP: ");
  delay(2000);
  digitalWrite(blink_esp32, LOW);
  Serial.println(ip);
  server.begin(); 
}

void loop()
{
  WiFiClient client = server.available();
  if (client)
  {
    Serial.println("Novo Client.");
    String currentLine = "";
    while (client.connected())
    {
      if (client.available())
      {
        char c = client.read();
        Serial.write(c);
        if (c == '\n') //linha completa
        {
          if (currentLine.indexOf("GET /") != -1)//leitura parâmetro
          {
            beginGet = currentLine.indexOf("GET /");
            dadosGet = currentLine.substring(beginGet, 100);
            Serial.println("Dados GET");
            Serial.println(dadosGet);
            /*
            if (litros.length()&&litros.length()<22)
            {
              time_t tt = time(NULL);//Obtem o tempo atual em segundos
              data_D = *gmtime(&tt);//Converte o tempo atual e atribui na estrutura
              strftime(data_formatada, 64, "'%Y/%m/%d', '%H:%M:%S'", &data_D);//Cria uma String formatada da estrutura "data"
              Serial.println("Producao de leite diaria = " + litros + " litros");
              dataMessage_litros = "INSERT INTO registro_prod VALUES("+String(litros)+","+data_formatada+");"+"\r\n";
              Serial.println(dataMessage_litros);
              appendFile(SD, "/Producao.txt", dataMessage_litros.c_str());
              digitalWrite(LED_AZUL, HIGH);
              delay(100);
              digitalWrite(LED_AZUL, LOW);
            }
            */
          }

          if (currentLine.length() == 0)
          {
            delay(2000);
            getReadings();

            //client.println("HTTP/1.1 200 OK");
            //client.println("Content-type:text/html");
            //client.println(temperature + "-" + humidity);
            //client.println(getReadings());
            client.println();
            break;
          }
          else
          {
            currentLine = "";
          }
        }
        else if (c != '\r')
        {
          currentLine += c;
        }
      }
    }
    client.stop();
    Serial.println("Client desconectado.");
  }
  else
  {
    delay(5000);
    getReadings();
  }
}

void getReadings()
{
  //sensors.requestTemperatures();
  //float temperature = sensors.getTempCByIndex(0);
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  float temperature2 = dht2.readTemperature();
  float humidity2 = dht2.readHumidity();
  bool onOffKey = false;

  if(!isnan(temperature)){
    Serial.print("DHT1 -> Temperatura: ");
    Serial.println(temperature);
  }
  
  if(!isnan(humidity)){
    Serial.print("DHT1 -> Umidade: ");
    Serial.println(humidity);
    Serial.println();
  }
   
  if(!isnan(temperature2)){
    Serial.print("DHT2 -> Temperatura: ");
    Serial.println(temperature2);
  }
  
  if(!isnan(humidity2)){
    Serial.print("DHT2 -> Umidade: ");
    Serial.println(humidity2);
    Serial.println();
  }
  


}

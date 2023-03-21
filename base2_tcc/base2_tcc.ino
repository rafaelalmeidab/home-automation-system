//____params?esp32led=off&infrared=off&relay=on

#include <WiFi.h>
#include <WebServer.h>

#define esp32led 2
#define infrared 22
#define relay 23

//const char* ssid = "Rafael - 2.4G";
//const char* password= "34627545";

//const char* ssid = "Rio Cable - Rafael 2.4";
//const char* password= "rafael1061";

const char* ssid = "RBMWEB";
const char* password= "rbmweb01";

WebServer server(80); // create a web server on port 80

void handleRoot() { // handle the root URL (/)
  server.send(200, "text/html", "<h1>Hello World!</h1>");
}

void handleParams() { // handle URL with parameters (/params?param1=value1&param2=value2)
  String ESP32LedParam  = server.arg("esp32led");
  String infraredParam  = server.arg("infrared");
  String relayParam     = server.arg("relay");

  //ESP32Led Parameters
  if (ESP32LedParam == "on") { 
    digitalWrite(esp32led, HIGH);
  } 
  else if (ESP32LedParam == "off") { 
    digitalWrite(esp32led, LOW);
  }
  
  //Infrared Parameters
  if (infraredParam == "on") { 
    digitalWrite(infrared, HIGH);
  } 
  else if (infraredParam == "off") { 
    digitalWrite(infrared, LOW);
  }
  
  //Relay Parameters
  if (relayParam == "on") { 
    digitalWrite(relay, HIGH);
  } 
  else if (relayParam == "off") { 
    digitalWrite(relay, LOW);
  }
  
  //Send a response back to the client
  server.send(200, "text/html", "Parameters received.<br><br>ESP32 Led: " + ESP32LedParam + "<br><br>Infrared: " + infraredParam + "<br><br>Relay: " + relayParam + "<br>");
}

void setup() { 
  pinMode(esp32led, OUTPUT); 
  pinMode(infrared, OUTPUT); 
  pinMode(relay, OUTPUT);
  
  Serial.begin(115200); // start serial communication
  WiFi.begin(ssid, password); // connect to Wi-Fi network

  while (WiFi.status() != WL_CONNECTED) { // wait for connection
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP()); // print local IP address

  server.on("/", handleRoot); // handle root URL
  server.on("/params", handleParams); // handle URL with parameters

  server.begin(); // start the server
}

void loop() {
  server.handleClient(); // handle client requests
}

//____params?param1=on&param2=value2

#include <WiFi.h>
#include <WebServer.h>

#define blink_led 2
#define relay 23

//const char* ssid = "Rafael - 2.4G";
//const char* password= "34627545";

const char* ssid = "Rio Cable - Rafael 2.4";
const char* password= "rafael1061";

WebServer server(80); // create a web server on port 80

void handleRoot() { // handle the root URL (/)
  server.send(200, "text/html", "<h1>Hello World!</h1>");
}

void handleParams() { // handle URL with parameters (/params?param1=value1&param2=value2)
  String param1Value = server.arg("param1"); // get the value of parameter 1
  String param2Value = server.arg("param2"); // get the value of parameter 2
  String relayParam  = server.arg("relay");
  
  if (param1Value == "on") { // if parameter 1 is "on"
    digitalWrite(blink_led, HIGH); // turn on the LED
  } else if (param1Value == "off") { // if parameter 1 is "off"
    digitalWrite(blink_led, LOW); // turn off the LED
  }

  /*
  if (param2Value.toInt() > 50) { // if parameter 2 is greater than 50
    int i = 0;
    while(i<20){
      digitalWrite(blink_led, HIGH);
      delay(100);
      digitalWrite(blink_led, LOW);
      delay(100);
      i++;
    }
  }
  else if (param2Value.toInt() <= 50) { // if parameter 2 is greater than 50
      digitalWrite(blink_led, HIGH);
      delay(2000);
      digitalWrite(blink_led, LOW);
  }
  */
  
  //Relay Parameters
  if (relayParam == "on") { // if parameter 1 is "on"
    digitalWrite(relay, HIGH); // turn on the LED
  } else if (relayParam == "off") { // if parameter 1 is "off"
    digitalWrite(relay, LOW); // turn off the LED
  }
  
  server.send(200, "text/html", "Parameters received.<br><br>Param1Value: " + param1Value + "<br>Param2Value: " + param2Value + "<br><br>Relay: " + relayParam + "<br>"); // send a response back to the client
}

void setup() {
  pinMode(blink_led, OUTPUT); // set the built-in LED pin as an output
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


#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#include <OneWire.h>
#include <DallasTemperature.h>

//Static IP address configuration
IPAddress staticIP(192, 168, 1, 160); //ESP static ip
IPAddress gateway(192, 168, 1, 1);   //IP Address of your WiFi Router (Gateway)
IPAddress subnet(255, 255, 255, 0);  //Subnet mask
IPAddress dns(8, 8, 8, 8);  //DNS

const char* ssid = "SBU";
const char* password = "123456YHN";

ESP8266WebServer server(80);
///------------------------------ Temp------------------------------------------//
#define ONE_WIRE_BUS 2
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
float temp = 0 , dtemp = 25;
///------------------------------ pins------------------------------------------//
int Pin_relay = 0, Pin_temp = 2;
///------------------------------ ethernet------------------------------------------//
String relay = "off";

void setup() {
  pinMode(Pin_relay, OUTPUT);
  digitalWrite(Pin_relay, LOW);
  ///------------------------------ Serial------------------------------------------//
  Serial.begin(9600);
  ///------------------------------ ethernet------------------------------------------//
  WiFi.config(staticIP, subnet, gateway, dns);
  WiFi.begin(ssid, password);

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  server.on("/", []() {
    server.send(1000, "text/html", ResponseHtml());
  });

  // And as regular external functions:
  server.on("/temp", handle_temp);

  server.begin();
  sensors.begin();
  Serial.println("Start");
}
void loop() {
  server.handleClient();
  sensors.requestTemperatures();
  temp = sensors.getTempCByIndex(0);
  if (temp > (dtemp + 1))
  {
    digitalWrite(Pin_relay, HIGH);
  }
  else if (temp < (dtemp - 1))
  {
    digitalWrite(Pin_relay, LOW);
  }
  delay(10);
}

void handle_temp()
{
  dtemp = server.arg("dtemp").toInt();

  server.send(1000, "text/html", ResponseHtml());
}
String ResponseHtml()
{
  String html = "<!DOCTYPE html> <html> <head> <title>TEMP CONTROLER</title> <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"> </head> <style> #idsub { height: 50px; width: 200px; border-radius:10%; border: 3px solid blue; background-color:rgba(255,100,100); } #idrang{ width: 200px; } </style> <body onload=\"StartNew()\"> <P>&nbsp&nbsp&nbsp Temperature = ";
  html += temp;
  html += "</P> <P>&nbsp&nbsp&nbsp Desire Threshold = <span id=\"iddtemp\">";
  html += dtemp;
  html += "</span></P> <form action=\"temp\"method=\"post\" target=\"_self\"> &nbsp&nbsp&nbsp<input type=\"range\" name=\"dtemp\" id=\"idrang\" min=\"15\" max=\"40\" onchange =\"showVal(this)\"> <br> &nbsp&nbsp&nbsp<input type=\"submit\" id=\"idsub\" value=\"Submit\"> </form> </body> <script> function StartNew() { document.getElementById(\"idrang\").value = document.getElementById(\"iddtemp\").innerHTML; } function showVal(e) { var bar = document.getElementById(\"iddtemp\"); bar.innerHTML = e.value } </script> </html>";
  return html;
}





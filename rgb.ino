#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>

const char* ssid     = "SSID";
const char* password = "SSID Password"; //Under 32 characters
const byte DNS_PORT = 53;
const int redLED = 0;   //D3
const int greenLED = 4; //D2
const int blueLED = 5;  //D1
IPAddress apIP(10, 0, 0, 59);
DNSServer dnsServer;
ESP8266WebServer webServer(80);

String webpage = ""
                 "<!DOCTYPE html><html><head><title>RGB control</title><meta name='mobile-web-app-capable' content='yes' />"
                 "<meta name='viewport' content='width=device-width' /></head><body style='margin: 0px; padding: 0px;'>"
                 "<canvas id='colorspace'></canvas></body>"
                 "<script type='text/javascript'>"
                 "(function () {"
                 " var canvas = document.getElementById('colorspace');"
                 " var ctx = canvas.getContext('2d');"
                 " function drawCanvas() {"
                 " var colours = ctx.createLinearGradient(0, 0, window.innerWidth, 0);"
                 " for(var i=0; i <= 360; i+=10) {"
                 " colours.addColorStop(i/360, 'hsl(' + i + ', 100%, 50%)');"
                 " }"
                 " ctx.fillStyle = colours;"
                 " ctx.fillRect(0, 0, window.innerWidth, window.innerHeight);"
                 " var luminance = ctx.createLinearGradient(0, 0, 0, ctx.canvas.height);"
                 " luminance.addColorStop(0, '#ffffff');"
                 " luminance.addColorStop(0.05, '#ffffff');"
                 " luminance.addColorStop(0.5, 'rgba(0,0,0,0)');"
                 " luminance.addColorStop(0.95, '#000000');"
                 " luminance.addColorStop(1, '#000000');"
                 " ctx.fillStyle = luminance;"
                 " ctx.fillRect(0, 0, ctx.canvas.width, ctx.canvas.height);"
                 " }"
                 " var eventLocked = false;"
                 " function handleEvent(clientX, clientY) {"
                 " if(eventLocked) {"
                 " return;"
                 " }"
                 " function colourCorrect(v) {"
                 " return Math.round(1023-(v*v)/64);"
                 " }"
                 " var data = ctx.getImageData(clientX, clientY, 1, 1).data;"
                 " var params = ["
                 " 'r=' + colourCorrect(data[0]),"
                 " 'g=' + colourCorrect(data[1]),"
                 " 'b=' + colourCorrect(data[2])"
                 " ].join('&');"
                 " var req = new XMLHttpRequest();"
                 " req.open('POST', '?' + params, true);"
                 " req.send();"
                 " eventLocked = true;"
                 " req.onreadystatechange = function() {"
                 " if(req.readyState == 4) {"
                 " eventLocked = false;"
                 " }"
                 " }"
                 " }"
                 " canvas.addEventListener('click', function(event) {"
                 " handleEvent(event.clientX, event.clientY, true);"
                 " }, false);"
                 " canvas.addEventListener('touchmove', function(event){"
                 " handleEvent(event.touches[0].clientX, event.touches[0].clientY);"
                 "}, false);"
                 " function resizeCanvas() {"
                 " canvas.width = window.innerWidth;"
                 " canvas.height = window.innerHeight;"
                 " drawCanvas();"
                 " }"
                 " window.addEventListener('resize', resizeCanvas, false);"
                 " resizeCanvas();"
                 " drawCanvas();"
                 " document.ontouchmove = function(e) {e.preventDefault()};"
                 " })();"
                 "</script></html>";

//////////////////////////////////////////////////////////////////////////////////////////////////

void handleRoot() {
  // Serial.println("handle root..");
  String red = webServer.arg(0); // read RGB arguments
  String green = webServer.arg(1);
  String blue = webServer.arg(2);

  if ((red != "") && (green != "") && (blue != ""))
  {
    analogWrite(redLED, 1023 - red.toInt());
    analogWrite(greenLED, 1023 - green.toInt());
    analogWrite(blueLED, 1023 - blue.toInt());
  }


  Serial.print("Red: ");
  Serial.println(red.toInt());
  Serial.print("Green: ");
  Serial.println(green.toInt());
  Serial.print("Blue: ");
  Serial.println(blue.toInt());
  Serial.println();

  webServer.send(200, "text/html", webpage);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

void setup() {

  pinMode(redLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  pinMode(blueLED, OUTPUT);
  analogWrite(redLED, 0);
  analogWrite(greenLED, 0);
  analogWrite(blueLED, 0);

  Serial.begin(115200);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  webServer.on("/", handleRoot);
  webServer.begin();

  testRGB();
}

//////////////////////////////////////////////////////////////////////////////////////////////////

void loop() {
  webServer.handleClient();
}
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

void testRGB() { // fade in and out of Red, Green, Blue

  analogWrite(redLED, 0); // R off
  analogWrite(greenLED, 0); // G off
  analogWrite(blueLED, 0); // B off

  fade(redLED); // R
  fade(greenLED); // G
  fade(blueLED); // B
}

//////////////////////////////////////////////////////////////////////////////////////////////////

void fade(int pin) {

  for (int u = 0; u < 1024; u++) {
    analogWrite(pin, u);
    delay(1);
  }
  for (int u = 0; u < 1024; u++) {
    analogWrite(pin, 1023 - u);
    delay(1);
  }
}

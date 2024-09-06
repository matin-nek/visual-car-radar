#include <Arduino.h>
#include <SPI.h>
#include <TFT_eSPI.h> // Hardware-specific library
#include <WiFi.h>
#include <WebServer.h>
#define TRIG_PIN_1 21
#define ECHO_PIN_1 22

#define TRIG_PIN_2 17
#define ECHO_PIN_2 16

#define TRIG_PIN_3 26
#define ECHO_PIN_3 25

#define TRIG_PIN_4 15
#define ECHO_PIN_4 2

#define SOUND_SPEED 0.0343 // Speed of sound in cm/us

#define BUZZER_PIN 23 // Pin connected to the buzzer

#define TONE_ZONE1 10 // Frequency for Zone 1 (closest)
#define TONE_ZONE2 5  // Frequency for Zone 2 (medium distance)
#define TONE_ZONE3 1  // Frequency for Zone 3 (far distance)

TFT_eSPI tft = TFT_eSPI(); // Create an instance of the TFT_eSPI library

// Define constants for drawing on the TFT
#define CAR_X 90
#define CAR_Y 100
#define CAR_W 60
#define CAR_H 120
#define CAR_R 15

#define MIN_DIST 10  // Define minimum distance
#define MAX_DIST 80 // Define maximum distance

// Define distance zones for color mapping
int Zone1 = 30, Zone2 = 50, Zone3 = 80;

volatile unsigned long duration1, duration2, duration3, duration4;
volatile unsigned long startTime1, startTime2, startTime3, startTime4;

int lastDistance1 = CAR_R + 5, lastDistance2 = CAR_R + 5, lastDistance3 = CAR_R + 5, lastDistance4 = CAR_R + 5;

WebServer server(80);

void IRAM_ATTR echoISR1()
{
  if (digitalRead(ECHO_PIN_1) == HIGH)
  {
    startTime1 = micros();
  }
  else
  {
    duration1 = micros() - startTime1;
  }
}

void IRAM_ATTR echoISR2()
{
  if (digitalRead(ECHO_PIN_2) == HIGH)
  {
    startTime2 = micros();
  }
  else
  {
    duration2 = micros() - startTime2;
  }
}

void IRAM_ATTR echoISR3()
{
  if (digitalRead(ECHO_PIN_3) == HIGH)
  {
    startTime3 = micros();
  }
  else
  {
    duration3 = micros() - startTime3;
  }
}

void IRAM_ATTR echoISR4()
{
  if (digitalRead(ECHO_PIN_4) == HIGH)
  {
    startTime4 = micros();
  }
  else
  {
    duration4 = micros() - startTime4;
  }
}

int returnColor(int Distance)
{
  return Distance < Zone1 ? TFT_RED : Distance < Zone2 ? TFT_ORANGE : Distance < Zone3   ? TFT_YELLOW : TFT_GREEN;
}

void setupHTTPServer()
{
  server.on("/", handleRoot);
  server.on("/setZones", HTTP_POST, handleSetZones);
  server.onNotFound(handleNotFound);
  server.begin();
}

void handleRoot()
{
  String html = "<!DOCTYPE html>";
  html += "<html>";
  html += "<head>";
  html += "<title>SRF05 Zone Control</title>";
  html += "<style>";
  html += "body { font-family: Arial, sans-serif; text-align: center; background-color: #f4f4f4; padding: 20px; }";
  html += "h1 { color: #333; }";
  html += ".zone-input { margin: 10px 0; }";
  html += ".zone-label { font-size: 20px; margin-right: 10px; }";
  html += "input[type='number'] { padding: 8px; width: 80px; font-size: 16px; }";
  html += "input[type='submit'] { background-color: #4CAF50; color: white; padding: 10px 20px; border: none; border-radius: 5px; cursor: pointer; font-size: 16px; }";
  html += "input[type='submit']:hover { background-color: #45a049; }";
  html += "</style>";
  html += "</head>";
  html += "<body>";
  html += "<h1>SRF05 Zone Control</h1>";
  html += "<form action='/setZones' method='POST'>";

  html += "<div class='zone-input'>";
  html += "<label class='zone-label' for='zone1'>Zone 1:</label>";
  html += "<input type='number' id='zone1' name='zone1' value='" + String(Zone1) + "'><br>";
  html += "</div>";

  html += "<div class='zone-input'>";
  html += "<label class='zone-label' for='zone2'>Zone 2:</label>";
  html += "<input type='number' id='zone2' name='zone2' value='" + String(Zone2) + "'><br>";
  html += "</div>";

  html += "<div class='zone-input'>";
  html += "<label class='zone-label' for='zone3'>Zone 3:</label>";
  html += "<input type='number' id='zone3' name='zone3' value='" + String(Zone3) + "'><br>";
  html += "</div>";

  html += "<input type='submit' value='Set Zones'>";
  html += "</form>";
  html += "</body>";
  html += "</html>";

  server.send(200, "text/html", html);
}

void handleSetZones()
{
  if (server.hasArg("zone1"))
    Zone1 = server.arg("zone1").toInt();
  if (server.hasArg("zone2"))
    Zone2 = server.arg("zone2").toInt();
  if (server.hasArg("zone3"))
    Zone3 = server.arg("zone3").toInt();

  // Redirect back to the main page
  server.sendHeader("Location", "/");
  server.send(303);
}

void handleNotFound()
{
  // String path = server.uri();
  // if (path != "")
  // {
  //   String file = readFile(path);
  //   String type = getContentType(path);
  //   HTTPSend(file, type);
  // }
  // else
  // {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++)
  {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  // }
}

void setup()
{
  Serial.begin(115200);

  WiFi.softAP("Radar");
  WiFi.mode(WIFI_AP);

  // if (!WiFi.softAP("Radar")) {
  //   log_e("Soft AP creation failed.");
  //   while (1);
  // }
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);

  // Initialize TFT
  tft.init();
  tft.setRotation(2);
  tft.fillScreen(TFT_BLACK);
  tft.fillRoundRect(CAR_X, CAR_Y, CAR_W, CAR_H, CAR_R, TFT_RED);

  // Setup SRF05 sensors
  pinMode(TRIG_PIN_1, OUTPUT);
  pinMode(ECHO_PIN_1, INPUT_PULLUP);
  pinMode(TRIG_PIN_2, OUTPUT);
  pinMode(ECHO_PIN_2, INPUT_PULLUP);
  pinMode(TRIG_PIN_3, OUTPUT);
  pinMode(ECHO_PIN_3, INPUT_PULLUP);
  pinMode(TRIG_PIN_4, OUTPUT);
  pinMode(ECHO_PIN_4, INPUT_PULLUP);

  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH);

  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, HIGH);
  delay(200);
  digitalWrite(BUZZER_PIN, LOW);

  attachInterrupt(digitalPinToInterrupt(ECHO_PIN_1), echoISR1, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ECHO_PIN_2), echoISR2, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ECHO_PIN_3), echoISR3, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ECHO_PIN_4), echoISR4, CHANGE);

  // Setup web server
  setupHTTPServer();
}

void loop()
{
  server.handleClient();

  // Trigger each sensor one by one
  triggerSensor(TRIG_PIN_1);
  // delay(60); // Ensure at least 60ms between each trigger

  triggerSensor(TRIG_PIN_2);
  // delay(60);

  triggerSensor(TRIG_PIN_3);
  // delay(60);

  triggerSensor(TRIG_PIN_4);
  // delay(60);

  // Calculate distances
  int distance1 = duration1 * SOUND_SPEED / 2;
  int distance2 = duration2 * SOUND_SPEED / 2;
  int distance3 = duration3 * SOUND_SPEED / 2;
  int distance4 = duration4 * SOUND_SPEED / 2;

  distance1 = constrain(distance1, MIN_DIST, MAX_DIST);
  distance2 = constrain(distance2, MIN_DIST, MAX_DIST);
  distance3 = constrain(distance3, MIN_DIST, MAX_DIST);
  distance4 = constrain(distance4, MIN_DIST, MAX_DIST);

  // Display the distance values in the corners
  tft.setTextColor(TFT_WHITE, TFT_BLACK); // Set text color to white with black background

  tft.fillRect(58, 0, 8, 16, TFT_BLACK);
  tft.drawString(String(distance1) + " cm", 0, 0, 2); // Top-left corner
  tft.fillRect(232, 0, 8, 16, TFT_BLACK);
  tft.drawString(String(distance2) + " cm", 174, 0, 2); // Top-right corner
  tft.fillRect(58, 304, 8, 16, TFT_BLACK);
  tft.drawString(String(distance3) + " cm", 0, 320 - 16, 2); // Bottom-left corner
  tft.fillRect(232, 304, 8, 16, TFT_BLACK);
  tft.drawString(String(distance4) + " cm", 174, 320 - 16, 2); // Bottom-right corner

  uint16_t color1 = returnColor(distance1);
  uint16_t color2 = returnColor(distance2);
  uint16_t color3 = returnColor(distance3);
  uint16_t color4 = returnColor(distance4);

  // Map and constrain distances for visual representation
  distance1 = map(distance1, MIN_DIST, MAX_DIST, CAR_R + 5, 100);
  distance2 = map(distance2, MIN_DIST, MAX_DIST, CAR_R + 5, 100);
  distance3 = map(distance3, MIN_DIST, MAX_DIST, CAR_R + 5, 100);
  distance4 = map(distance4, MIN_DIST, MAX_DIST, CAR_R + 5, 100);

  // Erase previous arcs
  if (distance1 < lastDistance1)
    tft.drawArc(CAR_X + CAR_R, CAR_Y + CAR_R, lastDistance1, CAR_R + 5, 90, 180, TFT_BLACK, TFT_BLACK, true);
  if (distance2 < lastDistance2)
    tft.drawArc(CAR_X + CAR_W - CAR_R, CAR_Y + CAR_R, lastDistance2, CAR_R + 5, 180, 270, TFT_BLACK, TFT_BLACK, true);
  if (distance3 < lastDistance3)
    tft.drawArc(CAR_X + CAR_R, CAR_Y + CAR_H - CAR_R, lastDistance3, CAR_R + 5, 0, 90, TFT_BLACK, TFT_BLACK, true);
  if (distance4 < lastDistance4)
    tft.drawArc(CAR_X + CAR_W - CAR_R, CAR_Y + CAR_H - CAR_R, lastDistance4, CAR_R + 5, 270, 360, TFT_BLACK, TFT_BLACK, true);

  // Draw new arcs
  tft.drawArc(CAR_X + CAR_R, CAR_Y + CAR_R, distance1, CAR_R + 5, 90, 180, color1, TFT_RED, true);
  tft.drawArc(CAR_X + CAR_W - CAR_R, CAR_Y + CAR_R, distance2, CAR_R + 5, 180, 270, color2, TFT_RED, true);
  tft.drawArc(CAR_X + CAR_R, CAR_Y + CAR_H - CAR_R, distance3, CAR_R + 5, 0, 90, color3, TFT_RED, true);
  tft.drawArc(CAR_X + CAR_W - CAR_R, CAR_Y + CAR_H - CAR_R, distance4, CAR_R + 5, 270, 360, color4, TFT_RED, true);

  // Store current distances
  lastDistance1 = distance1;
  lastDistance2 = distance2;
  lastDistance3 = distance3;
  lastDistance4 = distance4;

  // Find the minimum distance among all sensors
  int minDistance = min(min(distance1, distance2), min(distance3, distance4));

  // Determine the zone of the closest object based on the minimum distance
  int closestZone = determineZone(minDistance);

  // Control the buzzer based on the closest zone
  controlBuzzer(closestZone);


  delay(1000);
}

int determineZone(int distance) {
  if (distance < Zone1) {
    return 1;
  } else if (distance < Zone2) {
    return 2;
  } else if (distance < Zone3) {
    return 3;
  } else {
    return 4;
  }
}

void controlBuzzer(int zone) {
  switch (zone) {
    case 1:
      tone(BUZZER_PIN, TONE_ZONE1);
      break;
    case 2:
      tone(BUZZER_PIN, TONE_ZONE2);
      break;
    case 3:
      tone(BUZZER_PIN, TONE_ZONE3);
      break;
    case 4:
    default:
      noTone(BUZZER_PIN); // Stop the buzzer in Zone 4
      break;
  }
}

void triggerSensor(int trigPin)
{
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
}
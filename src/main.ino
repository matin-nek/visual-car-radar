#include <Arduino.h>
#include <SPI.h>
#include <TFT_eSPI.h> // Hardware-specific library
#include <WiFi.h>
#define TRIG_PIN_1 16
#define ECHO_PIN_1 17

#define TRIG_PIN_2 5
#define ECHO_PIN_2 18

#define TRIG_PIN_3 21
#define ECHO_PIN_3 19

#define TRIG_PIN_4 15
#define ECHO_PIN_4 2

#define SOUND_SPEED 0.0343 // Speed of sound in cm/us

TFT_eSPI tft = TFT_eSPI(); // Create an instance of the TFT_eSPI library

// Define constants for drawing on the TFT
#define CAR_X 90
#define CAR_Y 100
#define CAR_W 60
#define CAR_H 120
#define CAR_R 15

#define MIN_DIST 10  // Define minimum distance
#define MAX_DIST 400 // Define maximum distance

// Define distance zones for color mapping
int Zone1 = 30, Zone2 = 50, Zone3 = 80;

volatile unsigned long duration1, duration2, duration3, duration4;
volatile unsigned long startTime1, startTime2, startTime3, startTime4;

int lastDistance1 = CAR_R + 5, lastDistance2 = CAR_R + 5, lastDistance3 = CAR_R + 5, lastDistance4 = CAR_R + 5;

void IRAM_ATTR echoISR1() {
  if (digitalRead(ECHO_PIN_1) == HIGH) {
    startTime1 = micros();
  } else {
    duration1 = micros() - startTime1;
  }
}

void IRAM_ATTR echoISR2() {
  if (digitalRead(ECHO_PIN_2) == HIGH) {
    startTime2 = micros();
  } else {
    duration2 = micros() - startTime2;
  }
}

void IRAM_ATTR echoISR3() {
  if (digitalRead(ECHO_PIN_3) == HIGH) {
    startTime3 = micros();
  } else {
    duration3 = micros() - startTime3;
  }
}

void IRAM_ATTR echoISR4() {
  if (digitalRead(ECHO_PIN_4) == HIGH) {
    startTime4 = micros();
  } else {
    duration4 = micros() - startTime4;
  }
}

int returnColor(int Distance) {
  return Distance < Zone1 ? TFT_RED : Distance < Zone2 ? TFT_ORANGE
                                  : Distance < Zone3   ? TFT_YELLOW
                                                       : TFT_GREEN;
}

void setup() {
  Serial.begin(115200);

  // Initialize TFT
  tft.init();
  tft.setRotation(2);

  // Clear screen and draw a rounded rectangle
  tft.fillScreen(TFT_BLACK);
  tft.fillRoundRect(CAR_X, CAR_Y, CAR_W, CAR_H, CAR_R, TFT_RED);

  pinMode(TRIG_PIN_1, OUTPUT);
  pinMode(ECHO_PIN_1, INPUT);
  pinMode(TRIG_PIN_2, OUTPUT);
  pinMode(ECHO_PIN_2, INPUT);
  pinMode(TRIG_PIN_3, OUTPUT);
  pinMode(ECHO_PIN_3, INPUT);
  pinMode(TRIG_PIN_4, OUTPUT);
  pinMode(ECHO_PIN_4, INPUT);

  attachInterrupt(digitalPinToInterrupt(ECHO_PIN_1), echoISR1, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ECHO_PIN_2), echoISR2, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ECHO_PIN_3), echoISR3, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ECHO_PIN_4), echoISR4, CHANGE);
}

void loop() {
  // Trigger each sensor one by one
  triggerSensor(TRIG_PIN_1);
  delay(60); // Ensure at least 60ms between each trigger

  triggerSensor(TRIG_PIN_2);
  delay(60);

  triggerSensor(TRIG_PIN_3);
  delay(60);

  triggerSensor(TRIG_PIN_4);
  delay(60);

  // Calculate distances
  float distance1 = duration1 * SOUND_SPEED / 2;
  float distance2 = duration2 * SOUND_SPEED / 2;
  float distance3 = duration3 * SOUND_SPEED / 2;
  float distance4 = duration4 * SOUND_SPEED / 2;

  // Map and constrain distances for visual representation
  distance1 = map(constrain(distance1, MIN_DIST, MAX_DIST), MIN_DIST, MAX_DIST, CAR_R + 5, 100);
  distance2 = map(constrain(distance2, MIN_DIST, MAX_DIST), MIN_DIST, MAX_DIST, CAR_R + 5, 100);
  distance3 = map(constrain(distance3, MIN_DIST, MAX_DIST), MIN_DIST, MAX_DIST, CAR_R + 5, 100);
  distance4 = map(constrain(distance4, MIN_DIST, MAX_DIST), MIN_DIST, MAX_DIST, CAR_R + 5, 100);

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
  tft.drawArc(CAR_X + CAR_R, CAR_Y + CAR_R, distance1, CAR_R + 5, 90, 180, returnColor(distance1), TFT_RED, true);
  tft.drawArc(CAR_X + CAR_W - CAR_R, CAR_Y + CAR_R, distance2, CAR_R + 5, 180, 270, returnColor(distance2), TFT_RED, true);
  tft.drawArc(CAR_X + CAR_R, CAR_Y + CAR_H - CAR_R, distance3, CAR_R + 5, 0, 90, returnColor(distance3), TFT_RED, true);
  tft.drawArc(CAR_X + CAR_W - CAR_R, CAR_Y + CAR_H - CAR_R, distance4, CAR_R + 5, 270, 360, returnColor(distance4), TFT_RED, true);

  // Display the distance values in the corners
  tft.setTextColor(TFT_WHITE, TFT_BLACK); // Set text color to white with black background

  tft.fillRect(58,0,8,16,TFT_BLACK);
  tft.drawString(String(distance1) + " cm", 0, 0, 2);               // Top-left corner
  tft.fillRect(232,0,8,16,TFT_BLACK);
  tft.drawString(String(distance2) + " cm", 174, 0, 2);             // Top-right corner
  tft.fillRect(58,304,8,16,TFT_BLACK);
  tft.drawString(String(distance3) + " cm", 0, 320 - 16, 2);        // Bottom-left corner
  tft.fillRect(232,304,8,16,TFT_BLACK);
  tft.drawString(String(distance4) + " cm", 174, 320 - 16, 2);      // Bottom-right corner

  // Store current distances
  lastDistance1 = distance1;
  lastDistance2 = distance2;
  lastDistance3 = distance3;
  lastDistance4 = distance4;

  delay(1000);
}

void triggerSensor(int trigPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
}
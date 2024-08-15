#include <Arduino.h>
#include <SPI.h>
#include <TFT_eSPI.h> // Hardware-specific library
#include <BluetoothSerial.h>

TFT_eSPI tft = TFT_eSPI(); // Invoke custom library

#define SOUND_SPEED 0.0343 // Speed of sound in cm/us

// Define pins for each sensor
#define TRIG_PIN_1 16
#define ECHO_PIN_1 17

#define TRIG_PIN_2 5
#define ECHO_PIN_2 18

#define TRIG_PIN_3 21
#define ECHO_PIN_3 19

#define TRIG_PIN_4 4
#define ECHO_PIN_4 15

#define test_samples 4

float distances1[test_samples];
float distances2[test_samples];
float distances3[test_samples];
float distances4[test_samples];
float distance1;
float distance2;
float distance3;
float distance4;
BluetoothSerial serialbt;
void setup()
{
  Serial.begin(115200); // Start serial communication

  // Set pin modes for all sensors
  pinMode(TRIG_PIN_1, OUTPUT);
  pinMode(ECHO_PIN_1, INPUT);

  pinMode(TRIG_PIN_2, OUTPUT);
  pinMode(ECHO_PIN_2, INPUT);

  pinMode(TRIG_PIN_3, OUTPUT);
  pinMode(ECHO_PIN_3, INPUT);

  pinMode(TRIG_PIN_4, OUTPUT);
  pinMode(ECHO_PIN_4, INPUT);

  tft.init();
  tft.setRotation(2);

  tft.fillScreen(TFT_BLACK);

  tft.fillRoundRect(90, 100, 60, 120, 20, TFT_RED);
}

float measureDistance(int trigPin, int echoPin)
{
  // Clear the trigger pin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  // Send a 10 microsecond pulse to the trigger pin
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Read the echo pin
  long duration = pulseIn(echoPin, HIGH);

  // Calculate the distance in centimeters
  float distance = (duration / 2.0) * SOUND_SPEED;

  return distance;
}

void loop()
{

  tft.drawArc(90, 100, 100, 5, 128, 143, TFT_BLACK, TFT_BLACK, true);
  tft.drawArc(150, 100, 100, 5, 217, 232, TFT_BLACK, TFT_BLACK, true);
  tft.drawArc(90, 220, 100, 5, 37, 52, TFT_BLACK, TFT_BLACK, true);
  tft.drawArc(150, 220, 100, 5, 307, 322, TFT_BLACK, TFT_BLACK, true);
  // Measure distance from each sensor
  for (int i = 0; i < test_samples; i++)
  {
    distances1[i] = measureDistance(TRIG_PIN_1, ECHO_PIN_1);
    distances2[i] = measureDistance(TRIG_PIN_2, ECHO_PIN_2);
    distances3[i] = measureDistance(TRIG_PIN_3, ECHO_PIN_3);
    distances4[i] = measureDistance(TRIG_PIN_4, ECHO_PIN_4);
    if (i == 0)
    {
      distance1 = distances1[i];
      distance2 = distances2[i];
      distance3 = distances3[i];
      distance4 = distances4[i];
    }
    else
    {
      distance1 = (distance1 + distances1[i]) / 2;
      distance2 = (distance2 + distances2[i]) / 2;
      distance3 = (distance3 + distances3[i]) / 2;
      distance4 = (distance4 + distances4[i]) / 2;
    }
  }

  // Print the distances to the Serial Monitor
  Serial.print("Distance Sensor 1: ");
  Serial.print(distance1);
  Serial.println(" cm");

  Serial.print("Distance Sensor 2: ");
  Serial.print(distance2);
  Serial.println(" cm");

  Serial.print("Distance Sensor 3: ");
  Serial.print(distance3);
  Serial.println(" cm");

  Serial.print("Distance Sensor 4: ");
  Serial.print(distance4);
  Serial.println(" cm");

  tft.setCursor(10, 10);
  tft.print(distance1);
  tft.drawArc(90, 100, distance1 / 3, 5, 128, 143, TFT_DARKGREEN, TFT_GREEN, true);
  tft.setCursor(200, 10);
  tft.print(distance2);
  tft.drawArc(150, 100, distance2 / 3, 5, 217, 232, TFT_DARKGREEN, TFT_GREEN, true);
  tft.setCursor(10, 300);
  tft.print(distance3);
  tft.drawArc(90, 220, distance3 / 3, 5, 37, 52, TFT_DARKGREEN, TFT_GREEN, true);
  tft.setCursor(200, 300);
  tft.print(distance4);
  tft.drawArc(150, 220, distance4 / 3, 5, 307, 322, TFT_DARKGREEN, TFT_GREEN, true);

  // Small delay before next measurement cycle
  delay(500);
}

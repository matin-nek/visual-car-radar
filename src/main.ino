#include <Arduino.h>
#define SOUND_SPEED 0.0343  // Speed of sound in cm/us

// Define pins for each sensor
#define TRIG_PIN_1 5
#define ECHO_PIN_1 18

#define TRIG_PIN_2 19
#define ECHO_PIN_2 21

#define TRIG_PIN_3 23
#define ECHO_PIN_3 22

#define TRIG_PIN_4 4
#define ECHO_PIN_4 15

void setup() {
  Serial.begin(115200);  // Start serial communication

  // Set pin modes for all sensors
  pinMode(TRIG_PIN_1, OUTPUT);
  pinMode(ECHO_PIN_1, INPUT);

  pinMode(TRIG_PIN_2, OUTPUT);
  pinMode(ECHO_PIN_2, INPUT);

  pinMode(TRIG_PIN_3, OUTPUT);
  pinMode(ECHO_PIN_3, INPUT);

  pinMode(TRIG_PIN_4, OUTPUT);
  pinMode(ECHO_PIN_4, INPUT);
}

float measureDistance(int trigPin, int echoPin) {
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

void loop() {
  // Measure distance from each sensor
  float distance1 = measureDistance(TRIG_PIN_1, ECHO_PIN_1);
  float distance2 = measureDistance(TRIG_PIN_2, ECHO_PIN_2);
  float distance3 = measureDistance(TRIG_PIN_3, ECHO_PIN_3);
  float distance4 = measureDistance(TRIG_PIN_4, ECHO_PIN_4);

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

  // Small delay before next measurement cycle
  delay(500);
}

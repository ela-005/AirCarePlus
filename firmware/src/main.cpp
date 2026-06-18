#include <Arduino.h>
const int LED_PIN = 2;
void setup() {
  // put your setup code here, to run once:
  pinMode(LED_PIN,OUTPUT);
  Serial.begin(115200);
  Serial.println("ESP32 Hello World started");
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(LED_PIN, HIGH);
  Serial.println("LED ON");
  delay(1000);
  digitalWrite(LED_PIN,LOW);
  Serial.println("LED OFF");
  delay(1000);
}

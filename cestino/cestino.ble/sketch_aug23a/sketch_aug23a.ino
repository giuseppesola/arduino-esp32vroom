#include <ESP32Servo.h>
#include <NewPing.h>
#include <BluetoothSerial.h>

#define TRIG_PIN 5
#define ECHO_PIN 18
#define SERVO_PIN 17
#define MAX_DISTANCE 200 // Distanza massima in cm

// Configurazione Bluetooth
BluetoothSerial SerialBT;

Servo servo;
NewPing sonar(TRIG_PIN, ECHO_PIN, MAX_DISTANCE);

void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32_Servo_Controller"); // Nome del dispositivo Bluetooth
  servo.attach(SERVO_PIN);
  servo.write(180);  // Chiude il coperchio del cestino

  Serial.println("Bluetooth inizializzato.");
  Serial.println("In attesa di connessione Bluetooth...");
}

void loop() {
  // Controllo della connessione Bluetooth
  if (SerialBT.available()) {
    String command = SerialBT.readStringUntil('\n');  // Legge la richiesta del client Bluetooth
    command.trim();  // Rimuove spazi bianchi

    if (command.equals("open")) {
      servo.write(0);  // Apre il coperchio completamente
      Serial.println("Coperchio aperto.");
    }
    if (command.equals("close")) {
      servo.write(180);  // Chiude il coperchio
      Serial.println("Coperchio chiuso.");
    }
  }

  // Gestione del sensore di distanza
  delay(50);  // Piccola pausa per evitare falsi rilevamenti
  int distance = sonar.ping_cm();

  if (distance > 0 && distance < 20) {  // Se la mano è a meno di 20 cm
    servo.write(70);  // Apre il coperchio completamente
    delay(5000);  // Mantiene il coperchio aperto per 5 secondi
    servo.write(180);  // Chiude il coperchio
    Serial.println("Coperchio aperto per 5 secondi e poi chiuso.");
  }

  // Attesa breve prima di continuare il loop
  delay(500);  // Tempo di attesa per evitare continui cambiamenti
}

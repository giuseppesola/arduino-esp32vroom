#include <RCSwitch.h>
#include <ESP32Servo.h>
#include <NewPing.h>

#define TRIG_PIN 5
#define ECHO_PIN 18
#define SERVO_PIN 17
#define MAX_DISTANCE 200 // Distanza massima in cm

#define OPEN_RF_CODE 0x005DD530 // Codice RF per aprire
#define CLOSE_RF_CODE 0x005DD5C0 // Codice RF per chiudere

RCSwitch mySwitch = RCSwitch();
Servo servo;
NewPing sonar(TRIG_PIN, ECHO_PIN, MAX_DISTANCE);

bool isOpen = false;  // Stato del coperchio (aperto o chiuso)
unsigned long lastOpenedTime = 0;  // Tempo in cui il coperchio è stato aperto
const unsigned long OPEN_DURATION = 5000;  // Durata di apertura automatica (5 secondi)

void setup() {
  Serial.begin(115200);

  // Configura il pin di ricezione RF
  mySwitch.enableReceive(23); // Pin D23 (o qualsiasi pin supportato dal modulo RF)

  // Configura il servo
  servo.attach(SERVO_PIN);
  servo.write(180);  // Chiude il coperchio del cestino inizialmente

  Serial.println("Sistema pronto.");
}

void loop() {
  // Gestione del sensore di distanza
  delay(50);  // Piccola pausa per evitare falsi rilevamenti
  int distance = sonar.ping_cm();

  if (distance > 0 && distance < 20) {  // Se l'oggetto è a meno di 20 cm
    if (!isOpen) {
      servo.write(70);   // Apre il coperchio
      Serial.println("Coperchio aperto dal sensore.");
      isOpen = true;
      lastOpenedTime = millis();  // Memorizza il tempo in cui è stato aperto
    }
  }

  // Gestione del modulo RF
  if (mySwitch.available()) {
    // Ottiene il codice ricevuto
    unsigned long receivedCode = mySwitch.getReceivedValue();
    
    if (receivedCode == OPEN_RF_CODE) { // Verifica se il codice ricevuto corrisponde a quello per aprire
      if (!isOpen) {
        servo.write(70);   // Apre il coperchio
        Serial.println("Coperchio aperto con RF.");
        isOpen = true;
        lastOpenedTime = millis();  // Memorizza il tempo in cui è stato aperto
      }
    } else if (receivedCode == CLOSE_RF_CODE) { // Verifica se il codice ricevuto corrisponde a quello per chiudere
      if (isOpen) {
        servo.write(180);  // Chiude il coperchio
        Serial.println("Coperchio chiuso con RF.");
        isOpen = false;
      }
    } else {
      Serial.print("Codice ricevuto: ");
      Serial.println(receivedCode);
    }

    // Reset del ricevitore per il prossimo segnale
    mySwitch.resetAvailable();
  }

  // Controlla se il coperchio deve essere chiuso automaticamente
  if (isOpen && (millis() - lastOpenedTime > OPEN_DURATION)) {
    servo.write(180);  // Chiude il coperchio
    Serial.println("Coperchio chiuso automaticamente dopo timeout.");
    isOpen = false;
  }

  // Attesa breve prima di continuare il loop
  delay(500);  // Tempo di attesa per evitare continui cambiamenti
}

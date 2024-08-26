#include <Servo.h>

// Definisce i pin del sensore ad ultrasuoni
const int trigPin = 9;  // Pin del trigger
const int echoPin = 10; // Pin dell'eco

// Variabili per la durata e la distanza
long duration;
int distance;

// Crea un oggetto Servo per controllare il motore servo
Servo myServo;

void setup() {
  pinMode(trigPin, OUTPUT); // Imposta il pin trig come Uscita
  pinMode(echoPin, INPUT);  // Imposta il pin echo come Ingresso
  Serial.begin(9600);      // Inizia la comunicazione seriale
  myServo.attach(11);      // Definisce il pin a cui è collegato il motore servo (pin 11)
}

void loop() {
  // Ruota il motore servo da 15 a 165 gradi
  for (int i = 0; i <= 180; i +=1) {  
    myServo.write(i);
    delay(25); // Tempo di attesa per permettere al servo di raggiungere la posizione
    distance = calculateDistance(); // Chiama una funzione per calcolare la distanza misurata dal sensore ad ultrasuoni
    
    // Invia i dati tramite la porta seriale
    Serial.print(i);        // Invia l'angolo corrente
    Serial.print(",");     // Separatore tra angolo e distanza
    Serial.print(distance); // Invia il valore della distanza
    Serial.print(".");     // Separatore per il termine della misurazione
  }
  
  // Ripete la rotazione del servo da 165 a 15 gradi
  for (int i = 180; i >= 0; i -=1) {  
    myServo.write(i);
    delay(25); // Tempo di attesa per permettere al servo di raggiungere la posizione
    distance = calculateDistance();
    
    // Invia i dati tramite la porta seriale
    Serial.print(i);
    Serial.print(",");
    Serial.print(distance);
    Serial.print(".");
  }
}

// Funzione per calcolare la distanza misurata dal sensore ad ultrasuoni
int calculateDistance() { 
  digitalWrite(trigPin, LOW); 
  delayMicroseconds(2);
  
  // Imposta il pin trig su HIGH per 10 microsecondi
  digitalWrite(trigPin, HIGH); 
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  duration = pulseIn(echoPin, HIGH); // Legge il pin echo, restituisce il tempo di viaggio dell'onda sonora in microsecondi
  
  // Calcola la distanza in cm (0.034 cm/µs è la velocità del suono)
  distance = duration * 0.034 / 2; 
  
  return distance;
}

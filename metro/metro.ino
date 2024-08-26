#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Definizione dei pin per il sensore HC-SR04
#define TRIG_PIN 9
#define ECHO_PIN 10

// Definizione delle costanti per il display OLED
#define SCREEN_WIDTH 128 // Larghezza del display OLED
#define SCREEN_HEIGHT 64 // Altezza del display OLED
#define OLED_RESET    -1 // Reset pin (non utilizzato)

// Inizializzazione del display
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  // Inizializzazione del display
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Indirizzo I2C del display
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);

  // Inizializzazione dei pin del sensore HC-SR04
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  Serial.begin(115200); // Inizializzazione della comunicazione seriale
}

void loop() {
  // Misura della distanza
  long duration, distance;
  
  // Impulso del trigger
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Lettura del tempo di eco
  duration = pulseIn(ECHO_PIN, HIGH);

  // Calcolo della distanza in cm
  distance = duration * 0.034 / 2;

  // Visualizzazione della distanza sul display OLED
  display.clearDisplay();
  display.setCursor(0,20);
  display.print("Distanza: ");
  display.print(distance);
  display.println(" cm");
  display.display();

  // Stampa della distanza sulla seriale per debug
  Serial.print("Distanza: ");
  Serial.print(distance);
  Serial.println(" cm");

  delay(1000); // Aspetta un secondo prima della prossima misurazione
}

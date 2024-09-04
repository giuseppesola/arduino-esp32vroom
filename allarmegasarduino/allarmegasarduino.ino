#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <DHT.h>

// Definizioni per il display OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Definizioni per i sensori
#define DHTPIN 2       // Pin digitale collegato al DHT11
#define DHTTYPE DHT11  // Definisci il tipo di sensore DHT11
DHT dht(DHTPIN, DHTTYPE);

#define MQ2_PIN A0    // Pin analogico collegato al sensore MQ-2
#define BUZZER_PIN 3  // Pin digitale 8 collegato al buzzer

#define NUM_SAMPLES 100        // Numero di campioni per la calibrazione
#define CALIBRATION_TIME 2000  // Durata della calibrazione in millisecondi

int baseline_gas_value = 0;  // Variabile per il valore di base del gas

void setup() {
  Serial.begin(115200);

  // Aggiungi un ritardo per permettere al sensore MQ-2 di stabilizzarsi
  delay(5000);  // Attendi 5 secondi

  // Inizializzazione display OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;
  }
  display.clearDisplay();
  display.display();

  // Inizializzazione DHT11
  dht.begin();

  // Impostazione del pin del buzzer come output
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);  // Inizialmente il buzzer è spento

  // Calibrazione del sensore MQ-2
  Serial.println("Calibrazione del sensore MQ-2 in corso...");
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 25);
  display.print("    Calibrazione...");
  display.display();

  long total_gas_value = 0;

  for (int i = 0; i < NUM_SAMPLES; i++) {
    total_gas_value += analogRead(MQ2_PIN);
    delay(CALIBRATION_TIME / NUM_SAMPLES);
  }

  baseline_gas_value = total_gas_value / NUM_SAMPLES;

  Serial.print("Calibrazione completata. Valore di base del gas: ");
  Serial.println(baseline_gas_value);

  // Visualizza il valore base del gas sul display OLED
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 10);
  display.print("Calibrazione Finita");
  display.setCursor(0, 30);
  display.print("BaseGas:");
  display.setCursor(0, 45);
  display.print(baseline_gas_value);
  display.display();
  delay(2000);  // Ritardo per permettere di leggere il valore sul display
}
unsigned long previousMillisBuzzer = 0;   // Memorizza il tempo di esecuzione dell'ultimo ciclo del buzzer
unsigned long previousMillisDisplay = 0;  // Memorizza il tempo di esecuzione dell'ultimo aggiornamento del display
const long intervalBuzzer = 600;          // Intervallo per il buzzer in millisecondi (500 ms di suono + 100 ms di pausa)
const long intervalDisplay = 100;         // Intervallo per l'aggiornamento del display

int buzzerState = LOW;  // Stato del buzzer (ON/OFF)
int buzzerCount = 0;    // Contatore dei "bip" del buzzer

void loop() {
  // Lettura dati DHT11
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  // Controllo errori del DHT11
  if (isnan(h) || isnan(t)) {
    Serial.println(F("Errore nella lettura del DHT11!"));
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("Errore DHT11!");
    display.display();
    return;
  }

  // Lettura del valore dal sensore MQ-2
  int gas_value = analogRead(MQ2_PIN);
  int gas_percentage = map(gas_value - baseline_gas_value, 0, 1023 - baseline_gas_value, 0, 100);

  if (gas_percentage < 0) {
    gas_percentage = 0;
  }

  unsigned long currentMillis = millis();  // Legge il tempo corrente

  if (gas_percentage > 25) {
    // Gestione del buzzer con temporizzazione non bloccante
    if (currentMillis - previousMillisBuzzer >= intervalBuzzer && buzzerCount < 100) {
      previousMillisBuzzer = currentMillis;

      if (buzzerState == LOW) {
        buzzerState = HIGH;
        tone(BUZZER_PIN, 4000);  // Attiva il buzzer
      } else {
        buzzerState = LOW;
        noTone(BUZZER_PIN);  // Disattiva il buzzer
        buzzerCount++;
      }
    }

    // Aggiornamento del display con temporizzazione non bloccante
    if (currentMillis - previousMillisDisplay >= intervalDisplay) {
      previousMillisDisplay = currentMillis;

      display.clearDisplay();
      display.setTextSize(2);
      display.setCursor(0, 25);
      display.print("   Gas!!!");
      display.display();
    }
  } else {
    // Reset del contatore dei "bip" del buzzer e disattivazione del buzzer
    buzzerCount = 0;
    noTone(BUZZER_PIN);

    // Visualizza i dati normali sul display OLED
    if (currentMillis - previousMillisDisplay >= intervalDisplay) {
      previousMillisDisplay = currentMillis;

      display.clearDisplay();
      display.setTextSize(2);
      display.setCursor(0, 00);
      display.print("Gas:");
      display.print(gas_percentage);
      display.print("%");
      display.setCursor(0, 20);
      display.print("Temp:");
      display.print(t);
      display.setCursor(0, 40);
      display.print("Hum:");
      display.print(h);
      display.print("%");
      display.display();
    }
  }

  // Stampa i dati su Serial Monitor
  Serial.print("Gas:");
  Serial.print(gas_percentage);
  Serial.print("%, Temp:");
  Serial.print(t);
  Serial.print("C, Hum:");
  Serial.print(h);
  Serial.println("%");

  delay(250);  // Ritardo breve per evitare un aggiornamento troppo frequente
}

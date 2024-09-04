#include <ESP32Servo.h>
#include <NewPing.h>
#include <WiFi.h>
#include <WiFiClient.h>

#define TRIG_PIN 26
#define ECHO_PIN 25
#define SERVO_PIN 14
#define MAX_DISTANCE 200 // Distanza massima in cm

// Configurazione Access Point
const char* ssid = "ESP32_AP";           // Nome della rete Wi-Fi dell'ESP32
const char* password = "123456789";      // Password della rete Wi-Fi dell'ESP32

// Configurazione IP statico per l'Access Point
IPAddress local_IP(192, 168, 4, 1);      // Indirizzo IP dell'ESP32
IPAddress gateway(192, 168, 4, 1);       // Gateway dell'Access Point
IPAddress subnet(255, 255, 255, 0);      // Subnet mask

Servo servo;
NewPing sonar(TRIG_PIN, ECHO_PIN, MAX_DISTANCE);
WiFiServer server(80); // Crea un server web sulla porta 80 (HTTP)

void setup() {
  Serial.begin(115200);
  servo.attach(SERVO_PIN);
  servo.write(180);  // Chiude il coperchio del cestino

  // Configura l'ESP32 come Access Point
  WiFi.softAPConfig(local_IP, gateway, subnet);  // Configura l'IP statico dell'AP
  WiFi.softAP(ssid, password);                    // Avvia l'Access Point
  Serial.println("Access Point avviato.");
  Serial.print("Indirizzo IP dell'Access Point: ");
  Serial.println(WiFi.softAPIP());  // Stampa l'indirizzo IP dell'Access Point

  server.begin();  // Avvia il server
}

void loop() {
  WiFiClient client = server.available();  // Attende la connessione di un client

  if (client) {
    Serial.println("Nuovo client connesso.");
    String request = client.readStringUntil('\r');  // Legge la richiesta del client
    Serial.println(request);
    client.flush();

    // Controllo del coperchio tramite comandi HTTP
    if (request.indexOf("/open") != -1) {
      servo.write(0);  // Apre il coperchio completamente
    }
    if (request.indexOf("/close") != -1) {
      servo.write(180);  // Chiude il coperchio
    }

    // Risposta HTTP al client
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println("");
    client.println("<!DOCTYPE HTML>");
    client.println("<html>");
    client.println("<head>");
    client.println("<style>");
client.println("body {");
client.println("  font-family: Arial, sans-serif;");
client.println("  text-align: center;");
client.println("  margin: 0;");
client.println("  padding: 0;");
client.println("  background-color: #f4f4f4;");
client.println("}");
client.println("a {");
client.println("  display: inline-block;");
client.println("  padding: 90px 120px;"); // Aumentato il padding
client.println("  font-size: 80px;");    // Aumentata la dimensione del font
client.println("  text-decoration: none;");
client.println("  color: #fff;");
client.println("  background-color: #007BFF;");
client.println("  border-radius: 10px;");
client.println("  margin: 20px;");       // Aumentato il margine
client.println("  transition: background-color 0.3s;");
client.println("}");
client.println("a:hover {");
client.println("  background-color: #0056b3;");
client.println("}");
client.println("</style>");
    client.println("</head>");
    client.println("<body>");
    client.println("<p><a href=\"/open\">APRITI SESAMO</a></p>");
    client.println("<p><a href=\"/close\">CHIUDITI SESAMO</a></p>");
    client.println("</body>");
    client.println("</html>");
    client.stop();  // Chiude la connessione con il client
    Serial.println("Client disconnesso.");
  }

  // Gestione del sensore di distanza
  delay(50);  // Piccola pausa per evitare falsi rilevamenti
  int distance = sonar.ping_cm();

  if (distance > 0 && distance < 20) {  // Se la mano è a meno di 20 cm
    servo.write(70);  // Apre il coperchio completamente
    delay(5000);  // Mantiene il coperchio aperto per 5 secondi
    servo.write(190);  // Chiude il coperchio
  }

  // Attesa breve prima di continuare il loop
  delay(500);  // Tempo di attesa per evitare continui cambiamenti
}

#include <ESP32Servo.h>
#include <NewPing.h>
#include <WiFi.h>
#include <WiFiClient.h>

#define TRIG_PIN 5
#define ECHO_PIN 18
#define SERVO_PIN 17
#define MAX_DISTANCE 200 // Distanza massima in cm

// Configurazione Wi-Fi
const char* ssid = "EOLO_230957";       // Il nome della tua rete Wi-Fi
const char* password = "VWHhURduN";     // La password della tua rete Wi-Fi

// Configurazione IP statico
IPAddress local_IP(192, 168, 1, 184); // Sostituisci con l'IP desiderato
IPAddress gateway(192, 168, 1, 1);    // Gateway della tua rete
IPAddress subnet(255, 255, 255, 0);   // Subnet mask
IPAddress primaryDNS(8, 8, 8, 8);     // DNS primario (Google DNS)
IPAddress secondaryDNS(8, 8, 4, 4);   // DNS secondario (Google DNS)

Servo servo;
NewPing sonar(TRIG_PIN, ECHO_PIN, MAX_DISTANCE);
WiFiServer server(80); // Crea un server web sulla porta 80 (HTTP)

void setup() {
  Serial.begin(115200);
  servo.attach(SERVO_PIN);
  servo.write(180);  // Chiude il coperchio del cestino

  // Connetti con IP statico
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
    Serial.println("Configurazione IP statica fallita!");
  }

  // Connessione alla rete Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connessione in corso...");
  }
  Serial.println("Connesso al WiFi!");
  Serial.print("Indirizzo IP: ");
  Serial.println(WiFi.localIP());  // Stampa l'indirizzo IP dell'ESP32

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

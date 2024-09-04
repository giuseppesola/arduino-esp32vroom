#include <RCSwitch.h>

// Create an instance of RCSwitch
RCSwitch mySwitch = RCSwitch();

void setup() {
  // Initialize serial communication at a baud rate of 115200
  Serial.begin(115200);

  // Configure the receiving pin for the RF module (e.g., Pin D23)
  mySwitch.enableReceive(23); // Pin D23 (you can use any other supported pin)

  Serial.println("In attesa di ricezione...");
}

void loop() {
  // Check if a signal has been received
  if (mySwitch.available()) {
    // Get the received code
    unsigned long receivedCode = mySwitch.getReceivedValue();

    // Print the received code to the Serial Monitor
    if (receivedCode != 0) {
      Serial.print("Codice ricevuto: ");
      Serial.println(receivedCode);
    } else {
      // If no valid code is received (e.g., noise), print a message
      Serial.println("Nessun codice valido ricevuto. Forse rumore.");
    }

    // Reset the receiver for the next signal
    mySwitch.resetAvailable();
  }
}

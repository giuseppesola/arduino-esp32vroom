#include <RCSwitch.h>

RCSwitch mySwitch = RCSwitch();

void setup() {
  Serial.begin(115200);
  
  // Configura il pin di ricezione
  mySwitch.enableReceive(23); // Pin D23 (o qualsiasi pin supportato dal modulo RF)

  Serial.println("In attesa di ricezione...");
}

void loop() {
  if (mySwitch.available()) {
    // Ottiene il codice ricevuto
    unsigned long receivedCode = mySwitch.getReceivedValue();
    
    if (receivedCode == 0x00AF10C4) { // Verifica se il codice ricevuto corrisponde a quello atteso
      Serial.println("Ricevuto");
    } else {
      Serial.print("Codice ricevuto: ");
      Serial.println(receivedCode);
    }

    // Reset del ricevitore per il prossimo segnale
    mySwitch.resetAvailable();
  }
}

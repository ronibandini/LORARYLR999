// LORA config and receiver example
// Roni Bandini 11/2025
// MIT License
// Firebeetle 2 ESP32C6 + LoRa Reyax RYLR999
// LoRa Module RX → ESP32 GPIO 5 (TX), LoRa Module TX → ESP32 GPIO 4 (RX)

#include <HardwareSerial.h>

HardwareSerial LoRaSerial(2); // UART2
int led = 15;

void sendATCommand(String cmd) {
  Serial.print("Enviando: ");
  Serial.println(cmd);
  
  // Clear buffer
  while (LoRaSerial.available()) {
    LoRaSerial.read();
  }
  
  LoRaSerial.print(cmd + "\r\n");
  
  // Wait answer
  unsigned long startTime = millis();
  String response = "";
  bool gotResponse = false;
  
  while (millis() - startTime < 1500) { // Timeout  
    if (LoRaSerial.available()) {
      char c = LoRaSerial.read();
      response += c;
      gotResponse = true;
      
      // Wait a little longer just in case
      if (c == '\n') {
        delay(50);
      }
    }
    
    // No more data
    if (gotResponse && !LoRaSerial.available()) {
      delay(50); // Dar tiempo extra
      if (!LoRaSerial.available()) break;
    }
  }
  
  if (response.length() > 0) {
    Serial.print("Answer: ");
    Serial.println(response);
  } else {
    Serial.println("*** NO ANSWER ***");
  }
}

void setup() {
  Serial.begin(115200);
  LoRaSerial.begin(57600, SERIAL_8N1, 4, 5); // RX=4, TX=5 (Firebeetle 2)
  pinMode(led, OUTPUT);
  digitalWrite(led, LOW);

  delay(1000);
  Serial.println("LORA receiver with RYLR999 module");
  Serial.println("Roni Bandini, November 2025 ");
  Serial.println("");

  // Configuración del módulo con verificación
  Serial.println("\nConfiguring LoRa module...");
  sendATCommand("AT+ADDRESS=7");
  sendATCommand("AT+NETWORKID=18");

  Serial.println("Waiting for messages...");
}

void loop() {
  // data available?
  if (LoRaSerial.available()) {
    String msg = LoRaSerial.readStringUntil('\n');
    msg.trim(); 
    
    if (msg.length() == 0) return;

    Serial.print("RAW recibido: [");
    Serial.print(msg);
    Serial.println("]");

    String data = msg;
    
    if (msg.startsWith("+RCV=")) {
      int dataStart = msg.indexOf(',') + 1;  
      dataStart = msg.indexOf(',', dataStart) + 1;  
      int dataEnd = msg.indexOf(',', dataStart);
      
      if (dataEnd > 0) {
        data = msg.substring(dataStart, dataEnd);
      }
      Serial.print("Datos extraídos: ");
      Serial.println(data);
    }

    // If the messsage is on, turn the LED on
    if (data.indexOf("on") >= 0) {
      digitalWrite(led, HIGH);
      Serial.println(">>> Light! <<<");
    } else if (data.indexOf("off") >= 0) {
      digitalWrite(led, LOW);
      Serial.println(">>> Light off <<<");
    }
  }
  
  delay(10); 
}
// LORA config and sender example
// Roni Bandini 11/2025
// MIT License
// Firebeetle 2 ESP32C6 + LoRa Reyax RYLR999
// LoRa Module RX → ESP32 GPIO 5 (TX), LoRa Module TX → ESP32 GPIO 4 (RX)

#include <HardwareSerial.h>

HardwareSerial LoRaSerial(2); // UART2 for LoRa
int led = 15; // Onboard LED

void sendATCommand(String cmd) {
  Serial.print("Sending: ");
  Serial.println(cmd);

  // Clear buffer
  while (LoRaSerial.available()) LoRaSerial.read();

  LoRaSerial.print(cmd + "\r\n");

  unsigned long startTime = millis();
  String response = "";

  while (millis() - startTime < 1500) {
    if (LoRaSerial.available()) {
      response += char(LoRaSerial.read());
    }
  }

  if (response.length() > 0) {
    Serial.print("Answer: ");
    Serial.println(response);
  } else {
    Serial.println("*** No answer ***");
  }
}

void setup() {
  Serial.begin(115200);
  LoRaSerial.begin(57600, SERIAL_8N1, 4, 5); // RX=4, TX=5
  pinMode(led, OUTPUT);

  delay(1000);
  Serial.println("ESP32 Lora Sender");
  Serial.println("Roni Bandini, Movember 2025");
  Serial.println("");

  Serial.println("Configuring Lora Module...");
  sendATCommand("AT+ADDRESS=1");      
  sendATCommand("AT+NETWORKID=18");    

}

void loop() {
  static unsigned long lastSend = 0;
  if (millis() - lastSend > 5000) { 
    String mensaje = "Hola mundo";
    int largo = mensaje.length();

    String comando = "AT+SEND=7," + String(largo) + "," + mensaje;
    sendATCommand(comando);

    digitalWrite(led, HIGH);
    delay(200);
    digitalWrite(led, LOW);

    lastSend = millis();
  }

  delay(10);
}

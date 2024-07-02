#include <ESP8266WiFi.h>

const char* ssid = "OnePlus 8 Pro";
const char* password = "56785678";

WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop() {
  WiFiClient client = server.available();
  if (client) {
    Serial.println("New Client Connected");

    while (client.connected()) {
      if (client.available()) {
        if (receivePacket(client)) {
          Serial.println("Packet received and validated");
        } else {
          Serial.println("Invalid packet received");
        }
      }
    }
    client.stop();
    Serial.println("Client Disconnected");
  }
}

bool receivePacket(WiFiClient& client) {
  const uint8_t START_BYTE = 0xFF;
  const uint8_t END_BYTE = 0x00;
  const int PACKET_MAX_SIZE = 512;

  uint8_t packet[PACKET_MAX_SIZE];
  int index = 0;
  uint8_t checksum = 0;

  // Read until we find the start byte
  while (client.available() && client.read() != START_BYTE);

  // Read the packet
  while (client.available() && index < PACKET_MAX_SIZE) {
    uint8_t byte = client.read();
    if (byte == END_BYTE) {
      break;
    }
    packet[index++] = byte;
    checksum += byte;
  }

  // Validate checksum
  if (index == 0 || index >= PACKET_MAX_SIZE) {
    return false; // No valid data received
  }
  
  uint8_t received_checksum = packet[index - 1];
  checksum -= received_checksum; // Adjust checksum calculation

  if (checksum != received_checksum) {
    return false; // Invalid checksum
  }

  // Null-terminate the received data and print
  packet[index - 1] = '\0'; // Remove checksum byte for string termination
  Serial.printf("Received data: %s\n", packet);

  // Process the packet data
  processPacket(packet);

  return true;
}

void processPacket(uint8_t* data) {
  String receivedData = String((char*)data);

  // Parse the data
  int startIndex = 0;
  while (startIndex < receivedData.length()) {
    int endIndex = receivedData.indexOf(',', startIndex);
    if (endIndex == -1) endIndex = receivedData.length();

    String pair = receivedData.substring(startIndex, endIndex);
    int separatorIndex = pair.indexOf(':');
    if (separatorIndex != -1) {
      String key = pair.substring(0, separatorIndex);
      float value = pair.substring(separatorIndex + 1).toFloat();
      Serial.printf("%s: %f\n", key.c_str(), value);
    }

    startIndex = endIndex + 1;
  }
}

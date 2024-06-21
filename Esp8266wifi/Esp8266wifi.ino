#include <ESP8266WiFi.h>

const char* ssid = "OnePlus 8 Pro";
const char* password = "56785678";

WiFiServer server(80);

void setup() {
  Serial.begin(115200); // Serial for both debugging and communication with Teensy
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");
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
  float pitch_p = getFloatValue(receivedData, "pitch_p");
  float pitch_i = getFloatValue(receivedData, "pitch_i");
  float pitch_d = getFloatValue(receivedData, "pitch_d");
  float roll_p = getFloatValue(receivedData, "roll_p");
  float roll_i = getFloatValue(receivedData, "roll_i");
  float roll_d = getFloatValue(receivedData, "roll_d");
  float yaw_p = getFloatValue(receivedData, "yaw_p");
  float yaw_i = getFloatValue(receivedData, "yaw_i");
  float yaw_d = getFloatValue(receivedData, "yaw_d");

  Serial.printf("Pitch P: %f, Pitch I: %f, Pitch D: %f\n", pitch_p, pitch_i, pitch_d);
  Serial.printf("Roll P: %f, Roll I: %f, Roll D: %f\n", roll_p, roll_i, roll_d);
  Serial.printf("Yaw P: %f, Yaw I: %f, Yaw D: %f\n", yaw_p, yaw_i, yaw_d);

  // Send the values to the Teensy
  sendToTeensy(pitch_p, pitch_i, pitch_d, roll_p, roll_i, roll_d, yaw_p, yaw_i, yaw_d);
}

float getFloatValue(String data, String key) {
  int startIndex = data.indexOf(key + ":");
  if (startIndex == -1) return 0.0;

  startIndex += key.length() + 1;
  int endIndex = data.indexOf(",", startIndex);
  if (endIndex == -1) endIndex = data.length();

  return data.substring(startIndex, endIndex).toFloat();
}

void sendToTeensy(float pitch_p, float pitch_i, float pitch_d, float roll_p, float roll_i, float roll_d, float yaw_p, float yaw_i, float yaw_d) {
  Serial.print("pitch_p:"); Serial.println(pitch_p, 6);
  Serial.print("pitch_i:"); Serial.println(pitch_i, 6);
  Serial.print("pitch_d:"); Serial.println(pitch_d, 6);
  Serial.print("roll_p:"); Serial.println(roll_p, 6);
  Serial.print("roll_i:"); Serial.println(roll_i, 6);
  Serial.print("roll_d:"); Serial.println(roll_d, 6);
  Serial.print("yaw_p:"); Serial.println(yaw_p, 6);
  Serial.print("yaw_i:"); Serial.println(yaw_i, 6);
  Serial.print("yaw_d:"); Serial.println(yaw_d, 6);
}
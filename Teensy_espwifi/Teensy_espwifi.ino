#include <EEPROM.h>

#define EEPROM_SIZE 512
#define MAX_ENTRIES 20  // Adjust this as needed

struct KeyValue {
  String key;
  float value;
};

KeyValue keyValueArray[MAX_ENTRIES];
int keyValueCount = 0;

void setup() {
  Serial.begin(115200);  // Serial connection with the computer
  Serial1.begin(115200); // Serial connection with the ESP8266
  while (!Serial) {
    ; // Wait for serial port to connect. Needed for native USB port only
  }
  Serial.println("Teensy is ready to receive data");

  // Read EEPROM data into array
  readEEPROMToArray();
  printKeyValueArray();
}

void loop() {
  if (Serial1.available()) {
    if (receivePacket()) {
      Serial.println("Packet received and validated");
    } else {
      Serial.println("Invalid packet received");
    }
  }
}

bool receivePacket() {
  const uint8_t START_BYTE = 0xFF;
  const uint8_t END_BYTE = 0x00;
  const int PACKET_MAX_SIZE = 512;

  uint8_t packet[PACKET_MAX_SIZE];
  int index = 0;
  uint8_t checksum = 0;

  // Read until we find the start byte
  while (Serial1.available() && Serial1.read() != START_BYTE);

  // Read the packet
  while (Serial1.available() && index < PACKET_MAX_SIZE) {
    uint8_t byte = Serial1.read();
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
  unsigned int startIndex = 0;
  while (startIndex < receivedData.length()) {
    int endIndex = receivedData.indexOf(',', startIndex);
    if (endIndex == -1) endIndex = receivedData.length();

    String pair = receivedData.substring(startIndex, endIndex);
    int separatorIndex = pair.indexOf(':');
    if (separatorIndex != -1) {
      String key = pair.substring(0, separatorIndex);
      float value = pair.substring(separatorIndex + 1).toFloat();
      Serial.printf("%s: %f\n", key.c_str(), value);
      
      // Save or update the variable in EEPROM
      saveOrUpdateEEPROM(key, value);

      // Update the array
      updateKeyValueArray(key, value);
    }

    startIndex = endIndex + 1;
  }
}

void saveOrUpdateEEPROM(String key, float value) {
  int address = findKeyInEEPROM(key);
  if (address == -1) {
    address = findEmptyAddressInEEPROM();
    if (address == -1) {
      Serial.println("EEPROM is full!");
      return;
    }
    writeStringToEEPROM(address, key);
  }
  writeFloatToEEPROM(address + key.length() + 1, value);
}

int findKeyInEEPROM(String key) {
  unsigned int address = 0;
  while (address < EEPROM_SIZE) {
    String storedKey = readStringFromEEPROM(address);
    if (storedKey == key) {
      return address;
    }
    if (storedKey == "") {
      break;
    }
    address += storedKey.length() + 1 + sizeof(float);
  }
  return -1;
}

int findEmptyAddressInEEPROM() {
  unsigned int address = 0;
  while (address < EEPROM_SIZE) {
    String storedKey = readStringFromEEPROM(address);
    if (storedKey == "") {
      return address;
    }
    address += storedKey.length() + 1 + sizeof(float);
  }
  return -1;
}

String readStringFromEEPROM(int address) {
  String value;
  char ch = EEPROM.read(address);
  while (ch != '\0' && address < EEPROM_SIZE) {
    value += ch;
    address++;
    ch = EEPROM.read(address);
  }
  return value;
}

void writeStringToEEPROM(int address, String value) {
  for (unsigned int i = 0; i < value.length(); i++) {
    EEPROM.write(address + i, value[i]);
  }
  EEPROM.write(address + value.length(), '\0');
}

void writeFloatToEEPROM(int address, float value) {
  byte* p = (byte*)(void*)&value;
  for (unsigned int i = 0; i < sizeof(float); i++) {
    EEPROM.write(address + i, p[i]);
  }
}

float readFloatFromEEPROM(int address) {
  float value = 0.0;
  byte* p = (byte*)(void*)&value;
  for (unsigned int i = 0; i < sizeof(float); i++) {
    p[i] = EEPROM.read(address + i);
  }
  return value;
}

void readEEPROMToArray() {
  keyValueCount = 0;
  unsigned int address = 0;
  while (address < EEPROM_SIZE && keyValueCount < MAX_ENTRIES) {
    String key = readStringFromEEPROM(address);
    if (key == "") {
      break;
    }
    float value = readFloatFromEEPROM(address + key.length() + 1);
    keyValueArray[keyValueCount].key = key;
    keyValueArray[keyValueCount].value = value;
    keyValueCount++;
    address += key.length() + 1 + sizeof(float);
  }
}

void updateKeyValueArray(String key, float value) {
  for (int i = 0; i < keyValueCount; i++) {
    if (keyValueArray[i].key == key) {
      keyValueArray[i].value = value;
      return;
    }
  }
  if (keyValueCount < MAX_ENTRIES) {
    keyValueArray[keyValueCount].key = key;
    keyValueArray[keyValueCount].value = value;
    keyValueCount++;
  }
}

void printKeyValueArray() {
  Serial.println("EEPROM Key-Value Pairs:");
  for (int i = 0; i < keyValueCount; i++) {
    Serial.printf("%s: %f\n", keyValueArray[i].key.c_str(), keyValueArray[i].value);
  }
}

void setup() {
  Serial.begin(115200); // Serial for debug
  Serial7.begin(115200);  // Serial7 to communicate with ESP-01
}

void loop() {
  if (Serial7.available()) {
    String receivedData = Serial7.readStringUntil('\n');
    processReceivedData(receivedData);
  }
}

void processReceivedData(String data) {
  if (data.startsWith("pitch_p:")) {
    float pitch_p = data.substring(8).toFloat();
    Serial.printf("Pitch P: %f\n", pitch_p);
  } else if (data.startsWith("pitch_i:")) {
    float pitch_i = data.substring(8).toFloat();
    Serial.printf("Pitch I: %f\n", pitch_i);
  } else if (data.startsWith("pitch_d:")) {
    float pitch_d = data.substring(8).toFloat();
    Serial.printf("Pitch D: %f\n", pitch_d);
  } else if (data.startsWith("roll_p:")) {
    float roll_p = data.substring(7).toFloat();
    Serial.printf("Roll P: %f\n", roll_p);
  } else if (data.startsWith("roll_i:")) {
    float roll_i = data.substring(7).toFloat();
    Serial.printf("Roll I: %f\n", roll_i);
  } else if (data.startsWith("roll_d:")) {
    float roll_d = data.substring(7).toFloat();
    Serial.printf("Roll D: %f\n", roll_d);
  } else if (data.startsWith("yaw_p:")) {
    float yaw_p = data.substring(6).toFloat();
    Serial.printf("Yaw P: %f\n", yaw_p);
  } else if (data.startsWith("yaw_i:")) {
    float yaw_i = data.substring(6).toFloat();
    Serial.printf("Yaw I: %f\n", yaw_i);
  } else if (data.startsWith("yaw_d:")) {
    float yaw_d = data.substring(6).toFloat();
    Serial.printf("Yaw D: %f\n", yaw_d);
  }
}

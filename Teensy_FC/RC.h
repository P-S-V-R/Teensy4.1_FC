#define Channels 10

#define PPM
//#define CRSF

#include "CrsfSerial.h"

CrsfSerial crsf(Serial1, CRSF_BAUDRATE);

int Ch[Channels+1];
int a,b=0,c,d=1;

void packetChannels()
{
  for(int i=1; i<=Channels; i++){
    Ch[i]=crsf.getChannel(i);
    Serial.print(Ch[i]);
    Serial.print(", ");
    Serial.println();
  }
}

void read_PPM() {
    a = micros();
    c = a - b;
    b = a;

    if (c <= 2100) {
        if (d <= Channels) {  // Ensure d does not exceed the number of channels
            Ch[d] = c;
            Serial.print("Channel ");
            Serial.print(d);
            Serial.print(": ");
            Serial.print(Ch[d]);
            Serial.print(", ");
            d++;
        }
    } else {
        d = 1;  // Reset channel counter after a frame
        Serial.println();
    }
}
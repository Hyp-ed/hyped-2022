#include <SoftwareSerial.h>
#include <SerialProtocol.h>
#include <StreamSerialProtocol.h>
#include <ArduinoSerialProtocol.h>

int IRSensor1 = 2;
int IRSensor2 = 3;
int IRSensor3 = 4;
int IRSensor4 = 5;

// The payload that will be sent to the other device
struct Payload {
    int16_t wheelEncoderA;
    int16_t wheelEncoderB;
    int16_t wheelEncoderC;
    int16_t wheelEncoderD;
} payload;

ArduinoSerialProtocol protocol(&Serial, (uint8_t*)&payload, sizeof(payload));
uint8_t receiveState;

void setup() 
{
    Serial.begin(9600);

    payload.wheelEncoderA = 0;
    payload.wheelEncoderB = 0;
    payload.wheelEncoderC = 0;
    payload.wheelEncoderD = 0;
}

void loop()
{
    if(digitalRead(IRSensor1)){
        ++payload.wheelEncoderA;
    }
    if(digitalRead(IRSensor2)){
        ++payload.wheelEncoderB;
    }
    if(digitalRead(IRSensor3)){
        ++payload.wheelEncoderC;
    }
    if(digitalRead(IRSensor4)){
        ++payload.wheelEncoderD;
    }

    protocol.send();
}


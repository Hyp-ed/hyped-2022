// Connects IR Sensors to pins
int IRSensor1 = 2;
int IRSensor2 = 3;
int IRSensor3 = 4;
int IRSensor4 = 5;
// Connects output pins
int Output1 = 13;
int Output2 = 14;
int Output3 = 15;
int Output4 = 16;

void setup(){
    // IRSensors are inputs
    pinMode (IRSensor1, INPUT);
    pinMode (IRSensor2, INPUT);
    pinMode (IRSensor3, INPUT);
    pinMode (IRSensor4, INPUT);
    // Outputs
    pinMode (Output1, OUTPUT);
    pinMode (Output2, OUTPUT);
    pinMode (Output3, OUTPUT);
    pinMode (Output4, OUTPUT);
}
// Read IR Sensors + Output based on reading
void loop(){
    statusSensor = digitalRead (IRSensor1);

    if (statusSensor == 1){
        digitalWrite (Output1, LOW);
    }
    else {
        digitalWrite (Output1, HIGH);
    }
}
void loop(){
    statusSensor = digitalRead (IRSensor2);

    if (statusSensor == 1){
        digitalWrite (Output2, LOW);
    }
    else {
        digitalWrite (Output, HIGH);
    }
}
void loop(){
    statusSensor = digitalRead (IRSensor3);

    if (statusSensor == 1){
        digitalWrite (Output3, LOW);
    }
    else {
        digitalWrite (Output3, HIGH);
    }
}
void loop(){
    statusSensor = digitalRead (IRSensor4);

    if (statusSensor == 1){
        digitalWrite (Output4, LOW);
    }
    else {
        digitalWrite (Output4, HIGH);
    }
}
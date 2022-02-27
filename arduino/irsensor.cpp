#define MAX_LENGTH 4   //one more for null char
// Connects IR Sensors to pins
int IRSensor1 = 2;
int IRSensor2 = 3;
int IRSensor3 = 4;
int IRSensor4 = 5;
int wheelValues[MAX_LENGTH] = {0,0,0,0};

void setup() {
  Serial.begin(9600);

  sendArray(wheelValues);
}

void loop() {
  int wheelValues[MAX_LENGTH] = {0,0,0,0}; 

  if(digitalRead(IRSensor1)){
    wheelValues[0]++;
  }
  if(digitalRead(IRSensor2)){
    wheelValues[1]++;
  }
  if(digitalRead(IRSensor3)){
    wheelValues[2]++;
  }
  if(digitalRead(IRSensor4)){
    wheelValues[3]++;
  }

  sendArray(wheelValues);
}

void sendArray(int arr[]) 
{

  String msg;
  for(int i = 0; i < sizeof(arr) - 1; i++) 
  {
    char number[arr[i] > 0 ? (int) log10 (arr[i]) + 1 : 1]; 
    sprintf(number, "%d", arr[i]);
    msg += number; 
    msg += ","; 
  }

  Serial.println(msg);
  
}

#include <ESP32Servo.h>

Servo myServo;
const int servoPin = 12;
const int anglePin = 1;

bool changeDirection = false;
bool speedUp = false;
bool slowDown = false;
bool stop = false;
bool start = false;
bool reset = false;
bool Repeat = false;
bool motorlimits = false;

char input = '0';
char input2 = '0';
const int arraySize = 10;  
int inputs[arraySize]; 
int dataIndex = 0;

int startPos = 46; 
int endPos = 104; 

int delayTime = 20; 

int pos = startPos;
int increment = 1;


void setup() {
  Serial.begin(115200);  
  myServo.attach(servoPin); 
}

void loop() {

  move_motor(startPos, endPos);
  serialEvent(); 
  
  delay(delayTime);


}

void move_motor(int startPos,int endPos){

    pos += increment; 

    if (pos >= endPos || pos <= startPos) {
      increment *= -1;
    }
    myServo.write(pos);
  }

void serialEvent() {
  while (Serial.available()) {
    input2 =input;
    input = Serial.read(); 
    Serial.print(input);
    if (input >= '1' && input <= '5') {
      int value = input - '0';
      inputs[dataIndex] = value;  
      dataIndex++;                
      if (dataIndex >= arraySize) {
        dataIndex = arraySize - 1; 
      }
    }

    Serial.println();

    if (input == '1') {
      changeDirection = true; 
      change_direction();
      
    }

    if (input == '2') {
      speedUp = true;
      speed_up();
    }

    if (input == '3') {
      slowDown = true;
      slow_down();
    }

    if (input == '4') {
      stop = true;
      Stop();
    }

    if (input == '5') {
      start = true; 
      start_servo();
    }
    
    if (input == '6') {
      Repeat = true; 
      repeat();
    }

    if (input == '7') {
      reset = true; 
      reset_intervention();
    }

    if (input == '8') {
      //measure_motorlimits = true; 
      measure_motorlimits();
    }

    if (input == '9') {
      motorlimits = true; 
      set_motorlimits();
    }
    
  }
}

void change_direction() {
  if (changeDirection) {
      Serial.println("Direction changed");
      changeDirection = false; // Reset the flag
      increment *= -1; // Reverse the increment to change direction
    }
}
void speed_up() {
  if (speedUp) {
  Serial.println("Speed increased");
  speedUp = false;
  delayTime -= 5;
  }
}

void slow_down() {
  if (slowDown){
  Serial.println("Speed decreased");
  slowDown = false;
  delayTime += 5;
  }
}

void Stop() {
  Serial.println("Stop");
  stop = false;
  myServo.detach();
  return;
} 

void start_servo() {
  Serial.println("Start");
  start = false;
  myServo.attach(servoPin);
  return;
} 

void repeat() {
  if (Repeat) {
      Serial.println("Repeated");
      startPos = 70;
      endPos = 100;
      pos = startPos +1;
      Repeat = false;
    }
}

void reset_intervention() {
     Serial.println("Reset to:");
  int input2 = inputs[dataIndex-1];
  dataIndex--;  

  reset = false;

  if (input2 == 1) {
    changeDirection = true; 
    change_direction();
  }

  if (input2 == 2) {
    slowDown = true; 
    slow_down();
  }

  if (input2 == 3) {
    speedUp = true; 
    speed_up();
  }

  if (input2 == 4) {
    start = true; 
    start_servo();
  }

}

void measure_motorlimits(){
 int angle = analogRead(anglePin);
 float posIsDeg= (((angle - 1160.00)/(2230.00-1160.00))*(130-40))+40;
 Serial.print(posIsDeg);
 }

void set_motorlimits() {
  motorlimits = true;
  if (motorlimits) {
    Serial.println("Set the motor limits");
    
    Serial.println("Set start position: ");
    startPos = readnumber();

    while (Serial.available()) {
      Serial.read();}
    Serial.println("Set end position: ");
    endPos = readnumber();

    pos = startPos+1;
    motorlimits = false;
  }
}

uint8_t readnumber(void) {
  uint8_t num = 0;

  while (num == 0) {
    while (! Serial.available());
    num = Serial.parseInt();
  }
  return num;
}
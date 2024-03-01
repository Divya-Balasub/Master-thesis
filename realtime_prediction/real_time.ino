#include <Arduino.h>
#include <ESP32Servo.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

Servo myServo;


const int servoPin = 12;
int anglePin = 1;

bool motorAttached = true;

// Servo position limits
const int minAngle = 30;
const int maxAngle = 110;

int pos = minAngle; // Current position of the servo
int increment = 1;

const int FSR_PIN1 = 13; //white
const int FSR_PIN2 = 5; //purple
const int FSR_PIN3 = 15; //orange
const int FSR_PIN4 = 16; //yellow
TaskHandle_t Task1;
TaskHandle_t Task2;

uint16_t BNO055_SAMPLERATE_DELAY_MS = 200;

Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28, &Wire);

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

void setup() {
  Serial.begin(115200);
  myServo.attach(servoPin); 
  while (!Serial) delay(10); 

  /* Initialise the sensor */
  if (!bno.begin())
  {
    /* There was a problem detecting the BNO055, check your connections */
    Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    while (1);
  }

  delay(1000);

  //task 1 - continues motor movement
  xTaskCreatePinnedToCore(
                    Task1code,   /* Task function. */
                    "Task1",     /* name of task. */
                    10000,       /* Stack size of task */
                    NULL,        /* parameter of the task */
                    1,           /* priority of the task */
                    &Task1,      /* Task handle to keep track of created task */
                    0);          /* pin task to core 0 */                  
  delay(500); 

  //task 2 - get sensor readings
  xTaskCreatePinnedToCore(
                    Task2code,   /* Task function. */
                    "Task2",     /* name of task. */
                    10000,       /* Stack size of task */
                    NULL,        /* parameter of the task */
                    1,           /* priority of the task */
                    &Task2,      /* Task handle to keep track of created task */
                    1);          /* pin task to core 1 high priority */
    delay(500); 
}


void Task1code( void * pvParameters ){
  for(;;){
  if (motorAttached) {
    pos += increment;

    if (pos >= maxAngle || pos <= minAngle) {
      increment *= -1; 
    }
    myServo.write(pos);
    }
    serialEvent(); 
     delay(delayTime);
  }


}

void Task2code( void * pvParameters ){

  for(;;){
  unsigned long currentMillis = millis();

  int fsr1 = analogRead(FSR_PIN1);
  int fsr2 = analogRead(FSR_PIN2);
  int fsr3 = analogRead(FSR_PIN3);
  int fsr4 = analogRead(FSR_PIN4);
  int angle = analogRead(anglePin);

  Serial.print(currentMillis);
  Serial.print(",");
  Serial.print(fsr1);
  Serial.print(",");
  Serial.print(fsr2);
  Serial.print(",");
  Serial.print(fsr3);
  Serial.print(",");
  Serial.print(fsr4);
  Serial.print(",");
  Serial.print(angle);
  Serial.print(",");
  
  sensors_event_t orientationData , angVelocityData , linearAccelData, magnetometerData, accelerometerData, gravityData;
  bno.getEvent(&orientationData, Adafruit_BNO055::VECTOR_EULER);
  bno.getEvent(&angVelocityData, Adafruit_BNO055::VECTOR_GYROSCOPE);
  bno.getEvent(&linearAccelData, Adafruit_BNO055::VECTOR_LINEARACCEL);
  bno.getEvent(&magnetometerData, Adafruit_BNO055::VECTOR_MAGNETOMETER);
  bno.getEvent(&accelerometerData, Adafruit_BNO055::VECTOR_ACCELEROMETER);
  bno.getEvent(&gravityData, Adafruit_BNO055::VECTOR_GRAVITY);

  printEvent(&linearAccelData);
  printEvent(&angVelocityData);
  printEvent(&orientationData);
  printEvent(&accelerometerData);

  uint8_t system, gyro, accel, mag = 0;
  bno.getCalibration(&system, &gyro, &accel, &mag);

  Serial.println();
  delay(BNO055_SAMPLERATE_DELAY_MS);
}
}


void printEvent(sensors_event_t* event) {
  double x = -1000000, y = -1000000 , z = -1000000;
  if (event->type == SENSOR_TYPE_ACCELEROMETER) {
    x = event->acceleration.x;
    y = event->acceleration.y;
    z = event->acceleration.z;
  }
  else if (event->type == SENSOR_TYPE_ORIENTATION) {
    x = event->orientation.x;
    y = event->orientation.y;
    z = event->orientation.z;
    if(x == 0.0){
    bno.begin();
  }
  }
  else if (event->type == SENSOR_TYPE_MAGNETIC_FIELD) {
    x = event->magnetic.x;
    y = event->magnetic.y;
    z = event->magnetic.z;
  }

  else if (event->type == SENSOR_TYPE_ROTATION_VECTOR) {
    x = event->gyro.x;
    y = event->gyro.y;
    z = event->gyro.z;
  }
  else if (event->type == SENSOR_TYPE_LINEAR_ACCELERATION) {
    x = event->acceleration.x;
    y = event->acceleration.y;
    z = event->acceleration.z;
  }
  else if (event->type == SENSOR_TYPE_GRAVITY) {
    x = event->acceleration.x;
    y = event->acceleration.y;
    z = event->acceleration.z;
  }

  else if (event->type == SENSOR_TYPE_GYROSCOPE) {
    x = event->gyro.x;
    y = event->gyro.y;
    z = event->gyro.z;
  }
  else {
    Serial.print("Unk:");
  }
  
  Serial.print(x);
  Serial.print(",");
  Serial.print(y);
  Serial.print(",");
  Serial.print(z);
  if (event->type == SENSOR_TYPE_LINEAR_ACCELERATION)
  {
    Serial.print(",");
  }
    if (event->type == SENSOR_TYPE_GYROSCOPE)
  {
    Serial.print(",");
  }
      if (event->type == SENSOR_TYPE_ORIENTATION)
  {
    Serial.print(",");
  }
  }

  void loop()
{
  if (Serial.available() > 0)
  {
    char input = Serial.read(); 
        input2 =input;
    if (input >= '1' && input <= '5') {
      int value = input - '0';
      inputs[dataIndex] = value;  
      dataIndex++;                
      if (dataIndex >= arraySize) {
        dataIndex = arraySize - 1; 
      }
    }
    if (input == '1') {
      stop = true;
      Stop();
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

      changeDirection = true;
      change_direction();
    }

    if (input == '5') {
      Repeat = true; 
      //repeat();
    }
    
    if (input == '6') {
      reset = true; 
      reset_intervention();
    }

    if (input == '7') {
      start = true; 
      start_servo();
    }

    if (input == '8') {
      measure_motorlimits();
    }

    if (input == '9') {
      motorlimits = true; 
      set_motorlimits();
    }
    

    if (input == 'd')
    {
      detach_motor();
    }
    
    if (input == 'a')
    {
      attach_motor();
    }

  }
}

void detach_motor() {
  myServo.detach();
  motorAttached = false;
}

void attach_motor() {
  
  motorAttached = true;
  myServo.attach(servoPin);

}


void serialEvent() {
  while (Serial.available()) {
    input2 =input;
    input = Serial.read(); 
    if (input >= '1' && input <= '5') {
      int value = input - '0';
      inputs[dataIndex] = value;  
      dataIndex++;                
      if (dataIndex >= arraySize) {
        dataIndex = arraySize - 1; 
      }
    }


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
    // Serial.println("Direction changed");
      changeDirection = false; // Reset the flag
      increment *= -1; // Reverse the increment to change direction
    }
}
void speed_up() {
  if (speedUp) {
 // Serial.println("Speed increased");
  speedUp = false;
  delayTime -= 2;
  }
}

void slow_down() {
  if (slowDown){
 // Serial.println("Speed decreased");
  slowDown = false;
  delayTime += 2;
  }
}

void Stop() {
 // Serial.println("Stop");
  stop = false;
  myServo.detach();
  return;
} 

void start_servo() {
 // Serial.println("Start");
  start = false;
  myServo.attach(servoPin);
  return;
} 

void repeat() {
  if (Repeat) {
      //Serial.println("Repeated");
      startPos = 70;
      endPos = 100;
      pos = startPos +1;
      Repeat = false;
    }
}

void reset_intervention() {
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

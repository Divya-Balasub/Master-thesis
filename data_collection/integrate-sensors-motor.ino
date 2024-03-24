#include <Arduino.h>
#include <ESP32Servo.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

Servo myServo;

// Pin connected to the servo motor signal wire 3-black, 5-purple,15-orange,16-yellow
const int servoPin = 12;
int anglePin = 1;

bool motorAttached = true;
const int minAngle = 30;
const int maxAngle = 110;

int pos = minAngle; 
int increment = 1;

const int FSR_PIN1 = 13; //black
const int FSR_PIN2 = 5; //purple
const int FSR_PIN3 = 15; //orange
const int FSR_PIN4 = 16; //yellow
TaskHandle_t Task1;
TaskHandle_t Task2;

uint16_t BNO055_SAMPLERATE_DELAY_MS = 200;
const int motor_delay = 20; 

Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28, &Wire);

void setup() {
  Serial.begin(115200);
  myServo.attach(servoPin); 
  while (!Serial) delay(10); 

  /* Initialise the sensor */
  if (!bno.begin())
  {
    Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    while (1);
  }

  delay(1000);

  
  xTaskCreatePinnedToCore(
                    Task1code,   
                    "Task1",    
                    10000,      
                    NULL,       
                    1,          
                    &Task1,     
                    0);                           
  delay(500); 

  xTaskCreatePinnedToCore(
                    Task2code,   
                    "Task2",     
                    10000,       
                    NULL,        
                    1,           
                    &Task2,     
                    1);         
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
     delay(motor_delay);

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
  bno.getEvent(&accelerometerData, Adafruit_BNO055::VECTOR_ACCELEROMETER);

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
    char command = Serial.read();
    if (command == 'd')
    {
      detach_motor();
    }
    
    if (command == 'a')
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

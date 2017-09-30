#include <Wire.h> #include <Adafruit_Sensor.h>
#include <Adafruit_HMC5883_U.h>
#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
#include <SPI.h>//RF24 Comonents
#include <nRF24L01.h>
#include <RF24.h>
Dept. of ECE                                                                                                                                                           17
Gesture Controlled Robotic Vehicle
#define CE_PIN   9
#define CSN_PIN 10
#define OUTPUT_READABLE_YAWPITCHROLL
#define LED_PIN 13 // (Arduino is 13, Teensy is 11, Teensy++ is 6)
bool blinkState = false;
Adafruit_HMC5883_Unified mag = Adafruit_HMC5883_Unified(12345);
MPU6050 mpu;
bool dmpReady = false;  // set true if DMP init was successful
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64];
Quaternion q;           // [w, x, y, z]         quaternion container
VectorInt16 aa;         // [x, y, z]            accel sensor measurements
VectorInt16 aaReal;     // [x, y, z]            gravity-free accel sensor measurements
VectorInt16 aaWorld;    // [x, y, z]            world-frame accel sensor measurements
VectorFloat gravity;    // [x, y, z]            gravity vector
float euler[3];         // [psi, theta, phi]    Euler angle container
float ypr[3];
float yprDeg[3];

uint8_t teapotPacket[14] = { 
  '$', 0x02, 0,0, 0,0, 0,0, 0,0, 0x00, 0x00, '\r', '\n' };
volatile bool mpuInterrupt = false;     // indicates whether MPU interrupt pin has gone high
void dmpDataReady() {
  mpuInterrupt = true;
}
const uint64_t pipe = 0xE8E8F0F0E1LL; //RF24 pipe
RF24 radio(CE_PIN, CSN_PIN);
int trx[6];
void setup(void)
{
  // join I2C bus (I2Cdev library doesn't do this automatically)
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
  Wire.begin();
  TWBR = 24; // 400kHz I2C clock (200kHz if CPU is 8MHz)
#elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
  Fastwire::setup(400, true);
#endif
  Serial.begin(9600);
  //Serial.println("LOOKING FOR MAGNETOMETER AND COMPASS");
  if(!mag.begin())

  {
    //Serial.println("HMC5883 not detected");
    while(1); //INFINITE
  }
  mpu.initialize();
  devStatus = mpu.dmpInitialize();
  // supply your own gyro offsets here, scaled for min sensitivity
  mpu.setXGyroOffset(220);
  mpu.setYGyroOffset(76);
  mpu.setZGyroOffset(-85);
  mpu.setZAccelOffset(1788); // 1688 factory default for my test chip
  // make sure it worked (returns 0 if so)
  if (devStatus == 0) {
   
    mpu.setDMPEnabled(true);
  
    //        Serial.println(F("Enabling interrupt detection (Arduino external interrupt 0)..."));
    attachInterrupt(0, dmpDataReady, RISING);
    mpuIntStatus = mpu.getIntStatus();
    // set our DMP Ready flag so the main loop() function knows it's okay to use it
    //        Serial.println(F("DMP ready! Waiting for first interrupt..."));
    dmpReady = true;

    packetSize = mpu.dmpGetFIFOPacketSize();
  } 
  else {
    // ERROR!
    // 1 = initial memory load failed
    // 2 = DMP configuration updates failed
    // (if it's going to break, usually the code will be 1)
    //       Serial.print(F("DMP Initialization failed (code "));
    //       Serial.print(devStatus);
    //       Serial.println(F(")"));
  }
  // configure LED for output
  pinMode(LED_PIN, OUTPUT);
  
  //Mode configuration pin inputs
  pinMode(7,INPUT_PULLUP);
  pinMode(8,INPUT_PULLUP);
  radio.begin();//RF24 
  radio.openWritingPipe(pipe);
}
void loop()
{
  if (!dmpReady) return; //Dmp fail halt
Dept. of ECE                                                                                                                                                           21
Gesture Controlled Robotic Vehicle
  //Block For Magnetometer
  sensors_event_t event;
  mag.getEvent(&event);
  
  float heading = atan2(event.magnetic.y, event.magnetic.x); 
  float declinationAngle = 0.22;
  heading += declinationAngle;
  // Correct for when signs are reversed.
  if(heading < 0)
    heading += 2*PI;
  // Check for wrap due to addition of declination.
  if(heading > 2*PI)
    heading -= 2*PI;
  float headingDegrees = heading * 180/M_PI; 
  while (!mpuInterrupt && fifoCount < packetSize) {
  }
  mpuInterrupt = false;
  mpuIntStatus = mpu.getIntStatus();
  // get current FIFO count
  fifoCount = mpu.getFIFOCount();
  if ((mpuIntStatus & 0x10) || fifoCount == 1024) {

    // reset so we can continue cleanly
    mpu.resetFIFO();
    Serial.println(F("FIFO overflow!"));
    // otherwise, check for DMP data ready interrupt (this should happen frequently)
  } 
  else if (mpuIntStatus & 0x02) {
    // wait for correct available data length, should be a VERY short wait
    while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();
    // read a packet from FIFO
    mpu.getFIFOBytes(fifoBuffer, packetSize);
    // track FIFO count here in case there is > 1 packet available
    // (this lets us immediately read more without waiting for an interrupt)
    fifoCount -= packetSize;
#ifdef OUTPUT_READABLE_YAWPITCHROLL
    // display Euler angles in degrees
    mpu.dmpGetQuaternion(&q, fifoBuffer);
    mpu.dmpGetGravity(&gravity, &q);
    mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
    yprDeg[0]=ypr[0] * 180/M_PI;
    yprDeg[1]=ypr[1] * 180/M_PI;
    yprDeg[2]=ypr[2] * 180/M_PI;
    /*       Serial.print("ypr\t");
     Serial.print(yprDeg[0]);
     Serial.print("\t");
Dept. of ECE                                                                                                                                                           23
Gesture Controlled Robotic Vehicle
     Serial.print(yprDeg[1]);
     Serial.print("\t");
     Serial.println(yprDeg[2]);
     */#endif
    //blinkState = !blinkState;
    //digitalWrite(LED_PIN, blinkState);
  }
  trx[0] = headingDegrees;
  trx[1] = yprDeg[0];
  trx[2] = yprDeg[1];
  trx[3] = yprDeg[2];
  trx[4] = digitalRead(7);
  trx[5] = digitalRead(8);
  radio.write( trx, sizeof(trx) );
 } 

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
/*-----( Declare Constants and Pin Numbers )-----*/
#define CE_PIN   9
#define CSN_PIN 10
//--Magnetic sensor prerequisites
#include <Wire.h>
#include <Adafruit_Sensor.h>
Dept. of ECE                                                                                                                                                           24
Gesture Controlled Robotic Vehicle
#include <Adafruit_HMC5883_U.h>
// NOTE: the "LL" at the end of the constant is "LongLong" type
const uint64_t pipe = 0xE8E8F0F0E1LL; // Define the transmit pipe
Adafruit_HMC5883_Unified mag = Adafruit_HMC5883_Unified(12345); //Create an HMC5883 Object
//MaGNETOMETER Offset
const int defOffset=1;
int calOffset=0;
//Roll Offset
const int defRoll90=2;
int calRoll90=0;
//Mode storage incoming data from tx
short Mode1=1;
short Mode2=1;
/*-----( Declare objects )-----*/
RF24 radio(CE_PIN, CSN_PIN); // Create a Radio
/*-----( Declare Variables )-----*/
int rcx[6];  // 2 element array holding Joystick readings
int raWheadingDiff,headingDiff; //Heading difference between controller and host
int yaw,pitch,roll;
//current direction before turn
boolean acc=true; //true is forward

int calPin=4;
void setup()   /****** SETUP: RUNS ONCE ******/
{
  Serial.begin(9600);
  delay(1000);
  radio.begin();
  radio.openReadingPipe(1,pipe);
  radio.startListening();
  
  pinMode(calPin,INPUT_PULLUP);
  pinMode(3,OUTPUT);  
    if(!mag.begin())
  {
    /* There was a problem detecting the HMC5883 ... check your connections */
    while(1){
      
      int brightness = 0;    // how bright the LED is
      int fadeAmount = 5;
      analogWrite(3, brightness);    
  // change the brightness for next time through the loop:
  brightness = brightness + fadeAmount;
  // reverse the direction of the fading at the ends of the fade: 
  if (brightness == 0 || brightness == 255) {
    fadeAmount = -fadeAmount ;

      }
    }
  }
}//--(end setup )--
void loop()   /****** LOOP: RUNS CONSTANTLY ******/
{
  
  sensors_event_t event; //Declaration for magnetometer objects
  mag.getEvent(&event);
    // Calculate heading when the magnetometer is level, then correct for signs of axis.
  float heading = atan2(event.magnetic.y, event.magnetic.x);
  
  float declinationAngle = 0.22;
  heading += declinationAngle;
  
  // Correct for when signs are reversed.
  if(heading < 0)
    heading += 2*PI;
    
  // Check for wrap due to addition of declination.
  if(heading > 2*PI)
    heading -= 2*PI;
   
  // Convert radians to degrees for readability.

  int headingDegrees = heading * 180/M_PI;
  int remoteHeading;
  
  
  if ( radio.available() )
  {
    // Read the data payload until we've received everything
    bool done = false;
    while (!done)
    {
      // Fetch the data payload
      done = radio.read( rcx, sizeof(rcx) );
      remoteHeading = rcx[0];
      yaw=rcx[1];
      pitch=rcx[2];
      roll=rcx[3];
      Mode1=rcx[4];//Green Mode pin
      Mode2=rcx[5];//Blue Mode Pin
      if(digitalRead(calPin)==0)
      {
        calOffset=headingDegrees-defOffset;
      }
      raWheadingDiff = headingDegrees-remoteHeading;
      
      if(raWheadingDiff>180)
      {
        headingDiff=raWheadingDiff-360;
      }

      else
      {
        headingDiff=raWheadingDiff;
      }
      
      headingDiff=headingDiff-(defOffset+calOffset);
      int speedy=pitch + 50;
      int speedx=speedy + 192;
      //constrain(speedx,10,255);
      if(speedx<20)//constrain
      {  speedx=20;}
      else if(speedx>254)
       { speedx=254; }
   
   if(Mode1 == 0)
   {
      acc=false;
   }  
   else
      acc=true;
          
if(Mode2 == 1)
{
      //Compass reference Steering Mode
      
      Serial.println(speedx);
      if(headingDiff>10)

       {
         Serial.println(3);
         
       }
      else if(headingDiff<-10)
      {
        Serial.println(4);
        
      }
      else if(speedy<-22)                               
      {
        Serial.println(5);
      }else if(acc==true)
      {
        Serial.println(5);
      }
      else 
        Serial.println(5);
}
else if(Mode2 == 0)
{
      //Natural Steering Wheel Mode
      Serial.println(speedx);
      int roll90=defRoll90;
      int currentRoll;
      if(digitalRead(calPin)==0)
      {
        roll90 = calRoll90;

      }
      currentRoll=roll-roll90;
      if(currentRoll<-15)
       {
         Serial.println(4);
       }
      else if(currentRoll>15)
      {
        Serial.println(3);
      }
      else if(speedy<-20)                               
      {
        Serial.println(5);
      } else if(acc==true)
      {
        Serial.println(2);
      }
      else 
        {
          Serial.println(1);
        }      
}
      
       
      /*if(headingDiff>3)
      {
        if*(hadingDiff<-3)
        {
Dept. of ECE                                                                                                                                                           31
Gesture Controlled Robotic Vehicle
          digitalWrite(trightPin,HIGH);
          delay(50);
          digitalWrite(trightPin,LOW);
        }
        if(headingDiff>3)
        {
          
         digitalWrite(tleftPin,HIGH);
         delay(50);
         digitalWrite(tleftPin,LOW);
        }
      }   */ 
     
     
    }
  }
  else
  {    
      //Serial.println("No radio available");
      digitalWrite(3,HIGH);
      delay(100);
      digitalWrite(3,LOW);
  }
} 

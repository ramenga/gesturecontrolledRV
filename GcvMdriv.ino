 void forward(void)
{  digitalWrite(3,LOW);
  digitalWrite(11,LOW);
  digitalWrite(7,LOW);
  digitalWrite(13,LOW);
  digitalWrite(4,HIGH);
  digitalWrite(12,HIGH);
  digitalWrite(8,HIGH);
  digitalWrite(2,HIGH);
}
void turnleft(void)
{  
  digitalWrite(3,LOW);
  digitalWrite(11,HIGH);
  digitalWrite(7,HIGH);
  digitalWrite(13,LOW);
  digitalWrite(4,HIGH);
  digitalWrite(12,LOW);
  digitalWrite(8,LOW);
  digitalWrite(2,HIGH);
}
void turnright(void)
{  
  digitalWrite(3,HIGH);

  digitalWrite(11,LOW);
  digitalWrite(7,LOW);
  digitalWrite(13,HIGH);
  digitalWrite(4,LOW);
  digitalWrite(12,HIGH);
  digitalWrite(8,HIGH);
  digitalWrite(2,LOW);
}
void reverse(void)
{
  digitalWrite(3,HIGH);
  digitalWrite(11,HIGH);
  digitalWrite(7,HIGH);
  digitalWrite(13,HIGH);
  digitalWrite(4,LOW);
  digitalWrite(12,LOW);
  digitalWrite(8,LOW);
  digitalWrite(2,LOW);
}
void changespeed(int speedy)
{
  analogWrite(5, speedy);
  analogWrite(6, speedy);
}

void stopit(void)
{
  analogWrite(5, 0);
  analogWrite(6, 0);
}
  int val;
void setup()
{ 
  Serial.begin(9600);
  pinMode(3,OUTPUT);
  pinMode(4,OUTPUT);
  pinMode(7,OUTPUT);
  pinMode(8,OUTPUT);
  pinMode(11,OUTPUT);
  pinMode(12,OUTPUT);
  pinMode(13,OUTPUT);
  pinMode(2,OUTPUT);
  
  
  
 pinMode(5,INPUT_PULLUP);
 pinMode(6,INPUT_PULLUP);  
}
void loop()
{
//int ak1=0;  
   

  int incoming = Serial.available();
  
  if (incoming > 0)
  {
   val = Serial.parseInt();  //Reads integers as integer rather than ASCI. Anything else returns 0
   if(val==1)
   {
     forward();
   }
   if(val==2)
   {
     reverse();
   }
   if(val==3)
   {
     turnleft();
   }
   if(val==4)
   {
     turnright();
   }
   if(val==5)
   {
     stopit();
   }
   if((val>=10)&&(val<=255))
   {
     changespeed(val);

   } 
   Serial.println(val);
  }
 }

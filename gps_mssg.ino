#include <SoftwareSerial.h>
#include <TinyGPS.h>

#define interrupt 2
#define echo 13
#define trig 16
#define led 15

SoftwareSerial sim900(12,14); //rx,tx
SoftwareSerial gpsSerial(4,5);
TinyGPS gps;

String phoneNumber = "+91<yournumber>";

float latitude=0.0,longitude=0.0;
unsigned long age=0;
int year=0;
byte month=0, day=0, hour=0, minutes=0, second=0, hundredths=0;
unsigned long fix_age=0;
void ICACHE_RAM_ATTR sendMessage();

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  sim900.begin(9600);
  gpsSerial.begin(9600);
  
  pinMode(echo,INPUT);
  pinMode(trig,OUTPUT);
  pinMode(interrupt,INPUT_PULLUP);
  pinMode(led,OUTPUT);
  
  attachInterrupt(digitalPinToInterrupt(interrupt),sendMessage,FALLING);
}


void loop() {
  int dist = getDistance();
//  Serial.print("Distance : ");
//  Serial.println(latitude);
  if(dist < 20)
    digitalWrite(led,HIGH);
  else
    digitalWrite(led,LOW);
    
  while(gpsSerial.available()>0)
    if(gps.encode(gpsSerial.read()))
      gpsdump();
  
  if(sim900.available()>0)
    Serial.write(sim900.read());
//  delay(20);
}

void gpsdump(){
  gps.f_get_position(&latitude,&longitude,&age);
  gps.crack_datetime(&year,&month,&day,&hour,&minutes,&second,&hundredths,&fix_age);  
}

void sendMessage(){
  String ltd = String(latitude,6),
  lngd = String(longitude,6);
  String msg="A crash has occurred at \nlatitude: "+ltd+"\nlongitude: "+lngd+
  "\non "+String(day,2)+"/"+String(month,2)+"/"+String(year,4)+" at "+String(hour,2)+":"+String(minutes,2)+
  ":"+String(second,2)+" !\n http://maps.google.com/maps?q="+ltd+","+lngd;
  int prev = millis();
  sim900.println("AT+CMGF=1");  
  while(millis() - prev < 1000);
  prev = millis(); 
  sim900.println("AT+CMGS=\""+phoneNumber+"\"\r");
  while(millis() - prev < 1000);
  prev = millis();
  sim900.println(msg);// The SMS text you want to send
  while(millis() - prev < 100);
  prev = millis();
  sim900.println((char)26);
  while(millis() - prev < 1000);
  prev = millis();
}


int getDistance(){
  digitalWrite(trig,LOW);
  delayMicroseconds(2);
  digitalWrite(trig,HIGH);
  delayMicroseconds(10);
  digitalWrite(trig,LOW);
  delayMicroseconds(10);

  int duration = pulseIn(echo,HIGH);
  return duration*0.034/2;
}

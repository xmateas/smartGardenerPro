#include <SimpleTimer.h>
#include <CayenneMQTTYun.h>
#include <DHT.h>

// Cayenne authentication info
char username[] = "d6b6d0a0-3679-11eb-8779-7d56e82df461";
char password[] = "642b7087d0bfc6548e8786fd250e3f1b4e673343";
char clientID[] = "0a74aa20-367a-11eb-883c-638d8ce4c23d";

SimpleTimer timer;

//=====Moisture Sensor=====
#define MOISTURE_PIN A0
int soil_moisture_value = 0;
int soil_moisture_percentage = 0;
const int AIR_VALUE = 900 ;
const int WATER_VALUE = 250;

//=====Temperature Sensor=====
#define DHTPIN 7
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);
float temperature;

//=====Bulb=====
#define BULB_PIN 4
#define ENA_PIN 3

//=====Fan=====
#define FAN1_PIN 9
#define FAN2_PIN 10

//=====Water Pump=====
#define MOTOR_PIN A2

//=====Light Sensor=====
#define LIGHT_PIN A1
float light; //log variable for light intensity
int light_value; //log variable for raw input from sensor

//=====LED STRIP=====
#define BLUE_LED_PIN 2
#define GREEN_LED_PIN 12

void setup() {
  Serial.begin(9600); //baud rate to 9600 bps
  dht.begin();
  pinMode(MOTOR_PIN, OUTPUT);
  pinMode(FAN1_PIN, OUTPUT);
  pinMode(FAN2_PIN, OUTPUT);
  pinMode(BULB_PIN, OUTPUT);
  pinMode(ENA_PIN, OUTPUT);
  pinMode(BLUE_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  Cayenne.begin(username, password, clientID); //access to Cayenne dashboard
  timer.setInterval(2000, LightSensor);
  timer.setInterval(5000, Temperature);
  timer.setInterval(10000, MoistureSensor);
  timer.setInterval(30000, WaterPump);
  timer.setInterval(100, BulbControl);
  timer.setInterval(1000, FanControl);
  timer.setInterval(60000, FanControlEvery6Min);  
}

void loop() {
 Cayenne.loop();
 timer.run();
}

void Temperature(){
  temperature = dht.readTemperature();
  }
  
void MoistureSensor(){
  soil_moisture_value = analogRead(MOISTURE_PIN);
  soil_moisture_percentage = map(soil_moisture_value, AIR_VALUE, WATER_VALUE, 0, 100);
  if(soil_moisture_percentage >= 100)
  {  
    Serial.println("100 %");
    Serial.println("-------------");
  }
  else if(soil_moisture_percentage <= 0)
  {
    Serial.println("0 %");
    Serial.println("-------------");
  } 
  else if(soil_moisture_percentage > 0 && soil_moisture_percentage < 100)
   {
    Serial.println(soil_moisture_percentage);
    Serial.println("-------------");
   }
}
   
void WaterPump(){
  if (soil_moisture_percentage < 60){
    digitalWrite(MOTOR_PIN, HIGH);
    delay(5000);
    digitalWrite(MOTOR_PIN, LOW);
  }
  else{
    digitalWrite(MOTOR_PIN, LOW);
  }
}

void FanControlEvery6Min(){
  if(temperature <= 23.5){
  analogWrite(FAN1_PIN, 255/2);
  analogWrite(FAN2_PIN, 255/2);
  delay(5000);
  analogWrite(FAN1_PIN, 0);
  analogWrite(FAN2_PIN, 0);
  }
}

void FanControl(){
  if(temperature > 30.5){
    analogWrite(FAN1_PIN, 255); 
    analogWrite(FAN2_PIN, 255);
  }
  else{
    analogWrite(FAN1_PIN, 0);
    analogWrite(FAN2_PIN, 0);
  }
}

void LightSensor(){
  light_value = analogRead(LIGHT_PIN);
  light = light_value;
}

void BulbControl(){
  if(temperature < 30){
    digitalWrite(BULB_PIN, HIGH);
    analogWrite(ENA_PIN, 200);
  }
  else if(temperature > 23){
    analogWrite(ENA_PIN, 0);
  }
}

CAYENNE_OUT_DEFAULT()
{  
  Cayenne.virtualWrite(1, temperature); //write data from sensor to virtual PIN 1
  Cayenne.virtualWrite(2, soil_moisture_percentage); //write data from sensor to virtual PIN 2
  Cayenne.virtualWrite(3, light); //write data from sensor to virtual PIN 3
}

CAYENNE_IN(4)
{
  analogWrite(BLUE_LED_PIN, getValue.asInt()); //get analog vlaue from slider Blue led
}

CAYENNE_IN(5)
{
  analogWrite(GREEN_LED_PIN, getValue.asInt()); //get analog value from slider Green led
}

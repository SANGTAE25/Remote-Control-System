#include <Servo.h>
#include <TimerThree.h>
#include "DHT.h"
#include <LiquidCrystal_I2C.h>
#include <RobotIRremoteTools.h>
#include <Stepper.h>
const uint8_t SERVO_PIN = 12;
const uint8_t CDS_SENSOR {A9};
const uint8_t PINGR={51U};
const uint8_t PINGG={52U};
const uint8_t PINGB={53U};
const uint8_t SWITCH={46U};
const uint8_t DHT11_PIN={A8};
const uint8_t SPEAKER={45U}; 

int alarm_time=0;
class Servo servo = Servo();
class LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 16, 2);
int step_per_revolution = 2048;
class Stepper stepping(step_per_revolution,11,9,10,8);
class DHT dht {DHT(DHT11_PIN, 11)};
int measure = 0;
uint32_t KEY = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200UL);
  beginIRremote();
  stepping.setSpeed(14L);
  pinMode(PINGR,OUTPUT);
  pinMode(PINGB,OUTPUT);
  pinMode(PINGG,OUTPUT);
  pinMode(SWITCH,OUTPUT);
  pinMode(SPEAKER,OUTPUT);
  pinMode(CDS_SENSOR,INPUT);
  servo.attach(SERVO_PIN);
  dht.begin();
  lcd.init();
  lcd.backlight(); 
}
void light_moctor()
{
  lcd.init();
  lcd.setCursor(0,0);
  lcd.print("CUTTON CONTROL");
  lcd.setCursor(0,1);
  lcd.print("ON");
  delay(1000UL);
  while(1){
    while(IRrecived()){
      KEY = getIRresult();  
      resumeIRremote();  
      if(KEY == -1) {
        break;
      }
      Serial.println(KEY,HEX);
    }
  
  if(KEY == 0xFF38C7){
    lcd.init();
    lcd.setCursor(0,0);
    lcd.print("CUTTON CONTROL");
    lcd.setCursor(0,1);
    lcd.print("OFF");
    break;
  }
  else{
    int light = analogRead(CDS_SENSOR);
    if (light > 600){
      lcd.init();
      lcd.setCursor(0,0);
      lcd.print("LIGHT IS : ");
      lcd.setCursor(0,1);
      lcd.print(light);
      servo.write(90);
      delay(100);
    }
    else if (light > 500){
      lcd.init();
      lcd.setCursor(0,0);
      lcd.print("LIGHT IS : ");
      lcd.setCursor(0,1);
      lcd.print(light);
      servo.write(70);
      delay(100);      
    }
    else if (light > 400){
      lcd.init();
      lcd.setCursor(0,0);
      lcd.print("LIGHT IS : ");
      lcd.setCursor(0,1);
      lcd.print(light);
      servo.write(50);
      delay(100);      
    }
    else if (light > 300){
      lcd.init();
      lcd.setCursor(0,0);
      lcd.print("LIGHT IS : ");
      lcd.setCursor(0,1);
      lcd.print(light);
      servo.write(30);
      delay(100);      
    }
    else{
      lcd.init();
      lcd.setCursor(0,0);
      lcd.print("LIGHT IS : ");
      lcd.setCursor(0,1);
      lcd.print(light);
      servo.write(10);
      delay(100);      
    }
  }
  }
}
void detect_person()
{
  interrupts();
  ++alarm_time;
  if(alarm_time == measure) {
    digitalWrite(SPEAKER,HIGH);
  }
  noInterrupts();
}

void meaplus()
{
  measure += 1;        
}
void meaplus10()
{
  measure += 10;        
}
void meaplus30()
{
  measure += 30;        
}

void meaminus()
{
  measure -= 1;        
}
void meaminus10()
{
  measure -= 10;        
}
void meaminus30()
{
  measure -= 30;        
}

void loop() {
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity(); 
  float heat = dht.computeHeatIndex(temperature, humidity);
  int light = analogRead(CDS_SENSOR);
 
  while(IRrecived()){
    KEY = getIRresult();  
    resumeIRremote();  
    if(KEY == -1) {
      break;
    }
    Serial.println(KEY,HEX);
    switch(KEY){
        case 0xFFA25D:
          lcd.init();
          digitalWrite(PINGR, HIGH);
          digitalWrite(PINGG, LOW);
          digitalWrite(PINGB, LOW);
          lcd.setCursor(0,0); 
          lcd.print("LED ON");  
          lcd.setCursor(0,1);
          lcd.print("RED LIGHT");
          delay(500UL);
          break;
    
        case 0xFF629D:
          lcd.init();
          digitalWrite(PINGB, HIGH);
          digitalWrite(PINGR, HIGH);
          digitalWrite(PINGG, HIGH);
          lcd.setCursor(0,0); 
          lcd.print("LED ON");  
          lcd.setCursor(0,1);
          lcd.print("WHITE LIGHT");
          delay(500UL);
          break;

        case 0xFFE21D:
          lcd.init();
          digitalWrite(PINGB, LOW);
          digitalWrite(PINGG, LOW);
          digitalWrite(PINGR, LOW);
          lcd.setCursor(0,0); 
          lcd.print("LED OFF");  
          delay(500UL);
          break;

        case 0xFFC23D:
          lcd.init();
          lcd.setCursor(0,0); 
          lcd.print("OPEN DOOR");
          digitalWrite(SWITCH,HIGH);
          stepping.step(step_per_revolution);
          delay(500UL);
          break;

        case 0xFF906F:
          lcd.init();
          lcd.setCursor(0,0); 
          lcd.print("CLOSE DOOR");
          stepping.step(-step_per_revolution);
          digitalWrite(SWITCH,LOW);        
          delay(500UL);
          break;
          
        case 0xFF6897:
          lcd.clear(); 
          lcd.setCursor(0,0);
          lcd.print("Temperautre : ");
          lcd.setCursor(0,1);
          lcd.print(temperature);
          delay(500UL);
          break;

        case 0xFF30CF:
          lcd.init(); 
          lcd.setCursor(0,0);
          lcd.print("Humidity : ");
          lcd.setCursor(0,1);
          lcd.print(humidity);
          delay(500UL);
          break;

        case 0xFF18E7:
          lcd.init(); 
          lcd.setCursor(0,0);
          lcd.print("ALARM ON : ");
          lcd.setCursor(0,1);
          lcd.print("WAKE UP");
          alarm_time = 0;
          Timer3.initialize(1000000);
          Timer3.attachInterrupt(detect_person);
          break;
        
        case 0xFF7A85:
          lcd.init(); 
          lcd.setCursor(0,0);
          lcd.print("ALARM OFF : ");
          digitalWrite(SPEAKER,LOW);
          measure = 0;
          break;          

        case 0xFFA857:
          lcd.init(); 
          lcd.setCursor(0,0);
          lcd.print("TIME IS : ");
          meaplus();
          lcd.setCursor(0,1);
          lcd.print(measure);
          break;

        case 0xFFE01F:
          lcd.init(); 
          lcd.setCursor(0,0);
          lcd.print("TIME IS : ");
          meaminus();
          lcd.setCursor(0,1);
          lcd.print(measure);
          break;
                
        case 0xFF10EF:
          servo.write(0);
          light_moctor();   
          break;

        case 0xFF22DD:
          lcd.init(); 
          lcd.setCursor(0,0);
          lcd.print("TIME IS : ");
          meaminus10();
          lcd.setCursor(0,1);
          lcd.print(measure);
          break;

        case 0xFF02FD:
          lcd.init(); 
          lcd.setCursor(0,0);
          lcd.print("TIME IS : ");
          meaplus10();
          lcd.setCursor(0,1);
          lcd.print(measure);
          break;

        case 0xFF9867:
          lcd.init(); 
          lcd.setCursor(0,0);
          lcd.print("TIME IS : ");
          meaminus30();
          lcd.setCursor(0,1);
          lcd.print(measure);
          break;      

        case 0xFFB04F:
          lcd.init(); 
          lcd.setCursor(0,0);
          lcd.print("TIME IS : ");
          meaplus30();
          lcd.setCursor(0,1);
          lcd.print(measure);
          break;            


    }
  }                          
}

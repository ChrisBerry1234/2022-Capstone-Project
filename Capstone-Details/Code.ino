//include libraries 
#include <FiniteStateMachine.h>
#include <Servo.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_MLX90614.h>
#include "DHT.h"

//define PINS
#define OLED_RESET    4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define MOSFET 6
#define RED_LED 2
#define BLUE_LED 8
#define GREEN_LED 5
#define BUZZER 11
#define DHTPIN 4
#define DHTTYPE DHT11

//create objects
DHT dht(DHTPIN, DHTTYPE);
Adafruit_SSD1306 display(OLED_RESET);
Adafruit_MLX90614 mlx = Adafruit_MLX90614();
Servo myservo;
Servo myservo2;

//general variables
int pos = 0;
int currentpos = pos;
int y;
int temp;
int fanspeed;
float HIGHTEMP = 90.99; 
float MIDTEMP = 86.99;

//How many states will we have
const byte NUMBER_OF_STATES = 3;

//initialize states
State FanOff = State(FanOff_fn);
State FanOn = State(FanOn_fn);
State MaxFan = State(MaxFan_fn);

//initialize state machine 
FSM FanStateMachine = FSM(FanOff); // start state

void setup() {

  Serial.begin(9600);
  pinMode(RED_LED, OUTPUT); //setting RED LED as the OUTPUT
  pinMode(GREEN_LED, OUTPUT); //setting GREEN LED AS OUTPUT
  pinMode(BLUE_LED, OUTPUT); //setting BLUE LED AS OUTPUT
  pinMode(BUZZER, OUTPUT); //setting BUZZER as OUTPUT
  pinMode(MOSFET, OUTPUT);
  dht.begin();
  myservo2.attach(3); //  attaches the second servo to pin 3
  myservo.attach(7);  // attaches the servo on pin 7 to the servo object

  display.begin(SSD1306_SWITCHCAPVCC, 0x3c);
  display.display();
  delay(2000); // Pause for 2 seconds
  // Clear the buffer
  display.clearDisplay();

  while (!Serial);
  Serial.println("Adafruit MLX90614 test");

  if (!mlx.begin()) {
    Serial.println("Error connecting to MLX sensor. Check wiring.");
    while (1);
  };

  Serial.print("Emissivity = "); Serial.println(mlx.readEmissivity());
  Serial.println("================================================");
}


void loop(){

  float f = dht.readTemperature(true);
  float h = dht.readTemperature(); 
  int y = int(f);
  
  OLEDBEGIN();

  temp = constrain(y,70, 93);
  int t = map(temp, 70, 93,0,2);
         
switch(t){

// if temperature is between 70 and 83, turn on the GREEN LED
// This temperature is fine and our servo will not move
case 0:
FanStateMachine.transitionTo(FanOff);break;

//if temperature is between 84 and 91, turn on the BLUE LED as sell
// temperature is starting to rise so........ servo starts to look for the rising heat source
case 1:
FanStateMachine.transitionTo(FanOn);break;

/* if temperature is between 92 and 95, turn on the RED LED 
and buzzer and alert user.*/
case 2:       
FanStateMachine.transitionTo(MaxFan);
break;
}
FanStateMachine.update();
}

void FanOff_fn(){ 
digitalWrite(GREEN_LED, HIGH);
digitalWrite(BLUE_LED, LOW);
digitalWrite(RED_LED, LOW);
digitalWrite(BUZZER, LOW);
myservo.write(90);
myservo2.write(90);   
}

void FanOn_fn(){
digitalWrite(BLUE_LED, HIGH);
digitalWrite(GREEN_LED, HIGH);
digitalWrite(RED_LED,LOW);
digitalWrite(BUZZER, LOW);

            //Temperature is starting to get hot, so now we will turn on the Infrared sensor 
           /*Infrared Sensor is attached to Servo Motor which is rotating slowly
            to scan for heat source that is giving the DHT11 Sensor High readings*/
            mlx.begin();

          if (mlx.readObjectTempF() <= MIDTEMP){  
            moveServo(); 
            OLEDBEGIN();
                      
              if (pos == 180){
                    pos = 0;
                    }               
               } 
                    if (mlx.readObjectTempF() >= MIDTEMP){
                         currentpos = myservo.read();
                          
                          do{
                          OLEDBEGIN();
                          TurnFanOn();
                          myservo2.write(currentpos);
                          myservo.write(currentpos);

                          }while(mlx.readObjectTempF() >= MIDTEMP);//had to use while loop because code wouldnt know what to do if temp was not at 80.99
           
                            analogWrite(MOSFET, 0);
                    }
}
                                                        
void MaxFan_fn(){
digitalWrite(BLUE_LED, LOW);
digitalWrite(GREEN_LED, LOW);
digitalWrite(RED_LED, HIGH);
digitalWrite(BUZZER, HIGH);

            //Temperature is starting to get hot, so now we will turn on the Infrared sensor 
             /*Infrared Sensor is attached to) Servo Motor which is rotating slowly
            to scan for heat source that is giving the DHT11 Sensor High readings*/
             mlx.begin();
             
             if (mlx.readObjectTempF() <= HIGHTEMP){
             moveServo();
             OLEDTEMPHIGH();
              
              if (pos == 180){
              pos = 0;
               }
          }
                  if (mlx.readObjectTempF() >= HIGHTEMP){
                    currentpos = myservo.read();

                         do{
                            OLEDFOUNDHEATSOURCE();
                             TurnFanOn();
                             myservo2.write(currentpos);
                             myservo.write(currentpos);

                            }while(mlx.readObjectTempF() >= HIGHTEMP);//had to use while loop because code wouldnt know what to do if temp was not at 80.99
           
                             analogWrite(MOSFET, 0);
                             
                  }
}
//creating function that wil move the servo
 void moveServo(){
    myservo.write(pos);
    pos = pos +5;
    delay(1);
    Serial.print("Ambient = "); Serial.print(mlx.readAmbientTempF());
    Serial.print("*F\tObject = "); Serial.print(mlx.readObjectTempF()); Serial.println("*F");
    Serial.println();
    delay(5);
    return pos; 
  }

void OLEDBEGIN(){
  float f = dht.readTemperature(true);
  float h = dht.readTemperature(); 
  int y = int(f);

  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(35,0);
  display.print("TEMPERATURE: ");
  display.drawCircle(78, 12, 2, WHITE);
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(50,10);
  display.print(y);
  display.print(" F");
  display.display();
  display.clearDisplay();
  
}

void OLEDTEMPHIGH(){  
display.clearDisplay();
display.setTextSize(1);
display.setTextColor(WHITE);
display.setCursor(0,0);
display.println(" TEMPERATURE IS HIGH!");
display.display();
display.setTextSize(1);
display.setTextColor(WHITE);
display.setCursor(0,10);
display.print("  SCANNING FOR HEAT ");
display.display();
display.setTextSize(1);
display.setTextColor(WHITE);
display.setCursor(45,20);
display.print("SOURCE!! ");
display.display();
display.clearDisplay();
}

void OLEDFOUNDHEATSOURCE(){
display.clearDisplay();
display.setTextSize(1);
display.setTextColor(WHITE);
display.setCursor(0,10);
display.println("  FOUND HEAT SOURCE!!");
display.display();
display.setTextSize(1);
display.setTextColor(WHITE);
display.setCursor(20,20);
display.println(" COOLING AREA!!!");
display.display();
display.clearDisplay();
}

void TurnFanOn(){
  float b = mlx.readObjectTempF();
  int mlxreads = int(b);
  mlxreads = constrain(mlxreads,80,100);
  fanspeed = map(mlxreads, 80, 100,100,255);
  analogWrite(MOSFET, fanspeed);
}
        
        

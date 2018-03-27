#include <math.h>

unsigned long startMillis;
unsigned long currentMillis;
const unsigned long period = 5;
const int ledPin = 13;      // LED connected to digital pin 13
const int selectEqPin = 9;
const int knockSensor = A0; // the piezo is connected to analog pin 0
const int thresholdPot = A1; 
const int factorPot = A2; 
int kFactor=1;
bool forwardflag;
bool activeflag=false;
int optionflag=0;
bool selectflag=true;
float x=0;
int y=0;
int sensorpeak;
// these variables will change:
int sensorReading = 0;      // variable to store the value read from the sensor pin
int threshold=0;
int ledState = LOW;         // variable used to store the last LED status, to toggle the light

void setup() {
  pinMode(ledPin, OUTPUT); // declare the ledPin as as OUTPUT
  pinMode(selectEqPin, INPUT); 
  Serial.begin(9600);       // use the serial port
  startMillis = millis();
}

void loop() {
 
  // read the sensor and store it in the variable sensorReading:
  sensorReading = analogRead(knockSensor);
  threshold = analogRead(thresholdPot);
  kFactor= 0.0029*analogRead(factorPot)+1;
  forwardflag = digitalRead(selectEqPin);

  
  if(forwardflag && selectflag){
    if(optionflag==5){
       optionflag=0;
     }else{
      optionflag++;
      }
    selectflag=false;
    }else if(!forwardflag && !selectflag){
      selectflag=true;
    
    }

    
  // if the sensor reading is greater than the threshold:
  if (sensorReading >= threshold) {
    if(activeflag==false){
          sensorpeak=sensorReading;
          while(true){
            sensorReading = analogRead(knockSensor);
            if(sensorReading>sensorpeak){
              sensorpeak=sensorReading;
            }else{
              break;
            }
          }
          sensorReading=sensorpeak;
          x=sensorReading*kFactor;
          if(x>1023){
            x=1023;
          }
          ledState = HIGH;
          switch(optionflag){
            case 0:
              y = 0.1241*x;
              break;
            case 1: 
              y = (int) 42.1884*log10(x+1);
              break;
            case 2:
              y = (int) exp((x)/210.8395)-1; 
              break;
            case 3: 
              y = (int) (1/(1+exp(-(x-511.5)/(95))))*127;
              break;
            case 4: 
              y = (int) (atan((x-511.5)/50)+PI/2)*(127/PI);
              break;
            case 5: 
              y =(int) (tanh((x-511.5)/245)+1)*(127/2);
              break; 
            default:
              y=(int) 0.1241*x;
              break;  
          }
          
          // update the LED pin itself:
          digitalWrite(ledPin, ledState);
           
           
           
            
           startMillis = millis();
           activeflag=true;
          
          // send the string "Knock!" back to the computer, followed by newline
    }
  }else if(activeflag){
    currentMillis= millis();
    if(currentMillis-startMillis>=period){
      activeflag=false;
     }
     
  }else{
    ledState = LOW;
    digitalWrite(ledPin, ledState);
    
    //Serial.println(0);
    }
    Serial.print(sensorReading);
    Serial.print("\n");
  delayMicroseconds(800);  // delay to avoid overloading the serial port buffer
}

float tanh(float x)
{
  float x0 = exp(x);
  float x1 = 1.0 / x0;

  return ((x0 + x1) / (x0 - x1));
}
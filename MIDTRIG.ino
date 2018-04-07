#include <math.h>

unsigned long startMillis; //start milliseconds from attack detection
unsigned long currentMillis;//current milliseconds to finish the period
unsigned long period = 0; //period set by potenciometer
int pinRead;
const int thresholdPot = A1; //Threshold Potenciometer
const int ledPin = 13;      // LED connected to digital pin 13
const int factorPot = A2; //Gain Potenciometer
const int timPot=A3;//retrigger time potenciometer
const int selectEqPin = 9; //select velocity curve with digital 9
const int recPin = 8;
const int channelPin = 7;
//show  velocity status curve  in LED1, 2 and 3
const int led1 = 10; 
const int led2 = 11;
const int led3 = 12;

const int led1c = 5; //Channel LED indicators
const int led2c = 4; //Channel LED indicators
const int led3c = 3; //Channel LED indicators

const int ledlock = 6;
int pinAssignments[2] ={A0,A1};
byte PadNote[1] = {40};         //array for trigger position. Only using A0.
int PadCutOff[1] = {400};          // pitch information associated with the first trigger (Mid C = 60). More to be added
int MaxPlayTime[1] = {20};               //Unused variable. Replaced with retrigger time
#define  midichannel 1;                              // MIDI channel from 0 to 15 (+1 in "real world"). Set as the first channel
boolean VelocityFlag  = true;                           // Velocity ON (true) or OFF (false) for retrigger and attack information
int peak =0;
boolean activePad[1] = {0};                   // Array of flags of pad currently playing
int PinPlayTime[1] = {0};                     // Counter since pad started to play
byte status1; //status byte for MIDI triggering
int sensorReading = 0; //piezosensor measurement
int y=0; //velocity curve result
int optionflag=0; //sempahore for velocity curve option
int pin = 0;     //only using one pin for now. IMprovements will require more triggers connected 
int hitavg = 0; //hitavg replaced with "y" to obtain velocitu crve result
int kFactor=1; //Gain Factor 
float x=0; //the multiplication of kFactor by sensorReading
bool forwardflag; //variable to obtain digital measurements
bool selectflag=true; //Push-Button for velocity curve selection flag

int ledState = LOW;  //LED for process verification
int threshold=0; //threshold for midi detection

bool recordflag; //flag for recording push button
bool lockflag=false;//flag for recording configuration
bool selectRecflag=true;//stay on after push

bool forwardChannelflag;//flag for channel push button
bool selectChannelflag=true;//stay after push 
int channelflag=0;//flag for channel option selection


//**In the future there will be arrays for channel selection
int lockThreshold=0;//lock threshold settings after recording
int lockGain=0;//lock gain after recording
int lockRetrigg=0;//lock retrigger time after recording
int lockoption=0;// lock velocity curve  after recording
void setup() 
{
   pinMode(ledPin, OUTPUT); // declare the ledPin as OUTPUT
   //Declare LED1,2 and 3 as outputs
   pinMode(led1, OUTPUT);
   pinMode(led2, OUTPUT);
   pinMode(led3, OUTPUT);
   pinMode(ledlock, OUTPUT);
   pinMode(led1c, OUTPUT);
   pinMode(led2c, OUTPUT);
   pinMode(led3c, OUTPUT);

   pinMode(selectEqPin, INPUT);//selectEquationPin as INPUT
  Serial.begin(57600);                                  //Set hairless to the same baudrate for optimal performance

}

void loop() 
{
  //for(int pin=0; pin < 16; pin++)                          //
  //{
    int pin = 0; //redundant and process-consuming. Delete.
    //   for (pinRead=0; pinRead < 16, pin++){
    //***OBTAIN POTENCIOMETER AND PIEZOSENSOR MEASUREMENTS***
    sensorReading = analogRead(pinAssignments[pin]); //Read piezosensor

    if(lockflag){
        threshold = lockThreshold; 
        kFactor= lockGain;
        period=lockRetrigg;
    }else{
        threshold = analogRead(thresholdPot); //Read threshold 
        kFactor= 0.0088*analogRead(factorPot)+1;//read Gain factor(Range: 1-5 )
        period=390.2248*analogRead(timPot)+800;//read retrigger time(Range: 800us-10ms)
   }
    

    
    forwardflag = digitalRead(selectEqPin);//read velocity curve option
    
    recordflag=digitalRead(recPin);//read configuration lock button
    forwardChannelflag=digitalRead(channelPin);
    
    //Serial.println(hitavg);   
    // Read flags and change option status

    if(forwardflag && selectflag && lockflag){
    if(optionflag==5){
       optionflag=0;
     }else{
      optionflag++;
      }
    selectflag=false;
    }else if(!forwardflag && !selectflag){
      selectflag=true;
    
    }

   
    //update velocity curve option with LED1, 2 and 3. 6 possible cases.
    switch(optionflag){
      case 0:
        digitalWrite(led1, LOW);
        digitalWrite(led2, LOW);
        digitalWrite(led3, LOW);
        break; 
      case 1:
      
        digitalWrite(led1, HIGH);
        digitalWrite(led2, LOW);
        digitalWrite(led3, LOW);
        break;
      case 2: 
       
        digitalWrite(led1, LOW);
        digitalWrite(led2, HIGH);
        digitalWrite(led3, LOW);
        break;
      case 3:
       
        digitalWrite(led1, HIGH);
        digitalWrite(led2, HIGH);
        digitalWrite(led3, LOW);
        break;
      case 4: 

        digitalWrite(led1, LOW);
        digitalWrite(led2, LOW);
        digitalWrite(led3, HIGH);
        break;
      case 5: 
        
        digitalWrite(led1, HIGH);
        digitalWrite(led2, LOW);
        digitalWrite(led3, HIGH);
        break;
      default:
        
        digitalWrite(led1, LOW);
        digitalWrite(led2, LOW);
        digitalWrite(led3, LOW);
        break;  
    }
    
    //Read flags and change lockflag status to set recording.
    if(recordflag && selectRecflag){
      if(lockflag){
        lockflag=false;
        digitalWrite(ledlock,LOW);
        
      }else{
        //capture parameters and lock settings
        lockflag=true;
        lockThreshold=threshold; 
        lockGain=kFactor;
        lockRetrigg=period;
        lockoption=optionflag;
        digitalWrite(ledlock,HIGH);
        }
      selectRecflag=false;
    }else if(!recordflag && !selectRecflag){
      selectRecflag=true;
    
    }
    //PUsh button for editing channel configuration. Channel is set from 0 to 2
    if(forwardChannelflag && selectChannelflag && !lockflag){
      if(channelflag==2){
        channelflag=0;
      }else{
        channelflag++;
        }
      selectChannelflag=false;
    }else if(!forwardChannelflag && !selectChannelflag){
      selectChannelflag=true;
    
    }
    //Channel visualization
    switch(channelflag){
      case 0:
        digitalWrite(led1c, HIGH);
        digitalWrite(led2c, LOW);
        digitalWrite(led3c, LOW);
        break; 
      case 1:
      
        digitalWrite(led1c, LOW);
        digitalWrite(led2c, HIGH);
        digitalWrite(led3c, LOW);
        break;
      case 2: 
       
        digitalWrite(led1c, LOW);
        digitalWrite(led2c, LOW);
        digitalWrite(led3c, HIGH);
        break;
      default:
        
        digitalWrite(led1c, LOW);
        digitalWrite(led2c, LOW);
        digitalWrite(led3c, LOW);
        break;  
    }
    
    if((activePad[pin] == true))//If Curve is decaying and attack time has already passed
    {
      currentMillis=micros();//measure current milliseconds
      if(currentMillis-startMillis > period)//if the period is exceeded then finish MIDI transmission
      {
        activePad[pin] = false;//set activetrigger to false
        MIDI_TX(144,PadNote[pin],0); //Finish MIDI transmission
        ledState = LOW;//Set built-in LED to LOW
      }
    }else if(sensorReading >= threshold){//If attack  exceeds threshold
      //determine the peak of the signal to estimate the attack
      while(!(peak>sensorReading)){
        peak= sensorReading;
        sensorReading = analogRead(pinAssignments[pin]);  
        delay(2); 
      }
      sensorReading=peak;
      
      if((activePad[pin] == false)){//If pad is not active
        if(VelocityFlag == true){//If velocity flage is true then make velocity calculations
          x=sensorReading*kFactor;
          if(x>1023){
            x=1023;//set  velocity upper limit
          }
        }else{
          x = 1023;
        }
        //set equation determination for velocity curves. All equations are adjusted
        switch(optionflag){
      case 0:
        y = 0.1241*x;//linear equation
        break;
      case 1: 
        y = (int) 42.1884*log10(x+1);//logarithmic equation
        break;
      case 2:
        y = (int) exp((x)/210.8395)-1; //exponential equation
        break;
      case 3: 
        y = (int) (1/(1+exp(-(x-511.5)/(95))))*127;//sigmoid equation
        break;
      case 4: 
        y = (int) (atan((x-511.5)/50)+PI/2)*(127/PI);//atan(x) equation
        break;
      case 5: 
        y =(int) (tanh((x-511.5)/245)+1)*(127/2);//tanh(x) equation
        break; 
      default:
        y=(int) 0.1241*x;
        break;  
    }
        if(y>127){
          y=127;//Out of Bounds emergency case
          }else if(y<0){
          y=0;
          }


        
        MIDI_TX(144,PadNote[pin],y); //Send MIDI information

        startMillis=micros();//start measuring retrigger time in micrseconds
        activePad[pin] = true;//sets decay period to true
      }

      peak=0;
      ledState = HIGH;//LED shows MIDI transmission
    }else{
      
    }
    digitalWrite(ledPin, ledState);//SHOW OPERATION in built-in LED
  //} 
}

//MIDI TRANSMISSION
void MIDI_TX(byte MESSAGE, byte PITCH, byte VELOCITY) 
{
  status1 = MESSAGE + midichannel;//send to serial status (midi channel and note on[144])
  Serial.write(status1);
  Serial.write(PITCH);//send pitch to serial
  Serial.write(VELOCITY);//send note velocity(intensity) to serial

}
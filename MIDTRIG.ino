#include <math.h>


unsigned long currentMillis;//current milliseconds to finish the period
unsigned long period = 0; //period set by potenciometer
int pinRead;
const int thresholdPot = A1; //Threshold Potenciometer

const int factorPot = A2; //Gain Potenciometer
const int timPot=A3;//retrigger time potenciometer
const int selectEqPin = 9; //select velocity curve with digital 9
const int recPin = 8;
const int channelPin = 7;
//show  velocity status curve  in LED1, 2 and 3
const int led1 = 10; 
const int led2 = 11;
const int led3 = 12;

const int led1c = 2; //Channel LED indicators
const int led2c = 13; //Channel LED indicators

const int ledlock = 6;
int pinAssignments[3] ={A0,A4,A5};
byte PadNote[3] = {40,37,43};         //array for trigger position. Only using A0.
#define  midichannel 1;                              // MIDI channel from 0 to 15 (+1 in "real world"). Set as the first channel
boolean VelocityFlag  = true;                           // Velocity ON (true) or OFF (false) for retrigger and attack information
int peak =0;
boolean activePad[3] = {0,0,0};                   // Array of flags of pad currently playing
int PinPlayTime[3] = {0,0,0};                     // Counter since pad started to play
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


//**Parameter array
int lockThreshold[3]={100,100,100};//lock threshold settings after recording
int lockGain[3]={1,1,1};//lock gain after recording
unsigned long  lockRetrigg[3]={1000,1000,1000};//lock retrigger time after recording
int lockoption[3]={0,0,0};// lock velocity curve  after recording
unsigned long startMillis[3]={0,0,0}; //start milliseconds from attack detection array
const int ledPin[3]={5,4,3};// LED array
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
   pinMode(selectEqPin, INPUT);//selectEquationPin as INPUT
  Serial.begin(57600);                                  //Set hairless to the same baudrate for optimal performance

}

void loop() 
{
    //NAVIGATION CYCLE**********************
    if(lockflag){
        lockThreshold[channelflag] = analogRead(thresholdPot)-50; //Read threshold 
        lockGain[channelflag]= 0.0176*analogRead(factorPot)+1;//read Gain factor(Range: 1-5 )
        lockRetrigg[channelflag]=390.2248*analogRead(timPot)+800;//read retrigger time(Range: 800us-10ms)
        if(lockThreshold[channelflag]<0){
          lockThreshold[channelflag]=0;
        }
   }
    

    
    forwardflag = digitalRead(selectEqPin);//read velocity curve option
    
    recordflag=digitalRead(recPin);//read configuration lock button
    forwardChannelflag=digitalRead(channelPin);
    
    //Serial.println(hitavg);   
    // Read flags and change option status

    if(forwardflag && selectflag && lockflag){
    if(lockoption[channelflag]==5){
       lockoption[channelflag]=0;
     }else{
      lockoption[channelflag]=lockoption[channelflag]+1;
      }
      
    selectflag=false;
    }else if(!forwardflag && !selectflag){
      selectflag=true;
    
    }

   
    //update velocity curve option with LED1, 2 and 3. 6 possible cases.
    switch(lockoption[channelflag]){
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
        break; 
      case 1:
      
        digitalWrite(led1c, LOW);
        digitalWrite(led2c, HIGH);
        break;
      case 2: 
       
        digitalWrite(led1c, HIGH);
        digitalWrite(led2c, HIGH);
        break;
      default:
        
        digitalWrite(led1c, LOW);
        digitalWrite(led2c, LOW);
        break;  
    }

    //SENSOR CYCLE**********************
    for(int i=0; i < 3; i++){     
          sensorReading = analogRead(pinAssignments[i]); //Read piezosensor
          if((activePad[i] == true))//If Curve is decaying and attack time has already passed
          {
            currentMillis=micros();//measure current milliseconds
            if(currentMillis-startMillis[i] > lockRetrigg[i])//if the period is exceeded then finish MIDI transmission
            {
              activePad[i] = false;//set activetrigger to false
              MIDI_TX(144,PadNote[i],0); //Finish MIDI transmission
              digitalWrite(ledPin[i],LOW);//Set built-in LED to LOW
            }
          }else if(sensorReading >= lockThreshold[i]){//If attack  exceeds threshold
            //determine the peak of the signal to estimate the attack
            while(!(peak>sensorReading)){
              peak= sensorReading;
              sensorReading = analogRead(pinAssignments[i]);  
              delay(1); 
            }
            sensorReading=peak;
            
            if((activePad[i] == false)){//If pad is not active
              if(VelocityFlag == true){//If velocity flage is true then make velocity calculations
                x=sensorReading*lockGain[i];
                if(x>1023){
                  x=1023;//set  velocity upper limit
                }
              }else{
                x = 1023;
              }
              //set equation determination for velocity curves. All equations are adjusted
              switch(lockoption[i]){
            case 0:
              y = 0.1241*x;//linear equation
              break;
            case 1: 
              y = (int) 42.1884*log10((double) (x+1));//logarithmic equation
              break;
            case 2:
              y = (int) exp((double)(x/210.8395))-1; //exponential equation
              break;
            case 3: 
              y = (int) (1/(1+exp((double)(-(x-511.5)/95))))*127;//sigmoid equation
              break;
            case 4: 
              y = (int) (atan((double) ((x-511.5)/50)+PI/2))*(127/PI);//atan(x) equation
              break;
            case 5: 
              y =(int) (tanh((double)(x-511.5)/245)+1)*(127/2);//tanh(x) equation
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
      
      
              
              MIDI_TX(144,PadNote[i],y); //Send MIDI information
      
              startMillis[i]=micros();//start measuring retrigger time in micrseconds
              activePad[i] = true;//sets decay period to true
            }
      
            peak=0;
            digitalWrite(ledPin[i],HIGH);//LED shows MIDI transmission
          }else{
            
          }
          
    } 
}

//MIDI TRANSMISSION
void MIDI_TX(byte MESSAGE, byte PITCH, byte VELOCITY) 
{
  status1 = MESSAGE + midichannel;//send to serial status (midi channel and note on[144])
  Serial.write(status1);
  Serial.write(PITCH);//send pitch to serial
  Serial.write(VELOCITY);//send note velocity(intensity) to serial

}
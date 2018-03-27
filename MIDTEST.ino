
int pinRead;
const int thresholdPot = A1; //Threshold Potenciometer on A1
const int ledPin = 13;      //Use built-in LED in Digital 13
int pinAssignments[2] ={A0,A1}; //array for trigger position. Only using A0.
byte PadNote[1] = {57};         // pitch information associated with the first trigger (Mid C = 60). More to be added
int PadCutOff[1] = {400};           // Minimum Analog value to cause a drum hit
int MaxPlayTime[1] = {600};               // Cycles before a 2nd hit is allowed
#define  midichannel 1;                              // MIDI channel from 0 to 15 (+1 in "real world")
boolean VelocityFlag  = true;                           // Velocity ON (true) or OFF (false)

//*******************************************************************************************************************
// Internal Use Variables
//*******************************************************************************************************************
boolean activePad[1] = {0};                   // Array of flags of pad currently playing
int PinPlayTime[1] = {0};                     // Counter since pad started to play
byte status1;

int pin = 0;     
int hitavg = 0;

int ledState = LOW;  
int threshold=0;
//*******************************************************************************************************************
// Setup
//*******************************************************************************************************************
void setup() 
{
   pinMode(ledPin, OUTPUT); // declare the ledPin as as OUTPUT
  Serial.begin(57600);                                  // SET HAIRLESS TO THE SAME BAUD RATE IN THE SETTINGS

}
//*******************************************************************************************************************
// Main Program
//*******************************************************************************************************************
void loop() 
{
  //for(int pin=0; pin < 16; pin++)                          //
  //{
    int pin = 0;
    //   for (pinRead=0; pinRead < 16, pin++){
    hitavg = analogRead(pinAssignments[pin]); 
    threshold = analogRead(thresholdPot); 
    //Serial.println(hitavg);   
    // read the input pin

    if(hitavg > threshold){
      if((activePad[pin] == false)){
        if(VelocityFlag == true){
          //hitavg = 127 / ((1023 - PadCutOff[pin]) / (hitavg - PadCutOff[pin]));    // With full range (Too sensitive ?)
          //hitavg = (hitavg / 8) -1 ;                                                 // Upper range
        }
        else
        {
          hitavg = 127;
        }
        MIDI_TX(144,PadNote[pin],hitavg); //note on

        PinPlayTime[pin] = 0;
        activePad[pin] = true;
      }else{
        PinPlayTime[pin] = PinPlayTime[pin] + 1;
      }


      ledState = HIGH;
    }
    else if((activePad[pin] == true))
    {
      PinPlayTime[pin] = PinPlayTime[pin] + 1;
      if(PinPlayTime[pin] > MaxPlayTime[pin])
      {
        activePad[pin] = false;
        MIDI_TX(144,PadNote[pin],0); 
        ledState = LOW;
      }
    }else{
      //nothing here... until now
    }
    digitalWrite(ledPin, ledState);//always show the status in LED
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
  import processing.serial.*;
int xmax=1023;
int xfactor =5;
float k=2;
int ymax=123*xfactor;
  Serial myPort;        // The serial port
  int xPos = 1;         // horizontal position of the graph
  float inByte = 0;
boolean drawf=false;
  void setup () {
    // set the window size:
    size(1023, 635);

    // List all the available serial ports
    // if using Processing 2.1 or later, use Serial.printArray()
    println(Serial.list());

    // I know that the first port in the serial list on my Mac is always my
    // Arduino, so I open Serial.list()[0].
    // Open whatever port is the one you're using.
    myPort = new Serial(this, Serial.list()[2], 9600);

    // don't generate a serialEvent() unless you get a newline character:
    myPort.bufferUntil('\n');

    // set initial background:
    background(255);
    ploteq();
  }
  void draw () {
    background(255);
    if(drawf){
    //  ploteq();//plot equations the first time
    drawf=false;
    }
    stroke(#336699);
    strokeWeight(10);
    point(inByte,((1/(1+exp(-((xmax-inByte)-511.5)/(95))))*127)*xfactor);
  }
  
  void ploteq(){
    float y;
  strokeWeight(1);  
  stroke(#336699);
  for(float x=0; x<xmax ; x=x+0.01){
    y = 0.1241*(xmax-x)*xfactor;//plot linear equation
    point(x, y);
  }
  stroke(#88BDCD);
  for(float x=0; x<xmax ; x=x+0.01){
    y = 42.1884*log10((xmax-x)+1)*xfactor;//plot logarithmic equation
    point(x, y);
  }
  stroke(#AFD275);
  for(float x=0; x<xmax ; x=x+0.01){
    y = (exp((xmax-x)/210.8395)-1)*xfactor;//plot exponential equation
    point(x, y);
  }
  stroke(#F76C6C);
  for(float x=0; x<xmax ; x=x+0.01){
    y = ((1/(1+exp(-((xmax-x)-511.5)/(95))))*127)*xfactor;//plot exponential equation
    point(x, y);
  }
  stroke(#F8E9A1);
  for(float x=0; x<xmax ; x=x+0.01){
    y = ((atan(((xmax-x)-511.5)/50)+PI/2)*(127/PI))*xfactor;//plot exponential equation
    point(x, y);
  }
  stroke(#F98866);
  for(float x=0; x<xmax ; x=x+0.01){
    y = (((float)Math.tanh(((xmax-x)-511.5)/245)+1)*(127/2))*xfactor;//plot exponential equation
    point(x, y);
  }
  }
  float log10(float x){ 
    return (log(x) / log(10));
  }
  
   void serialEvent (Serial myPort) {
    // get the ASCII string:
    String inString = myPort.readStringUntil('\n');

    if (inString != null) {
      // trim off any whitespace:
      inString = trim(inString);
      // convert to an int and map to the screen height:
      inByte = float(inString)*k;
      if(inByte>1023){
        inByte=1023;
      }
      println(inByte);
      //inByte = map(inByte, 0, 1023, 0, height);
    }
  }
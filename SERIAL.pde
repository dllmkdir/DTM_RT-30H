

  import processing.serial.*;

  Serial myPort;        // The serial port
  int xPos = 1;         // horizontal position of the graph
  float inByte = 0;
  float inTrigg = 0;

  void setup () {
    // set the window size:
    size(2000, 400);

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
    background(0);
  }

  void draw () {
    // draw the line:
    stroke(127, 34, 255);
    line(xPos, height, xPos, height - inByte);
    //stroke(#F76C6C);
    //line(xPos, height, xPos, height - inTrigg);

    // at the edge of the screen, go back to the beginning:
    if (xPos >= width) {
      xPos = 0;
      background(0);
    } else {
      // increment the horizontal position:
      xPos+=5;
    }
  }

  void serialEvent (Serial myPort) {
    // get the ASCII string:
    String inString = myPort.readStringUntil('\n');

    if (inString != null) {
      // trim off any whitespace:
      inString = trim(inString);
      //String[] listS = split(inString, ',');
      //inByte=float(listS[1]);
      //inTrigg=float(listS[0]);
      // convert to an int and map to the screen height:
      inByte = float(inString);
      println(inByte);
      inByte = map(inByte, 0, 1023, 0, height);
      //inTrigg = map(inTrigg, 0, 127, 0, height);
    }
  }
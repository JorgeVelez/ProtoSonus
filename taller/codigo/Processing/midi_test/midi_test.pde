import controlP5.*;
import themidibus.*; //Import the library

MidiBus myBus; // The MidiBus
ControlP5 cp5;

int nx = 16;
int ny = 12;

  int channel = 0;
  int pitch = 60;
  int velocity = 127;
  
  ArrayList<Knob>  buttons = new ArrayList<Knob> ();
  String[] buttonNames = { "OSC1", "OSC2","NOISE", "OCTAVE","ATTACK", "DECAY","SUSTAIN", "RELEASE","OSC1WAVE"
, "OSC2WAVE","DETUNE", "FILTER FREC","FILTER RES", "BEND RANGE","LFO SPEED", "LFO DEPTH","LFO MODE"};

void setup() {
  size(1200, 800);
  
   MidiBus.list();
  myBus = new MidiBus(this, 0, 3); 
  cp5 = new ControlP5(this);
  
  delay(1000);
  
  
  
  for (int i = 0; i < buttonNames.length; i++) {
    buttons.add(createButton(buttonNames[i], i, 64));
  }

  cp5.addMatrix("stepper")
     .setPosition(50, 200)
     .setSize(1100, 300)
     .setGrid(nx, ny)
     .setGap(10, 10)
     .setInterval(225)
     .setMode(ControlP5.MULTIPLES)
     .setColorBackground(color(120))
     .setBackground(color(40))
     ;
  
  cp5.getController("stepper").getCaptionLabel().hide();
  
  cp5.addKnob("BPM")
    .setValue(120)
    .setRange(1,400)
     .setPosition(20,20)
     .setRadius(15)
     .setDragDirection(Knob.VERTICAL)
    .setBroadcast(true)
    .onChange(new CallbackListener() { 
    public void controlEvent(CallbackEvent theEvent) {
      float value = theEvent.getController().getValue();
       //cp5.getController("stepper").setInterval((int)(60000/4/value));
    }
  }
  );
  
   for (int i = 0; i < buttons.size(); i++) {
    Knob but =buttons.get(i);
    but.setValue(but.getValue());
  }

  noStroke();
  smooth();
}

static final int arrayIndexOf(Object[] arr, Object search) {
  return arrayIndexOf(arr, search, 0);
}

static final int arrayIndexOf(Object[] arr, Object search, int start) {
  if (arr == null)   return -1;

  start = abs(start);

  if (search == null) {
    for (int i = start; i != arr.length; ++i)
      if (arr[i] == null)   return i;
  }

  else for (int i = start; i != arr.length; ++i)
    if (search.equals(arr[i]))  return i;

  return -1;
}

Knob createButton(String thePrefix, int theIndex, int theValue) {
  int rangeStart=1;
  int rangeEnd=128;
  int tickMarks=127;
  if(thePrefix=="OCTAVE"){
     rangeStart=0;
    rangeEnd=4;
    tickMarks=4;
  }
  if(thePrefix=="OSC1WAVE" || thePrefix=="OSC2WAVE" ){
     rangeStart=0;
    rangeEnd=3;
    tickMarks=3;
  }
  
  if(thePrefix=="LFO MODE"){
     rangeStart=0;
    rangeEnd=13;
    tickMarks=13;
  }

  return cp5.addKnob(thePrefix)
    .setValue(theValue)
    .setRange(rangeStart,rangeEnd)
     .setPosition(100 + (theIndex * 60),80)
     .setRadius(20)
     .setDragDirection(Knob.VERTICAL)
    .setNumberOfTickMarks(tickMarks)
    .setTickMarkLength(4)
    .snapToTickMarks(true)
    .setBroadcast(true)
    .onChange(new CallbackListener() { 
    public void controlEvent(CallbackEvent theEvent) {
      String name = theEvent.getController().getName();
      float value = theEvent.getController().getValue();
      println("got a press from a " + arrayIndexOf(buttonNames, name));
      
      //theEvent.getController().setValueLabel("1"); 

      
      int number=100+ arrayIndexOf(buttonNames, name);
      myBus.sendControllerChange(channel, number, (int)value);
    }
  }
  );
}

//void OSC1(int value) {
// int number = 100;
//  myBus.sendControllerChange(channel, number, value);
//}
//void OSC2(int value) {
// int number = 101;
//  myBus.sendControllerChange(channel, number, value);
//}
//void NOISE(int value) {
// int number = 102;
//  myBus.sendControllerChange(channel, number, value);
//}

void slider(int value) {
 int number = 16;

  myBus.sendControllerChange(channel, number, value);
  println("a knob event. setting background to "+value);
}


void draw() {
  background(0);
  fill(255, 100);
 
}


void stepper(int theX, int theY) {
  println("got it: "+theX+", "+theY);
  myBus.sendNoteOn(channel, pitch+theY, velocity); 
    delay(200);
  myBus.sendNoteOff(channel, pitch+theY, velocity); 
  }


void keyPressed() {
  if (key=='1') {
    cp5.get(Matrix.class, "myMatrix").set(0, 0, true);
  } 
  else if (key=='2') {
    cp5.get(Matrix.class, "myMatrix").set(0, 1, true);
  }  
  else if (key=='3') {
    cp5.get(Matrix.class, "myMatrix").trigger(0);
  }
  else if (key=='p') {
    if (cp5.get(Matrix.class, "myMatrix").isPlaying()) {
      cp5.get(Matrix.class, "myMatrix").pause();
    } 
    else {
      cp5.get(Matrix.class, "myMatrix").play();
    }
  }  
  else if (key=='0') {
    cp5.get(Matrix.class, "myMatrix").clear();
  }
}

void controlEvent(ControlEvent theEvent) {
   //println("this event was triggered by Controller n1"+theEvent.getType());
}

void noteOn(int channel, int pitch, int velocity) {
  // Receive a noteOn
  println();
  println("Note On:");
  println("--------");
  println("Channel:"+channel);
  println("Pitch:"+pitch);
  println("Velocity:"+velocity);
}

void noteOff(int channel, int pitch, int velocity) {
  // Receive a noteOff
  println();
  println("Note Off:");
  println("--------");
  println("Channel:"+channel);
  println("Pitch:"+pitch);
  println("Velocity:"+velocity);
}

void controllerChange(int channel, int number, int value) {
  // Receive a controllerChange
  println();
  println("Controller Change:");
  println("--------");
  println("Channel:"+channel);
  println("Number:"+number);
  println("Value:"+value);
}

void delay(int time) {
  int current = millis();
  while (millis () < current+time) Thread.yield();
}

#include "MIDIUSB.h"

#include <Audio.h>
#include <Wire.h>
#ifndef __SAMD51__
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#endif

//Mux Pins
#define MS0 4
#define MS1 5
#define MS2 6
#define MS3 9
#define MZ A3
#define MnumControls 15

int controlPin[] = {MS0, MS1, MS2, MS3};

int muxChannel[16][4] = {
  {0, 0, 0, 0}, //channel 0
  {1, 0, 0, 0}, //channel 1
  {0, 1, 0, 0}, //channel 2
  {1, 1, 0, 0}, //channel 3
  {0, 0, 1, 0}, //channel 4
  {1, 0, 1, 0}, //channel 5
  {0, 1, 1, 0}, //channel 6
  {1, 1, 1, 0}, //channel 7
  {0, 0, 0, 1}, //channel 8
  {1, 0, 0, 1}, //channel 9
  {0, 1, 0, 1}, //channel 10
  {1, 1, 0, 1}, //channel 11
  {0, 0, 1, 1}, //channel 12
  {1, 0, 1, 1}, //channel 13
  {0, 1, 1, 1}, //channel 14
  {1, 1, 1, 1} //channel 15
};

byte muxInput = 0;
int muxValues[MnumControls] = {};

#define MUXosc1 1
#define MUXosc2 2//c
#define MUXdetune 6//a
#define MUXmix1 3//c
#define MUXmix2 4
#define MUXmix3 5
#define MUXattack 8//d
#define MUXdecay 9//d
#define MUXsustain 11
#define MUXrelease 13//b
#define MUXlfospeed 12//b
#define MUXlfodepth 10
#define MUXlfomode 14
#define MUXfilterres 7//a
#define MUXfilterfreq 0

//POTENTIOMETERS
int potPin1 = A4;
int potPin2 = A5;
int outputValuePotPin1 = 0.0;
int sensorValuePotPin1 = 0;

//Switch pin numbers
#define numSwitch 3
#define SWosc1 2
#define SWosc2 3
#define SWlfo A5

AudioSynthWaveform       waveform2;      //xy=414,383
AudioSynthWaveform       waveform1;      //xy=418,324
AudioSynthNoisePink      pink1;          //xy=426,434
AudioMixer4              mixer1;         //xy=609,384
AudioFilterStateVariable filter1;        //xy=757,387
AudioEffectEnvelope      envelope1;      //xy=920,388
AudioOutputAnalogStereo        audioOut;           //xy=1059,388
AudioConnection          patchCord1(waveform2, 0, mixer1, 1);
AudioConnection          patchCord2(waveform1, 0, mixer1, 0);
AudioConnection          patchCord3(pink1, 0, mixer1, 2);
AudioConnection          patchCord4(mixer1, 0, filter1, 0);
AudioConnection          patchCord5(filter1, 0, envelope1, 0);
AudioConnection          patchCord6(envelope1, 0, audioOut, 0);
AudioConnection          patchCord7(envelope1, 0, audioOut, 1);

// GLOBAL VARIABLES
const byte BUFFER = 8; //Size of keyboard buffer
const float noteFreqs[128] = {8.176, 8.662, 9.177, 9.723, 10.301, 10.913, 11.562, 12.25, 12.978, 13.75, 14.568, 15.434, 16.352, 17.324, 18.354, 19.445, 20.602, 21.827, 23.125, 24.5, 25.957, 27.5, 29.135, 30.868, 32.703, 34.648, 36.708, 38.891, 41.203, 43.654, 46.249, 48.999, 51.913, 55, 58.27, 61.735, 65.406, 69.296, 73.416, 77.782, 82.407, 87.307, 92.499, 97.999, 103.826, 110, 116.541, 123.471, 130.813, 138.591, 146.832, 155.563, 164.814, 174.614, 184.997, 195.998, 207.652, 220, 233.082, 246.942, 261.626, 277.183, 293.665, 311.127, 329.628, 349.228, 369.994, 391.995, 415.305, 440, 466.164, 493.883, 523.251, 554.365, 587.33, 622.254, 659.255, 698.456, 739.989, 783.991, 830.609, 880, 932.328, 987.767, 1046.502, 1108.731, 1174.659, 1244.508, 1318.51, 1396.913, 1479.978, 1567.982, 1661.219, 1760, 1864.655, 1975.533, 2093.005, 2217.461, 2349.318, 2489.016, 2637.02, 2793.826, 2959.955, 3135.963, 3322.438, 3520, 3729.31, 3951.066, 4186.009, 4434.922, 4698.636, 4978.032, 5274.041, 5587.652, 5919.911, 6271.927, 6644.875, 7040, 7458.62, 7902.133, 8372.018, 8869.844, 9397.273, 9956.063, 10548.08, 11175.3, 11839.82, 12543.85};
byte globalNote = 0;
byte globalVelocity = 0;
byte globalChannel = 1;
int octave1 = 0;
int octave2 = 0;
const float DIV127 = (1.0 / 127.0);
float detuneFactor = 1;
float bendFactor = 1;
int bendRange = 12;

unsigned int LFOspeed = 2000;
float LFOpitch = 1;
float LFOdepth = 0;
byte LFOmodeSelect = 0;

int FILfreq =  10000;
float FILfactor = 1;

byte osc1Mode = 255; // 255 = Nonsense value to force startup read
byte osc2Mode = 255;

//Switch pin numbers
#define numSwitch 3
#define SWosc1 2
#define SWosc2 3
#define SWlfo 4

//MIDI CC control numbers
#define CCmixer1 100
#define CCmixer2 101
#define CCmixer3 102
#define CCoctave 103
#define CCattack 104
#define CCdecay 105
#define CCsustain 106
#define CCrelease 107
#define CCosc1 108
#define CCosc2 109
#define CCdetune 110
#define CCfilterfreq 111
#define CCfilterres 112
#define CCbendrange 113
#define CClfospeed 114
#define CClfodepth 115
#define CClfomode 116

void setup() {
  Serial.begin(115200);

  AudioMemory(20);

  pinMode(SWosc1, INPUT_PULLUP);
  pinMode(SWosc2, INPUT_PULLUP);
  pinMode(SWlfo, INPUT_PULLUP);

  pinMode(MS0, OUTPUT);
  pinMode(MS1, OUTPUT);
  pinMode(MS2, OUTPUT);
  pinMode(MS3, OUTPUT);

  digitalWrite(MS0, LOW);
  digitalWrite(MS1, LOW);
  digitalWrite(MS2, LOW);
  digitalWrite(MS3, LOW);

  waveform1.begin(WAVEFORM_SAWTOOTH);
  waveform1.amplitude(0.75);
  waveform1.frequency(82.41);
  waveform1.pulseWidth(0.5);

  waveform2.begin(WAVEFORM_SAWTOOTH);
  waveform2.frequency(123);
  waveform2.pulseWidth(0.5);
  waveform2.amplitude(0.75);

  pink1.amplitude(1.0);

  filter1.frequency(FILfreq);
  filter1.resonance((4.3 * (.5)) + 0.7);

  mixer1.gain(0, 1.0);
  mixer1.gain(1, 1.0);
  mixer1.gain(2, 0.0);

  envelope1.attack(50);
  envelope1.decay(50);
  envelope1.release(250);
}

void loop() {
  parseMidi();
  //
  //   sensorValuePotPin1 = analogRead(potPin1);
  //  outputValuePotPin1 = map(sensorValuePotPin1, 1024, 0, 0, 127);
  //  //Serial.print("sensor = ");
  //  //Serial.print(sensorValuePotPin1);
  //  //Serial.print("\t output = ");
  //  //Serial.println(outputValuePotPin1/1000.0);
  //  mixer1.gain(0, (outputValuePotPin1 * DIV127));
  //
  //  sensorValuePotPin1 = analogRead(potPin2);
  //  outputValuePotPin1 = map(sensorValuePotPin1, 1024, 0, 0, 127);
  //  FILfactor = outputValuePotPin1 * DIV127;
  //        FILfreq = 10000 * (outputValuePotPin1 * DIV127);
  //    filter1.frequency(FILfreq);

  //LFOupdate(false, LFOmodeSelect, FILfactor, LFOdepth);
  checkMux();
  //checkSwitch();
}

void parseMidi() {
  midiEventPacket_t rx;
  do {
    rx = MidiUSB.read();
    if (rx.header != 0) {
      if (rx.header == 0x8) {
        //        Serial.print("Received note off: ");
        //        Serial.print(rx.byte2);
        //        Serial.print("- speed: ");
        //        Serial.println(rx.byte3);

        myNoteOff(globalChannel, rx.byte2, rx.byte3);

      }
      if (rx.header == 0x9) {
        //        Serial.print("Received note on: ");
        //        Serial.print(rx.byte2);
        //        Serial.print("- speed: ");
        //        Serial.println(rx.byte3);

        myNoteOn(globalChannel, rx.byte2, rx.byte3);
      }

      if (rx.header == 0xb) {
        Serial.print("Received CC: ");
        Serial.print(rx.byte2);
        Serial.print("- value: ");
        Serial.println(rx.byte3);

        myControlChange(globalChannel, rx.byte2, rx.byte3);
      }

      /*Serial.print("Received: ");
        Serial.print(rx.header, HEX);
        Serial.print("-");
        Serial.print(rx.byte1, HEX);
        Serial.print("-");
        Serial.print(rx.byte2, DEC);
        Serial.print("-");
        Serial.println(rx.byte3, DEC);
      */
    }
  } while (rx.header != 0);
}

static unsigned long ReadTimetime = 0;

void checkMux() {

  int muxRead = analogRead(MZ);

  if (muxRead > (muxValues[muxInput] + 7) || muxRead < (muxValues[muxInput] - 7)) {
    muxValues[muxInput] = muxRead;
    muxRead = (muxRead >> 3); //Change range to 0-127
    switch (muxInput) {
      case MUXosc1:
        if (muxRead != osc1Mode) myControlChange(0, CCosc1, muxRead / 32);
        break;
      case MUXosc2:
        if (muxRead != osc2Mode) myControlChange(0, CCosc2, muxRead / 32);
        break;
      case MUXdetune:
        myControlChange(0, CCdetune, muxRead);
        break;
      case MUXmix1:
        myControlChange(0, CCmixer1, muxRead);
        break;
      case MUXmix2:
        myControlChange(0, CCmixer2, muxRead);
        break;
      case MUXmix3:
        myControlChange(0, CCmixer3, muxRead);
        break;
      case MUXattack:
        myControlChange(0, CCattack, muxRead);
        break;
      case MUXdecay:
        myControlChange(0, CCdecay, muxRead);
        break;
      case MUXsustain:
        myControlChange(0, CCsustain, muxRead);
        break;
      case MUXrelease:
        myControlChange(0, CCrelease, muxRead);
        break;
      case MUXlfospeed:
        myControlChange(0, CClfospeed, muxRead);
        break;
      case MUXlfodepth:
        myControlChange(0, CClfodepth, muxRead);
        break;
      case MUXlfomode:
        myControlChange(0, CClfomode, muxRead / 24);
        break;
      case MUXfilterres:
        myControlChange(0, CCfilterres, muxRead);
        break;
      case MUXfilterfreq:
        myControlChange(0, CCfilterfreq, muxRead);
        break;
    }
  }
  muxInput++;
  if (muxInput >= MnumControls) muxInput = 0;
  for (int i = 0; i < 4; i ++) {
    digitalWrite(controlPin[i], muxChannel[muxInput][i]);
  }

}

void checkSwitch() {

  if (digitalRead(SWosc1)) {
    octave1 = 12;
  } else {
    octave1 = 0;
  }

  if (digitalRead(SWosc2)) {
    octave2 = -12;
  } else {
    octave2 = 0;
  }

  if (digitalRead(SWlfo)) {
    if (LFOmodeSelect < 8) LFOmodeSelect = LFOmodeSelect + 8;
  } else {
    if (LFOmodeSelect >= 8) LFOmodeSelect = LFOmodeSelect - 8;
  }
}


void myNoteOn(byte channel, byte note, byte velocity) {
  // if ( note > 23 && note < 108 ) {
  globalNote = note;
  globalVelocity = velocity;
  //  keyBuff(note, true);
  //  LFOupdate(true, LFOmodeSelect, FILfactor, LFOdepth);
  //}
  oscPlay(note);
  //envelope1.noteOn();
}

void myNoteOff(byte channel, byte note, byte velocity) {
  //if ( note > 23 && note < 108 ) {
  // keyBuff(note, false);
  //}
  oscStop();

  //envelope1.noteOff();
}

void myControlChange(byte channel, byte control, byte value) {
  switch (control) {
    case CCmixer1:
      mixer1.gain(0, (value * DIV127)); //TEST gain limit to 0.3
      Serial.print("CC mix1");
      Serial.println(value, DEC);
      break;

    case CCmixer2:
      mixer1.gain(1, (value * DIV127));
      Serial.print("CC mix2");
      Serial.println(value, DEC);
      break;

    case CCmixer3:
      mixer1.gain(2, (value * DIV127));
      Serial.print("CC mix3");
      Serial.println(value, DEC);
      break;

    case CCoctave:
      Serial.print("CC octave");
      Serial.println(value, DEC);
      switch (value) {
        case 0:
          octave2 = 24;
          break;
        case 1:
          octave2 = 12;
          break;
        case 2:
          octave2 = 0;
          break;
        case 3:
          octave2 = -12;
          break;
        case 4:
          octave2 = -24;
          break;
      }
      oscSet();
      break;

    case CCattack:
      Serial.print("CC attack");
      Serial.println(value, DEC);
      envelope1.attack((3000 * (value * DIV127)) + 10.5);//TEST Attack min limit to 10.5ms
      break;

    case CCdecay:
      Serial.print("CC decay");
      Serial.println(value, DEC);
      envelope1.decay(3000 * (value * DIV127));
      break;

    case CCsustain:
      Serial.print("CCsustain");
      Serial.println(value, DEC);
      envelope1.sustain(value * DIV127);
      break;

    case CCrelease:
      Serial.print("CCrelease");
      Serial.println(value, DEC);
      envelope1.release(3000 * (value * DIV127));
      break;

    case CCosc1:
      Serial.print("CCosc1");
      Serial.println(value, DEC);
      switch (value) {
        case 0:
          waveform1.begin(WAVEFORM_SINE);
          osc1Mode = 0;
          break;
        case 1:
          waveform1.begin(WAVEFORM_TRIANGLE);
          osc1Mode = 1;
          break;
        case 2:
          waveform1.begin(WAVEFORM_SAWTOOTH);
          osc1Mode = 2;
          break;
        case 3:
          waveform1.begin(WAVEFORM_PULSE);
          osc1Mode = 3;
          break;
      }
      break;

    case CCosc2:
      Serial.print("CCosc2");
      Serial.println(value, DEC);
      switch (value) {
        case 0:
          waveform2.begin(WAVEFORM_SINE);
          osc2Mode = 0;
          break;
        case 1:
          waveform2.begin(WAVEFORM_TRIANGLE);
          osc2Mode = 1;
          break;
        case 2:
          waveform2.begin(WAVEFORM_SAWTOOTH);
          osc2Mode = 2;
          break;
        case 3:
          waveform2.begin(WAVEFORM_PULSE);
          osc2Mode = 3;
          break;
      }
      break;

    case CCdetune:
      Serial.print("CCdetune");
      Serial.println(value, DEC);
      detuneFactor = 1 - (0.05 * (value * DIV127));
      oscSet();
      break;

    case CCfilterfreq:
      Serial.print("CCfilterfreq");
      Serial.println(value, DEC);
      FILfactor = value * DIV127;
      FILfreq = 10000 * (value * DIV127);
      //    if (LFOmodeSelect < 1 || LFOmodeSelect > 5)
      filter1.frequency(FILfreq);
      break;

    case CCfilterres:
      Serial.print("CCfilterres");
      Serial.println(value, DEC);
      filter1.resonance((4.3 * (value * DIV127)) + 0.7);
      break;

    case CCbendrange:
      Serial.print("CCbendrange");
      Serial.println(value, DEC);
      if (value <= 12 && value > 0) {
        bendRange = value;
      }
      break;

    case CClfospeed:
      {
        Serial.print("CClfospeed");
        Serial.println(value, DEC);
        float xSpeed = value * DIV127;
        xSpeed = pow(100, (xSpeed - 1));
        LFOspeed = (70000 * xSpeed);
        break;
      }

    case CClfodepth:
      Serial.print("CClfodepth");
      Serial.println(value, DEC);
      LFOdepth = value * DIV127;
      break;

      //    case CClfomode:
      //      LFOmodeSelect = value;
      //      break;
  }
  //Serial.print(": ");
  //Serial.println(value, DEC);

}


void LFOupdate(bool retrig, byte mode, float FILtop, float FILbottom) {
  static float LFO = 0;
  static unsigned long LFOtime = 0;
  static bool LFOdirection = false;
  unsigned long currentMicros = micros();
  static bool LFOstop = false;
  static float LFOrange = 0;
  static byte oldMode = 0;
  static bool retriggered = false;

  if (retrig == true) retriggered = true;


  if (currentMicros - LFOtime >= LFOspeed) {
    LFOtime = currentMicros;

    if (mode != oldMode) {
      if (mode == 0 || mode == 8) {
        LFOpitch = 1;
        oscSet();
        filter1.frequency(FILfreq);
      }
      else if (mode >= 1 || mode <= 7) {
        LFOpitch = 1;
        oscSet();
      }
      else if (mode >= 9 || mode <= 13) {
        filter1.frequency(FILfreq);
      }
      oldMode = mode;
    }

    LFOrange = FILtop - FILbottom;
    if (LFOrange < 0) LFOrange = 0;

    // LFO Modes
    switch (mode) {

      case 0: //Filter OFF
        return;
        break;
      case 1: //Filter FREE
        filter1.frequency(10000 * ((LFOrange * LFO) + LFOdepth));
        break;
      case 2: //Filter DOWN
        if (retriggered == true) {
          LFOdirection = true;
          LFO = 1.0;
        }
        filter1.frequency(10000 * ((LFOrange * LFO) + LFOdepth));
        break;
      case 3: //Filter UP
        if (retriggered == true) {
          LFOdirection = false;
          LFO = 0;
        }
        filter1.frequency(10000 * ((LFOrange * LFO) + LFOdepth));
        break;
      case 4: //Filter 1-DN
        if (retriggered == true) {
          LFOstop = false;
          LFOdirection = true;
          LFO = 1.0;
        }
        if (LFOstop == false) filter1.frequency(10000 * ((LFOrange * LFO) + LFOdepth));
        break;
      case 5: //Filter 1-UP
        if (retriggered == true) {
          LFOstop = false;
          LFOdirection = false;
          LFO = 0;
        }
        if (LFOstop == false) filter1.frequency(10000 * ((LFOrange * LFO) + LFOdepth));
        break;
      case 8: //Pitch OFF
        return;
        break;
      case 9: //Pitch FREE
        LFOpitch = (LFO * LFOdepth) + 1;
        oscSet();
        break;
      case 10: //Pitch DOWN
        if (retriggered == true) {
          LFOdirection = true;
          LFO = 1.0;
        }
        LFOpitch = (LFO * LFOdepth) + 1;
        oscSet();
        break;
      case 11: //Pitch UP
        if (retriggered == true) {
          LFOdirection = false;
          LFO = 0;
        }
        LFOpitch = (LFO * LFOdepth) + 1;
        oscSet();
        break;
      case 12: //Pitch 1-DN
        if (retriggered == true) {
          LFOstop = false;
          LFOdirection = true;
          LFO = 1.0;
        }
        if (LFOstop == false) {
          LFOpitch = (LFO * LFOdepth) + 1;
          oscSet();
        }
        break;
      case 13: //Pitch 1-UP
        if (retriggered == true) {
          LFOstop = false;
          LFOdirection = false;
          LFO = 0;
        }
        if (LFOstop == false) {
          LFOpitch = (LFO * LFOdepth) + 1;
          oscSet();
        }
        break;
    }

    retriggered = false;

    // Update LFO
    if (LFOdirection == false) { //UP
      LFO = (LFO + 0.01);
      if (LFO >= 1) {
        LFOdirection = true;
        LFOstop = true;
      }
    }

    if (LFOdirection == true) { //Down
      LFO = (LFO - 0.01);
      if (LFO <= 0) {
        LFOdirection = false;
        LFOstop = true;
      }
    }
  }
}

//OSCILLATORS
void oscPlay(byte note) {
  waveform1.frequency(noteFreqs[note + octave1] * bendFactor * LFOpitch);
  waveform2.frequency(noteFreqs[note + octave2] * detuneFactor * bendFactor * LFOpitch);
  float velo = 0.75 * (globalVelocity * DIV127);//TEST velocity limit to 0.75
  waveform1.amplitude(velo);
  waveform2.amplitude(velo);
  pink1.amplitude(velo);
  //envelope1.releaseNoteOn(5);//TEST
  envelope1.noteOn();
}

void oscStop() {
  envelope1.noteOff();
}

void oscSet() {
  waveform1.frequency(noteFreqs[globalNote + octave1] * bendFactor * LFOpitch);
  waveform2.frequency(noteFreqs[globalNote + octave2] * detuneFactor * bendFactor * LFOpitch);
}

//MIDI IN

void noteOn(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOn);
}

void noteOff(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOff = {0x08, 0x80 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOff);
}

void controlChange(byte channel, byte control, byte value) {
  midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
  MidiUSB.sendMIDI(event);
}

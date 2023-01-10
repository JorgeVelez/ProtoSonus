
#include <Audio.h>
#include <Wire.h>
#ifndef __SAMD51__
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#endif


// GUItool: begin automatically generated code
AudioSynthWaveform       waveform1;      //xy=86,211
AudioSynthWaveformSineModulated waveformMod1;   //xy=281,216
AudioSynthWaveform       waveform2;      //xy=316,278
AudioSynthWaveformSineModulated waveformMod3;   //xy=503,215
AudioSynthWaveformSineModulated waveformMod2;   //xy=511,292
AudioSynthNoiseWhite     noise1;         //xy=527,385
AudioSynthWaveform       waveform3;      //xy=609,477
AudioMixer4              mixer1;         //xy=746,288
AudioFilterStateVariable filter1;        //xy=887,379
AudioOutputAnalogStereo        audioOut;
AudioConnection          patchCord1(waveform1, 0, waveformMod1, 0);
AudioConnection          patchCord2(waveformMod1, 0, waveformMod3, 0);
AudioConnection          patchCord3(waveform2, 0, waveformMod2, 0);
AudioConnection          patchCord4(waveformMod3, 0, mixer1, 0);
AudioConnection          patchCord5(waveformMod2, 0, mixer1, 1);
AudioConnection          patchCord6(noise1, 0, mixer1, 2);
AudioConnection          patchCord7(waveform3, 0, filter1, 1);
AudioConnection          patchCord8(mixer1, 0, filter1, 0);
AudioConnection          patchCord9(filter1, 0, audioOut, 0);
AudioConnection          patchCord10(filter1, 0, audioOut, 1);
// GUItool: end automatically generated code

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
// GLOBAL VARIABLES
const float noteFreqs[128] = {8.176, 8.662, 9.177, 9.723, 10.301, 10.913, 11.562, 12.25, 12.978, 13.75, 14.568, 15.434, 16.352, 17.324, 18.354, 19.445, 20.602, 21.827, 23.125, 24.5, 25.957, 27.5, 29.135, 30.868, 32.703, 34.648, 36.708, 38.891, 41.203, 43.654, 46.249, 48.999, 51.913, 55, 58.27, 61.735, 65.406, 69.296, 73.416, 77.782, 82.407, 87.307, 92.499, 97.999, 103.826, 110, 116.541, 123.471, 130.813, 138.591, 146.832, 155.563, 164.814, 174.614, 184.997, 195.998, 207.652, 220, 233.082, 246.942, 261.626, 277.183, 293.665, 311.127, 329.628, 349.228, 369.994, 391.995, 415.305, 440, 466.164, 493.883, 523.251, 554.365, 587.33, 622.254, 659.255, 698.456, 739.989, 783.991, 830.609, 880, 932.328, 987.767, 1046.502, 1108.731, 1174.659, 1244.508, 1318.51, 1396.913, 1479.978, 1567.982, 1661.219, 1760, 1864.655, 1975.533, 2093.005, 2217.461, 2349.318, 2489.016, 2637.02, 2793.826, 2959.955, 3135.963, 3322.438, 3520, 3729.31, 3951.066, 4186.009, 4434.922, 4698.636, 4978.032, 5274.041, 5587.652, 5919.911, 6271.927, 6644.875, 7040, 7458.62, 7902.133, 8372.018, 8869.844, 9397.273, 9956.063, 10548.08, 11175.3, 11839.82, 12543.85};
int FILfreq =  800;


void setup() {
  Serial.begin(115200);

  AudioMemory(20);


  waveform1.begin(WAVEFORM_SAWTOOTH);
  waveform1.frequency(noteFreqs[48]);
  waveform1.pulseWidth(0.5);
  waveform1.amplitude(0.75);

  waveform2.begin(WAVEFORM_SAWTOOTH);
  waveform2.frequency(noteFreqs[52]);
  waveform2.pulseWidth(0.5);
  waveform2.amplitude(0.75);

  waveform3.begin(WAVEFORM_SAWTOOTH);
  waveform3.frequency(noteFreqs[55]);
  waveform3.pulseWidth(0.5);
  waveform3.amplitude(0.75);

  //waveformMod1.begin(WAVEFORM_SAWTOOTH);
  waveformMod1.frequency(noteFreqs[48]);
  //waveformMod1.pulseWidth(0.5);
  waveformMod1.amplitude(0.75);

  //waveformMod2.begin(WAVEFORM_SAWTOOTH);
  waveformMod2.frequency(noteFreqs[48]);
  //waveformMod2.pulseWidth(0.5);
  waveformMod2.amplitude(0.65);

  //waveformMod3.begin(WAVEFORM_SAWTOOTH);
  waveformMod3.frequency(noteFreqs[48]);
  //waveformMod3.pulseWidth(0.5);
  waveformMod3.amplitude(0.65);

  noise1.amplitude(0.5);

  filter1.frequency(FILfreq);
  filter1.resonance((4.3 * (.5)) + 0.7);

  mixer1.gain(0, 0.5);
  mixer1.gain(1, 0.5);
  mixer1.gain(2, 0.0);

  pinMode(MS0, OUTPUT);
  pinMode(MS1, OUTPUT);
  pinMode(MS2, OUTPUT);
  pinMode(MS3, OUTPUT);

  digitalWrite(MS0, LOW);
  digitalWrite(MS1, LOW);
  digitalWrite(MS2, LOW);
  digitalWrite(MS3, LOW);
}

void loop() {

  checkMux();

}

void checkMux() {
  int muxRead = map(analogRead(MZ), 1024, 0, 0, 1024);

  if (muxRead > (muxValues[muxInput] + 7) || muxRead < (muxValues[muxInput] - 7)) {
    muxValues[muxInput] = muxRead;
    switch (muxInput) {
      case 0:
        Serial.print("CCfilterfreq");
        Serial.println(muxRead);
        FILfreq = 10000.0 * (muxRead / 1024.0);
        filter1.frequency(FILfreq);
        break;

    }
    muxInput++;
    if (muxInput >= MnumControls) muxInput = 0;
    for (int i = 0; i < 4; i ++) {
      digitalWrite(controlPin[i], muxChannel[muxInput][i]);
    }

  }
}

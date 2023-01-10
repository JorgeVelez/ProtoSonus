#include <Audio.h>
#include <Wire.h>
#ifndef __SAMD51__
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#endif



 AudioSynthWaveform       waveform2;     
 AudioSynthWaveform       waveform3;   
 AudioSynthWaveform       waveform1;    

 AudioMixer4              mixer1;      
 AudioFilterStateVariable filter1;     
 AudioOutputAnalogStereo  audioOut;

 AudioConnection          patchCord1(waveform2, 0, mixer1, 1);
 AudioConnection          patchCord2(waveform1, 0, mixer1, 0);
 AudioConnection          patchCord8(waveform3, 0, mixer1, 2);

 AudioConnection          patchCord4(mixer1, 0, filter1, 0);
 AudioConnection          patchCord6(filter1, 0, audioOut, 0);
 AudioConnection          patchCord7(filter1, 0, audioOut, 1);



// GLOBAL VARIABLES
const float noteFreqs[128] = {8.176, 8.662, 9.177, 9.723, 10.301, 10.913, 11.562, 12.25, 12.978, 13.75, 14.568, 15.434, 16.352, 17.324, 18.354, 19.445, 20.602, 21.827, 23.125, 24.5, 25.957, 27.5, 29.135, 30.868, 32.703, 34.648, 36.708, 38.891, 41.203, 43.654, 46.249, 48.999, 51.913, 55, 58.27, 61.735, 65.406, 69.296, 73.416, 77.782, 82.407, 87.307, 92.499, 97.999, 103.826, 110, 116.541, 123.471, 130.813, 138.591, 146.832, 155.563, 164.814, 174.614, 184.997, 195.998, 207.652, 220, 233.082, 246.942, 261.626, 277.183, 293.665, 311.127, 329.628, 349.228, 369.994, 391.995, 415.305, 440, 466.164, 493.883, 523.251, 554.365, 587.33, 622.254, 659.255, 698.456, 739.989, 783.991, 830.609, 880, 932.328, 987.767, 1046.502, 1108.731, 1174.659, 1244.508, 1318.51, 1396.913, 1479.978, 1567.982, 1661.219, 1760, 1864.655, 1975.533, 2093.005, 2217.461, 2349.318, 2489.016, 2637.02, 2793.826, 2959.955, 3135.963, 3322.438, 3520, 3729.31, 3951.066, 4186.009, 4434.922, 4698.636, 4978.032, 5274.041, 5587.652, 5919.911, 6271.927, 6644.875, 7040, 7458.62, 7902.133, 8372.018, 8869.844, 9397.273, 9956.063, 10548.08, 11175.3, 11839.82, 12543.85};
int FILfreq =  800;

void setup() {
  Serial.begin(115200);

  AudioMemory(20);

   waveform1.begin(WAVEFORM_SAWTOOTH);
   waveform1.frequency(noteFreqs[48]);
   waveform1.pulseWidth(0.5);
   waveform1.amplitude(.5);

   waveform2.begin(WAVEFORM_SAWTOOTH);
   waveform2.frequency(noteFreqs[52]);
   waveform2.pulseWidth(0.5);
   waveform2.amplitude(.5);

   waveform3.begin(WAVEFORM_SAWTOOTH);
   waveform3.frequency(noteFreqs[55]);
   waveform3.pulseWidth(0.5);
   waveform3.amplitude(.5);


   filter1.frequency(FILfreq);
   filter1.resonance((4.3 * (.5)) + 0.7);

   mixer1.gain(0, 0.4);
   mixer1.gain(1, 0.4);
   mixer1.gain(2, 0.4);

}

void loop() {

  for (float x=0; x<1000; x++) {
    float val = 500+(500*(sin(x*(6.28/1000))));
    filter1.frequency(((val/1000)*2200));
    //Serial.println(val);
    delay(10);
  }

}

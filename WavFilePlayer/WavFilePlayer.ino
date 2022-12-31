// Simple WAV file player example for SD or QSPI flash storage
//
// Three types of output may be used, by configuring the code below.
//
//   1: Digital I2S - Normally used with the audio shield:
//         http://www.pjrc.com/store/teensy3_audio.html
//
//   2: Digital S/PDIF - Connect pin 22 to a S/PDIF transmitter
//         https://www.oshpark.com/shared_projects/KcDBKHta
//
//   3: Analog DAC - Connect the DAC pin to an amplified speaker
//         http://www.pjrc.com/teensy/gui/?info=AudioOutputAnalog
//
// To configure the output type, first uncomment one of the three
// output objects.  If not using the audio shield, comment out
// the sgtl5000_1 lines in setup(), so it does not wait forever
// trying to configure the SGTL5000 codec chip.
//
// The SD card may connect to different pins, depending on the
// hardware you are using.  Uncomment or configure the SD card
// pins to match your hardware.
//
// Data files to put on your SD card can be downloaded here:
//   http://www.pjrc.com/teensy/td_libs_AudioDataFiles.html
//
// This example code is in the public domain.

#include <Audio.h>
#include <SdFat.h>
#include <Adafruit_SPIFlash.h>
#include <play_fs_wav.h>
#include "Adafruit_TinyUSB.h"

AudioPlayFSWav           playWav1;
// Use one of these 3 output types: Digital I2S, Digital S/PDIF, or Analog DAC
//AudioOutputI2S           audioOutput;
//AudioOutputSPDIF       audioOutput;
AudioOutputAnalogStereo  audioOutput;    // Dual DACs
AudioConnection          patchCord1(playWav1, 0, audioOutput, 1);
AudioConnection          patchCord2(playWav1, 1, audioOutput, 0);
//AudioControlSGTL5000     sgtl5000_1;

#if defined(EXTERNAL_FLASH_USE_QSPI)
    Adafruit_FlashTransport_QSPI flashTransport;

  #elif defined(EXTERNAL_FLASH_USE_SPI)
    Adafruit_FlashTransport_SPI flashTransport(EXTERNAL_FLASH_USE_CS, EXTERNAL_FLASH_USE_SPI);

  #else
    #error No QSPI/SPI flash are defined on your board variant.h !
  #endif

Adafruit_SPIFlash flash(&flashTransport);
FatFileSystem QSPIFS;
SdFat SD;
bool SDOK=false, QSPIOK=false;

// USB Mass Storage object
Adafruit_USBD_MSC usb_msc;

// Set to true when PC write to flash
bool fs_changed;

void setup() {
  Serial.begin(9600);
  while (!Serial) delay(10);
  Serial.println("Wave player demo");
  delay(3000);

#ifdef SPEAKER_ENABLE
  pinMode(SPEAKER_ENABLE, OUTPUT);
  digitalWrite(SPEAKER_ENABLE, HIGH);
#endif

  // Audio connections require memory to work.  For more
  // detailed information, see the MemoryAndCpuUsage example
  AudioMemory(8);

  // Comment these out if not using the audio adaptor board.
  // This may wait forever if the SDA & SCL pins lack
  // pullup resistors
  //sgtl5000_1.enable();
  //sgtl5000_1.volume(0.5);


  
  QSPIOK = false;
  if (!flash.begin()) {
    Serial.println("Error, failed to initialize flash chip!");
  } else if (!QSPIFS.begin(&flash)) {
    Serial.println("Failed to mount QSPI filesystem!");
    Serial.println("Was CircuitPython loaded on the board first to create the filesystem?");
  } else {
    Serial.println("QSPI OK!");
    QSPIOK = true;
  }
}

void playFile(const char *filename)
{
  Serial.print("Playing file: "); Serial.println(filename);

  File f;
  
  if (SDOK) {
    f = SD.open(filename);
  } else if (QSPIOK) {
    f = QSPIFS.open(filename);
  }  
  // Start playing the file.  This sketch continues to
  // run while the file plays.
  if (!playWav1.play(f)) { 
    Serial.println("Failed to play");
    return;
  }

  // A brief delay for the library read WAV info
  delay(5);

  // Simply wait for the file to finish playing.
  while (playWav1.isPlaying()) {
    Serial.print(".");
    delay(100);
    // uncomment these lines if you audio shield
    // has the optional volume pot soldered
    //float vol = analogRead(15);
    //vol = vol / 1024;
    // sgtl5000_1.volume(vol);
  }
}


void loop() {
  playFile("SDTEST1.WAV");
  delay(500);
  playFile("SDTEST2.WAV");
  delay(500);
  playFile("SDTEST3.WAV");
  delay(500);
  playFile("SDTEST4.WAV");
  delay(1500);
}

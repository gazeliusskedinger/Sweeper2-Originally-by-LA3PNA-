/*
 Si5351 Simple Sweep Generator
 connect Si5351 to I2C
 Sweep out is on pin 5, ranging from 0-5V (3.3V).
 Use a filter on sweep out voltage. 100K + 1uF should be a good start.
 A op-amp can be used to improve the filtering of the DC voltage.

 Copyright (c) 2016 Thomas S. Knutsen <la3pna@gmail.com>

 Permission is hereby granted, free of charge, to any person obtaining
 a copy of this software and associated documentation files (the
 "Software"), to deal in the Software without restriction, including
 without limitation the rights to use, copy, modify, merge, publish,
 distribute, sublicense, and/or sell copies of the Software, and to
 permit persons to whom the Software is furnished to do so, subject
 to the following conditions:
 
 The above copyright notice and this permission notice shall be
 included in all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
 ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 Modified to my need and style by Rasmus Gazelius Skedinger 
 
 Added A sweep pauser function and two more variables that are settable, that the program needs.
 */

int correction = -15000; // use the Si5351 correction sketch to find the frequency correction factor
 
int inData = 0;
long steps = 100;
unsigned long  startFreq = 10000000;
unsigned long  stopFreq = 100000000;
unsigned long  pauseFreq1 = 0;
unsigned long  pauseFreq2 = 0;
int analogpin = 5;
int delaytime = 50;

#include <si5351.h>
#include "Wire.h"

Si5351 si5351;

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  si5351.init(SI5351_CRYSTAL_LOAD_8PF, 0,0);
  si5351.drive_strength(SI5351_CLK0, SI5351_DRIVE_6MA);
  //si5351.set_correction(correction);
  
  info();
}


void info(){
  Serial.println("SWEEPER");
  Serial.println("------");
  Serial.println(" Settings ");
  Serial.println("------");
  Serial.println("A = Start frequency");
  Serial.println("B = Stop frequency");
  Serial.println("S = Stepsize");
  Serial.println("M = single sweep");
  Serial.println("C = continious sweep untill Q");
  Serial.println("T = timestep in ms, currently 50.");
  Serial.println("1 = Set Pause freq 1(it has to be set together with steps so it triggers att frequency)");
  Serial.println("2 = Set Pause freq 2(it has to be set together with steps so it triggers att frequency)");
  Serial.println("V = continious sweep with pause or Q");
  
  Serial.print("Start: ");
  Serial.println(startFreq);
  Serial.print("Stop: ");
  Serial.println(stopFreq);
  Serial.print("time pr step: ");
  Serial.println(delaytime);
  Serial.print("Pause Freq 1: ");
  Serial.println(pauseFreq1);
  Serial.print("Pause Freq 2: ");
  Serial.println(pauseFreq2);
  
}


void loop() {
  inData = 0;
  if (Serial.available() > 0)   // see if incoming serial data:
  {
    inData = Serial.read();  // read oldest byte in serial buffer:
  }
  if (inData == 'M') {
    inData = 0;
    unsigned long freqstep = (stopFreq - startFreq) / steps  ;
    for (int i = 0; i < (steps+1); i++ ) {
      unsigned long freq = startFreq + (freqstep*i);
      si5351.set_freq_manual(freq*100ULL, 0, SI5351_CLK0);
      analogWrite(analogpin, map(i, 0, steps,0,255));
      delay(delaytime); 
      }
  }

  if (inData == 'C'){
    boolean runing = true;
    inData = 0;
    while(runing){
    unsigned long freqstep = (stopFreq - startFreq) / steps  ;
    for (int i = 0; i < (steps+1); i++ ) {
      
      unsigned long freq = startFreq + (freqstep*i);
      si5351.set_freq_manual(freq*100ULL, 0, SI5351_CLK0);
      Serial.println(freq);
      analogWrite(analogpin, map(i, 0, steps,0,255));
      delay(delaytime); 
     if (Serial.available() > 0)   // see if incoming serial data:
        {
        inData = Serial.read();  // read oldest byte in serial buffer:
        if (inData == 'Q'){
        runing = false;
        inData = 0;
        }
       }
      }
      info();
     }
    }
  

  if (inData == 'S') {
    steps = Serial.parseInt();
    Serial.print("Steps: ");
    Serial.println(steps);
    inData = 0;
  }
  
  if (inData == 'H'){
    info();
    }

  if (inData == 'T'){
    delaytime = Serial.parseInt(); 
    Serial.print("time pr step: ");
    Serial.println(delaytime);
    inData = 0;
    }

  if (inData == 'L') {
        for (int i = 0; i < (steps+1); i++ ) {

      // print out the value you read:
      Serial.print(i*10);
      Serial.print(';');
      Serial.print(steps);
      Serial.print(';');
      Serial.println(-i);
      delay(10);        // delay in between reads for stability
    }
    inData = 0;
  }
  
  if (inData == 'A') {
    startFreq = Serial.parseInt(); 
    Serial.print("Start: ");
    Serial.println(startFreq);
    inData = 0;

  }
  if (inData == 'B') {
    stopFreq = Serial.parseInt(); 
    Serial.print("Stop: ");
    Serial.println(stopFreq);
    inData = 0;
  }

  if (inData == '1') {
    pauseFreq1 = Serial.parseInt(); 
    Serial.print("Pause Freq 1: ");
    Serial.println(pauseFreq1);
    inData = 0;

  }
  if (inData == '2') {
    pauseFreq2 = Serial.parseInt(); 
    Serial.print("Pause Freq 2: ");
    Serial.println(pauseFreq2);
    inData = 0;
  }
  
  if (inData == 'V'){
    boolean runing = true;
    inData = 0;
    while(runing){
    unsigned long freqstep = (stopFreq - startFreq) / steps  ;
    for (int i = 0; i < (steps+1); i++ ) {
      
      unsigned long freq = startFreq + (freqstep*i);
      si5351.set_freq_manual(freq*100ULL, 0, SI5351_CLK0);
      Serial.println(freq);
      analogWrite(analogpin, map(i, 0, steps,0,255));
      delay(delaytime); 
      if(freq == 3500000 ){
        delay(1000);
        }
      if(freq == 3800000){
        delay(1000);
        }
      if (Serial.available() > 0)   // see if incoming serial data:
        {
        inData = Serial.read();  // read oldest byte in serial buffer:
        if (inData == 'Q'){
        runing = false;
        inData = 0;
        info();
        }
       }
      }
      info();
     }
    }
}

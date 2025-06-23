#ifndef talking_h
#define talking_h
#include <parameters.h>
#include <Wire.h>

int mapToFixedSet(uint8_t input) {
    const int lookup[] = {4, 2, 0, 1, 3};
    int index = input / 51; // 256 / 5 = approx. 51.2
    if (index > 4) index = 4;
    return lookup[index];
  }

void updateSliderValues() {
    for (int i = 0; i < 6; ++i) {
        int baseIndex = i * 7;
        channels[i].sinFreq = sliderValues[baseIndex + 0] / 16.0;
        channels[i].amp     = sliderValues[baseIndex + 1];
        channels[i].bias    = 255 - sliderValues[baseIndex + 2];
        channels[i].phase   = sliderValues[baseIndex + 3] * 4;
        channels[i].xScale  = sliderValues[baseIndex + 4] * 64;
        channels[i].yScale  = sliderValues[baseIndex + 5] * 64;
        channels[i].tScale  = sliderValues[baseIndex + 6];
    }

    //scramble1 = mapToFixedSet(sliderValues[42]);
    //scramble2 = mapToFixedSet(sliderValues[43]);
    hueShift=sliderValues[42];
    contrast=sliderValues[43];
    saturation=sliderValues[44];
}

/*void updateSliderValues(){
    sinFreqR=sliderValues[0]/16;
    amp_R=sliderValues[1];
    bias_R=255-sliderValues[2];
    phase_R=sliderValues[3]*4;
    xScaleR=sliderValues[4]*64;
    yScaleR=sliderValues[5]*64;
    tScaleR=sliderValues[6];

        sinFreqG=sliderValues[7]/16;
    amp_G=sliderValues[8];
    bias_G=255-sliderValues[9];
    phase_G=sliderValues[10]*4;
    xScaleG=sliderValues[11]*64;
    yScaleG=sliderValues[12]*64;
    tScaleG=sliderValues[13];

        sinFreqB=sliderValues[14]/16;
    amp_B=sliderValues[15];
    bias_B=255-sliderValues[16];
    phase_B=sliderValues[17]*4;
    xScaleB=sliderValues[18]*64;
    yScaleB=sliderValues[19]*64;
    tScaleB=sliderValues[20];

  
   scramble1=mapToFixedSet(sliderValues[21]);
   scramble2=mapToFixedSet(sliderValues[22]);
}*/

void initSliderValues(){
    for(int i=0;i<NUM_SLIDERS;i++){
        sliderValues[i]=128;
    }
    updateSliderValues();
}
void receiveEvent(int numBytes) {
  int i = 0;
  while (Wire1.available() && i < NUM_SLIDERS) {
    sliderValues[i++] = Wire1.read();
  }
  updateSliderValues();

}
void requestEvent() {
    Wire1.write(sliderValues, NUM_SLIDERS); 
}

void initTalking(){
    Wire1.begin(I2C_SLAVE_ADDR); // Initialize as I2C slave
    Wire1.onReceive(receiveEvent); // Register receive handler
    Wire1.onRequest(requestEvent);
}


#endif
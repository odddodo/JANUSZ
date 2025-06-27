#ifndef CONVOLUTION_H
#define CONVOLUTION_H
#include <FastLED.h>
#include "colorutils.h"
#include "colorpalettes.h"

int matrixsize=3;

float identity[3][3]={{0.0f,0.0f,0.0f},
                      {0.0f,1.0f,0.0f},
                      {0.0f,0.0f,0.0f}};
float darken[3][3]={{0.0f,0.0f,0.0f},
                      {0.0f,0.5f,0.0f},
                      {0.0f,0.0f,0.0f}};

float emboss[3][3]={{-2.0f,-1.0f,0.0f},
                      {-1.0f,1.0f,1.0f},
                      {0.0f,1.0f,2.0f}};

float edge_det[3][3]={{0.0f,1.0f,0.0f},
                      {1.0f,-4.0f,1.0f},
                      {0.0f,1.0f,0.0f}};

float sharpen[3][3]={{0.0f,-1.0f,0.0f},
                      {-1.0f,5.0f,-1.0f},
                      {0.0f,-1.0f,0.0f}};

float blur[3][3]={{1.0f/9.0f,1.0f/9.0f,1.0f/9.0f},
                  {1.0f/9.0f,1.0f/9.0f,1.0f/9.0f},
                  {1.0f/9.0f,1.0f/9.0f,1.0f/9.0f}};

float radicalEdgeKernel[3][3] = {
  {-2.0, -1.0,  0.0},
  {-1.0,  6.0,  1.0},
  { 0.0,  1.0,  2.0}
};

float negativeEmbossKernel[3][3] = {
  { 2.0,  1.0,  0.0},
  { 1.0, -1.0, -1.0},
  { 0.0, -1.0, -2.0}
};


float inversionKernel[3][3] = {
  { 1.0,  1.0,  1.0},
  { 1.0, -9.0,  1.0},
  { 1.0,  1.0,  1.0}
};


float ghostKernel[3][3] = {
  {-3, 0, 3},
  {-6, 0, 6},
  {-3, 0, 3}
};


CRGB convolution(int x, int y,float matrix[3][3], CRGB *img){
    
CRGB p_color=CRGB::Black;
float rtotal=0.0f;
float gtotal=0.0f;
float btotal=0.0f;
int offset = matrixsize/2;
for(int i=0;i<matrixsize;i++){
    for(int j=0;j<matrixsize;j++){
        int xloc=x+i-offset;
        int yloc=y+j-offset;
        int loc=xloc+64*yloc;
        loc=constrain(loc,0,4095);

        rtotal+=(img[loc].r*matrix[i][j]);
        gtotal+=(img[loc].g*matrix[i][j]);
        btotal+=(img[loc].b*matrix[i][j]);
    }
}

rtotal=constrain(rtotal,0,255);
gtotal=constrain(gtotal,0,255);
btotal=constrain(btotal,0,255);
p_color=CRGB(rtotal,gtotal,btotal);
return p_color;
}



#endif
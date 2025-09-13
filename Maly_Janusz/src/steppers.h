#ifndef STEPPERS_H
#define STEPPERS_H
#include <Arduino.h>
const int motorA_pins[4] = {41, 40, 39, 38};

// Motor B pins
const int motorB_pins[4] = {37, 36, 35, 34};

// 28BYJ-48 half-step sequence (8 steps per cycle)
const int steps[4][4] = {
    {1, 0, 0, 0},
    {0, 1, 0, 0},
    {0, 0, 1, 0},
    {0, 0, 0, 1}};

// Timing control
unsigned long stepInterval = 1; // ms between steps (speed control)
unsigned long lastStepTime = 0;
int stepIndex = 0;

void stepMotor(const int motorPins[4], int stepIdx)
{
    for (int i = 0; i < 4; i++)
    {
        digitalWrite(motorPins[i], steps[stepIdx][i]);
    }
}

void initSteppers()
{
    for (int i = 0; i < 4; i++)
    {
        pinMode(motorA_pins[i], OUTPUT);
        pinMode(motorB_pins[i], OUTPUT);
    }
}

void updateSteppers(uint16_t t)
{

    // Check if it's time for the next step
    // if (t - lastStepTime >= stepInterval) {
    // lastStepTime = t;

    // Step both motors clockwise
    stepMotor(motorA_pins, stepIndex);
    stepMotor(motorB_pins, stepIndex);

    stepIndex = (stepIndex + 1) % 4; // next step in sequence
    //}
}

#endif
// DspTests.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "TeaLFO/TeaLFO.h"
#include <iostream>
#include <fstream>
#include <string>

using namespace TTModules;
using namespace std;

float pi = 3.14159f;

float testwave(int idx) {
    float w = 2 * pi * (500.f / 44100.f);
    return cos(w*idx);
}

int main() {
    TeaLFO alfo;
    TeaLFO alfo2(pi/2);
    float freq = 1.f;
    float fs = 44100;
    alfo.setFreq(freq);
    alfo2.setFreq(freq);
    float* buffer;
    float* buffer2;
    int len = fs * 20;

    buffer = (float*)malloc(sizeof(float) * len);
    buffer2 = (float*)malloc(sizeof(float) * len);

    ofstream datafile("data.txt");
    ofstream datafile2("data2.txt");
    for (int i = 0; i < len; i++) {
        *buffer = 10*testwave(i);
        *buffer2 = 10*testwave(i);
        alfo.modulate(buffer, true);
        alfo2.modulate(buffer2, true);
        datafile << std::to_string(*buffer);
        datafile << ",";
        datafile2 << std::to_string(*buffer2);
        datafile2 << ",";
        buffer++;
        buffer2++;
    }
}
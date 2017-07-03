#ifndef ARDUINO_CONFIG_H
#define ARDUINO_CONFIG_H

#include "avr/pgmspace.h"

const int numInputNodes = 20;
const int numHiddenNodes = 10;
const int numOutputNodes = 1;
const float learningRate = 0.300000;
const float momentum = 0.900000;
const float initialWeightMax = 0.500000;

// TrainingCycle (not needed on Arduino): 317

const float hiddenWeights[numInputNodes +1][numHiddenNodes] PROGMEM = {
    { -0.208936, 0.465415, -0.051608, 0.266630, 0.244777, -0.345782, 0.473185, 0.130999, -0.512141, 0.541225 }, 
    { -0.184242, -0.185961, 0.048651, -0.447796, -0.403301, -0.377087, -0.363783, -0.344564, 0.268100, 0.156894 }, 
    { -0.438798, -0.231550, -0.432171, 0.417060, 0.295765, 0.006337, 0.381011, -0.045423, 0.253946, -0.132465 }, 
    { 0.157203, -0.181294, -0.175319, 0.237207, 0.400133, -0.268108, -0.034001, 0.321978, -0.561890, 0.015838 }, 
    { 0.095073, -0.414909, 0.395242, 0.111011, 0.269341, -0.449858, 0.206365, 0.524708, -0.181234, -0.351355 }, 
    { -0.466847, -0.126106, 0.147103, -0.334612, -0.083822, -0.268722, -0.237934, -0.125875, -0.332713, -0.008082 }, 
    { 0.123222, -0.198338, -0.274432, 0.513173, 0.302451, -0.456392, 0.008966, 0.280207, -0.410660, 0.301137 }, 
    { -0.513978, 0.149379, -0.238859, -0.288574, 0.239729, -0.248091, 0.330426, -0.257049, 0.216124, 0.568632 }, 
    { -0.367217, -0.258441, -0.506060, -0.255655, -0.499114, 0.113885, 0.188771, 0.014348, 0.213099, -0.113877 }, 
    { -0.480785, -0.364806, -0.396680, 0.497976, -0.502551, 0.128596, -0.261498, 0.462252, -0.598261, 0.253812 }, 
    { -0.567159, 0.236277, 0.162137, -0.427903, 0.198453, -0.061932, -0.172801, 0.418995, -0.272858, -0.358574 }, 
    { 0.159899, -0.399958, 0.233211, -0.149129, 0.289593, -0.324470, 0.244713, 0.587779, 0.016516, 0.539349 }, 
    { 0.129768, 0.396204, 0.135740, 0.012989, -0.305828, -0.443811, -0.277537, -0.153022, -0.337330, 0.496737 }, 
    { -0.569687, 0.106501, -0.409859, -0.446173, -0.044025, 0.183276, 0.356145, 0.125618, -0.659715, 0.259530 }, 
    { -0.560354, -0.289071, 0.103752, -0.370594, 0.240134, -0.449281, 0.337401, 0.220689, -0.067533, -0.147390 }, 
    { -0.059864, -0.357426, 0.268295, -0.025435, 0.287175, -0.130794, -0.345558, -0.056022, -0.592401, 0.409725 }, 
    { 0.070506, 0.494606, 0.012306, 0.027240, -0.112442, 0.128436, -0.409556, 0.127022, 0.280491, 0.565825 }, 
    { 0.368003, -0.237027, -0.127455, -0.135200, 0.429812, -0.477968, 0.417062, 0.129507, -0.251470, -0.289104 }, 
    { 0.244408, -0.091057, -0.426142, 0.103974, -0.089830, 0.466919, -0.213426, -0.357478, -0.044970, -0.003307 }, 
    { -0.186364, -0.420361, 0.170605, 0.392511, 0.417764, -0.235246, 0.305627, -0.136452, -0.280800, 0.048744 }, 
    { -0.090482, -0.197617, 0.347492, -0.188813, 0.381779, -0.349393, -0.213947, -0.088926, -0.425888, 0.317125 }, 
};

const float outputWeights[numHiddenNodes +1][numOutputNodes] PROGMEM = {
    { 0.033657 }, 
    { 0.187269 }, 
    { 0.944105 }, 
    { 0.532517 }, 
    { 1.497310 }, 
    { 0.390718 }, 
    { 0.715542 }, 
    { 0.833513 }, 
    { 0.245622 }, 
    { 1.482894 }, 
    { 1.962036 }, 
};

#endif // ARDUINO_CONFIG_H
/*
 * A C++ library reworking of ArduinoANN - An artificial neural network for the Arduino
 * See robotics.hobbizine.com/arduinoann.html for more information about ArduinoANN.
 */

#include <random>

#include "network.hpp"

Network::Network(std::mt19937 m_mt): m_mt(std::random_device()()) {

    dist = std::uniform_real_distribution<float>(-1.0f, 1.0f);

    TrainingCycle = 0;
    RandomFloat = 0.0f;
    ErrorRate = 1.0f;
    AccumulatedInput = 0.0f;
}


void Network::initialiseHiddenWeights() {
    for (int i = 0; i < HiddenNodes; i++) {
        for (int j = 0; j <= InputNodes; j++) {
            ChangeHiddenWeights[j][i] = 0.0;
            RandomFloat = dist(m_mt);
            HiddenWeights[j][i] = RandomFloat * InitialWeightMax;
        }
    }
}


void Network::initialiseOutputWeights() {
    for(int i = 0 ; i < OutputNodes ; i ++ ) {
        for(int j = 0 ; j <= HiddenNodes ; j++ ) {
            ChangeOutputWeights[j][i] = 0.0 ;
            RandomFloat = dist(m_mt);
            OutputWeights[j][i] = RandomFloat * InitialWeightMax ;
        }
    }
}


float Network::trainNetwork(float inputs[InputNodes], float targets[OutputNodes]) {
    computeHiddenLayerActivations(inputs);
    computeOutputLayerActivations(targets);
    backpropogateErrors();
    updateInputToHiddenWeights(inputs);
    updateHiddenToOutputWeights();

    TrainingCycle++;
    return ErrorRate;
}


void Network::computeHiddenLayerActivations(float inputs[InputNodes]) {
    for(int i = 0 ; i < HiddenNodes ; i++ ) {
        AccumulatedInput = HiddenWeights[InputNodes][i] ;
        for(int j = 0 ; j < InputNodes ; j++ ) {
            AccumulatedInput += inputs[j] * HiddenWeights[j][i] ;
        }
        Hidden[i] = float(1.0/(1.0 + exp(-AccumulatedInput))) ;
    }
}


void Network::computeOutputLayerActivations(float targets[OutputNodes]) {
    for(int i = 0 ; i < OutputNodes ; i++ ) {
        AccumulatedInput = OutputWeights[HiddenNodes][i] ;
        for(int j = 0 ; j < HiddenNodes ; j++ ) {
            AccumulatedInput += Hidden[j] * OutputWeights[j][i] ;
        }
        Output[i] = float(1.0/(1.0 + exp(-AccumulatedInput))) ;
        OutputDelta[i] = (targets[i] - Output[i]) * Output[i] * (1.0f - Output[i]) ;
        ErrorRate += 0.5 * (targets[i] - Output[i]) * (targets[i] - Output[i]) ;
    }
}


void Network::backpropogateErrors() {
    for(int i = 0 ; i < HiddenNodes ; i++ ) {
        AccumulatedInput = 0.0 ;
        for(int j = 0 ; j < OutputNodes ; j++ ) {
            AccumulatedInput += OutputWeights[i][j] * OutputDelta[j] ;
        }
        HiddenDelta[i] = float(AccumulatedInput * Hidden[i] * (1.0 - Hidden[i])) ;
    }
}


void Network::updateInputToHiddenWeights(float inputs[InputNodes]) {
    for(int i = 0 ; i < HiddenNodes ; i++ ) {
        ChangeHiddenWeights[InputNodes][i] = LearningRate * HiddenDelta[i] + Momentum * ChangeHiddenWeights[InputNodes][i] ;
        HiddenWeights[InputNodes][i] += ChangeHiddenWeights[InputNodes][i] ;
        for(int j = 0 ; j < InputNodes ; j++ ) {
            ChangeHiddenWeights[j][i] = LearningRate * inputs[j] * HiddenDelta[i] + Momentum * ChangeHiddenWeights[j][i];
            HiddenWeights[j][i] += ChangeHiddenWeights[j][i] ;
        }
    }
}


void Network::updateHiddenToOutputWeights() {
    for(int i = 0 ; i < OutputNodes ; i ++ ) {
        ChangeOutputWeights[HiddenNodes][i] = LearningRate * OutputDelta[i] + Momentum * ChangeOutputWeights[HiddenNodes][i] ;
        OutputWeights[HiddenNodes][i] += ChangeOutputWeights[HiddenNodes][i] ;
        for(int j = 0 ; j < HiddenNodes ; j++ ) {
            ChangeOutputWeights[j][i] = LearningRate * Hidden[j] * OutputDelta[i] + Momentum * ChangeOutputWeights[j][i] ;
            OutputWeights[j][i] += ChangeOutputWeights[j][i] ;
        }
    }
}


std::string Network::writeReport() {
    return "Training cycle: " + std::to_string(TrainingCycle) + ". Error rate: " + std::to_string(ErrorRate);
}


void Network::classify(float inputs[InputNodes]) {}

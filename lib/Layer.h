#pragma once
#include "Perceptron.h"

class Layer 
{
    public:
        const char * name;
        int size;
        Perceptron ** perceptrons;

        Layer * prevLayer = NULL;
        Layer * nextLayer = NULL;

        float learningRate = 0.05;

        Layer(const char * name, int size,  Layer * previous);
        void calculate_results();
        void findErrors();
        void update_weights();
        bool isInput(){
            return this->prevLayer == NULL;
        }
        bool isOutput(){
            return this->nextLayer == NULL;
        }
        float errorSum();
        float errorAbsSum();
        float errorSqrtSum();


        ~Layer();

};
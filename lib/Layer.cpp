#include <cstdio>
#include "Perceptron.h"
#include "Layer.h"

Layer::Layer(const char * name, int size, Layer * previous)
{
    this->name = name;
    this->size = size;
    this->perceptrons = new Perceptron * [size];
    this->prevLayer = previous;

    if (previous) {
        // save nextLayer 
        previous->nextLayer = this;

        // bind results of previous layer to all input of all perceptrons
        for(int i = 0; i < this->size; i++){
            this->perceptrons[i] = new Perceptron(this->name, previous->size);

            for (int k = 0; k < previous->size; k++)
            {
                this->perceptrons[i]->inputs[k] = & previous->perceptrons[k]->result;
                //pf("bind item %d input %d %f\n", i,k, *this->perceptrons[i]->inputs[k]);
            }
        }
    }
    else {
        // input layer
        for(int i = 0; i < this->size; i++){
            this->perceptrons[i] = new Perceptron(this->name, 0 /*has no inputs*/);
        }
    }
}

Layer::~Layer()
{
    pf("~Layer() %s\n", this->name);
    for(int i = 0; i < size; i++){
        delete this->perceptrons[i];
    }

    delete this->perceptrons;
}

void Layer::calculate_results()
{
    for (int i = 0; i < this->size; i++)
    {
        this->perceptrons[i]->calculate_and_update_result();
    }
    
}

void Layer::findErrors()
{
    if(this->nextLayer == NULL) { 
        // last layer (out layer)
        // errors for last layer must be set manually (err = target - result)
        return;
    }

    // for each this layer perceptron
    for (int i = 0; i < this->size; i++)
    {
        float calculated_error = 0;
        
        // for each next layer perceptron
        for (int n = 0; n < this->nextLayer->size; n++)
        {
            // find my weight
            float myweight = this->nextLayer->perceptrons[n]->weights[i/*!!!*/];

            // sum next layer (errors*weight)
            calculated_error += myweight * this->nextLayer->perceptrons[n]->error;
            //pf_green("findErrors: [%s] i=%d n=%d calculated=%f myweight=%f %s_Layer->perceptrons[%d]->error=%f\n", this->name, i, n, calculated_error, myweight, this->nextLayer->name, n, this->nextLayer->perceptrons[n]->error);
        }

        // set new error value
        this->perceptrons[i]->error = calculated_error;
    }
}

void Layer::update_weights()
{
    for (int i = 0; i < this->size; i++)
    {
        this->perceptrons[i]->update_weights();
    }
}


float Layer::errorSum()
{
    float sum = 0;
    for (int i = 0; i < this->size; i++){
        sum += this->perceptrons[i]->error;
    }
    return sum;
}

float Layer::errorAbsSum()
{
    float sum = 0;
    for (int i = 0; i < this->size; i++){
        sum += std::abs(this->perceptrons[i]->error);
    }
    return sum;
}

float Layer::errorSqrtSum()
{
    float sum = 0;
    for (int i = 0; i < this->size; i++){
        sum += std::pow(this->perceptrons[i]->error, 2);
    }
    return sum;
}
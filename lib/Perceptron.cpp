//#include <cstdlib>
//#include <cmath>
#include <random>
#include "definitions.h"
#include "Perceptron.h"

typedef float (*FLOAT_FUNCTION)(float);
struct ActivationStruct {
    FLOAT_FUNCTION activation;
    FLOAT_FUNCTION derivative;
};


float learning_rate = 0.05;
int activation_function_index = 0;


ActivationStruct activation_groups [3] = {
    ActivationStruct{ // Sigmoid / Logistic Function
        activation: [](float x) { return 1.0f / (1.0f + (float)exp(-x));},
        derivative: [](float y) { return y * (1.0f - y);},
    },

    ActivationStruct{ // Leaky ReLU
        activation: [](float x) { return x >= 0.0f ? x : 0.01f * x;},
        derivative: [](float y) { return y >= 0.0f ? 1.0f : 0.01f;},
    },

    ActivationStruct{ // Caped Leaky ReLU 
        activation: [](float x) { 
            if(x >= 0.0f){
                if(x > 1.0f){
                    return 1 + 0.01f * (x-1);
                }
                else{ 
                    return x;
                }
            }
            else{
                return 0.01f * x;
            }
        },
        derivative: [](float y) { 
            if(y < 0 || y > 1.0f){
                return 0.01f;
            }
            else{
                return 1.0f;
            }
        },
    },
};



FLOAT_FUNCTION activation = [](float x) { 
    return activation_groups[activation_function_index].activation(x);
};

FLOAT_FUNCTION derivative = [](float y) { 
    return activation_groups[activation_function_index].derivative(y);
};



Perceptron::Perceptron(const char * name, int input_count)
{
    this->name = name;
    this->input_count = input_count;
    this->inputs  = new float*[input_count]; // Array of pointers(!) Because they will refer to previous perceptron 'result'
    this->weights = new float[input_count];
    this->result = 0;

    for (int i = 0; i < input_count; i++)
    {
        this->weights[i] = (float) rand() / RAND_MAX - 0.5;
    }
    this->bias = (float) rand() / RAND_MAX - 0.5;
}

Perceptron::~Perceptron()
{
    delete this->inputs;
    delete this->weights;
}

void Perceptron::calculate_and_update_result()
{
    if(!this->input_count){ // fake neuron
        pf_yellow("Warning: unable to calculate result. No inputs. Skipping.\n");
        return;
    }

    this->result = this->_calculate_result();
}

float Perceptron::_calculate_result()
{
    if(!this->input_count){
        pf_red("Error: unable to calculate result. No inputs. Returning zero.\n");
        return 0;
    }

    float sum = 0;
    for(int i = 0; i < this->input_count; i++){
        float value = * this->inputs[i] * this->weights[i];
        //pf("[%d] input: %f, weight: %f, value: %f\n", i, *this->inputs[i], this->weights[i], value);
        sum += value;
    }
    sum += bias; // add bias

    return activation(sum);
}


void Perceptron::update_weights()
{
    pf(_BLUE _BOLD "%-7s" _RST, this->name);
    pf(_MAGENTA " update_weights: error: " _RST _RED "%+05.2f" _RST " weights: ", this->error);

    // weight correction formula
    auto correct_weight = [](float rate, float old_weight, float err, float result, float input){
        return old_weight + rate * err * derivative(result) * input;
    };

    // update weights
    for(int i = 0; i < this->input_count; i++){
        float old_weight = this->weights[i];
        this->weights[i] = correct_weight(learning_rate, this->weights[i], this->error, this->result, *this->inputs[i]);
        pf("w%+5.3f", this->weights[i]);
        pf(_CYAN "Δ%+.0fm " _RST, (this->weights[i] - old_weight) * 1000);
    }
    // correct bias same way as other weights have been corrected
    float old_bias = this->bias;
    this->bias = correct_weight(learning_rate, this->bias, this->error, this->result, 1);
    pf_gray("b%+.2f", this->bias);
    pf_gray(_CYAN2 "Δ%+.0fm " _RST, (this->bias - old_bias) * 1000);
    
    float new_result = this->_calculate_result();
    pf_blue("NR: %+.2fΔ%+0.fm \n", new_result, (this->result - new_result) * 1000);
}
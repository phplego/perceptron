#pragma once


typedef float (*FLOAT_FUNCTION)(float);
struct ActivationBundle {
    const char *   name;
    FLOAT_FUNCTION activation;
    FLOAT_FUNCTION derivative;
};


#define ACTIVATION_BUNDLES_SIZE 3

ActivationBundle activation_bundles [ACTIVATION_BUNDLES_SIZE] = {
    
    ActivationBundle{ // Sigmoid / Logistic Function
        name: "Sigmoid",
        activation: [](float x) { return 1.0f / (1.0f + (float)exp(-x));},
        derivative: [](float y) { return y * (1.0f - y);},
    },

    ActivationBundle{ // Leaky ReLU
        name: "L-ReLU",
        activation: [](float x) { return x < 0.0f ? 0.01f * x : x;},
        derivative: [](float y) { return y < 0.0f ? 0.01f : 1.0f;},
    },

    ActivationBundle{ // Leaky Capped ReLU 
        name: "LC-ReLU",
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


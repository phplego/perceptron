#include <cstdio>
#include "definitions.h"
#include "Perceptron.h"
#include "Perceptron.cpp"
#include "Layer.cpp"



int main(int argc, char *argv[]) 
{ 
    pf("Starting teaching one neuron...\n");

    // lear one neuron
    srand(345345);
    Perceptron p("test", 3);

    float inputs [] = {10, -2, 1.5};
    p.inputs[0] = &inputs[0];
    p.inputs[1] = &inputs[1];
    p.inputs[2] = &inputs[2];


    for(int i = 0; i < 3; i++){
        pf_blue("INPUTS:  i%d: %0.2f  ", i, * p.inputs[i]);
    }

    p.calculate_and_update_result();

    float target = 0.5;
    p.error = target - p.result;
    pf("initial result: %f\n", p.result);
    pf("target: %f\n", target);
    
    int step = 0;
    while(p.error*p.error > 0.0001)
    {
        pf("%.2d ", step++);
        p.update_weights(0.05);
        p.calculate_and_update_result();
        p.error = target - p.result;
        if(step > 100)
            break;
    }

    
    pf("New Result: %f\n", p.result);
  
}
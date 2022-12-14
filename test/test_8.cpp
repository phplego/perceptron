#include <cstdio>
#include "colored_output.h"
#include "Perceptron.h"
#include "Perceptron.cpp"
#include "Layer.cpp"



int main(int argc, char *argv[]) 
{ 
    pf("Rand sequence for seed 0...\n");

    srand(0);
    for (int i = 0; i < 10; i++){
        pf("val %d\n", rand());
    }
  
}
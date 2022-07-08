#include <cstdio>
#include "definitions.h"
#include "Perceptron.h"
#include "Perceptron.cpp"
#include "Layer.cpp"


// auto activation = [](auto x) { 
//     return 1.0 / (1 + exp(-x));
// };

// auto derivative = [](auto y) { 
//     return y * (1 - y);
// };



int main(int argc, char * argv []) 
{ 
    pf_green("Activation chart..\n");

    FILE * file = fopen("plot2.data", "w");

    for (int i = 0; i < 100; i++)
    {
        float value = -5+(float)i/10;
        pf("[%02d] activation(%.2f) = %.2f\n" _RST, i, value, activation(value));

        char buf [100];
        sprintf(buf, "%f\n", activation(value));
        fputs(buf, file);  
    }

    fclose(file);
}
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

    std::string filename = "test_4.data";
    FILE * file = fopen(filename.c_str(), "w");

    char buf [100];

    for (int i = 0; i < 100; i++)
    {
        float value = -5 + (float)i / 10;
        sprintf(buf, "%f %f\n", value, activation(value));
        fputs(buf, file);  
    }

    fclose(file);

    // run gnuplot
    std::string cmd = "gnuplot -e \"plot '" + filename + "'; pause -1\"";
    printf("EXEC CMD: %s\n", cmd.c_str());
    int res = system(cmd.c_str());
    printf("EXIT CODE: %d\n", res);

}
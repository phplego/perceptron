#include <cstdio>
#include "definitions.h"
#include "Perceptron.h"
#include "Perceptron.cpp"
#include "Layer.cpp"



int main(int argc, char * argv []) 
{ 
    pf_green("Arguments..\n");

    for (int i = 0; i < argc; i++)
    {
        pf("arg %d = %s\n" _RST, i, argv[i]);

    }
}
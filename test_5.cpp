#include <cstdio>
#include <cmath>

#include "definitions.h"



int main(int argc, char * argv []) 
{ 
    pf_green("Expotential func..\n");

    for(int i = -10; i <= 10; i++){
        pf("exp(%.1f) = %f\n", (float)i, exp((float)i));
    }
}
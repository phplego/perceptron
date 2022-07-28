#include <cstdio>
#include "colored_output.h"
#include "Perceptron.h"
#include "Perceptron.cpp"
#include "Layer.cpp"



int main(int argc, char * argv []) 
{ 
    pf_green("Print all colors..\n");

    for (int i = 0; i < 100; i++)
    {
        pf("\\x1b[%dm: \x1b[%dmTEST_STRING\n" _RST, i,i);

        for (int k = 0; k < 100; k++){
            pf("\x1b[%dm", i);
            pf("\x1b[%dm %02d ", k, k);
            if(k == 49)
                pf("\n");
        }
        pf( _RST "\n\n");
    }
}
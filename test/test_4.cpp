#include <cstdio>
#include "definitions.h"
#include "Perceptron.h"
#include "Perceptron.cpp"
#include "Layer.cpp"


// auto activation_function = [](auto x) { 
//     return 1.0 / (1 + exp(-x));
// };

// auto derivative_function = [](auto y) { 
//     return y * (1 - y);
// };



int main(int argc, char * argv []) 
{ 
    pf_green("Compare activation functions (chart)..\n");

    std::string filename = "test_4.data";
    FILE * file = fopen(filename.c_str(), "w");

    char buf [100];

    for (int i = 0; i < 100; i++)
    {
        float value = -5 + (float)i / 10;
        sprintf(buf, "%f ", value);
        fputs(buf, file);  

        for(int i=0; i < ACTIVATION_BUNDLES_COUNT; i++){
            sprintf(buf, "%f ", activation_bundles[i].activation(value));
            fputs(buf, file);  
        }

        sprintf(buf, "\n");
        fputs(buf, file);  
    }

    fclose(file);

    std::string plot_parts = "";
    
    for(int i=0; i < ACTIVATION_BUNDLES_COUNT; i++){
        if(i > 0){
            plot_parts += ",\n";
        }
        plot_parts += "'" + filename + "' using 1:"+std::to_string(i+2)+" title '"+activation_bundles[i].name+"' with line linewidth 2";
    }

    // run gnuplot
    std::string cmd = "gnuplot -e \"set grid; set key left top; set yrange [-0.1:1.1];\n plot \n" + plot_parts + ";  pause mouse close;\"";
    printf("EXEC CMD: %s\n", cmd.c_str());
    int res = system(cmd.c_str());
    printf("EXIT CODE: %d\n", res);

}
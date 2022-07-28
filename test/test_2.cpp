#include <cstdio>
#include "colored_output.h"
#include "Perceptron.h"
#include "Perceptron.cpp"
#include "Layer.cpp"



int main(int argc, char *argv[]) 
{ 
    pf_green("Starting teaching two neurons...\n");

    srand(time(NULL));

    Layer input("input", 3, NULL);
    Layer out("outlay", 1, &input); // два нейрона на выходе


    input.perceptrons[0]->result = 10;
    input.perceptrons[1]->result = -2;
    input.perceptrons[2]->result = 1.5;

    Perceptron * p = out.perceptrons[0];

    out.calculate_results();

    pf("initial result: %f\n", p->result);

    
    float target = 0.5;
    pf("target: %f\n", target);
    p->error = target - p->result;

    int step = 0;
    while(std::abs(p->error) > 0.001)
    {
        pf("%.2d ", step++);
        out.update_weights();
        out.calculate_results();
        p->error = target - p->result;
        if(step > 100)
            break;
    }        

    pf("New Result: %f\n", p->result);

}
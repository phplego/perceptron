#include <cstdio>
#include <unistd.h>
#include <cstring>
#include "definitions.h"
#include "Network.h"
#include "Perceptron.cpp"
#include "Layer.cpp"

#define _1 0.9
#define _0 0.1

const int SAMPLES_COUNT = 20;

float const learn_data[SAMPLES_COUNT][11] = { 
    {   _0, _0, _0, 
        _0, _0, _0,   // empty  
        _0, _0, _0,   _0 /*vert*/, _0 /*horiz*/},  

    {   _0, _0, _0, 
        _0, _1, _0,   // point 
        _0, _0, _0,   _0 /*vert*/, _0 /*horiz*/},  

    {   _1, _1, _1, 
        _0, _0, _0,   // h-line 1 
        _0, _0, _0,   _0 /*vert*/, _1 /*horiz*/},  

    {   _0, _0, _0, 
        _1, _1, _1,   // h-line 2 
        _0, _0, _0,   _0 /*vert*/, _1 /*horiz*/},  

    {   _0, _0, _0, 
        _0, _0, _0,   // h-line 3   
        _1, _1, _1,   _0 /*vert*/, _1 /*horiz*/},  

    {   _1, _0, _0, 
        _1, _0, _0,   // v-line 1
        _1, _0, _0,   _1 /*vert*/, _0 /*horiz*/},  
    
    {   _0, _1, _0, 
        _0, _1, _0,   // v-line 2
        _0, _1, _0,   _1 /*vert*/, _0 /*horiz*/},  

    {   _0, _0, _1, 
        _0, _0, _1,   // v-line 3
        _0, _0, _1,   _1 /*vert*/, _0 /*horiz*/},  
    
    {   _1, _1, _1,   
        _0, _0, _1,   // corner 1
        _0, _0, _1,   _1 /*vert*/, _1 /*horiz*/},  

    {   _1, _0, _0, 
        _1, _0, _0,   // corner 2
        _1, _1, _1,   _1 /*vert*/, _1 /*horiz*/},  

    {   _1, _1, _1, 
        _1, _0, _0,   // corner 3
        _1, _0, _0,   _1 /*vert*/, _1 /*horiz*/},  

    {   _0, _0, _1, 
        _0, _0, _1,   // corner 4
        _1, _1, _1,   _1 /*vert*/, _1 /*horiz*/},  

    {   _1, _1, _1, 
        _0, _1, _0,   // T
        _0, _1, _0,   _1 /*vert*/, _1 /*horiz*/},  

    {   _0, _1, _0, 
        _0, _1, _0,   // T upside
        _1, _1, _1,   _1 /*vert*/, _1 /*horiz*/},  

    {   _0, _0, _1, 
        _1, _1, _1,   // T turned right
        _0, _0, _1,   _1 /*vert*/, _1 /*horiz*/},  

    {   _1, _0, _0, 
        _1, _1, _1,   // T turned left
        _1, _0, _0,   _1 /*vert*/, _1 /*horiz*/},  

    {   _0, _1, _0, 
        _1, _1, _1,   // cross
        _0, _1, _0,   _1 /*vert*/, _1 /*horiz*/},  

    {   _0, _1, _0, 
        _0, _1, _0,   // палочка |
        _0, _0, _0,   _1 /*vert*/, _0 /*horiz*/},  

    {   _0, _0, _0, 
        _1, _1, _0,   // short h-line
        _0, _0, _0,   _0 /*vert*/, _1 /*horiz*/},  
    
    {   _0, _1, _1, 
        _0, _0, _0,   // short h-line
        _0, _0, _0,   _0 /*vert*/, _1 /*horiz*/},  
};

const int TESTS_COUNT = 4;

float const test_data[TESTS_COUNT][9] = { 
    {   _0, _0, _0, 
        _0, _0, _0,   // empty
        _0, _0, _0},  

    {   _0, _0, _0, 
        _0, _1, _0,   // dot
        _0, _0, _0},  

    {   _0, _0, _1, 
        _0, _0, _1,   // short v-line
        _0, _0, _0},  

    {   _1, _1, _0, 
        _0, _0, _0,   // short h-line
        _0, _0, _0},  
};



void print_results(float vert_value, float horz_value){
    pf("  Results:  %.2f %.2f  ", vert_value, horz_value);
    pf(vert_value > 0.5 ? _BOLD "✓ vertical   " _RST: "");
    pf(horz_value > 0.5 ? _BOLD "✓ horisontal " _RST: "");
    pf("\n");
}


int main(int argc, char * argv []) 
{ 
    pf("Usage: %s [seed] [steps] \n", argv[0]);

    int seed = time(NULL); 
    int total_steps =  3000;


    if(argc > 1){
        seed = std::stoi(argv[1]);
    }

    if(argc > 2){
        total_steps = std::stoi(argv[2]);
    }

    // initialize random generator with seed
    srand(seed);


    Network net("net1");
    net.createLayer("input",    9);   // 9 пикселей на входе
    net.createLayer("lay1",     5);
    net.createLayer("lay2",     3);
    net.createLayer("out",      2);   // два нейрона на выходе

    //net.loadWeights("weights.txt");

    FILE * file_errors_by_sample = fopen("plot1.data", "w");
    FILE * file_errors_summary = fopen("plot2.data", "w");


    // learn cycle
    for(int step = 0; step < total_steps; step++){
        //usleep(1);
        float step_out_err_max = 0;

        for(int sample = 0; sample < SAMPLES_COUNT; sample ++){

            //PRINT_ON = step > total_steps - 16;           // print only last 100
            PRINT_ON = (step + 1) % 1000 == 0;  // print every 1000-th learn set
            
            pf_green("\nsample #%d  step %d\n", sample, step);

            for(int k=0; k < 9; k++){ // print sample square
                if((k) % 3 == 0) pf("\n");
                pf("%s ", learn_data[sample][k] > 0.5 ? "◼" : "◻");
            }

            for(int k=0; k < 9; k++){ // fill input layer with sample
                net.setInputValue(k, learn_data[sample][k]);
            }

            // provide signal through the network
            net.forward();
    
            print_results(net.outLayer()->perceptrons[0]->result, net.outLayer()->perceptrons[1]->result);
            net.printState();
            
            // lear the sample
            float data [2] = {learn_data[sample][9], learn_data[sample][10]};
            net.learn(data, 2);

            pf("error sum: " _RED "%+.3f  " _RST " outerr:" _YELLOW " %f" _RST " LR: %f\n", net.errorSum(), net.outLayer()->errorSum(), net.outLayer()->learningRate);

            // save to file
            char buf [100];
            sprintf(buf, "%f %f\n", net.errorSum(), net.outLayer()->errorSum());
            fputs(buf, file_errors_summary);    // save to summary plot file
            
            sprintf(buf, "%f ", net.outLayer()->errorAbsSum());
            fputs(buf, file_errors_by_sample);  // save to by_sample plot file
            step_out_err_max = std::max(step_out_err_max, net.outLayer()->errorAbsSum());
        }
        fputs("\n", file_errors_by_sample);

        pf("step_out_err_max: " _BG_BLUE " %f " _RST " seed: %d\n", step_out_err_max, seed);
    }

    fclose(file_errors_summary);
    fclose(file_errors_by_sample);
    PRINT_ON = 1;
    pf_bold("SEED: %d\n", seed);

    // run tests
    for(int tindex = 0; tindex < TESTS_COUNT; tindex ++)
    {
        pf_green("\ntest #%d", tindex);
        for(int k=0; k < 9; k++){ // print test square
            if((k) % 3 == 0) pf("\n");
            pf("%s ", test_data[tindex][k] > 0.5 ? "◼" : "◻");
        }

        for(int k=0; k < 9; k++){ // fill input layer with sample
            net.setInputValue(k, test_data[tindex][k]);
        }

        net.forward();

        print_results(net.outLayer()->perceptrons[0]->result, net.outLayer()->perceptrons[1]->result);

        net.printState();
      
    }

    net.dumpWeights("weights.txt");
}
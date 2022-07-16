#pragma once

class Perceptron {

    // class variables
    public: 
        static float learning_rate;

    // instance variables
    public: 
        const char * name;   // Perceptron shiny name.
        float ** inputs;     // Array of pointers(!) Because they will refer to previous perceptron 'result'   
        int input_count;
        float * weights;
        float bias;   
        float error;
        float result;

    // instance methods
    public:
        Perceptron(const char * name, int input_count);
        ~Perceptron();
        void calculate_and_update_result();
        float _calculate_result();
        void update_weights();

};


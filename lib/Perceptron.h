#pragma once

class Perceptron {
    public: 
        const char * name;   // Perceptron shiny name.
        float ** inputs;     // Array of pointers(!) Because they will refer to previous perceptron 'result'   
        int input_count;
        float * weights;
        float bias;   
        float error;
        float result;

    public:
        Perceptron(const char * name, int input_count);
        ~Perceptron();
        void calculate_and_update_result();
        float _calculate_result();
        void update_weights(float learning_rate);

};


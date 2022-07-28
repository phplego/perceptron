#pragma once
#include "colored_output.h"
#include "Layer.h"

class Network 
{
    private:
        int layers_count = 0;
        Layer * layers [100] = {0};

    public:
        const char * name;

        Network(const char * name) {
            this->name = name;
        }

        int layersCount(){
            return this->layers_count;
        }

        Layer * getLayer(int index) {
            return this->layers[index];
        }
        
        /**
         * Create new network layer
         * 
         * @param name char * Layer name
         * @param size int Number of perceptorns in the layer
         */
        void createLayer(const char * name, int size) {
            Layer * previous = this->layers_count > 0 ? this->layers[this->layers_count - 1] : NULL;
            Layer * layer = new Layer (name, size, previous);
            this->layers[this->layers_count] = layer;
            this->layers_count ++;
        }

        Layer * outLayer() {
            return layers_count > 0 ? this->layers[layers_count - 1] : NULL;
        }

        Layer * inputLayer() {
            return layers_count > 0 ? this->layers[0] : NULL;
        }

        /**
         * Set input layer value.
         * 
         * @param index int Input's layer perceptron index
         * @param value float The value 
         */
        void setInputValue(int index, float value) {
            if(!inputLayer()){
                pf_red("Error: inputLayer is null.\n");
                return;
            }

            if(index < 0 || index >= inputLayer()->size){
                pf_red("Error: setInputValue: Out of bounds (%d).\n", index);
                return;
            }

            inputLayer()->perceptrons[index]->result = value;
        }

        /**
         * Calculate results for each layer. From the input to the output
         */
        void forward() {
            for(int i = 1 /* skip input layer */; i < layers_count; i++) {
                this->layers[i]->calculate_results();
            }
        }

        /**
         * Teach the network with target values. 
         * This method finds errors for each layer from 
         * the output to the input and then update weights.
         */
        void learn(float data [], int size) {
            // validations    
            if(!outLayer()){
                pf_red("Error: outLayer is null.\n");
                return;
            }
            if(size != outLayer()->size){
                pf_red("Error: Data size doesn't match out layer size.\n");
                return;
            }

            // set learn data to output layer: set errors as (target - result)
            for(int i = 0; i < outLayer()->size; i++){
                outLayer()->perceptrons[i]->error = data[i] - outLayer()->perceptrons[i]->result;
            }

            // for each layer (in reverse order) find errors
            for(int i = layers_count - 2; i > 0; i--) {
                this->layers[i]->findErrors();
            }

            // for each layer (in reverse order) update weights
            for(int i = layers_count - 1; i > 0; i--) {
                this->layers[i]->update_weights();
            }
        }

        /**
         * Sum of all the layers error
         */
        float errorSum() {
            float summary = 0;
            for(int i = 1; i < layers_count; i++) {
                summary += this->layers[i]->errorSum();
            }
            return summary;
        }

        /**
         * Print network state to the console 
         */
        void printState() {
            for (int l = 0; l < this->layers_count; l ++) {
                Layer * layer = this->layers[l];
                pf("%-10s", layer->name);
                for(int p = 0; p < layer->size; p ++) {
                    if(layer->perceptrons[p]->result > 0.75){
                        pf_green("%.2f ", layer->perceptrons[p]->result);
                    }
                    else if(layer->perceptrons[p]->result > 0.5){
                        pf_blue("%.2f ", layer->perceptrons[p]->result);
                    }
                    else if(layer->perceptrons[p]->result > 0.25){
                        pf_yellow("%.2f ", layer->perceptrons[p]->result);
                    }
                    else {
                        pf_red("%.2f ", layer->perceptrons[p]->result);
                    }

                }
                pf("\n");
            }
        }

        /**
         * Save network state (weights + biases) to the file
         * 
         * @param filename char * Filename 
         */
        void saveWeights(const char * filename){
            // calculate buffer size
            int values_count = 0;
            for (int l = 1; l < this->layers_count; l ++) {
                Layer * layer = this->layers[l];
                for(int p = 0; p < layer->size; p ++) {
                    values_count += layer->perceptrons[p]->input_count;
                    values_count += 1; // and bias
                }
            }

            int buffer_size = values_count * sizeof(float);
            char buf[buffer_size] = {0};

            // write weights values (and biases) to the buffer
            int pos = 0;
            for (int l = 1; l < this->layers_count; l ++) {
                Layer * layer = this->layers[l];
                for(int p = 0; p < layer->size; p ++) {
                    for(int w = 0; w < layer->perceptrons[p]->input_count; w++){
                        float weight = layer->perceptrons[p]->weights[w];
                        memcpy(buf + pos, (char*)&weight, sizeof(float));
                        pos += sizeof(float);
                    }
                    float bias = layer->perceptrons[p]->bias;
                    memcpy(buf + pos, (char*)&bias, sizeof(float));
                    pos += sizeof(float);
                }
            }

            if(pos != buffer_size){
                pf_red("Error: buffer size is not correct!\n");
                return;
            }

            FILE * file = fopen(filename, "w");
            fwrite(buf, 1, buffer_size, file);
            fclose(file);
        }

        /**
         * Load network state (weights + biases) from the file
         * 
         * @param filename char * Filename 
         */
        void loadWeights(const char * filename){
            FILE * file = fopen(filename, "r");
            if(!file){
                pf_red("Error: Unable to open file '%s'\n", filename);
                return;
            }

            for (int l = 1; l < this->layers_count; l ++) {
                Layer * layer = this->layers[l];
                for(int p = 0; p < layer->size; p ++) {
                    for(int w = 0; w < layer->perceptrons[p]->input_count; w++){
                        float weight = 0;
                        int readed = fread(&weight, 1, sizeof(float), file);
                        if(readed != sizeof(float)){
                            pf_red("Error: File dump is smaller than network size. File: '%s'\n", filename);
                            fclose(file);
                            return;
                        }
                        layer->perceptrons[p]->weights[w] = weight;
                    }
                    float bias = 0;
                    int readed = fread(&bias, 1, sizeof(float), file);
                    if(readed != sizeof(float)){
                        pf_red("Error: File dump is smaller than network size. File: '%s'\n", filename);
                        fclose(file);
                        return;
                    }
                    layer->perceptrons[p]->bias = bias;
                }
            }
            int dummy;
            if(fread(&dummy, 1, 1, file) != 0){
                pf_red("Error: File dump is larger than network size. File: '%s'\n", filename);
                fclose(file);
                return;
            }


            fclose(file);
        }

        ~Network(){
            pf("~Network() %s\n", this->name);
            for(int i = 0; i < layers_count; i++) {
                delete this->layers[i];
            }
        }
};
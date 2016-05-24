#ifndef BPNEURALNETWORK_H
#define BPNEURALNETWORK_H

#include "opennn.h"
#include <string>

using namespace std;
using namespace OpenNN;

class BPNeuralNetwork
{
private:
	NeuralNetwork neuralNetwork;

public:
	BPNeuralNetwork(unsigned int a, unsigned int b, unsigned int c) :
		neuralNetwork(a, b, c)
	{
	}

	void train(string fileName);
	void save(string fileName);
	void load(string fileName);
	void getOutput(vector<double> input, vector<double> *outputVector);
};


#endif
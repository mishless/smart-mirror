#include "BPNeuralNetwork.h"

void BPNeuralNetwork::train(string fileName)
{
	DataSet data_set;
	data_set.load_data(fileName.c_str());

	const OpenNN::Vector<unsigned int> inputs_indices(0, 1, 3);
	const OpenNN::Vector<unsigned int> targets_indices(4, 1, 6);

	VariablesInformation* variables_information_pointer =
		data_set.get_variables_information_pointer();

	variables_information_pointer->set_inputs_indices(inputs_indices);
	variables_information_pointer->set_targets_indices(targets_indices);

	variables_information_pointer->set_name(0, "Height");
	variables_information_pointer->set_units(0, "cm");

	variables_information_pointer->set_name(1, "Weight");
	variables_information_pointer->set_units(1, "kg");

	variables_information_pointer->set_name(2, "Pulse Transit Time");
	variables_information_pointer->set_units(2, "sec");

	variables_information_pointer->set_name(3, "Age");
	variables_information_pointer->set_units(3, "years");

	variables_information_pointer->set_name(4, "Low pressure");
	variables_information_pointer->set_name(5, "Medium pressure");
	variables_information_pointer->set_name(6, "High pressure");

	const OpenNN::Vector< OpenNN::Vector<std::string> > inputs_targets_information =
		variables_information_pointer->arrange_inputs_targets_information();

	InstancesInformation* instances_information_pointer =
		data_set.get_instances_information_pointer();

	instances_information_pointer->split_random_indices(1.0, 0.0, 0.0);

	const OpenNN::Vector< OpenNN::Vector<double> > inputs_targets_statistics =
		data_set.scale_inputs_minimum_maximum();

	neuralNetwork.set_inputs_outputs_information(inputs_targets_information);
	neuralNetwork.set_inputs_outputs_statistics(inputs_targets_statistics);
	neuralNetwork.set_scaling_unscaling_layers_flag(false);
	PerformanceFunctional performance_functional(&neuralNetwork, &data_set);
	TrainingStrategy training_strategy(&performance_functional);
	TrainingStrategy::Results training_strategy_results =
		training_strategy.perform_training();
	neuralNetwork.set_scaling_layer_flag(true);
}

void BPNeuralNetwork::save(string fileName)
{
	neuralNetwork.save(fileName.c_str());
}

void BPNeuralNetwork::load(string fileName)
{
	neuralNetwork.load(fileName.c_str());
}

void BPNeuralNetwork::getOutput(vector<double> input, vector<double> *outputVector)
{
	OpenNN::Vector<double> inputVec(4);
	OpenNN::Vector<double> outputVec;

	int i;
	for (i = 0; i < input.size(); i++)
	{
		inputVec[i] = input[i];
	}
	outputVec = neuralNetwork.calculate_outputs(inputVec);
	for (i = 0; i < outputVec.size(); i++)
	{
		outputVector->push_back(outputVec[i]);
	}
}
#include "recognizer.h"
#include <iostream>
#include <fstream>


void Recognizer::initialize(double threshold)
{
	model = createFisherFaceRecognizer();
	model->set("threshold", threshold);
}

bool Recognizer::train(string fileInfo, string databaseFolder)
{
	ifstream info;
	string input, path;
	info.open(fileInfo);
	
	vector<Mat> images;
	vector<int> labels;
	int label = 0;

	while (!info.eof())
	{
		PersonInfo newPerson;

		info >> input;
		if (input != "NEW_ENTRY")
		{
			cout << "Info file corrupt, no NEW_ENTRY found where expected" << endl;
			return false;
		}

		info >> input; /* Read "Name:" */
		getline(info, newPerson.fullName);

		info >> input; /* Read "Gender:" */
		info >> input;
		if (input == "Female") newPerson.gender = FEMALE;
		else if (input == "Male") newPerson.gender = MALE;
		else newPerson.gender = UNKNOWN;

		info >> input; /* Read Birth: */
		info >> input;
		newPerson.yearOfBirth = stoi(input);

		info >> input; /* Read "Height:" */
		info >> input;
		newPerson.height = stoi(input);

		info >> input; /* Read "Weight:" */
		info >> input;
		newPerson.weight = stoi(input);

		info >> input; /* Read "Pictures:" */

		info >> input;
		while (input != "END_ENTRY")
		{
			path = databaseFolder + "\\" + input;
			images.push_back(imread(path, CV_LOAD_IMAGE_GRAYSCALE));
			labels.push_back(label);
			info >> input;
		}

		/* Add person information to runtime database */
		database.push_back(newPerson);

		/* Next label so each person has their own label */
		label++;
	}

	/* Train the recognizer */
	model->train(images, labels);

	return true;
}

bool Recognizer::recognize(Mat image, PersonInfo *outputPersonInfo)
{
	int label;

	label = model->predict(image);

	/* If the person is unknown, return false*/
	if (label == -1)
	{
		return false;
	}

	/* Else, fill in the info and return true */
	*outputPersonInfo = database[label];
	return true;
}

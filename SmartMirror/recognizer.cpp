#include "recognizer.h"
#include <iostream>
#include <fstream>
#include "sqlite3.h"

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

	/* Connect to the database */
	sqlite3 *db;
	char *error;
	char *zErrMsg = 0;
	int rc;

	rc = sqlite3_open(databaseLocation, &db);

	if (rc) {
		cout << "Can't open database: " << sqlite3_errmsg(db) << endl;
		exit(0);
	}
	else {
		cout << "Opened database successfully." << endl;
	}

	const char *sqlSelectUsers = "SELECT id, username, fullName, gender, year, weight, height FROM Users;";
	char **results = NULL;
	int rows, columns;
	sqlite3_get_table(db, sqlSelectUsers, &results, &rows, &columns, &error);
	if (rc)
	{
		cout << "Error executing SQLite3 query: " << sqlite3_errmsg(db) << endl;
		sqlite3_free(error);
	}
	else
	{
		// Display Table
		for (int rowCtr = 1; rowCtr <= rows; ++rowCtr)
		{
			PersonInfo newPerson;
			string id;
			for (int colCtr = 0; colCtr < columns; ++colCtr)
			{
				// Determine Cell Position
				int cellPosition = (rowCtr * columns) + colCtr;
				if (results[cellPosition] == NULL) {
					results[cellPosition] = "\0";
				}
				switch (cellPosition % 7) {
				case 0:
					id = results[cellPosition];
					break;
				case 1:
					newPerson.username = results[cellPosition];
					break;
				case 2:
					newPerson.fullName = results[cellPosition];
					break;
				case 3:
					if (results[cellPosition] == "male") {
						newPerson.gender = MALE;
					}
					else if (results[cellPosition] == "female") {
						newPerson.gender = FEMALE;
					}
					else {
						newPerson.gender = UNKNOWN;
					}
					break;
				case 4:
					newPerson.yearOfBirth = (int)results[cellPosition];
					break;
				case 5:
					newPerson.weight = (int)results[cellPosition];
					break;
				case 6:
					newPerson.height = (int)results[cellPosition];
					break;
				}
			}
			
			string temp = "SELECT title, recurrence, day, date, description FROM reminder WHERE user_id=" + id + ";";
			char *sqlSelectUsers = &(temp[0]);
			char **results = NULL;
			int rows, columns;
			sqlite3_get_table(db, sqlSelectUsers, &results, &rows, &columns, &error);
			if (rc)
			{
				cout << "Error executing SQLite3 query: " << sqlite3_errmsg(db) << endl;
				sqlite3_free(error);
			}
			for (int rowCtr = 1; rowCtr <= rows; ++rowCtr)
			{
				Reminder newReminder;
				for (int colCtr = 0; colCtr < columns; ++colCtr) {
					// Determine Cell Position
					int cellPosition = (rowCtr * columns) + colCtr;
					if (results[cellPosition] == NULL) {
						results[cellPosition] = "\0";
					}
					switch (cellPosition % 5) {
					case 0:
						newReminder.title = results[cellPosition];
						break;
					case 1:
						newReminder.recurrency = results[cellPosition];
						break;
					case 2:
						newReminder.day = results[cellPosition];
						break;
					case 3:
						newReminder.date = results[cellPosition];
						break;
					case 4:
						newReminder.description = results[cellPosition];
						break;
					}
				}
				newPerson.reminders.push_back(newReminder);
			}

			/* Add person information to runtime database */
			database.push_back(newPerson);
		}
	}

	sqlite3_close(db);

	while (!info.eof())
	{
		PersonInfo newPerson;
		bool personPresent = false;
		int indexOfPresentPerson = -1;
		info >> input;
		if (input != "NEW_ENTRY")
		{
			cout << "Info file corrupt, no NEW_ENTRY found where expected" << endl;
			return false;
		}
		info >> input; /* Read "Username:" */
		info >> input;
		for (int i = 0; i < database.size(); i++) {
			if (input == database[i].username) {
				indexOfPresentPerson = i;
				personPresent = true;
			}
		}

		if (!personPresent) {
			newPerson.username = input;
		}

		info >> input; /* Read "Name:" */

		if (!personPresent) {
			getline(info, newPerson.fullName);
		} else {
			info >> input;
			info >> input;
		}

		info >> input; /* Read "Gender:" */
		info >> input;

		if (!personPresent) {
			if (input == "Female") newPerson.gender = FEMALE;
			else if (input == "Male") newPerson.gender = MALE;
			else newPerson.gender = UNKNOWN;
		}

		info >> input; /* Read Birth: */
		info >> input;

		if (!personPresent) {
			newPerson.yearOfBirth = stoi(input);
		}

		info >> input; /* Read "Height:" */
		info >> input;
		if (!personPresent) {
			newPerson.height = stoi(input);
		}

		info >> input; /* Read "Weight:" */
		info >> input;

		if (!personPresent) {
			newPerson.weight = stoi(input);
		}

		info >> input; /* Read "Pictures:" */

		info >> input;
		while (input != "END_ENTRY")
		{
			path = databaseFolder + "\\" + input;
			images.push_back(imread(path, CV_LOAD_IMAGE_GRAYSCALE));
			if (personPresent) {
				labels.push_back(indexOfPresentPerson);
			}
			else {
				labels.push_back(database.size());
			}
			info >> input;
		}

		/* Add person information to runtime database */
		if (!personPresent) {
			database.push_back(newPerson);
		}

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

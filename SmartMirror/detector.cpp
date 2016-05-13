#include "detector.h"

void Detector::initialize(String xmlPath, bool loadFromOpenCV) {

	if (loadFromOpenCV == true)
	{
		// Initialize Boost library path
		String boostLibraryPath;
		char* buf = 0;
		size_t sz = 0;
		if (_dupenv_s(&buf, &sz, "OPENCV_DIR") == 0)
		{
			boostLibraryPath = (String)buf;
			free(buf);
		}
		boostLibraryPath += "\\..\\..\\..\\sources\\data\\haarcascades\\";
		objectClassifier.load(boostLibraryPath + xmlPath);
	}
	else
	{
		objectClassifier.load(xmlPath);
	}
}

bool Detector::detect(Mat* frame, Rect* faceRect) {

	/* Detect objects and fill recognizedObjects with rectangles*/
	objectClassifier.detectMultiScale((*frame), recognizedObjects);

	/* If no objects are found, return false*/
	if (recognizedObjects.size() <= 0) {
		return false;
	}

	/* Draw rectangles on the detected faces
	for (int i = 0; i < ob.size(); i++)
	{
		Point center(faces[i].x, faces[i].y);
		rectangle((*frame), Rect(faces[0].x, faces[0].y, faces[0].width, faces[0].height), Scalar(255, 0, 255));
	}*/

	/* Fill the output mask with ones where the first object was detected */
	(*faceRect) = Rect(recognizedObjects[0].x, recognizedObjects[0].y,
		               recognizedObjects[0].width, recognizedObjects[0].height);

	return true;
}
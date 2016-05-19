#include "tracker.h"

#define TRANSFORM_DATA_TYPE CV_64FC1

void Tracker::initialize() {
	rigidTransform = Mat::eye(3, 3, TRANSFORM_DATA_TYPE);

}

void Tracker::initializeFeatures(Mat* frame, Rect faceRect) {
	
	Mat greyFrame, mask;
	vector<Point2f> points;
	previousPoints.clear();

	/* Convert image to grayscale*/
	cvtColor((*frame), greyFrame, CV_BGR2GRAY);

	/* Initialize empty mask */
	mask = Mat::zeros(frame->size(), CV_8U);

	/* Fill mask */
	mask(faceRect) = 1;

	/* Find good features and store them in previousPoints */
	goodFeaturesToTrack(greyFrame, points, 500, QUALITY_LEVEL, MIN_DISTANCE, mask);
	for (int i = 0; i < points.size(); i++) {
		previousPoints.push_back(points[i]);
	}

	/* Save frame*/
	greyFrame.copyTo(previousGreyFrame);

	/* Save initial mask */
	initialMask = mask;

	/* Initialize */
	rigidTransform = Mat::eye(3, 3, TRANSFORM_DATA_TYPE);
}

bool Tracker::track(Mat* frame, Mat* outputMask) {
	
	vector<uchar> status;
	vector<float> errors;
	Mat nrt33;
	Mat newRigidTransform;
	Mat invTrans;
	Mat greyFrame;
	vector<Point2f> points;

	cvtColor((*frame), greyFrame, CV_BGR2GRAY);

	calcOpticalFlowPyrLK(previousGreyFrame, greyFrame, previousPoints, points, status, errors, Size(50, 50));

	if (countNonZero(status) < status.size() * 0.8) {
		rigidTransform = Mat::eye(3, 3, TRANSFORM_DATA_TYPE);
		previousPoints.clear();
		previousGreyFrame.release();
		return false;
	}

	/* This should not happen */
	if (previousPoints.size() != points.size()) {
		cout << "Previous points: " << previousPoints.size() << " Points: " << points.size() << endl;
		return false;
	}

	newRigidTransform = estimateRigidTransform(previousPoints, points, false);

	/* If it is empty, reset everything */
	if (newRigidTransform.size() == Size(0, 0)) {
		rigidTransform = Mat::eye(3, 3, TRANSFORM_DATA_TYPE);
		previousPoints.clear();
		previousGreyFrame.release();
		return false;
	}

	/* Update rigid trasnform */
	try {
		nrt33 = Mat::eye(3, 3, TRANSFORM_DATA_TYPE);
		newRigidTransform.copyTo(nrt33.rowRange(0, 2));
		rigidTransform *= nrt33;
	}
	catch (...) {
		//cout << e << endl;
		cout << "FAILED!" << endl;
		return false;
	}

	/* Update previousPoints with newly found points */
	previousPoints.clear();
	for (int i = 0; i < status.size(); i++) {
		if (status[i]) {
			previousPoints.push_back(points[i]);
		}
	}

	/* Put circles on found points */
	/*for (int i = 0; i < previousPoints.size(); i++) {
		circle((*frame), previousPoints[i], 3, Scalar(0, 0, 255), CV_FILLED);
	}*/

	previousGreyFrame = greyFrame;

	warpAffine(initialMask, (*outputMask), rigidTransform.rowRange(0,2), Size());

	return true;
}

size_t Tracker::getPointNum() {
	int a = 5;
	return previousPoints.size();
}

void Tracker::rotate(Mat* inputFrame, Mat* inputMask, Mat * outputFrame)
{
	Mat maskedObject, invRigidTransform;
	(*inputFrame).copyTo(maskedObject, *inputMask);
	invertAffineTransform(rigidTransform.rowRange(0,2), invRigidTransform);
	warpAffine(maskedObject, (*outputFrame), invRigidTransform, Size());
}

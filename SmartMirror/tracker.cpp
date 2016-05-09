#include "tracker.h"

void Tracker::initialize() {
	rigidTransform = Mat::eye(3, 3, CV_32FC1);
}

void Tracker::track(Mat* frame, Mat* mask, Mat* outputMask) {
	Mat greyFrame;
	cvtColor((*frame), greyFrame, CV_BGR2GRAY);
	cout << previousPoints.size() << endl;
	cout << points.size() << endl;
	if (previousPoints.size() < 50) {
		previousPoints.clear();
		goodFeaturesToTrack(greyFrame, points, 500, QUALITY_LEVEL, MIN_DISTANCE, (*mask), BLOCK_SIZE, USE_HARRIS_DETECTOR, K);
		for (int i = 0; i < points.size(); i++) {
			previousPoints.push_back(points[i]);
		}
	}
	if (!previousGreyFrame.empty()) {
		calcOpticalFlowPyrLK(previousGreyFrame, greyFrame, previousPoints, points, status, errors, Size(10, 10));
		if (countNonZero(status) < status.size() * 0.8) {
			rigidTransform = Mat::eye(3, 3, CV_32FC1);
			previousPoints.clear();
			previousGreyFrame.release();
			return;
		}
		if (previousPoints.size() != points.size()) {
			cout << "Previous points: " << previousPoints.size() << " Points: " << points.size() << endl;
			return;
		}
		newRigidTransform = estimateRigidTransform(previousPoints, points, false);
		if (newRigidTransform.size() == Size(0, 0)) {
			rigidTransform = Mat::eye(3, 3, CV_32FC1);
			previousPoints.clear();
			previousGreyFrame.release();
			return;
		}
		nrt33 = Mat_<float>::eye(3, 3);
		newRigidTransform.copyTo(nrt33.rowRange(0, 2));
		rigidTransform *= nrt33;

		previousPoints.clear();
		for (int i = 0; i < status.size(); i++) {
			if (status[i]) {
				previousPoints.push_back(points[i]);
			}
		}
		for (int i = 0; i < previousPoints.size(); i++) {
			circle((*frame), previousPoints[i], 3, Scalar(0, 0, 255), CV_FILLED);
		}

	}

	greyFrame.copyTo(previousGreyFrame);

	invTrans = rigidTransform.inv(DECOMP_SVD);
	warpAffine((*frame), (*outputMask), invTrans.rowRange(0, 2), Size());
}
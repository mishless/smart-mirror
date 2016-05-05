#include "detector.h"
#include "main.h"

using namespace std;
using namespace cv;

void *extractParameters(void *frameBuffer) {
	bool respirationRateExecuted = false;
	while (true) {
		// Here goes the code that check if it is time to extract a certain parameter
		// This will be executed by another thread
		FrameBuffer* localFrameBuffer = ((FrameBuffer*)frameBuffer);
		if (localFrameBuffer->size() == 0) {
			// Well obviously this is not right, is it?
		}
		if (false && respirationRateExecuted && (Timer::getTimestamp() % REFRESH_PERIOD*1000 == 0)) {
			// Once the respiration rate ran, so now we can just update via the moving window method
			std::cout << "Refreshing respiration rate..." << std::endl;
		}
		if (!respirationRateExecuted && Timer::getTimestamp() >= RESPIRATION_RATE_WINDOW*1000) {
			// The respiration rate extraction haven't run yet, so we have to run it
			int numberOfFrames = localFrameBuffer->size();
			std::vector<Scalar> blueChannelMean;
			std::vector<Scalar> greenChannelMean;
			std::vector<Scalar> redChannelMean;
			while (localFrameBuffer->size() > 0) {
				Mat channels[3];
				Mat frame = localFrameBuffer->front()->getMatrix();
				localFrameBuffer->pop_front();
				split(frame, channels);
				blueChannelMean.push_back(mean(channels[0]));
				greenChannelMean.push_back(mean(channels[1]));
				redChannelMean.push_back(mean(channels[2]));
			}
			std::cout << "Number of frames: " << numberOfFrames << std::endl;
			for (int i = 0; i < numberOfFrames; i++) {
				std::cout << blueChannelMean[i] << " " << greenChannelMean[i] << " " << redChannelMean[i] << std::endl;
			}
			respirationRateExecuted = true;
		}
	}
	pthread_exit(NULL);
	return 0;
}

void *trackAndDetect(void *buffers) {
	// Here goes the code that gets a frame and tracks/detects
	// This will be executed by another thread
	State state = NO_FACE_DETECTED;
	FrameBuffer* rawFramesBuffer = ((FrameBuffer**)buffers)[0];
	FrameBuffer* faceBuffer = ((FrameBuffer**)buffers)[1];
	FrameBuffer* eyesBuffer = ((FrameBuffer**)buffers)[2];
	FrameBuffer* handsBuffer = ((FrameBuffer**)buffers)[3];
	Mat mask, frame, previousGreyFrame, warpedFrame;
	vector<Point2f> previousPoints, points;
	vector<uchar> status;
	vector<float> errors;
	Mat nrt33, newRigidTransform, rigidTransform = Mat::eye(3, 3, CV_32FC1);
	
	Mat invTrans;
	Detector detector;
	detector.initialize();
	std::vector<Rect> faces;
	while (true) {
		if (rawFramesBuffer->size() == 0) {
			//std::cout << "track-and-detect-thread: buffer is empty." << std::endl;
			continue;
		}

		// Get the next frame from raw buffer
		frame = rawFramesBuffer->front()->getMatrix();
		rawFramesBuffer->pop_front();

		Mat greyFrame;
		cvtColor(frame, greyFrame, CV_BGR2GRAY);

		RNG rng(12345);
		switch (state) {
		case NO_FACE_DETECTED:
			// Detect faces
			if (detector.detectFace(&frame, &mask)) {
				state = FACE_DETECTED;
			}
			break;
		case FACE_DETECTED:
			const double qualityLevel = 0.01;
			const double minDistance = 10;
			const int blockSize = 3;
			const bool useHarrisDetector = false;
			const double k = 0.04;
			if (previousPoints.size() < 200) {
				goodFeaturesToTrack(greyFrame, points, 500, qualityLevel, minDistance, mask, blockSize, useHarrisDetector, k);
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
					break;
				}
				if (previousPoints.size() != points.size()) {
					cout << "Previous points: " << previousPoints.size() << " Points: " << points.size() << endl;
					return 0;
				}
				newRigidTransform = estimateRigidTransform(previousPoints, points, false);
				if (newRigidTransform.size() == Size(0, 0)) {
					rigidTransform = Mat::eye(3, 3, CV_32FC1);
					previousPoints.clear();
					previousGreyFrame.release();
					break;
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
					circle(frame, previousPoints[i], 3, Scalar(0, 0, 255), CV_FILLED);
				}

			}

			greyFrame.copyTo(previousGreyFrame);

			invTrans = rigidTransform.inv(DECOMP_SVD);
			warpAffine(frame, warpedFrame, invTrans.rowRange(0, 2), Size());
			imshow("Test", warpedFrame);
			waitKey(1);
			break;
		}
	}

	//Process frames from raw frames buffer and put the result in processed frames buffer

	pthread_exit(NULL);
	return 0;
}

void *collectFrames(void *frameBuffer) {
	// Here goes the code that stores frames ina circular buffer
	// This will be executed by another thread
	VideoCapture vc;
	Mat frame;
	char ch;
	bool isVideo = true;
	try {
		vc.open(0); //open default camera
	}
	catch (Exception e) {
		frame = Mat::eye(100, 100, CV_32FC1);
		isVideo = false;
	} while ((ch = waitKey(10)) != 'q') {
		if (isVideo)
		{
			vc >> frame;
			long long int frameTimestamp = Timer::getTimestamp();
			((FrameBuffer*)frameBuffer)->push_back(new Frame(frame, frameTimestamp));
		}
	}
	if (isVideo)
		vc.release();
	pthread_exit(NULL);
	return 0;
}

int main(int argc, char* argv[])
{
	FrameBuffer rawFramesBuffer{ MAX_BUFFER_SIZE };
	FrameBuffer faceBuffer{ MAX_BUFFER_SIZE };
	FrameBuffer eyesBuffer{ MAX_BUFFER_SIZE };
	FrameBuffer handsBuffer{ MAX_BUFFER_SIZE };

	FrameBuffer* buffers[4]{ &rawFramesBuffer , &faceBuffer, &eyesBuffer, &handsBuffer };
	Thread frameBufferThread;
	Thread detectAndTrackThread;
	Thread extractParametersThread;
	pthread_create(&frameBufferThread, NULL, collectFrames, &rawFramesBuffer);
	pthread_create(&detectAndTrackThread, NULL, trackAndDetect, &buffers);
	//pthread_create(&extractParametersThread, NULL, extractParameters, &buffers);
	while (true) {

	}
	return 0;
}
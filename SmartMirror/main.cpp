#include "detector.h"
#include "tracker.h"
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
		if (false && respirationRateExecuted && (Timer::getTimestamp() % REFRESH_PERIOD * 1000 == 0)) {
			// Once the respiration rate ran, so now we can just update via the moving window method
			std::cout << "Refreshing respiration rate..." << std::endl;
		}
		if (!respirationRateExecuted && Timer::getTimestamp() >= RESPIRATION_RATE_WINDOW * 1000) {
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
				Mat m = Mat(numberOfFrames - 2, 1, CV_8UC3, 1);
				//L = spdiags([e -2*e e], 0:2, numberOfFrames-2, numberOfFrames);
				int alpha = 10;
				//int I = speye(numberOfFrames, numberOfFrames);
				//	int detrendedFrames = (I - inv(I + alpha ^ 2 * (L'*L)))*framesAveraged;
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
	Mat mask, frame, warpedFrame;

	Detector faceDetector;
	faceDetector.initialize("/../../../sources/data/haarcascades/haarcascade_frontalface_alt2.xml", true);
	Detector handDetector;
	handDetector.initialize("hands_final.xml", false);
	Detector eyesDetector;
	eyesDetector.initialize("/../../../sources/data/haarcascades/haarcascade_mcs_eyepair_big.xml", true);
	Tracker faceTracker;
	faceTracker.initialize();
	Tracker handTracker;
	handTracker.initialize();
	Tracker eyesTracker;
	eyesTracker.initialize();
	std::vector<Rect> faces;
	while (true) {
		if (rawFramesBuffer->size() == 0) {
			//std::cout << "track-and-detect-thread: buffer is empty." << std::endl;
			continue;
		}

		// Get the next frame from raw buffer
		frame = rawFramesBuffer->front()->getMatrix();
		rawFramesBuffer->pop_front();

		RNG rng(12345);
		switch (state) {
		case NO_FACE_DETECTED:
			// Detect face
			if (faceDetector.detect(&frame, &mask)) {
				state = FACE_DETECTED;
			}
			break;
		case FACE_DETECTED:
			// Track face
			faceTracker.track(&frame, &mask, &warpedFrame);
			imshow("Test", warpedFrame);
			waitKey(1);
			//detect and track hand
			if (handDetector.isDetected()) {
				handTracker.track(&frame, &mask, &warpedFrame);
				imshow("Hand Test", warpedFrame);
				waitKey(1);
			}else{
			/*	if (handDetector.detect(&frame, &mask)) {
					handDetector.setDetected(true);
				}*/
			}
			//detect and track eyes
			if (eyesDetector.isDetected()) {
				//eyesTracker.track(? ? ? );
				imshow("Eyes test", warpedFrame);
				waitKey(1);
			}else{
				/*if (eyesDetector.detect(? ? )) {
					eyesDetector.setDetected(true);
				}*/
			}
			break;
		}
	}
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
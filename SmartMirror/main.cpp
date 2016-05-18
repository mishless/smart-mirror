#include "hunter.h"
#include "main.h"
#include "opencv2/core/core.hpp"
#include "opencv2/contrib/contrib.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <string>

using namespace std;
using namespace cv;

void *extractParameters(void *buffers) {
	
	FrameBuffer* rawFramesBuffer = ((FrameBuffer**)buffers)[0];
	FrameBuffer* faceBuffer = ((FrameBuffer**)buffers)[1];
	FrameBuffer* eyesBuffer = ((FrameBuffer**)buffers)[2];
	FrameBuffer* handsBuffer = ((FrameBuffer**)buffers)[3];

	Mat face;
	long long int timeStamp;
	vector<Mat> foreheads;

	/* Forehead rectange */
	Rect foreheadRect = Rect(FACE_W * 0.15, 0, FACE_W*0.7, FACE_H * 0.3);

	while (faceBuffer->size() < 60)
	{
		;
	}

	int i = 0;
	while (i < 60)
	{
		face = faceBuffer->front()->getMatrix();
		timeStamp = faceBuffer->front()->getTimestamp();
		faceBuffer->pop_front();

		foreheads.push_back(face(foreheadRect));
		i++;
	}

	/* Convert whole vector of foreheads to YCrCb */
	vector<Mat> foreheadsYCrCb;
	for (i = 0; i < 60; i++)
	{
		Mat foreheadConverted;
		cvtColor(foreheads[i], foreheadConverted, CV_BGR2YCrCb);
		foreheadsYCrCb.push_back(foreheadConverted);
	}

	/* Calculate means of frames */
	vector<double> means;
	for (i = 0; i < 60; i++)
	{
		Scalar meansScalar;
		double totalMean;
		meansScalar = mean(foreheadsYCrCb[i]);
		totalMean = (meansScalar[0] + meansScalar[1] + meansScalar[2]) / 3;
		means.push_back(totalMean);
	}
	
	/* Finding the sum */
	double sum = 0;
	for (i = 0; i < 60; i++)
	{
		sum += means[i];
	}

	/* Partially normalize */
	vector<double> partiallyNormalized;
	double minVal = 266;
	double maxVal = -1;
	for (i = 0; i < 60; i++)
	{
		double tmp = means[i] / sum;
		if (tmp < minVal) minVal = tmp;
		if (tmp > maxVal) maxVal = tmp;
		partiallyNormalized.push_back(means[i] / sum);
	}

	/* Normalize */
	vector<double> normalized;
	for (i = 0; i < 60; i++)
	{
		double x = (partiallyNormalized[i] - minVal) / (maxVal - minVal);
		normalized.push_back(x);
	}

	
	cout << "End!" << endl;
	while (1)
		;

	/*bool respirationRateExecuted = false;
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
			size_t numberOfFrames = localFrameBuffer->size();
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
	pthread_exit(NULL);*/
	return 0;
}

void *trackAndDetect(void *buffers) {

	/* Here goes the code that gets a frame and tracks/detects
	   This will be executed by another thread */

	State state = NO_FACE_DETECTED;
	FrameBuffer* rawFramesBuffer = ((FrameBuffer**)buffers)[0];
	FrameBuffer* faceBuffer = ((FrameBuffer**)buffers)[1];
	FrameBuffer* eyesBuffer = ((FrameBuffer**)buffers)[2];
	FrameBuffer* handsBuffer = ((FrameBuffer**)buffers)[3];
	Mat mask, frame, warpedFrame, face, faceMask, unflippedFrame, grayFace;
	Mat eyes, hand;
	long long int timeStamp;
	PersonInfo personInfo;

	Recognizer recognizer;
	recognizer.initialize(RECOGNITION_TRESHOLD);
	if (!recognizer.train("info.txt", "Database"))
	{
		cout << "Training recognizer failed!" << endl;
		while (1);
	}

	Hunter faceHunter, handHunter, eyesHunter;
	if (!faceHunter.initialize("haarcascade_frontalface_alt2.xml", true, FACE_W, FACE_H))
	{
		while (1);
	}
	if (!eyesHunter.initialize("haarcascade_mcs_eyepair_big.xml", true, EYES_W, EYES_H))
	{
		while (1);
	}



	while (true) {

		/* If there are no new frames, continue */
		if (rawFramesBuffer->size() == 0) {
			continue;
		}
		
		/* Get the next frame from raw buffer */
		unflippedFrame = rawFramesBuffer->front()->getMatrix();
		timeStamp = rawFramesBuffer->front()->getTimestamp();
		rawFramesBuffer->pop_front();

		/* Mirror it so it looks like a mirror */
		flip(unflippedFrame, frame, 1);

		/* Clean old face so old frames don't appear */
		face.release();
		grayFace.release();

		/* Find face*/
		if (faceHunter.hunt(&frame, &face)) {	

			/* Convert to gray */
			cvtColor(face, grayFace, CV_BGR2GRAY);

#if 0
			/* Recognize person */
			if (!recognizer.recognize(grayFace, &personInfo))
				cout << "Unknown" << endl;
			else
				cout << personInfo.fullName << endl;
#endif

			/*Add the face to the face buffer */
			faceBuffer->push_back(new Frame(face, timeStamp));

			/* Find eyes */
			if (eyesHunter.hunt(&face, &eyes))
			{
				/* Add eyes to the eyes buffer */
				eyesBuffer->push_back(new Frame(eyes, timeStamp));
			}
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
	pthread_create(&extractParametersThread, NULL, extractParameters, &buffers);
	
	pthread_join(detectAndTrackThread, NULL);
	return 0;
}
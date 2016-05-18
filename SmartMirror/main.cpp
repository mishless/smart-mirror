#include "hunter.h"
#include "main.h"
#include "opencv2/core/core.hpp"
#include "opencv2/contrib/contrib.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <string>
#include <math.h>

using namespace std;
using namespace cv;

void convertSpectrum(vector<double> inputSp, vector<double>* outputSp)
{
	/* Get dft point number */
	int N = (int)inputSp.size();

	/* DC component should be in first place of vector */
	outputSp->push_back(inputSp[0]);

	/* Calculate the rest of amplitude spectrum by using the pythogarian theorem
	   on real-complex value pairs from the input spectrum */
	int i = 1;
	while (i < N - 2)
	{
		double tmp;
		tmp = sqrt(inputSp[i] * inputSp[i] + inputSp[i + 1] * inputSp[i + 1]);
		outputSp->push_back(tmp);
		i += 2;
	}
}

void filterSpectrum(vector<double> *inputSp, double lowFreq, double highFreq, 
	double samplingFrequency, int *lowIndex, int *highIndex)
{
	int N = (int)inputSp->size();

	*lowIndex = (int) round(lowFreq*N / samplingFrequency);
	if (*lowIndex == 0) *lowIndex = 1;
	*highIndex = (int) round(highFreq*N / samplingFrequency);
}

void *extractParameters(void *buffers) {
	
	FrameBuffer* rawFramesBuffer = ((FrameBuffer**)buffers)[0];
	FrameBuffer* faceBuffer = ((FrameBuffer**)buffers)[1];
	FrameBuffer* eyesBuffer = ((FrameBuffer**)buffers)[2];
	FrameBuffer* handsBuffer = ((FrameBuffer**)buffers)[3];

	Mat face;
	long long int timeStamp;
	vector<Mat> foreheads;

	/* Forehead rectange */
	Rect foreheadRect = Rect((int)(FACE_W * 0.15), (int)0, (int)(FACE_W*0.7), 
		(int)(FACE_H * 0.3));

	while (faceBuffer->size() < HR_WINDOW)
	{
		;
	}

	int i = 0;
	while (i < HR_WINDOW)
	{
		face = faceBuffer->front()->getMatrix();
		timeStamp = faceBuffer->front()->getTimestamp();
		faceBuffer->pop_front();

		foreheads.push_back(face(foreheadRect));
		i++;
	}

	/* Convert whole vector of foreheads to YCrCb */
	vector<Mat> foreheadsYCrCb;
	for (i = 0; i < HR_WINDOW; i++)
	{
		Mat foreheadConverted;
		cvtColor(foreheads[i], foreheadConverted, CV_BGR2YCrCb);
		foreheadsYCrCb.push_back(foreheadConverted);
	}

	/* Calculate means of frames */
	vector<double> means;
	for (i = 0; i < HR_WINDOW; i++)
	{
		Scalar meansScalar;
		double totalMean;
		meansScalar = mean(foreheadsYCrCb[i]);
		totalMean = (meansScalar[0] + meansScalar[1] + meansScalar[2]) / 3;
		means.push_back(totalMean);
	}
	
	/* Finding the sum */
	double sum = 0;
	for (i = 0; i < HR_WINDOW; i++)
	{
		sum += means[i];
	}

	/* Partially normalize */
	vector<double> partiallyNormalized;
	double minVal = 266;
	double maxVal = -1;
	for (i = 0; i < HR_WINDOW; i++)
	{
		double tmp = means[i] / sum;
		if (tmp < minVal) minVal = tmp;
		if (tmp > maxVal) maxVal = tmp;
		partiallyNormalized.push_back(means[i] / sum);
	}

	/* Normalize */
	vector<double> normalized;
	for (i = 0; i < HR_WINDOW; i++)
	{
		double x = (partiallyNormalized[i] - minVal) / (maxVal - minVal);
		normalized.push_back(x);
	}

	/* Do DFT */
	vector<double> complexSpectrum;
	vector<double> ampSpectrum;

	/* Get amplitude spectrum */
	dft(normalized, complexSpectrum);
	convertSpectrum(complexSpectrum, &ampSpectrum);

	/* Filter it */
	int lowInd, highInd;
	filterSpectrum(&ampSpectrum, HR_LOW_FREQ, HR_HIGH_FREQ, 1000 / SAMPLING_PERIOD,
		&lowInd, &highInd);

	/* Find biggest peak */
	int maxInd = lowInd;
	for (i = lowInd + 1; i <= highInd; i++)
	{
		if (ampSpectrum[i] > ampSpectrum[maxInd])
		{
			maxInd = i;
		}
	}

	/* Convert it to heartbeat */
	double maxFrequency, heartBeat;
	double ibi;

	maxFrequency = (double) maxInd * SAMPLING_FREQUENCY / (double) ampSpectrum.size();
	heartBeat = 60 * maxFrequency;
	ibi = 60 * 1000 / heartBeat; /* Milliseconds */

	cout << "Heartbeat: " << heartBeat << endl;
	cout << "Ibi : " << ibi << endl;

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
	/* Here goes the code that stores frames in a circular buffer
	   This will be executed by another thread */

	VideoCapture vc;
	Mat frame;
	long long int newTimeStamp, lastTimeStamp;

	/* Try opening the default camera */
	try {
		vc.open(0);
	}
	catch (Exception e) {
		/* If unsuccessful, exit */

		cout << "Opening camera failed!" << endl;
		pthread_exit(NULL);
		return 0;
	} 
	
	/* Read first frame and throw it away.
	   This is done because reading the first frame always bugs */
	vc >> frame;

	lastTimeStamp = Timer::getTimestamp();

	while (1) 
	{
		/* Wait until the time is right to get next frame */
		while (Timer::getTimestamp() - lastTimeStamp < SAMPLING_PERIOD)
			;

		/* Get next frame from the webcam */
		vc >> frame;
			
		/* Update time stamps */
		newTimeStamp = Timer::getTimestamp();
		cout << newTimeStamp - lastTimeStamp << endl;
		lastTimeStamp = newTimeStamp;

		/* Add the frame to the buffer */
		((FrameBuffer*)frameBuffer)->push_back(new Frame(frame, newTimeStamp));
	}


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
	
	pthread_join(frameBufferThread, NULL);
	return 0;
}
#include "hunter.h"
#include "main.h"
#include "opencv2/core/core.hpp"
#include "opencv2/contrib/contrib.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <string>
#include <math.h>

using namespace std;
using namespace cv;


void *extractParameters(void *buffers) {
	
	FrameBuffer* rawFramesBuffer = ((FrameBuffer**)buffers)[0];
	FrameBuffer* foreheadBuffer = ((FrameBuffer**)buffers)[1];
	FrameBuffer* eyesBuffer = ((FrameBuffer**)buffers)[2];
	FrameBuffer* handsBuffer = ((FrameBuffer**)buffers)[3];

	FrequencyDetector HBDetector;
	HBDetector.initialize(HR_LOW_FREQ, HR_HIGH_FREQ);

	FrequencyDetector RRDetector;
	RRDetector.initialize(RR_LOW_FREQ, RR_HIGH_FREQ);

	Mat forehead, foreheadConverted;
	double maxFrequencyHB, maxFrequencyRR, heartBeat;
	double respRate;
	double ibi;
	vector<Mat> foreheads;
	double HBMean, RRMean;
	double HBSum = 0;
	double RRSum = 0;
	int cnt = 0;

	while (1)
	{
		/* Save foreheads in temp vector, used for HR detection */
		while (foreheads.size() < HR_WINDOW)
		{
			while (foreheadBuffer->size() == 0) {
				;
			}
			foreheads.push_back(foreheadBuffer->front()->getMatrix());
			foreheadBuffer->pop_front();
		}	
	
		/* Calculate frequency with HB detector */
		maxFrequencyHB = HBDetector.detectFrequency(&foreheads); /* Hertz */
		heartBeat = 60 * maxFrequencyHB; /* Beats per minute */
		ibi = 60 * 1000 / heartBeat; /* Milliseconds */

		/* Calculate respiration rate with RR detector */
		maxFrequencyRR = RRDetector.detectFrequency(&foreheads); /* Hertz */
		respRate = maxFrequencyRR * 60;

		/* Update statistics for mean values */
		cnt++;
		HBSum += heartBeat;
		HBMean = HBSum / cnt;
		RRSum += respRate;
		RRMean = RRSum / cnt;
		
		cout << "*************************" << endl;
		cout << "Heartbeat: " << heartBeat << endl;
		cout << "Heartbeat mean: " << HBMean << endl;
		cout << "Ibi : " << ibi << endl;
		cout << "Respiration rate: " << respRate << endl;
		cout << "Respiration rate mean: " << RRMean << endl;
		
		foreheads.clear();
	}

	return 0;
}

void *trackAndDetect(void *buffers) {

	/* Here goes the code that gets a frame and tracks/detects
	   This will be executed by another thread */

	State state = NO_FACE_DETECTED;
	FrameBuffer* rawFramesBuffer = ((FrameBuffer**)buffers)[0];
	FrameBuffer* foreheadBuffer = ((FrameBuffer**)buffers)[1];
	FrameBuffer* eyesBuffer = ((FrameBuffer**)buffers)[2];
	FrameBuffer* handsBuffer = ((FrameBuffer**)buffers)[3];
	Mat mask, frame, warpedFrame, face, faceMask, unflippedFrame, grayFace;
	Mat eyes, hand;
	long long int timeStamp;
	PersonInfo personInfo;
	bool isRecognized = false;

	/* Forehead rectange,
	   Will be used to extract forehead from the face */
	Rect foreheadRect = Rect((int)(FACE_W * 0.15), (int)0, (int)(FACE_W*0.7),
		(int)(FACE_H * 0.3));

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

		/* If removed, causes errors */
		face.release();
		eyes.release();

		/* Find face*/
		if (faceHunter.hunt(&frame, &face)) {	

			/* Convert to gray */
			cvtColor(face, grayFace, CV_BGR2GRAY);

			/* Recognize person */
			if (!isRecognized)
			{
				if (!recognizer.recognize(grayFace, &personInfo))
					cout << "Unknown Person" << endl;
				else
				{
					cout << "Found " << personInfo.fullName << endl;
					isRecognized = true;
				}
			}

			/*Add the forehead to the forehead buffer */
			foreheadBuffer->push_back(new Frame(face(foreheadRect), timeStamp));

			/* Find eyes */
			if (eyesHunter.hunt(&face, &eyes))
			{
				/* Add eyes to the eyes buffer */
				eyesBuffer->push_back(new Frame(eyes, timeStamp));
			}
		}
		else
		{
			/* If face is lost, reset isRecognized flag */
			isRecognized = false;
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
	vc >> frame;
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
	FrameBuffer foreheadBuffer{ MAX_BUFFER_SIZE };
	FrameBuffer eyesBuffer{ MAX_BUFFER_SIZE };
	FrameBuffer handsBuffer{ MAX_BUFFER_SIZE };

	FrameBuffer* buffers[4]{ &rawFramesBuffer , &foreheadBuffer, &eyesBuffer, &handsBuffer };
	Thread frameBufferThread;
	Thread detectAndTrackThread;
	Thread extractParametersThread;
	pthread_create(&frameBufferThread, NULL, collectFrames, &rawFramesBuffer);
	pthread_create(&detectAndTrackThread, NULL, trackAndDetect, &buffers);
	pthread_create(&extractParametersThread, NULL, extractParameters, &buffers);
	
	pthread_join(frameBufferThread, NULL);
	return 0;
}
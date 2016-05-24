#include "hunter.h"
#include "main.h"
#include "opencv2/core/core.hpp"
#include "opencv2/contrib/contrib.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <math.h>

using namespace std;
using namespace cv;
using namespace OpenNN;

void *extractParameters(void *buffers) {
	
	FrameBuffer* rawFramesBuffer = ((FrameBuffer**)buffers)[0];
	FrameBuffer* foreheadBuffer = ((FrameBuffer**)buffers)[1];
	FrameBuffer* eyesBuffer = ((FrameBuffer**)buffers)[2];
	FrameBuffer* palmBuffer = ((FrameBuffer**)buffers)[3];

	FrequencyDetector HBDetector;
	HBDetector.initialize(HR_LOW_FREQ, HR_HIGH_FREQ);

	FrequencyDetector RRDetector;
	RRDetector.initialize(RR_LOW_FREQ, RR_HIGH_FREQ);

	PhaseDetector phaseDetector;

	Mat forehead, foreheadConverted;
	double maxFrequencyHB, maxFrequencyRR, heartBeat;
	double respRate;
	double ibi;
	vector<Mat> foreheads;
	vector<Mat> palms;
	double HBMean, RRMean;
	double HBSum = 0;
	double RRSum = 0;
	double something;
	int cnt = 0;
	Mat palm;


	while (1)
	{
		/* Save foreheads in temp vector, used for HR detection */
		while (foreheads.size() < HR_WINDOW)
		{
			while (foreheadBuffer->size() == 0 || palmBuffer->size() == 0) {
				;
			}
			foreheads.push_back(foreheadBuffer->front()->getMatrix());
			palms.push_back(palmBuffer->front()->getMatrix());
			foreheadBuffer->pop_front();
			palmBuffer->pop_front();
		}	
	
		/* Calculate frequency with HB detector */
		maxFrequencyHB = HBDetector.detectFrequency(&foreheads); /* Hertz */
		heartBeat = 60 * maxFrequencyHB; /* Beats per minute */
		ibi = 60 * 1000 / heartBeat; /* Milliseconds */

		/* Calculate respiration rate with RR detector */
		maxFrequencyRR = RRDetector.detectFrequency(&foreheads); /* Hertz */
		respRate = maxFrequencyRR * 60;

		/* Get Pulse Transit Time using the detected HB frequency */
		something = phaseDetector.detect(&foreheads, &palms, maxFrequencyHB);
		//cout << "Something: " << something << endl;

		/* Update statistics for mean values */
		cnt++;
		HBSum += heartBeat;
		HBMean = HBSum / cnt;
		RRSum += respRate;
		RRMean = RRSum / cnt;
		
		//cout << "*************************" << endl;
		cout << "Heartbeat: " << heartBeat << endl;
		cout << "Heartbeat mean: " << HBMean << endl;
		cout << "Ibi : " << ibi << endl;
		cout << "Respiration rate: " << respRate << endl;
		cout << "Respiration rate mean: " << RRMean << endl;
		cout << "*************************" << endl;

		foreheads.clear();
		palms.clear();
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
	FrameBuffer* palmBuffer = ((FrameBuffer**)buffers)[3];
	Mat mask, frame, warpedFrame, face, faceMask, unflippedFrame, grayFace;
	Mat eyes, hand;
	long long int timeStamp;
	PersonInfo personInfo;
	bool isRecognized = false;

	/* Forehead rectange,
	   Will be used to extract forehead from the face */
	Rect foreheadRect = Rect((int)(FACE_W * 0.15), (int)0, 
		(int)(FACE_W*0.7),
		(int)(FACE_H * 0.3));

	/* Palm rectangle,
	   Will be used to extract palm from the hand */
	Rect palmRect = Rect((int)(HAND_W * 0.2), (int)(HAND_H * 0.5), 
		(int)(HAND_W*0.6),
		(int)(HAND_H * 0.5));

	Recognizer recognizer;
	recognizer.initialize(RECOGNITION_TRESHOLD);
	if (!recognizer.train("info.txt", "Database"))
	{
		cout << "Training recognizer failed!" << endl;
		while (1);
	}

	Hunter faceHunter, handHunter, eyesHunter;
	if (!faceHunter.initialize("haarcascade_frontalface_alt2.xml", true, 
		FACE_W, FACE_H))
	{
		while (1);
	}
	if (!eyesHunter.initialize("haarcascade_mcs_eyepair_big.xml", true, 
		EYES_W, EYES_H))
	{
		while (1);
	}
	if (!handHunter.initialize("hands_final.xml", false, 200, 200))
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
		hand.release();

		/* Find face*/
		if (faceHunter.hunt(&frame, &face)) {	
			
			/* Convert to gray */
			cvtColor(face, grayFace, CV_BGR2GRAY);

			/* Recognize person */
			if (!isRecognized)
			{
				if (!recognizer.recognize(grayFace, &personInfo))
					//cout << "Unknown Person" << endl;
					;
				else
				{
					cout << "Found " << personInfo.fullName << endl;
					isRecognized = true;
				}
			}

			/*Add the forehead to the forehead buffer */
			//foreheadBuffer->push_back(new Frame(face(foreheadRect), timeStamp));

			/* Find eyes */
			if (eyesHunter.hunt(&face, &eyes))
			{

				/* Add eyes to the eyes buffer */
				eyesBuffer->push_back(new Frame(eyes, timeStamp));
			}

			/* Find hand */
			if (handHunter.hunt(&frame, &hand))
			{
				foreheadBuffer->push_back(new Frame(face(foreheadRect), timeStamp));
				palmBuffer->push_back(new Frame(hand(palmRect), timeStamp));
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

void probaj()
{
	BPNeuralNetwork bpnn(4,6,3);
	vector<double> input(4);
	vector<double> output;

	input[0] = 185;
	input[1] = 85;
	input[2] = 0.28;
	input[3] = 23;
	
	//bpnn.train("svoje.dat");
	bpnn.load("kolja");
	bpnn.getOutput(input, &output);
	while (1)
		;
}

int main(int argc, char* argv[])
{

	//probaj();

	FrameBuffer rawFramesBuffer{ MAX_BUFFER_SIZE };
	FrameBuffer foreheadBuffer{ MAX_BUFFER_SIZE };
	FrameBuffer eyesBuffer{ MAX_BUFFER_SIZE };
	FrameBuffer handsBuffer{ MAX_BUFFER_SIZE };

	FrameBuffer* buffers[4]{ &rawFramesBuffer , &foreheadBuffer,
		                     &eyesBuffer, &handsBuffer };
	Thread frameBufferThread;
	Thread detectAndTrackThread;
	Thread extractParametersThread;
	pthread_create(&frameBufferThread, NULL, collectFrames, &rawFramesBuffer);
	pthread_create(&detectAndTrackThread, NULL, trackAndDetect, &buffers);
	pthread_create(&extractParametersThread, NULL, extractParameters, &buffers);
	
	pthread_join(frameBufferThread, NULL);
	return 0;
}
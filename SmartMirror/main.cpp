#include "hunter.h"
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
	Mat mask, frame, warpedFrame, face, faceMask, imageToShow;

	Hunter faceHunter, handHunter, eyeHunter;
	faceHunter.initialize("haarcascade_frontalface_alt2.xml", true);
	std::vector<Rect> faces;

	Rect faceRect;

	while (true) {

		if (rawFramesBuffer->size() == 0) {
			continue;
		}
		
		/* Get the next frame from raw buffer */
		frame = rawFramesBuffer->front()->getMatrix();
		rawFramesBuffer->pop_front();

		face.release();
		if (faceHunter.hunt(&frame, &faceRect, &face)) {
			flip(face, imageToShow, 1);
			imshow("hunt face", imageToShow);
			waitKey(1);
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




	//float aData[3][3] = { {1, 0, 0},{0, 1, 0}, {0, 0, 1} };
	//float bData[3][3] =
	//	/*{
	//	{0.99990714, -0.00084443804, 0.35972571},
	//	{0.00084443804, 0.99990714, -0.2556518 },
	//	{0, 0, 1} };*/
	//{ {2,3,4}, {5,6,7}, {8,9,10} };
	//Mat a(3, 3, CV_32FC1, &aData);
	//Mat b(3, 3, CV_32FC1, &bData);
	//cout << "A: " << a << endl;
	//cout << "B: " << b << endl;
	//a *= b;
	//cout << "Result:" << endl << a;
	//while (1);
	//return 0;

	FrameBuffer rawFramesBuffer{ MAX_BUFFER_SIZE };
	FrameBuffer faceBuffer{ MAX_BUFFER_SIZE };
	FrameBuffer eyesBuffer{ MAX_BUFFER_SIZE };
	FrameBuffer handsBuffer{ MAX_BUFFER_SIZE };

	FrameBuffer* buffers[4]{ &rawFramesBuffer , &faceBuffer, &eyesBuffer, &handsBuffer };
	Thread frameBufferThread;
	Thread detectAndTrackThread;
	//Thread extractParametersThread;
	pthread_create(&frameBufferThread, NULL, collectFrames, &rawFramesBuffer);
	pthread_create(&detectAndTrackThread, NULL, trackAndDetect, &buffers);
	//pthread_create(&extractParametersThread, NULL, extractParameters, &buffers);
	while (true) {

	}
	return 0;
}
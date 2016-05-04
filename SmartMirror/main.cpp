#include "main.h"

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
	bool faceDetected = false;
	FrameBuffer* rawFramesBuffer = ((FrameBuffer**)buffers)[0];
	FrameBuffer* faceBuffer = ((FrameBuffer**)buffers)[1];
	FrameBuffer* eyesBuffer = ((FrameBuffer**)buffers)[2];
	FrameBuffer* handsBuffer = ((FrameBuffer**)buffers)[3];
	if (!faceDetected) {
		// Detect a face
	}
	else {
		// Detect hands and eyes
	}
	while (true) {
		if (rawFramesBuffer->size() == 0) {
			continue;
		}
		if (faceDetected) {
			continue;
		}
		std::cout << rawFramesBuffer->size() << std::endl;
		// Get the next frame from raw buffer
		Mat image = rawFramesBuffer->front()->getMatrix();
		rawFramesBuffer->pop_front();
		// Load Face cascade (.xml file)
		CascadeClassifier face_cascade; 
		String boostLibraryPath;
		char* buf = 0;
		size_t sz = 0;
		if (_dupenv_s(&buf, &sz, "OPENCV_DIR") == 0)
		{
			boostLibraryPath = (String)buf;
			free(buf);
		}
		face_cascade.load(boostLibraryPath + "/../../../sources/data/haarcascades/haarcascade_frontalface_alt2.xml");

		// Detect faces
		std::vector<Rect> faces;
		Rect rect;
		faces.push_back(rect);
		face_cascade.detectMultiScale(image, faces, 1.3, 3);
		if (faces.size() != 0) {
			faceDetected = true;
		}
		// Draw circles on the detected faces
		for (int i = 0; i < faces.size(); i++)
		{
			Point center(faces[i].x + faces[i].width*0.5, faces[i].y + faces[i].height*0.5);
			ellipse(image, center, Size(faces[i].width*0.5, faces[i].height*0.5), 0, 0, 360, Scalar(255, 0, 255), 4, 8, 0);
		}
		imshow("Detected Face", image);
		waitKey(1);
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
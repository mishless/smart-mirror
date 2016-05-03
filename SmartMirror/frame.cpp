#include "frame.h"

Frame::Frame(cv::Mat frameMatrix, long long int frameTimestamp)
{
	matrix = frameMatrix;
	timestamp = frameTimestamp;
}

void Frame::setMatrix(cv::Mat frameMatrix)
{
	matrix = frameMatrix;
}

cv::Mat Frame::getMatrix(void)
{
	return matrix;
}

void Frame::setTimestamp(long long int frameTimestamp)
{
	timestamp = frameTimestamp;
}

long long int Frame::getTimestamp(void)
{
	return timestamp;
}
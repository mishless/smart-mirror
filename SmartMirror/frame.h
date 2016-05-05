#ifndef __FRAME_H_INCLUDED__
#define __FRAME_H_INCLUDED__

#include <opencv2\core\core.hpp>
#include <boost\circular_buffer.hpp>

using namespace cv;
using namespace boost;

class Frame
{
public:
	void setMatrix(Mat frameMatrix);
	Mat getMatrix(void);
	void setTimestamp(long long int frameTimestamp);
	long long int getTimestamp(void);
	Frame(Mat frameMatrix, long long int frameTimestamp);
private:
	Mat matrix;
	long long int timestamp;
};

typedef circular_buffer_space_optimized<Frame*> FrameBuffer;

#endif

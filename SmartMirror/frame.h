// A2DD.h
#ifndef frame_h
#define frame_h
#ifndef core_h
#include <opencv2\core\core.hpp>
#endif

class Frame
{
public:
	void setMatrix(cv::Mat frameMatrix);
	cv::Mat getMatrix(void);
	void setTimestamp(long long int frameTimestamp);
	long long int getTimestamp(void);
	Frame(cv::Mat frameMatrix, long long int frameTimestamp);
private:
	cv::Mat matrix;
	long long int timestamp;
};
#endif

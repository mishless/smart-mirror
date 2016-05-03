#include <iostream>
#include <boost\circular_buffer.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <pthread.h>
#include "frame.h"
#include "timer.h"

typedef boost::circular_buffer_space_optimized<Frame*> FrameBuffer;
typedef pthread_t Thread;

const unsigned long long int MAX_BUFFER_SIZE = 2000;
const int REFRESH_PERIOD = 20;
const int RESPIRATION_RATE_WINDOW = 30;
const int HEARTBEAT_WINDOW = 6;
const int BLOOD_PRESSURE_WINDOW = 20;
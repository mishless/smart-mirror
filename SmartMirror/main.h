#include <opencv2\features2d\features2d.hpp>
#include <opencv2\video\video.hpp>
#include <pthread.h>
#include <string>
#include "frame.h"
#include "timer.h"

typedef pthread_t Thread;

const unsigned long long int MAX_BUFFER_SIZE = 2000;
const int REFRESH_PERIOD = 20;
const int RESPIRATION_RATE_WINDOW = 30;
const int HEARTBEAT_WINDOW = 6;
const int BLOOD_PRESSURE_WINDOW = 20;

enum State { FACE_DETECTED, NO_FACE_DETECTED };

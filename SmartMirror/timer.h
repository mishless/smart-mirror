#ifndef __TIMER_H_INCLUDED__
#define __TIMER_H_INCLUDED__

#include <boost\date_time\posix_time\posix_time.hpp>

using namespace boost;

typedef posix_time::ptime Time;
typedef posix_time::time_duration TimeDuration;

class Timer {
public:
	static void initializeTimer();
	static long long int getTimestamp();
	static Time startTime;
private:
	static long long int timestamp;
};
#endif
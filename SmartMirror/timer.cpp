#include "timer.h"

Time Timer::startTime = posix_time::microsec_clock::local_time();
long long int Timer::timestamp = 0;

void Timer::initializeTimer() {
	Timer::startTime = posix_time::microsec_clock::local_time();
}

long long int Timer::getTimestamp() {
	Time currentTime = posix_time::microsec_clock::local_time();
	TimeDuration diff = currentTime - startTime;
	return diff.total_milliseconds();
}
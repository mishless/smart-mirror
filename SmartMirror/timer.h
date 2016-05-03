#include <boost\date_time\posix_time\posix_time.hpp>

typedef boost::posix_time::ptime Time;
typedef boost::posix_time::time_duration TimeDuration;

class Timer {
public:
	static void initializeTimer();
	static long long int getTimestamp();
	static Time startTime;
private:
	static long long int timestamp;
};
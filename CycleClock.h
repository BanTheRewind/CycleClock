#pragma once

#include "boost/signals2.hpp"
#include <thread>

typedef std::shared_ptr<class CycleClock> CycleClockRef;

class CycleClock : public std::enable_shared_from_this<CycleClock>
{
public:
	static CycleClockRef				create();
	~CycleClock();

	void								tick( double seconds, bool repeat = false );

	double								getCpuFreq() const;

	double								getInterval() const;
	void								setInterval( double interval );
	
	bool								isRepeating() const;
	void								enableRepeat( bool repeat );
	
	void								stop();
	
	template<typename T, typename Y>
	inline void							setCallback( T callback, Y* callbackObject )
	{
		mSignal.connect( std::bind( callback, callbackObject ) );
	}
protected:
	typedef std::shared_ptr<std::thread>	ThreadRef;

	union int64U
	{
		struct 
		{
			int32_t low;
			int32_t high;
		} ints;
		__int64 int64;
	};

	CycleClock();

	void								calcCpuFreq();
	volatile double						mCpuFreq;
	ThreadRef							mThreadCpuFreq;
	
	__int64								mDelay;
	__int64								mInterval;
	double								mIntervalSeconds;
	bool								mRepeat;
	volatile bool						mRunningTick;
	boost::signals2::signal<void ()>	mSignal;
	ThreadRef							mThreadTick;
	__int64								rdtsc();
	void								runTick();
};

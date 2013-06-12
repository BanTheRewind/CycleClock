#include "CycleClock.h"

#include <Windows.h>

using namespace std;

CycleClockRef CycleClock::create()
{
	return CycleClockRef( new CycleClock() )->shared_from_this();
}

CycleClock::CycleClock()
	: mCpuFreq( 0.0 ), mDelay( 0 ), mInterval( 0 ), mIntervalSeconds( 0.0 ), 
	mRepeat( false ), mRunningTick( false )
{
	mThreadCpuFreq	= ThreadRef( new thread( &CycleClock::calcCpuFreq, this ) );
}

CycleClock::~CycleClock()
{
	tick( 0.0, false );
	if ( mThreadCpuFreq ) {
		mThreadCpuFreq->join();
		mThreadCpuFreq.reset();
	}
}

void CycleClock::tick( double seconds, bool repeat )
{
	mRunningTick = false;
	setInterval( seconds );
	enableRepeat( repeat );

	if ( mThreadTick ) {
		mThreadTick->join();
		mThreadTick.reset();
	}

	if ( mIntervalSeconds > 0.0 ) {
		mRunningTick	= true;
		mThreadTick		= ThreadRef( new thread( &CycleClock::runTick, this ) );
	}
}

double CycleClock::getCpuFreq() const
{
	return mCpuFreq;
}

double CycleClock::getInterval() const
{
	return mIntervalSeconds;
}

void CycleClock::setInterval( double interval )
{
	mIntervalSeconds	= interval;
	mInterval			= (__int64)( mIntervalSeconds * mCpuFreq ); 
	mDelay				= mInterval;
}

bool CycleClock::isRepeating() const
{
	return mRepeat;
}

void CycleClock::enableRepeat( bool repeat )
{
	mRepeat = repeat;
}

void CycleClock::calcCpuFreq()
{
	__int64 cycleStart		= 0;
	__int64 cycleStop		= 0;
	unsigned __int64 count	= 0;
	unsigned __int64 freq	= 0;
	unsigned __int64 i		= 0;

	if ( !QueryPerformanceFrequency( (LARGE_INTEGER*)&freq ) ) {
		mCpuFreq = 0.0;
		return;
	}

	QueryPerformanceCounter( (LARGE_INTEGER*)&count );
	count += freq;

	__asm {
		_EMIT 0x0F;
		_EMIT 0x31;
		MOV DWORD PTR cycleStart, EAX;
		MOV DWORD PTR [ cycleStart + 4 ], EDX;
	}

	while ( i < count ) {
		QueryPerformanceCounter( (LARGE_INTEGER*)&i );
	}

	__asm {
		__EMIT 0x0F;
		__EMIT 0x31;
		MOV DWORD PTR cycleStop, EAX;
		MOV DWORD PTR [ cycleStop + 4 ], EDX;
	}

	mCpuFreq = (double)cycleStop - (double)cycleStart;
	if ( mIntervalSeconds > 0.0 ) {
		setInterval( mIntervalSeconds );
	}
}

__int64 CycleClock::rdtsc()
{
	int64U v;
	__asm {
		_EMIT 0x0F;
		_EMIT 0x31;
		MOV v.ints.low, EAX;
		MOV v.ints.high, EDX;
	}
	return v.int64;
}

void CycleClock::runTick()
{
	while ( mCpuFreq == 0.0 ) {
	}
	__int64 c1 = rdtsc();
	while ( mRunningTick ) {
		__int64 c2		= rdtsc();
		__int64 elapsed = c2 - c1;
		if ( elapsed >= mDelay ) {
			mDelay = mInterval - ( elapsed - mInterval );
			c1 = c2;
			mSignal();
			if ( !mRepeat ) {
				mRunningTick = false;
			}
		}
	}
}

void CycleClock::stop()
{
	mRunningTick = false;
}

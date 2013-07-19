/*
* 
* Copyright (c) 2013, Ban the Rewind
* All rights reserved.
* 
* Redistribution and use in source and binary forms, with or 
* without modification, are permitted provided that the following 
* conditions are met:
* 
* Redistributions of source code must retain the above copyright 
* notice, this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright 
* notice, this list of conditions and the following disclaimer in 
* the documentation and/or other materials provided with the 
* distribution.
* 
* Neither the name of the Ban the Rewind nor the names of its 
* contributors may be used to endorse or promote products 
* derived from this software without specific prior written 
* permission.
* 
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS 
* FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE 
* COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, 
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF 
* ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
* 
*/

#pragma once

#include <memory>
#include <thread>

typedef std::shared_ptr<class CycleClock> CycleClockRef;

class CycleClock : public std::enable_shared_from_this<CycleClock>
{
public:
	static CycleClockRef	create();
	~CycleClock();

	void					tick( double seconds, bool repeat = false );

	double					getCpuFreq() const;

	double					getInterval() const;
	void					setInterval( double interval );
	
	bool					isRepeating() const;
	void					enableRepeat( bool repeat );
	
	void					stop();
	
	template<typename T, typename Y>
	inline void				connectEventHandler( T callback, Y* callbackObject )
	{
		connectEventHandler( std::bind( callback, callbackObject ) );
	}

	inline void				connectEventHandler( std::function<void ()> eventHandler )
	{
		mEventHandler = eventHandler;
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

	void					calcCpuFreq();
	volatile double			mCpuFreq;
	ThreadRef				mThreadCpuFreq;
	
	__int64					mDelay;
	std::function<void ()>	mEventHandler;
	__int64					mInterval;
	double					mIntervalSeconds;
	bool					mRepeat;
	volatile bool			mRunningTick;
	ThreadRef				mThreadTick;
	__int64					rdtsc();
	void					runTick();
};

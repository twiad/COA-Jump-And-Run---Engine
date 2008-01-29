

#include <iostream>
#include <boost/thread.hpp>
#define uint unsigned int

typedef double TimeValue;
typedef double TimeStamp;

TimeStamp g_startTime = 0;

TimeStamp timeStamp()
{
    boost::xtime timeStamp;
    boost::xtime_get(&timeStamp, boost::TIME_UTC);
    return ((timeStamp.sec) + timeStamp.nsec / 1000000000.0) - g_startTime;
}

TimeValue waitBoost(TimeValue p_duration)
{
    TimeStamp waitStart = timeStamp();

    boost::xtime wakeupTime;
    boost::xtime_get(&wakeupTime, boost::TIME_UTC);

    uint timeToWaitSec = (uint)p_duration;
    uint timeToWaitNSec = (uint)((p_duration - timeToWaitSec) * 1000000000);

    if(timeToWaitNSec + wakeupTime.nsec > 1000000000)
    {
        wakeupTime.sec += timeToWaitSec + 1;
        wakeupTime.nsec += timeToWaitNSec - 1000000000;
    }
    else
    {
        wakeupTime.sec += timeToWaitSec;
        wakeupTime.nsec += timeToWaitNSec;
    }

    boost::thread::sleep(wakeupTime);

    return timeStamp() - waitStart;
}

//TimeValue waitSpinLock(TimeValue p_duration)
//{
//    TimeStamp waitStart = timeStamp();
//    TimeStamp wakeupTime = waitStart + p_duration;
//
//    while(timeStamp() < wakeupTime);
//
//    return timeStamp() - waitStart;
//
//}
//
//TimeValue waitSpinSleep(TimeValue p_duration)
//{
//    TimeStamp waitStart = timeStamp();
//    TimeStamp wakeupTime = waitStart + p_duration;
//
//    while(timeStamp() < wakeupTime)
//        sleep(0);
//
//    return timeStamp() - waitStart;
//
//}

int main(int argc, char** argv)
{
    g_startTime = timeStamp();
    TimeValue worstTime = 0;
    uint failCount = 0;
    TimeValue waited;
    TimeValue waitAvg = 0;



    TimeValue waitTime = 0.01;
    uint testCount = 2000;

    std::cout << "trying to wait " << waitTime 
            << " seconds " << testCount << " times" << std::endl;


    for(int i = 0; i < testCount; i++)
    {
        waited = waitBoost(waitTime);
//        waited = waitSpinLock(waitTime);
//        waited = waitSpinSleep(waitTime);

        if(waited > (waitTime * 1.02))
        {
            failCount++;

            if(waited > worstTime)
                worstTime = waited;
        }
        
        waitAvg += waited;

            
    }


    std::cout << ((double)failCount / (double)testCount) * 100.0f 
            << "%  failed" << std::endl;

    std::cout << "avg:   " << waitAvg / (double)testCount << std::endl;
    std::cout << "worst: " << worstTime << std::endl;

    return 0;
}



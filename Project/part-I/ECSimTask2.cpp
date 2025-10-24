//
//  ECSimTask2.cpp
//  
//
//

#include "ECSimTask2.h"
#include <string>
#include <vector>
#include <iostream>
using namespace std;

ECMultiIntervalsTask::ECMultiIntervalsTask(const string &tid) : ECSimTask(tid) {}

void ECMultiIntervalsTask::AddInterval(int a, int b){
    intervals.push_back(make_pair(a, b));
}

bool ECMultiIntervalsTask::IsReadyToRun(int tick) const{
    for (auto i : intervals){
        if (tick >= i.first && tick <= i.second){
            return true;
        }
    }
    return false;
}

bool ECMultiIntervalsTask::IsFinished(int tick) const{
    for (auto i : intervals){
        if (tick <= i.second){
            return false;
        }
    }
    return true;
}

ECHardIntervalTask::ECHardIntervalTask(const string &tid, int tmStart, int tmEnd) : ECSimTask(tid), tmStart(tmStart), tmEnd(tmEnd) {}

bool ECHardIntervalTask::IsReadyToRun(int tick) const{
    if (tick < tmStart || tick > tmEnd) {
        return false;
    }
    if (tick == tmStart) {
        return true;
    }
    return GetTotRunTime() > 0;
}

bool ECHardIntervalTask::IsFinished(int tick) const{
    if (tick > tmEnd) {
        return true;
    }
    if (tick > tmStart && GetTotRunTime() == 0) {
        return true;
    }
    return false;
}

ECConsecutiveIntervalTask::ECConsecutiveIntervalTask(const string &tid, int tmStart, int tmEnd) : ECSimTask(tid), tmStart(tmStart), tmEnd(tmEnd), started(false), interrupted(false) {}

void ECConsecutiveIntervalTask::Wait(int tick, int duration){
    if (started == true && tick <= tmEnd){
        interrupted = true;
    }
    ECSimTask::Wait(tick, duration);
}

void ECConsecutiveIntervalTask::Run(int tick, int duration){
    if (tick < tmStart || tick > tmEnd || interrupted) {
        return;
    }
    if (!started) {
        started = true;
    }
    ECSimTask::Run(tick, duration);
}

bool ECConsecutiveIntervalTask::IsReadyToRun(int tick) const{
    if (interrupted == true)
        return false;
    if (started == false)
        return tick >= tmStart;
    return tick <= tmEnd;
}

bool ECConsecutiveIntervalTask::IsFinished(int tick) const{
    if (interrupted || tick > tmEnd)
        return true;
    return false;
}

ECPeriodicTask::ECPeriodicTask(const string &tid, int tmStart, int runLen, int sleepLen) : ECSimTask(tid), tmStart(tmStart), runLen(runLen), sleepLen(sleepLen) {}

bool ECPeriodicTask::IsReadyToRun(int tick) const{
    int start = tick - tmStart;
    int cycleTotal = runLen + sleepLen;
    if (tick < tmStart){
        return false;
    }
    return ((start % cycleTotal) < runLen);
}

bool ECPeriodicTask::IsFinished(int tick) const{
    return false;
}

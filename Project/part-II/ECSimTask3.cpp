//
//  ECSimTask3.cpp
//  
//

#include "ECSimTask3.h"
#include <iostream>
#include <string>
using namespace std;

//***********************************************************
// Basic task
//***********************************************************

//***********************************************************
// Interval task: a single interval.
// YW: you shouldn't need to change this class!

ECSimIntervalTask :: ECSimIntervalTask(const std::string &tidIn, int tmStartIn, int tmEndIn) : tid(tidIn), tmStart(tmStartIn), tmEnd(tmEndIn), tmTotWait(0), tmTotRun(0)
{
}

// Is task ready to run at certain time? tick: the current clock time (in simulation unit)
bool ECSimIntervalTask :: IsReadyToRun(int tick) const
{
    return tick >= tmStart && tick <= tmEnd;
}

// Is task complete at certain time? If so, scheduler may remove it from the list. tick the current clock time (in simulation unit)
bool ECSimIntervalTask :: IsFinished(int tick) const
{
    return tick > tmEnd;
}

//***********************************************************
// Consecutive task: a task that can early abort

ECSimConsecutiveTask :: ECSimConsecutiveTask(ECSimTask *pTask) : decorator(pTask), interrupted(false)
{
}

// your code here

bool ECSimConsecutiveTask::IsReadyToRun(int tick) const{
    if (decorator->IsReadyToRun(tick) == true && interrupted == false){ // Check for interruption and normal ready to run check
        return true; // Can run
    }
    return false; // Can't run
}

bool ECSimConsecutiveTask::IsFinished(int tick) const{
    if (decorator->IsFinished(tick) == true || interrupted == true){ // Check for interruption or normal finish check
        return true; // Has finished
    }
    return false; // Hasn't finished
}

bool ECSimConsecutiveTask::IsAborted(int tick) const{
    if (interrupted == true){ // CHecks for interruption
        return true; // It was interrupted, so its aborted
    }
    return false; // It wasn't interrupted, so it was not aborted
}

void ECSimConsecutiveTask::Run(int tick, int duration){
    decorator->Run(tick, duration); // normal version
}

void ECSimConsecutiveTask::Wait(int tick, int duration){
    interrupted = true; // Marks as interrupted, so aborted
    decorator->Wait(tick, duration); // Nromal version
}

//***********************************************************
// Periodic task: a task that can early abort

ECSimPeriodicTask :: ECSimPeriodicTask(ECSimTask *pTask, int ls) : decorator(pTask), currentStart(0), currentEnd(0), lenSleep(ls), nextStart(0), nextEnd(0), started(false), interrupted(false)
{
}

// your code here

bool ECSimPeriodicTask::IsFinished(int tick) const{
    return false; // Always return false since periodic loops unless another decorator task ends it forcefully
}

bool ECSimPeriodicTask::IsReadyToRun(int tick) const{
    if (IsAborted(tick) == true){ // Check for abortion to end in advance
        return false; // Can't run since it was aborted
    }
    
    if (tick == nextEnd + 1){ // Check for next tick loop
        currentStart = nextStart; // Moves to the next set
        currentEnd = nextEnd; // Moves to the next set
        nextStart = currentEnd + lenSleep + 1; // Moves this to the calculated following set from the new last loop
        nextEnd = nextStart + (currentEnd - currentStart); // Subtracts to find the new end for the upcoming loop
    }

    if (started == false){ // Checks to make sure its a new loop and started hasn't been marked yet
        if (decorator->IsReadyToRun(tick) == true && interrupted == false){ // Does the normal ready run check and checks if no interruptions
            int temp = tick; // Use a temp variable for the upcoming loop
            while (decorator->IsReadyToRun(temp) == true){ // Checks that the temp var for tick is still ready to run with at that time
                temp++; // Increments the temp variable instead of tick itself
            }

            started = true; // It has started now
            currentStart = tick; // Mark the start time
            currentEnd = temp - 1; // Uses temp after the loop to calculate the ending time properly
            nextStart = currentEnd + lenSleep + 1; // Uses the new lastEnd to calculate the next starting time after sleeping + 1
            nextEnd = nextStart + (currentEnd - currentStart); // Subtracts with the new vars to find the upcoming end time
            return true; // Can actually run
        }
        return false; // Can't run, but the preceding if statement might have run to prepare it
    }
    if ((tick >= currentStart && tick <= currentEnd) || (tick >= nextStart && tick <= nextEnd)){ // Checks if tick is within the proper range of either of the two sets of loops
        return true; // Can run
    } else{ // failed the if condition for within the bounds, so can't run
        return false; // Can't run
    }
}

void ECSimPeriodicTask::Run(int tick, int duration){
    if (decorator->IsAborted(tick) == false){ // Check for abortion
        decorator->Run(tick, duration);
    }
}

void ECSimPeriodicTask::Wait(int tick, int duration){
    if (decorator->IsAborted(tick) == false){ // Check for abortion
        interrupted = true; // Marks as interrupted since it was waited
        decorator->Wait(tick, duration);
    }
}

//***********************************************************
// Task with a deadline to start: a task that must start by some time; otherwise terminate

ECSimStartDeadlineTask :: ECSimStartDeadlineTask(ECSimTask *pTask, int tmStartDeadlineIn)
 : decorator(pTask), deadline(tmStartDeadlineIn), started(false)
{
}

// your code here

bool ECSimStartDeadlineTask::IsFinished(int tick) const{
    if (IsAborted(tick) == true || decorator->IsFinished(tick) == true){ // Abortion check using the deadline check in IsAborted
        return true;
    }
    return false;
}

bool ECSimStartDeadlineTask::IsReadyToRun(int tick) const{
    if (IsAborted(tick) == false || tick <= deadline && started == false && decorator->IsReadyToRun(tick) == true){ // Deadline check using abortion and the deadline and normal ready run check
        return decorator->IsReadyToRun(tick);
    }
    return false;
}

bool ECSimStartDeadlineTask::IsAborted(int tick) const{
    if (started == false && tick > deadline){ // Deadline check for abortion
        return true;
    }
    return decorator->IsAborted(tick);
}

void ECSimStartDeadlineTask::Run(int tick, int duration){
    if (IsAborted(tick) == false){
        started = true; // Marks it as started in run
        decorator->Run(tick, duration);
    }
}

void ECSimStartDeadlineTask::Wait(int tick, int duration){
    if (IsAborted(tick) == false){
        decorator->Wait(tick, duration);
    }
}

//***********************************************************
// Task must end by some fixed time click: this is useful e.g. when a task is periodic

ECSimEndDeadlineTask :: ECSimEndDeadlineTask(ECSimTask *pTask, int tmEndDeadlineIn)
 : decorator(pTask), deadline(tmEndDeadlineIn)
{
}

// your code here

bool ECSimEndDeadlineTask::IsFinished(int tick) const{
    if (IsAborted(tick) == true || decorator->IsFinished(tick) == true){ // Also uses aborted test to check for finish
        return true;
    }
    return false;
}

bool ECSimEndDeadlineTask::IsReadyToRun(int tick) const{
    if (IsAborted(tick) == true){ // If its aborted, it can't run
        return false;
    }
    return decorator->IsReadyToRun(tick);
}

bool ECSimEndDeadlineTask::IsAborted(int tick) const{
    if (decorator->IsFinished(tick) == false && tick > deadline){ // Checks finish test and that it doesn't meet the deadline for aborted chekc
        return true;
    }
    return false;
}

void ECSimEndDeadlineTask::Run(int tick, int duration){
    if (IsAborted(tick) == false){
        decorator->Run(tick, duration);
    }
}

void ECSimEndDeadlineTask::Wait(int tick, int duration){
    if (IsAborted(tick) == false){
        decorator->Wait(tick, duration);
    }
}

//***********************************************************
// Composite task: contain multiple sub-tasks

ECSimCompositeTask :: ECSimCompositeTask(const std::string &tidIn) : tid(tidIn), runtime(0), waittime(0)
{
}


// your code here

void ECSimCompositeTask::AddSubtask(ECSimTask *pt){
    list.push_back(pt); // Adds tasjs to the list
}

bool ECSimCompositeTask::IsReadyToRun(int tick) const{
    for (auto& i : list){ // Iterates through all tasks
        if (i->IsReadyToRun(tick) == true){ // Ready Run check for all tasks
            return true;
        }
    }
    return false;
}

bool ECSimCompositeTask::IsFinished(int tick) const{
    for (auto& i : list) {
        if (i->IsFinished(tick) == false){ // Finished test for all tasks
            return false;
        }
    }
    return true;
}

bool ECSimCompositeTask::IsAborted(int tick) const{
    for (auto& i : list){
        if (i->IsAborted(tick) == true){ // Abortion test for all tasks
            return true;
        }
    }
    return false;
}

void ECSimCompositeTask::Run(int tick, int duration){
    for (auto& i : list){
        if (i->IsReadyToRun(tick) == true){ // Ready Run check for actual runs
            i->Run(tick, duration); // Runs each task in  the list
        }
    }
    runtime++; // Increments the runtime manually instead of using GetTotRunTime
}

void ECSimCompositeTask::Wait(int tick, int duration){
    for (auto& i : list){
        if (i->IsReadyToRun(tick) == true){ // Ready run check for actual waits
            i->Wait(tick, duration); // Does the waits for each task in the list
        }
    }
    waittime++; // INcrements the Wait Time manually instead of using GetTotWaitTime
}

int ECSimCompositeTask::GetTotWaitTime() const{
    return waittime; // Returns the manual variable we used to store wait time
}

int ECSimCompositeTask::GetTotRunTime() const{
    return runtime; // Returns the manual variable we used to store run time
}

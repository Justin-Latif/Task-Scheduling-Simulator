//
//  ECSimTaskScheduler2.cpp
//  
//
//

#include <vector>
#include <iostream>
using namespace std;

#include "ECSimTaskScheduler2.h"
#include "ECSimTask.h"

ECSimLWTFTaskScheduler::ECSimLWTFTaskScheduler() {}

ECSimTask* ECSimLWTFTaskScheduler::ChooseTaskToSchedule(const vector<ECSimTask*> &listReadyTasks) const{
    if (listReadyTasks.empty() == true){
        return nullptr;
    }
    ECSimTask* best = listReadyTasks[0];
    int bestTime = best->GetTotWaitTime();
    for (size_t i = 1; i < listReadyTasks.size(); i++){
        int newTime = listReadyTasks[i]->GetTotWaitTime();
        if (newTime > bestTime){
            best = listReadyTasks[i];
            bestTime = newTime;
        }
    }
    return best;
}

ECSimPriorityScheduler::ECSimPriorityScheduler() {}

ECSimTask* ECSimPriorityScheduler::ChooseTaskToSchedule(const vector<ECSimTask*> &listReadyTasks) const{
    if (listReadyTasks.empty() == true){
        return nullptr;
    }
    ECSimTask* best = listReadyTasks[0];
    int bestPriority = best->GetPriority();
    for (size_t i = 1; i < listReadyTasks.size(); i++){
        int newPriority = listReadyTasks[i]->GetPriority();
        if (newPriority < bestPriority){
            best = listReadyTasks[i];
            bestPriority = newPriority;
        }
    }
    return best;
}

ECSimRoundRobinTaskScheduler::ECSimRoundRobinTaskScheduler() {}

ECSimTask* ECSimRoundRobinTaskScheduler::ChooseTaskToSchedule(const vector<ECSimTask*> &listReadyTasks) const{
    if (listReadyTasks.empty() == true){
        return nullptr;
    }
    ECSimTask* best = listReadyTasks[0];
    int bestTime = best->GetTotRunTime();
    for (size_t i = 1; i < listReadyTasks.size(); i++){
        int newTime = listReadyTasks[i]->GetTotRunTime();
        if (newTime < bestTime){
            best = listReadyTasks[i];
            bestTime = newTime;
        }
    }
    return best;
}

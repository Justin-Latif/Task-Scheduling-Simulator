//
//  ECSimTask3.h
//  
//
//  Flexible task classes, with the following attribues
//  that you can choose any subsets:
//  (i) consequitive: if asked to wait while it wants to run before finish, terminates before completing all the planned time
//  (ii) delayable (can be delayed; e.g. [1,3] can become [4,5] (delayed by 3 time ticks)
//  (iii) deadline to start (must start by certain time)
//  (iv) deadline to finish (must end by some time)
//  (v) periodic: this task runs restarts after it finishes its current iteration
//  (vi) can contain sub-tasks: if any subtask aborts, this task is finished; otherwise it finishes when all subtasks finishes.

#ifndef ECSimTask3_h
#define ECSimTask3_h

#include <vector>
#include <string>

//***********************************************************
// Generic simulation task
//***********************************************************

class ECSimTask
{
public:
    virtual ~ECSimTask() {}
    
    // Get the task id
    virtual std::string GetId() const = 0;
    
    // Is task ready to run at certain time? tick: the current clock time (in simulation unit)
    virtual bool IsReadyToRun(int tick) const = 0;
    
    // Is task complete at certain time? If so, scheduler may remove it from the list. tick the current clock time (in simulation unit)
    virtual bool IsFinished(int tick) const = 0;
    
    // Is task early abort? There can be various reasons for abort: e.g., missed deadline
    virtual bool IsAborted(int tick) const = 0;
    
    // Run the task for some duration (usually 1, but can be more) starting from time tick
    virtual void Run(int tick, int duration) = 0;
    
    // Wait for some duration (usually 1, but can be more), starting from time tick
    virtual void Wait(int tick, int duration) = 0;
    
    // How much total time does the task has to wait to get its turn so far?
    virtual int GetTotWaitTime() const = 0;
    
    // Get total run-time (so far)
    virtual int GetTotRunTime() const = 0;
};


//***********************************************************
// Basic task: you shouldn't need to change this class!
//***********************************************************

//***********************************************************
// Interval task: a single interval

class ECSimIntervalTask : public ECSimTask
{
public:
    ECSimIntervalTask(const std::string &tid, int tmStart, int tmEnd);
    
    // Get the task id
    virtual std::string GetId() const { return this->tid; }
    
    // Is task ready to run at certain time? tick: the current clock time (in simulation unit)
    virtual bool IsReadyToRun(int tick) const;
    
    // Is task complete at certain time? If so, scheduler may remove it from the list. tick the current clock time (in simulation unit)
    virtual bool IsFinished(int tick) const;
    
    // Is task early abort? There can be various reasons for abort: e.g., missed deadline
    virtual bool IsAborted(int tick) const { return false; }
    
    // Run the task for some duration (usually 1, but can be more) starting from time tick
    virtual void Run(int tick, int duration) { tmTotRun += duration; }
    
    // Wait for some duration (usually 1, but can be more), starting from time tick
    virtual void Wait(int tick, int duration) { tmTotWait += duration; }
    
    // How much total time does the task has to wait to get its turn so far?
    virtual int GetTotWaitTime() const { return tmTotWait; }
    
    // Get total run-time (so far)
    virtual int GetTotRunTime() const { return tmTotRun; }
    
private:
    std::string tid;
    int tmStart;
    int tmEnd;
    int tmTotWait;
    int tmTotRun;
};

//***********************************************************
// Consecutive task: a task that can early abort
// This class modifies the passed-in task to have the consecutive property

class ECSimConsecutiveTask : public ECSimTask
{
public:
  ECSimConsecutiveTask(ECSimTask *pTask);
  
  // your code ehre 
  virtual std::string GetId() const {return decorator->GetId();} // Virtual overrides for methods for cpp with implementation
  virtual bool IsReadyToRun(int tick) const; // Virtual overrides for methods for cpp
  virtual bool IsFinished(int tick) const; // Virtual overrides for methods for cpp
  virtual bool IsAborted(int tick) const; // Virtual overrides for methods for cpp
  virtual void Run(int tick, int duration); // Virtual overrides for methods for cpp
  virtual void Wait(int tick, int duration); // Virtual overrides for methods for cpp
  virtual int GetTotWaitTime() const {return decorator->GetTotWaitTime();} // Virtual overrides for methods for cpp with implementation
  virtual int GetTotRunTime() const {return decorator->GetTotRunTime();} // Virtual overrides for methods for cpp with implementation

private:
  ECSimTask* decorator; // Decorator pointer
  bool interrupted; // Represents whether or not the task was interrupted, causing immediate shutdown of the task since consecutive
};

//***********************************************************
// Periodic task: a task that can early abort
// This class modifies the passed-in task to have the early abort property

class ECSimPeriodicTask : public ECSimTask
{
public:
  ECSimPeriodicTask(ECSimTask *pTask, int lenSleep);

  // your code here
  virtual std::string GetId() const {return decorator->GetId();} // Virtual overrides for methods for cpp with implementation
  virtual bool IsReadyToRun(int tick) const; // Virtual overrides for methods for cpp
  virtual bool IsFinished(int tick) const; // Virtual overrides for methods for cpp
  virtual bool IsAborted(int tick) const {return decorator->IsAborted(tick);} // Virtual overrides for methods for cpp with implementation
  virtual void Run(int tick, int duration); // Virtual overrides for methods for cpp
  virtual void Wait(int tick, int duration); // Virtual overrides for methods for cpp
  virtual int GetTotWaitTime() const {return decorator->GetTotWaitTime();} // Virtual overrides for methods for cpp with implementation
  virtual int GetTotRunTime() const {return decorator->GetTotRunTime();} // Virtual overrides for methods for cpp with implementation

private:
  ECSimTask* decorator; // Decorator pointer
  int lenSleep; // Copy of the lenSleep constructor variable that represents the length of sleep in between tasks
  mutable int currentStart; // Represents the last tick's start time, all has to be mutable or errors generate
  mutable int currentEnd; // Represents the last tick's end time, all has to be mutable or errors generate
  mutable int nextStart; // Represents the next tick's start time, all has to be mutable or errors generate
  mutable int nextEnd; // Represents the next tick's end time, all has to be mutable or errors generate
  mutable bool started; // Represents whether or  not the tasks are running, all has to be mutable or errors generate
  mutable bool interrupted; // Represents if the task was interrupted or waited, all has to be mutable or errors generate
};

//***********************************************************
// Task with a deadline to start: a task that must start by some time; otherwise terminate
// This class modifies the passed-in task to have a deadline to start 

class ECSimStartDeadlineTask : public ECSimTask
{
public:
  ECSimStartDeadlineTask(ECSimTask *pTask, int tmStartDeadline);

  // your code here
  virtual std::string GetId() const {return decorator->GetId();} // Virtual overrides for methods for cpp with implementation
  virtual bool IsReadyToRun(int tick) const; // Virtual overrides for methods for cpp
  virtual bool IsFinished(int tick) const; // Virtual overrides for methods for cpp
  virtual bool IsAborted(int tick) const; // Virtual overrides for methods for cpp
  virtual void Run(int tick, int duration); // Virtual overrides for methods for cpp
  virtual void Wait(int tick, int duration); // Virtual overrides for methods for cpp
  virtual int GetTotWaitTime() const {return decorator->GetTotWaitTime();} // Virtual overrides for methods for cpp with implementation
  virtual int GetTotRunTime() const {return decorator->GetTotRunTime();} // Virtual overrides for methods for cpp with implementation

private:
  ECSimTask* decorator; // Decorator pointer
  bool started; // Started variable to mark whether it has started or not for checks
  int deadline; // Specific time that the deadline is marked on, from the constructor variable tmStartDeadline
};

//***********************************************************
// Task must end by some fixed time click: this is useful e.g. when a task is periodic
// This class modifies the passed-in task to have a deadline to end 

class ECSimEndDeadlineTask : public ECSimTask
{
public:
  ECSimEndDeadlineTask(ECSimTask *pTask, int tmEndDeadline);

  // your code here
  virtual std::string GetId() const {return decorator->GetId();} // Virtual overrides for methods for cpp with implementation
  virtual bool IsReadyToRun(int tick) const; // Virtual overrides for methods for cpp
  virtual bool IsFinished(int tick) const; // Virtual overrides for methods for cpp
  virtual bool IsAborted(int tick) const; // Virtual overrides for methods for cpp
  virtual void Run(int tick, int duration); // Virtual overrides for methods for cpp
  virtual void Wait(int tick, int duration); // Virtual overrides for methods for cpp
  virtual int GetTotWaitTime() const {return decorator->GetTotWaitTime();} // Virtual overrides for methods for cpp with implementation
  virtual int GetTotRunTime() const {return decorator->GetTotRunTime();} // Virtual overrides for methods for cpp with implementation

private:
  ECSimTask* decorator; // Decorator pointer
  int deadline; // Specific time that the deadline is marked on, from the constructor variable tmEndDeadLine
};

//***********************************************************
// Composite task: contain multiple sub-tasks

class ECSimCompositeTask : public ECSimTask
{
public:
  ECSimCompositeTask(const std::string &tidIn);
    
  virtual std::string GetId() const{return tid;}
    
  // Add subtask
  void AddSubtask(ECSimTask *pt);

  // your code ehre
  virtual bool IsReadyToRun(int tick) const; // Virtual overrides for methods for cpp
  virtual bool IsFinished(int tick) const; // Virtual overrides for methods for cpp
  virtual bool IsAborted(int tick) const; // Virtual overrides for methods for cpp
  virtual void Run(int tick, int duration); // Virtual overrides for methods for cpp
  virtual void Wait(int tick, int duration); // Virtual overrides for methods for cpp
  virtual int GetTotWaitTime() const; // Virtual overrides for methods for cpp
  virtual int GetTotRunTime() const; // Virtual overrides for methods for cpp

private:
  std::string tid; // For the id variable in the constructor
  std::vector<ECSimTask*> list; // List to represent the composite tasks altogether for iteration
  int runtime; // Runtime counter instead of using the normal GetTotRunTime to compile all tasks together
  int waittime; // Waittime counter instead of using the normal GetTotWaitTime to compile all tasks together
};

#endif /* ECSimTask3_h */

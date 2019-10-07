#pragma once
#include "Observer.h"
#include "Observable.h"
#include "boost/thread.hpp"
#include "boost/signals2.hpp"
#include <mutex>
#include <ctime>
#include <ratio>
#include <chrono>

struct BaseWorkerClassObservers
{
    enum { OnWorkerEvent, OnRunEvent, OnCloseEvent };
    using ObserverTable = std::tuple<
        Observer<void(double& fps)>,
        Observer<void(void)>,
        Observer<void(void)>
    >;
};

class BaseWorkerClass : public Observable<BaseWorkerClassObservers>
{
public:
    bool isRunning;
    BaseWorkerClass();
    ~BaseWorkerClass();
    virtual void Stop(void) = 0;
    virtual void Run(void) = 0;
protected:
    std::mutex m;
    boost::thread* thr;
    virtual void worker(void) = 0;
    std::chrono::steady_clock::time_point t1;
    std::chrono::steady_clock::time_point t2;
    double worker_FPS;
};

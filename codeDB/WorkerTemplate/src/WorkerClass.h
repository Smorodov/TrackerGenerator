
#pragma once
#include <stdarg.h>
#include <stdio.h>
#include <ctime>
#include <ratio>
#include <chrono>
#include "BaseWorkerClass.h"
// -----------------------------
//
// -----------------------------
class WorkerClass : public BaseWorkerClass
{
public:
    void worker(void);
    void Run(void);
    void Stop(void);
    WorkerClass();
    ~WorkerClass();
};

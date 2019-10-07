
#include "BaseWorkerClass.h"

using namespace boost;

BaseWorkerClass::BaseWorkerClass() 
{
    t1 = std::chrono::steady_clock::now();
    t2 = std::chrono::steady_clock::now();
    worker_FPS = 0;
}

BaseWorkerClass::~BaseWorkerClass() {}

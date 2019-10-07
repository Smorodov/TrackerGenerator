#include "WorkerClass.h"
#include <iostream>
// -----------------------------
//
// -----------------------------

    void WorkerClass::worker(void)
    {
        isRunning = true;
        
        int counter = 0;
        int N_avg = 0;
        int N_avg_max = 1000;
        worker_FPS = 0;

        Notify<BaseWorkerClassObservers::OnRunEvent>();
        while (!boost::this_thread::interruption_requested())
        {
            m.lock();

            t2 = std::chrono::steady_clock::now();
            if (counter < N_avg_max)
            {
                N_avg++;
            }
            double FPS = 1.0 / std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1).count();
            worker_FPS = (worker_FPS * float(N_avg) + FPS) / float(N_avg+1);
            t1 = t2;
            counter++;
            m.unlock();

            Notify<BaseWorkerClassObservers::OnWorkerEvent>(worker_FPS);
        }
        isRunning = false;
        Notify<BaseWorkerClassObservers::OnCloseEvent>();
    }

    void WorkerClass::Run(void)
    {
        m.lock();
        t2 = std::chrono::steady_clock::now();
        t1 = t2;
        worker_FPS = 0;
        m.unlock();

        if (!isRunning)
        {
            thr = new boost::thread(&WorkerClass::worker, this);
            isRunning = true;
        }
    }

    void WorkerClass::Stop(void)
    {
        if (isRunning)
        {
            thr->interrupt();
            thr->join();
            delete thr;
            thr = nullptr;
            isRunning = false;
        }
    }

    WorkerClass::WorkerClass()
    {
        isRunning = false;
        thr = nullptr;
    }

    WorkerClass::~WorkerClass()
    {
        if (thr != nullptr)
        {
            thr->interrupt();
            thr->join();
            delete thr;
            thr = nullptr;
        }
    }


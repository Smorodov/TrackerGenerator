
#include <iostream>
#include "BaseWorkerClass.h"
#include "WorkerClass.h"
// -----------------------------
//
// -----------------------------
// Application: our Observer.
class Application
{
public:
    // -----------------------------
    //
    // -----------------------------
    explicit Application(std::vector<BaseWorkerClass*>& workers, int workerToRun) :
        workers_(workers),
        workerToRun_(workerToRun)
    {
        finished = false;
        for (int i = 0; i < workers_.size(); ++i)
        {
            workers_[i]->Register<BaseWorkerClassObservers::OnWorkerEvent>([this](double& worker_FPS)
            {
                OnWorker(worker_FPS);
            });

            workers_[i]->Register<BaseWorkerClassObservers::OnRunEvent>([this](void)
                {
                    OnRun();
                });

            workers_[i]->Register<BaseWorkerClassObservers::OnCloseEvent>([this](void)
                {
                    OnClose();
                });
        }
        std::cout << "Events - registered" << std::endl;      

        workers_[workerToRun_]->Run();

        while (!finished)
        {
            Sleep(100);
            std::cin.ignore();
            finished = true;
        }

        for (int i = 0; i < workers_.size(); ++i)
        {
            workers_[i]->Stop();
        }
    }
    // -----------------------------
    //
    // -----------------------------
    ~Application()
    {

    }

private:
    bool finished;

    void OnRun()
    {        
        std::cout << "On run event." << std::endl;
    }

    void OnClose()
    {
        std::cout << "On close event." << std::endl;
    }
    
    void OnWorker(double worker_FPS)
    {
       std::cout << "On worker. FPS=" << worker_FPS << std::endl;
    }

    std::vector<BaseWorkerClass*>& workers_;
    int workerToRun_;
};
// -----------------------------
//
// -----------------------------
int main()
{
    int workerToRun = 2;
    WorkerClass worker1;
    WorkerClass worker2;
    WorkerClass worker3;
    std::vector<BaseWorkerClass*> workers = { (BaseWorkerClass*)& worker1,(BaseWorkerClass*)& worker2,(BaseWorkerClass*)& worker3 };
    Application application{workers, workerToRun };
}

//
// Created by leclercsyl on 05/10/23.
//
#include <memory>
#include "yuni/job/job.h"
#include "antares/concurrency/concurrency.h"

namespace Antares::Concurrency
{

class JobImpl : public Yuni::Job::IJob {
public:
    JobImpl(const Task& task) : task_(task) {}

    TaskFuture getFuture() {
        return task_.get_future();
    }

protected:
    void onExecute() override {
        task_();
    }

private:
    std::packaged_task<void()> task_;
};

std::unique_ptr<Yuni::Job::IJob> MakeJob(const Task& task) {
    return std::make_unique<JobImpl>(task);
}

std::unique_ptr<JobImpl> MakePackagedJob(const Task& task) {
    return std::make_unique<JobImpl>(task);
}

std::future<void> AddTask(Yuni::Job::QueueService& threadPool, const Task& task) {
    auto job = MakePackagedJob(task);
    auto future = job->getFuture();
    threadPool.add(Yuni::Job::IJob::Ptr(job.release()));
    return future;
}

void FutureSet::add(TaskFuture&& f) {
    futures_.push_back(std::move(f));
}

void FutureSet::join() {
    for (auto& f: futures_) {
        f.get();
    }
}

}

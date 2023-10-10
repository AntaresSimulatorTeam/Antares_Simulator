//
// Created by leclercsyl on 05/10/23.
//
#include <memory>
#include "yuni/job/job.h"
#include "antares/concurrency/concurrency.h"

namespace Antares::Concurrency
{

namespace {

/*!
* Just wraps an arbitrary task as a yuni job, and allows to retrieve the corresponding future.
*/
class PackagedJob : public Yuni::Job::IJob {
public:
    PackagedJob(const Task& task) : task_(task) {}

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

}

std::future<void> AddTask(Yuni::Job::QueueService& threadPool,
                          const Task& task,
                          Yuni::Job::Priority priority) {
    auto job = std::make_unique<PackagedJob>(task);
    auto future = job->getFuture();
    threadPool.add(job.release(), priority);
    return future;
}

void FutureSet::add(TaskFuture&& f) {
    std::lock_guard lock(mutex_);
    futures_.push_back(std::move(f));
}

void FutureSet::join() {
    std::vector<TaskFuture> moved;
    {
        std::lock_guard lock(mutex_);
        moved = std::move(futures_);
        futures_ = std::vector<TaskFuture>{};
    }
    for (auto& f: moved) {
        f.get();
    }
}

}

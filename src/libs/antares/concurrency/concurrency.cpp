//
// Created by leclercsyl on 05/10/23.
//
#include <memory>
#include "yuni/job/job.h"
#include "antares/concurrency/concurrency.h"

namespace {

using namespace Antares::Concurrency;

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

namespace Antares::Concurrency
{

std::future<void> AddTask(Yuni::Job::QueueService& threadPool,
                          const Task& task,
                          Yuni::Job::Priority priority) {
    auto job = std::make_unique<PackagedJob>(task);
    auto future = job->getFuture();
    threadPool.add(Yuni::Job::IJob::Ptr(job.release()), priority);
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

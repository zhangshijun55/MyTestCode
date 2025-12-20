#include "TaskManager.h"
#include <QDebug>

TaskManager::TaskManager(QObject *parent)
    : QObject{parent}
{
    // 线程池最大线程数
    taskPool.setMaxThreadCount(QThread::idealThreadCount());
    // 线程空闲时，销毁判定时间，单位ms，默认半分钟
    taskPool.setExpiryTimeout(1000 * 60);
}

TaskManager::~TaskManager()
{
    cancel();
    wait();
}

TaskManager *TaskManager::instance()
{
    static TaskManager manager;
    return &manager;
}

auto TaskManager::create() -> std::shared_ptr<TWatcher>
{
    return std::make_shared<TWatcher>();
}

void TaskManager::run(const std::function<void (TPromise &)> &task, std::shared_ptr<TWatcher> watcher)
{
    if (!task || !watcher)
        return;

    TPromise promise{};
    auto future = promise.future();

    watcher->setFuture(future);
    {
        // 添加到任务列表
        std::lock_guard<std::mutex> guard(listMutex); Q_UNUSED(guard)
        taskList.push_back(watcher);
        // qDebug() << "push task";
    }

    // 丢进线程池泡澡
    taskPool.start([this, task, watcher, promise = std::move(promise)]() mutable {
        promise.start();
        task(promise);
        promise.finish();
        {
            // 任务结束从列表移除
            std::lock_guard<std::mutex> guard(listMutex); Q_UNUSED(guard)
            taskList.remove(watcher);
            // qDebug() << "remove task";
        }
    });
}

bool TaskManager::busy() const
{
    // 活跃线程数达到上限
    return (taskPool.activeThreadCount() >= taskPool.maxThreadCount());
}

void TaskManager::cancel()
{
    // 移除未启动的任务
    taskPool.clear();
    // 取消所有任务
    std::lock_guard<std::mutex> guard(listMutex); Q_UNUSED(guard)
    for (auto &handle : taskList) {
        handle->cancel();
    }
}

void TaskManager::wait()
{
    // 等待最后一个线程开始
    taskPool.waitForDone(-1);
}

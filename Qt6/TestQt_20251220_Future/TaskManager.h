#pragma once
#include <QThreadPool>
#include <QFuture>
#include <QFutureWatcher>
#include <QPromise>
#include <functional>
#include <list>

// 任务管理
class TaskManager : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(TaskManager)
private:
    explicit TaskManager(QObject *parent = nullptr);
public:
    ~TaskManager();
    static TaskManager *instance();
    // 任务结果暂时为QObject*，可以用一个自定义基类，包含type判断类型
    using TPromise = QPromise<QObject*>;
    using TWatcher = QFutureWatcher<QObject*>;

    // 创建任务future watcher
    // 如果将槽函数作为run参数传入，代码有点乱，拆分为两步，先创建watcher关联信号槽，再run任务
    auto create() -> std::shared_ptr<TWatcher>;
    // 执行任务，繁忙时任务排队
    void run(const std::function<void (TPromise &)> &task, std::shared_ptr<TWatcher> watcher);
    // 判断是否繁忙
    bool busy() const;
    // 取消所有任务
    void cancel();
    // 等待全部任务结束
    void wait();

signals:
    void taskStarted();
    void taskFinished();

private:
    // 任务线程池
    QThreadPool taskPool;
    // 内部管理任务列表
    std::list<std::shared_ptr<TWatcher>> taskList;
    // 任务列表锁
    std::mutex listMutex;
};

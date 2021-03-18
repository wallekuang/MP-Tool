#ifndef WORKER_H
#define WORKER_H

#include <QVector>
#include "flasherrunnable.h"
#include <QThreadPool>
#include <QObject>
#include <QMutex>

class Worker
{
    #define FULL_TASK_NUM (10)

public:
    Worker(QThreadPool* poll);
    void free(void);
    bool is_busy(void);
    uint8_t get_progress(void);
    bool be_assigned_task(FlasherRunnable *task);
    bool start_work(void);
    void stop_work(void);
    bool start_next_task(uint8_t &progress);

signals:
    void tasks_finished(void);

private:
    QVector<FlasherRunnable*> mToDoList;

    QMutex mMutex;
    // 工作饱满了
    bool mFull;
    // 当前执行的任务
    int mCurTask;
    // 忙碌位，只要有没有运行完的任务并且start了，都是属于忙碌
    bool mBusy;
    // 进度
    int mProgress;
    QThreadPool* mPoll;
};

#endif // WORKER_H

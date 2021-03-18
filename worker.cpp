#include "worker.h"

Worker::Worker(QThreadPool* poll)
{
    mPoll = poll;
    free();
}
// 释放所有变量，变成闲人
void Worker::free(void)
{
    mBusy = false;
    mCurTask = -1;
    mProgress = 0;
    mFull = false;
    mToDoList.clear();
}

bool Worker::is_busy(void)
{
    return mBusy;
}
// 获取所有任务进度
uint8_t Worker::get_progress(void)
{
    return mProgress;
}

// 接收增加一项任务
bool Worker::be_assigned_task(FlasherRunnable *task)
{
    bool ret = false;
    if(!mFull){
        mMutex.lock();
        mToDoList.append(task);
        if(mToDoList.length() >= FULL_TASK_NUM){
            mFull = false;
         }
        else{
            mFull = true;
        }
        ret = true;
        mMutex.unlock();
    }
    return ret;
}

// 开始工作
bool Worker::start_work(void)
{
    bool ret = false;
    mMutex.lock();
    if(!mBusy && (mToDoList.length() > 0)){
        mCurTask = 0;
        mProgress = 0;
        mPoll->start(mToDoList.first());
        mBusy = true;
        ret = true;
    }
    mMutex.unlock();
    return ret;
}

void Worker::stop_work(void)
{
    mMutex.lock();
    free();
    mMutex.unlock();
}

bool Worker::start_next_task(uint8_t &progress)
{
    bool ret = false;
    // 如果还没有开工，开工
    if(!mBusy){
        ret = start_work();
        return ret;
    }
    else{
        mMutex.lock();
        mCurTask++;
        // 计算工作进度
        mProgress = mCurTask*100/mToDoList.length();
        progress = mProgress;
        if(mCurTask >= mToDoList.length()){
            free();
            progress = 100;
            // 告知外部， 完成一次任务集
            //tasks_finished();
        }
        else{
            mPoll->start(mToDoList[mCurTask]);
            ret = true;
        }
        mMutex.unlock();
    }
    return ret;
}





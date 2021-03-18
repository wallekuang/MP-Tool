#include "flasherrunnable.h"
#include <QtDebug>
#include <QThread>


FlasherRunnable::FlasherRunnable(QString cmd, QStringList list, int worker_id)
{
    mCmd = cmd;
    mList = list;
    mWorkerId = worker_id;
}

void FlasherRunnable::append_log_line(QString &log, QString &line)
{
    if(line.trimmed().length() > 0){
        log += line.trimmed() + "\n";
    }
}


void FlasherRunnable::run()
{
    QString log;
    mProcess = new QProcess();
    //mProgram = "c:Users/Lucien/ST/BlueNRG-X Flasher Utility 4.1.0/Application/BlueNRG-X_Flasher_Launcher.exe";
    //mProcess->setWorkingDirectory("c:Users/Lucien/ST/BlueNRG-X Flasher Utility 4.1.0/Application");

    mProgram = "../BlueNRG-X_Flasher_Launcher.exe";
    mProcess->setWorkingDirectory("../");

    mList.push_front(mCmd);

    mProcess->start(mProgram, mList);
    mProcess->waitForReadyRead();
    QString line = mProcess->readLine();
    append_log_line(log,line);
    while (mProcess->state() == QProcess::Running) {
        mProcess->waitForReadyRead();
        line = mProcess->readLine();
        append_log_line(log,line);
        // 当即发送实时log
        if(log.length() > 0){
            QStringList list;
            list.append(log);
            emit report_flasher_state(mWorkerId, mCmd, list);
            log.clear();
        }
    }
    // 程序已结束 ，读取剩余的log
    line = mProcess->readLine();
    log += line.trimmed() + "\n";
    while (line.length() != 0) {
        line = mProcess->readLine();
        append_log_line(log,line);
    }

    QStringList list;
    list.append(log);
    list.append("finish");
    emit report_flasher_state(mWorkerId, mCmd, list);
}






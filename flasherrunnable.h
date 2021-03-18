#ifndef FLASHERRUNNABLE_H
#define FLASHERRUNNABLE_H

#include <QObject>
#include <QRunnable>
#include <QProcess>

#include "app_const_cfg.h"

class FlasherRunnable : public QObject, public QRunnable
{
    Q_OBJECT
public:
    explicit FlasherRunnable(QString cmd, QStringList list, int worker_id);

protected:
    void run() override;
    void append_log_line(QString &log, QString &line);

signals:
    // update job log to UI
    void report_flasher_state(int run_id, QString cmd, QStringList list);

private:
    QString mCmd;
    QStringList mList;
    QProcess *mProcess;
    QString mProgram;
    int mWorkerId;
};

#endif // FLASHERRUNNABLE_H

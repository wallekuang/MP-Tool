#ifndef CONTROLER_H
#define CONTROLER_H

#include <QObject>
#include <QThread>
#include <QVector>
#include <QThreadPool>
#include <QRunnable>
#include <QVariant>
#include "flasherrunnable.h"
#include "worker.h"

class Controler : public QThread
{
    Q_OBJECT
    // 每个worker最多每次安排10个任务
    #define MAX_TASKS       (10)
    // 支持选择文件的个数
    #define MAX_FILE_SELCECT_NUM (5)
    // 支持同时运行綫程的个数 支持8个同时下载 + 1路设置
    #define MAX_WORKER      (9)
    #define SWD_RUNNER_ID        (MAX_WORKER-1)
    #define MAX_DOWNLOAD_WORKER  (MAX_WORKER-1)

public:
    enum STATE_CTL{
        STATE_IDEL,                  // 空闲状态
        STATE_SETTING,               // 设置参数中...
        STATE_DOWNLOADING,           // 下载中
    };

    enum DOWNLOAD_MODE{
        //DOWNLOAD_SCAN_QRCODE,       //扫码下载
        DOWNLOAD_MANUAL,            //手工下载
        DOWNLOAD_AUTO,              //
    };

public:
    Controler();
    void init(void);
    enum STATE_CTL get_current_mode(void);
    bool refresh_interface(void);
    void start_auto_download(void);
    void stop_auto_download(void);
    bool start_manual_download(int which);
    void erase_one(int which);
    void erase_all(void);
    void read_device_firmware(void);
    bool check_download_condition(int which);
    void test_fun(void);

    // 解析日志
    void parse_run_log(int worker_id, QString cmd, QString &log,bool &result);

    // 设置参数更新
    bool update_MAC_check(bool checked);
    bool update_scan_mac(bool checkBoxScanMac);
    bool add_selected_file_path(int which, QString &path);
    //bool update_download_mode(DOWNLOAD_MODE mode);
    // MAC地址相关
    bool update_MAC_locate(QString at);
    bool update_started_mac(QString at);

    bool update_MAC(int which, QString mac);
    void clean_MAC(int which);


    QString get_file_path(int which);
    QList<QString>* get_file_paths(void);
    QList<QVariant>* get_file_paths_pos(void);
    QList<QString>* get_interface(void);
    QVariant get_file_position(int which);
    QVariant get_mac_position(void);
    bool is_mac_addr_checked(void);
    bool is_scan_mac_checked(void);

    void SaveSettings(void);
    void ResetSettings(void);
    void LoadSettings(void);

private slots:
    void process_worker_report(int worker_id, QString cmd, QStringList list);

signals:
    // update job log to UI
    void report_flasher_state(int run_id, QString cmd, QStringList list);

private:
    // 线程池  最大数量为工人数
    QThreadPool mPool;
    Worker *mWorker[MAX_WORKER];
    QString mRunnerLog[MAX_WORKER];
    bool mResult[MAX_WORKER];

    // 非易失性存储配置
    QList<QString> mFilePath;
    QList<QVariant> mFilePosition;
    QVariant mMacPosition;
    bool mMacAddrChecked;
    bool mCheckBoxScanMac;

    // 临时配置
    QVariant mMac[MAX_DOWNLOAD_WORKER];
    QList<QString> mInterface;

};

#endif // CONTROLER_H

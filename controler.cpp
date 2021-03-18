#include "controler.h"
#include <QDebug>
#include <QSettings>
#include <QFileInfo>

Controler::Controler()
{
    for(int i=0;i<MAX_WORKER;i++){
        mWorker[i] = new Worker(&mPool);
    }

    mInterface.clear();

    mMacPosition = DEF_MAC_LOCATION;
}

void Controler::init(void)
{
    LoadSettings();
}

enum Controler::STATE_CTL Controler::get_current_mode(void)
{
    return STATE_IDEL;
}

bool Controler::refresh_interface(void)
{
    bool ret = false;
    QStringList list;
    // 刷新接口，使用固定的 SWD_RUNNER_ID 号 worker
    if(!mWorker[SWD_RUNNER_ID]->is_busy()){
        //清除日志
        mRunnerLog[SWD_RUNNER_ID].clear();
        mInterface.clear();
        FlasherRunnable* run = new FlasherRunnable(CMD_SWD, list, SWD_RUNNER_ID);
        connect(run,SIGNAL(report_flasher_state(int, QString, QStringList)), this, SLOT(process_worker_report(int, QString, QStringList)));
        mWorker[SWD_RUNNER_ID]->be_assigned_task(run);
        mWorker[SWD_RUNNER_ID]->start_work();
        ret = true;
    }

    return ret;
}

void Controler::start_auto_download(void)
{

}
void Controler::stop_auto_download(void)
{


}

bool Controler::check_download_condition(int which)
{
    bool ret = true;
    ret &= (which < MAX_DOWNLOAD_WORKER);
    ret &= (mFilePath.length() != 0);
    if(ret && mMacAddrChecked && mCheckBoxScanMac){
       ret &=  (mMac[which].toByteArray().length() == 12);
    }

    ret &= (mInterface.length() > which);
    return ret;
}

bool Controler::start_manual_download(int which)
{
    bool ret = false;
    // 检查下载条件是否满足
    ret = check_download_condition(which);

    if(ret){
        for(int i=0;i<mFilePath.length();i++){
            QStringList list;
            list.append("-SWD");
            list.append("-d");
            list.append(mInterface[which]);
            // 是否需要下载 mac 地址
            if(mMacAddrChecked && (i==0)){
                list.append("-erase");
                list.append("-mac");
                list.append("-mac_address");
                QString pos = "0x" + QString::number(mMacPosition.toInt(),16);
                list.append(pos);
                list.append("-mac_start");
                list.append("0x" + mMac[i].toString());
            }
            list.append("-f");
            list.append(mFilePath[i]);

            FlasherRunnable* run = new FlasherRunnable(CMD_FLASH, list, which);
            connect(run,SIGNAL(report_flasher_state(int, QString, QStringList)), this, SLOT(process_worker_report(int, QString, QStringList)));

            ret = mWorker[which]->be_assigned_task(run);
            // 失败则退出
            if(!ret){
                delete run;
                break;
            }
        }
        if(ret){
            mWorker[which]->start_work();
        }
    }

    return ret;
}

void Controler::erase_one(int which)
{
    QStringList list;
    list.append("-SWD");
    list.append("-d");
    list.append("0720000105d8ff313438504d43066627a5a5a5a597969908");
    FlasherRunnable* run = new FlasherRunnable(CMD_MASS_ERASE, list, 0);
    connect(run,SIGNAL(report_flasher_state(int, QString, QStringList)), this, SLOT(process_worker_report(int, QString, QStringList)));
    mWorker[0]->be_assigned_task(run);
    mWorker[0]->start_work();
}


void Controler::erase_all(void)
{


}
void Controler::read_device_firmware(void)
{

}
void Controler::test_fun(void)
{


}

void Controler::parse_run_log(int worker_id, QString cmd, QString &log, bool &result)
{
    result = false;
    if(worker_id == SWD_RUNNER_ID){
        // 用正则表达式提取当前正在使用的SW接口
        QRegExp rx("[0-9a-z]{48}");
        QStringList _textLineList = log.split("\n");
        //
        for(QStringList::ConstIterator _iter = _textLineList.begin(); _iter != _textLineList.end(); ++_iter)
        {
            if((rx.indexIn(*_iter) > -1)){
                QString str = rx.cap(0);
                mInterface.append(str);
                result = true;
            }
        }
    }

    if(QString::compare(cmd, CMD_FLASH) == 0){
        // 用正则表达式提取当前正在使用的SW接口
        QRegExp rx("Flash Programming: SUCCESS");
        QStringList _textLineList = log.split("\n");
        //
        for (QStringList::ConstIterator _iter = _textLineList.begin(); _iter != _textLineList.end(); ++_iter)
        {
            if((rx.indexIn(*_iter) > -1)){
                QString str = rx.cap(0);
                result = true;
            }
        }
    }

}

// 设置参数更新
bool Controler::update_MAC_check(bool checked)
{
    bool ret = true;
    mMacAddrChecked = checked;
    return ret;
}

bool Controler::update_scan_mac(bool checkBoxScanMac)
{
    bool ret = true;
    mCheckBoxScanMac = checkBoxScanMac;
    return ret;

}

bool Controler::add_selected_file_path(int which, QString &path)
{
    bool ret = false;
    // 新增文件路径
    if((which >= mFilePath.length()) && (path.length() > 0)){
        mFilePath.append(path);
        mFilePosition.append(QString::number(0x10040000,16));
        ret = true;
    }
    else if(which > 0){
        // 替换文件路径
        if(path.length() > 0){
            mFilePath[which] = path;
            ret = true;
        } // 移除
        else{
            mFilePath.removeAt(which);
            mFilePosition.removeAt(which);
        }
    }
    return ret;
}

bool Controler::update_MAC_locate(QString at)
{
    bool ret = false;
    bool ok;
    int addr = at.toInt(&ok,16);
    if(ok && (addr > _MEMORY_FLASH_BEGIN_) && (addr < _MEMORY_FLASH_END_) && ((addr %4)==0) ){
        mMacPosition = at;
        ret = true;
    }

    return ret;
}

bool Controler::update_started_mac(QString at)
{
    bool ret = true;
    mMacPosition = at;
    return ret;
}

bool Controler::update_MAC(int which, QString mac)
{
    bool ret = false;

    if( (mac.length() == 12) && (which < MAX_DOWNLOAD_WORKER) && (QString::compare(mMac[which].toString(), mac) != 0 ) ){
        ret = true;
        mMac[which] = mac;
        // 更新完后 启动一次下载
        start_manual_download(which);
    }
    return ret;
}

void Controler::clean_MAC(int which)
{
    if(which < MAX_DOWNLOAD_WORKER){
        mMac[which].clear();
    }
}

QString Controler::get_file_path(int which)
{
    return mFilePath[which];
}

QList<QString>* Controler::get_file_paths(void)
{
    return &mFilePath;
}

QList<QVariant>* Controler::get_file_paths_pos(void)
{
    return &mFilePosition;
}

QList<QString>* Controler::get_interface(void)
{
    return &mInterface;
}


QVariant Controler::get_file_position(int which)
{
    return mFilePosition[which];
}

QVariant Controler::get_mac_position(void)
{
    return mMacPosition;
}
bool Controler::is_mac_addr_checked(void)
{
    return mMacAddrChecked;
}

bool Controler::is_scan_mac_checked(void)
{
    return mCheckBoxScanMac;
}


void Controler::SaveSettings(void)
{
    //Save
    QSettings setting("MyCompany","myapp");
    setting.beginGroup("MainWindow");

    QString keys[MAX_FILE_SELCECT_NUM] = {"mFilePath[0]","mFilePath[1]","mFilePath[2]","mFilePath[3]","mFilePath[4]"};
    QString pos_keys[MAX_FILE_SELCECT_NUM] = {"mFilePosition[0]","mFilePosition[1]","mFilePosition[2]","mFilePosition[3]","mFilePosition[4]"};
    for(int i=0;i<MAX_FILE_SELCECT_NUM;i++){
        if(i<mFilePath.length()){
            setting.setValue(keys[i], mFilePath[i]);
            setting.setValue(pos_keys[i], mFilePosition[i]);
        }
        else{
            setting.setValue(keys[i], "");
            setting.setValue(pos_keys[i], "");
        }
    }

    setting.setValue("mMacPosition", mMacPosition);
    setting.setValue("mMacAddrChecked", mMacAddrChecked);
    setting.setValue("mCheckBoxScanMac", mCheckBoxScanMac);
    setting.endGroup();
    qDebug() << "Saved";
}
void Controler::ResetSettings(void)
{
    mFilePath.clear();

    mFilePosition.clear();

    mMacPosition = DEF_MAC_LOCATION;
    mMacAddrChecked = false;
    mCheckBoxScanMac = false;
}

void Controler::LoadSettings(void)
{
    //Load
    QSettings setting("MyCompany","myapp");
    setting.beginGroup("MainWindow");

    //
    QString keys[MAX_FILE_SELCECT_NUM] = {"mFilePath[0]","mFilePath[1]","mFilePath[2]","mFilePath[3]","mFilePath[4]"};
    QString pos_keys[MAX_FILE_SELCECT_NUM] = {"mFilePosition[0]","mFilePosition[1]","mFilePosition[2]","mFilePosition[3]","mFilePosition[4]"};
    for(int i=0;i<MAX_FILE_SELCECT_NUM;i++){
        QString path = setting.value(keys[i]).toString();
        QString filePosition = setting.value(pos_keys[i]).toString();
        if(path.length() > 0){
            QFileInfo file(path);
            if(file.exists()==true){
                // 增加一个文件
                mFilePath.append(path);
                mFilePosition.append(filePosition);
            }
        }
    }

    mMacPosition = setting.value("mMacPosition");
    mMacAddrChecked = setting.value("mMacAddrChecked").toBool();
    mCheckBoxScanMac = setting.value("mCheckBoxScanMac").toBool();

    setting.endGroup();

    qDebug() << "Loaded";
}

void Controler::process_worker_report(int worker_id, QString cmd, QStringList list)
{
    // 刷新日志  list[0] 是日志
    //OnRefresh_log(list[0]);
    mRunnerLog[worker_id].append(list[0]);
    qDebug() << mRunnerLog[worker_id];

    // runnable 结束判断
    if( (list.length() >= 2) && (QString::compare(list[1], "finish") == 0 )){
        //解析进度
        bool result = false;
        parse_run_log(worker_id, cmd, mRunnerLog[worker_id], result);
        // 开始下一个task
        uint8_t progress = 0;
        mWorker[worker_id]->start_next_task(progress);

        if(QString::compare(cmd, CMD_FLASH) == 0){
            list.append(QString::number(progress));
            list.append(QString::number(result));
        }
    }

    emit report_flasher_state(worker_id,cmd, list);
}

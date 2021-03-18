#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QThread>
#include <QFileDialog>
#include <QSettings>
#include "app_const_cfg.h"
#include "flasherrunnable.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    mControler = new Controler();
    connect(mControler, SIGNAL(report_flasher_state(int, QString, QStringList)), this, SLOT(refresh_ui_slots(int, QString, QStringList)));
    mControler->init();
    mControler->moveToThread(mControler);
    mControler->start();

    // limiting input
    QRegExp regx("[A-Fa-f0-9]{8}");
    QValidator *validator = new QRegExpValidator(regx, ui->lineEditMacPosition);
    ui->lineEditMacPosition->setValidator(validator);
    // 限制手工输入mac地址
    QRegExp regx2("[A-Fa-f0-9]{12}");
    QValidator *validator2 = new QRegExpValidator(regx2, ui->lineEditMac1);
    ui->lineEditMac1->setValidator(validator2);
    ui->lineEditMac2->setValidator(validator2);
    ui->lineEditMac3->setValidator(validator2);
    ui->lineEditMac4->setValidator(validator2);

    QValidator *validatorDesAddr1 = new QRegExpValidator(regx, ui->lineEditDesAddr1);
    ui->lineEditDesAddr1->setValidator(validatorDesAddr1);
    ui->lineEditDesAddr2->setValidator(validatorDesAddr1);
    ui->lineEditDesAddr3->setValidator(validatorDesAddr1);
    ui->lineEditDesAddr4->setValidator(validatorDesAddr1);
    ui->lineEditDesAddr5->setValidator(validatorDesAddr1);

    ui->textBrowserLog->document()->setMaximumBlockCount(300);
    initShow();


    on_refreshInterfaceButton_clicked();
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_startAutoBtn_clicked()
{

}

void MainWindow::OnRefresh_log(QString log)
{
    ui->textBrowserLog->append(log);
    ui->textBrowserLog->moveCursor(QTextCursor::End);
}

void MainWindow::uiProcess(int pos, int processVal)
{
    QProgressBar* progressBars[] = {ui->progressBar_1, ui->progressBar_2,
                                   ui->progressBar_3, ui->progressBar_4};

    progressBars[pos]->setValue(processVal);
    progressBars[pos]->setStyleSheet("QProgressBar::chunk{background:green}");
}

void MainWindow::uiProcess(int pos, int processVal, bool result)
{
    QProgressBar* progressBars[] = {ui->progressBar_1, ui->progressBar_2,
                                   ui->progressBar_3, ui->progressBar_4};

    progressBars[pos]->setValue(processVal);
    if(result){
        progressBars[pos]->setStyleSheet("QProgressBar::chunk{background:green}");
    }
    else{
        progressBars[pos]->setStyleSheet("QProgressBar::chunk{background:red}");
    }
}

void MainWindow::uiInterface(void)
{
    QComboBox * comboxs[] = {ui->comboBoxInterface1,
                             ui->comboBoxInterface2,
                             ui->comboBoxInterface3,
                             ui->comboBoxInterface4,
                            };

    QList<QString>* list = mControler->get_interface();

    for(int i=0;i<4;i++){
        if(i<list->length()){
            comboxs[i]->setItemText(0,list->value(i));
            comboxs[i]->setCurrentIndex(0);
        }
        else{
            comboxs[i]->setItemText(0,"");
            comboxs[i]->setCurrentIndex(0);
        }

    }

}

//
void MainWindow::refresh_ui_slots(int run_id, QString cmd, QStringList list)
{
    // CMD_FLASH
    if(list.length()>3 && (QString::compare(list[1], "finish") == 0 ) && QString::compare(cmd, CMD_FLASH) == 0){
        bool result = list[3].toInt();
        uiProcess(run_id,list[2].toInt(), result);
    }
    // CMD_CMD_SWD
    if(list.length()>1 && (QString::compare(list[1], "finish") == 0 ) && QString::compare(cmd, CMD_SWD) == 0){
        uiInterface();
    }

    OnRefresh_log(list[0]);
}

void MainWindow::on_pushButtonSelect(int pos)
{
    QLabel* selectPath[] = {ui->labelSelectPath1,
                            ui->labelSelectPath2,
                            ui->labelSelectPath3,
                            ui->labelSelectPath4,
                            ui->labelSelectPath5};

    if(pos >= 0 && pos < MAX_FILE_SELCECT_NUM){
        QString file_name = QFileDialog::getOpenFileName(NULL,"File",".","*.hex *.bin");
        if(mControler->add_selected_file_path(pos, file_name)){
            selectPath[pos]->setText(file_name);
            mControler->SaveSettings();
        }
    }
}

void MainWindow::refresh_mac_ui()
{
    if( mControler->is_mac_addr_checked()){
        ui->lineEditMacPosition->setEnabled(true);
        ui->checkBoxScanMac->setEnabled(true);
        if(!ui->checkBoxScanMac->isChecked()){
            ui->lineEditMacStartAddr->setEnabled(true);
            ui->lineEditMac1->setEnabled(false);
            ui->lineEditMac2->setEnabled(false);
            ui->lineEditMac3->setEnabled(false);
            ui->lineEditMac4->setEnabled(false);
        }
        else{
            ui->lineEditMacStartAddr->setEnabled(false);
            ui->lineEditMac1->setEnabled(true);
            ui->lineEditMac2->setEnabled(true);
            ui->lineEditMac3->setEnabled(true);
            ui->lineEditMac4->setEnabled(true);
        }
    }else{
        ui->lineEditMac1->setEnabled(false);
        ui->lineEditMac2->setEnabled(false);
        ui->lineEditMac3->setEnabled(false);
        ui->lineEditMac4->setEnabled(false);
        ui->checkBoxScanMac->setCheckState(Qt::Unchecked);
        if(mControler->is_scan_mac_checked()){
            mControler->update_scan_mac(false);
            //SaveSettings();
        }
        ui->lineEditMacPosition->setEnabled(false);
        ui->checkBoxScanMac->setEnabled(false);
        ui->lineEditMacStartAddr->setEnabled(false);
    }
}

void MainWindow::refresh_downloadfile_ui()
{
    QLabel* lableSelectPaths[] = {ui->labelSelectPath1,
                                ui->labelSelectPath2,
                                ui->labelSelectPath3,
                                ui->labelSelectPath4,
                                ui->labelSelectPath5};

    QLineEdit* lineEditDesAddrs[] = {ui->lineEditDesAddr1,
                                     ui->lineEditDesAddr2,
                                     ui->lineEditDesAddr3,
                                     ui->lineEditDesAddr4,
                                     ui->lineEditDesAddr5,};

    QList<QVariant>* paths_pos = mControler->get_file_paths_pos();

    for(int i=0;i<MAX_FILE_SELCECT_NUM;i++){
        if( i < mControler->get_file_paths()->length()){
            lableSelectPaths[i]->setText(mControler->get_file_path(i));
            QFileInfo fileinfo = QFileInfo(mControler->get_file_path(i));
            if(fileinfo.suffix() == "bin"){
                lineEditDesAddrs[i]->setEnabled(true);
                lineEditDesAddrs[i]->setText(paths_pos->value(i).toString());
            }
            else{
                lineEditDesAddrs[i]->setEnabled(false);
                lineEditDesAddrs[i]->setText("");
            }
        }
        else{
            lableSelectPaths[i]->setText("没有选中任何文件");
           lineEditDesAddrs[i]->setEnabled(false);
           lineEditDesAddrs[i]->setText("");
        }

    }
}

void MainWindow::initShow(void)
{
    QProgressBar* progeressBars[] = {ui->progressBar_1,
                                     ui->progressBar_2,
                                     ui->progressBar_3,
                                     ui->progressBar_4};

    for(int i=0;i<4;i++){
       progeressBars[i]->setValue(0);
    }

    refresh_downloadfile_ui();

    ui->checkBoxMacAddr->setChecked(mControler->is_mac_addr_checked());
    ui->checkBoxScanMac->setChecked(mControler->is_scan_mac_checked());
    // 16进制显示
    QString pos = QString::number(mControler->get_mac_position().toInt() , 16);
    ui->lineEditMacPosition->setText(pos);

    ui->comboBoxInterface1->setDisabled(true);
    ui->comboBoxInterface2->setDisabled(true);
    ui->comboBoxInterface3->setDisabled(true);
    ui->comboBoxInterface4->setDisabled(true);

    refresh_mac_ui();
}


void MainWindow::ResetSettings()
{
    mControler->ResetSettings();
    mControler->init();
    initShow();

}


void MainWindow::on_pushButtonSelect1_clicked()
{
    on_pushButtonSelect(0);
}

void MainWindow::on_pushButtonSelect2_clicked()
{
    on_pushButtonSelect(1);
}

void MainWindow::on_pushButtonSelect3_clicked()
{
    on_pushButtonSelect(2);
}

void MainWindow::on_pushButtonSelect4_clicked()
{
    on_pushButtonSelect(3);
}

void MainWindow::on_pushButtonSelect5_clicked()
{
    on_pushButtonSelect(4);
}



void MainWindow::refreshInterface()
{
    mControler->test_fun();
}

void MainWindow::on_refreshInterfaceButton_clicked()
{

    mControler->refresh_interface();
}

void MainWindow::on_actionSave_configures_triggered()
{
    mControler->SaveSettings();
}

void MainWindow::on_actionClear_configures_triggered()
{
    mControler->ResetSettings();
    mControler->SaveSettings();
    mControler->init();
    initShow();
}

void MainWindow::on_checkBoxMacAddr_stateChanged(int arg1)
{
    bool checkBoxMacAddr = false;
    if(arg1 == 0)
        checkBoxMacAddr = false;
    else
        checkBoxMacAddr = true;

    if(mControler->update_MAC_check(checkBoxMacAddr)){
        refresh_mac_ui();
        mControler->SaveSettings();
    }
}

void MainWindow::on_lineEditMacPosition_editingFinished()
{
    bool ret = mControler->update_MAC_locate(ui->lineEditMacPosition->text());
    if(ret){
        qDebug()<<"on_lineEditMacPosition_editingFinished";
        mControler->SaveSettings();
    }
    else{
        QString pos = QString::number(mControler->get_mac_position().toInt() , 16);
        ui->lineEditMacPosition->setText(pos);
    }
}



void MainWindow::on_start1Button_clicked()
{
    if(mControler->start_manual_download(0)){
        uiProcess(0, DEF_STARTED_PROGRESS);
    }
}

void MainWindow::on_pushButtonClearLog_clicked()
{
    ui->textBrowserLog->clear();
}

void MainWindow::on_start2Button_clicked()
{
    if(mControler->start_manual_download(1)){
        uiProcess(1, DEF_STARTED_PROGRESS);
    }
}

void MainWindow::on_checkBoxScanMac_stateChanged(int arg1)
{
    bool check = false;
    if(arg1 == 0)
        check = false;
    else
        check = true;

    if(mControler->update_scan_mac(check)){
        refresh_mac_ui();
        mControler->SaveSettings();
    }
}

void MainWindow::on_start3Button_clicked()
{
    if(mControler->start_manual_download(2)){
        uiProcess(2,DEF_STARTED_PROGRESS);
    }
}

void MainWindow::on_lineEditMacs_editingFinished(int which)
{
    if(which < 4){
        QLineEdit *lineEdits[] = {ui->lineEditMac1,ui->lineEditMac2,ui->lineEditMac3,ui->lineEditMac4};
        if(lineEdits[which]->text().length() == 0){
            mControler->clean_MAC(which);
        }
        else{
            bool ret = mControler->update_MAC(which,lineEdits[which]->text());
            if(ret){
                lineEdits[which]->clearFocus();
                uiProcess(which,DEF_STARTED_PROGRESS);
            }
        }
    }
}

void MainWindow::on_lineEditMac1_editingFinished()
{
    on_lineEditMacs_editingFinished(0);
}

void MainWindow::on_lineEditMac1_selectionChanged()
{
    qDebug("on_lineEditMac1_selectionChanged");
}



void MainWindow::on_lineEditMac1_cursorPositionChanged(int arg1, int arg2)
{
    qDebug()<<"on_lineEditMac1_cursorPositionChanged"<<arg1<<arg2;
}

void MainWindow::on_start4Button_clicked()
{
    if(mControler->start_manual_download(3)){
        uiProcess(3,DEF_STARTED_PROGRESS);
    }
}

void MainWindow::on_lineEditMac2_editingFinished()
{
    on_lineEditMacs_editingFinished(1);
}

void MainWindow::on_lineEditMac3_editingFinished()
{
    on_lineEditMacs_editingFinished(2);
}

void MainWindow::on_lineEditMac4_editingFinished()
{
    on_lineEditMacs_editingFinished(3);
}

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>
#include <QStringList>
#include <QThreadPool>
#include "controler.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private:
    void on_pushButtonSelect(int pos);
    void initShow(void);
    void ResetSettings();
    void uiProcess(int pos, int processVal);
    void uiProcess(int pos, int processVal, bool result);
    void refreshInterface();
    void uiInterface(void);
    void refresh_mac_ui();
    void refresh_downloadfile_ui();

signals:
    void flasher_signals(QString cmd, QStringList list);

private:
    void on_lineEditMacs_editingFinished(int which);

private slots:
    void on_startAutoBtn_clicked();
    void OnRefresh_log(QString log);

    void on_pushButtonSelect1_clicked();

    void on_pushButtonSelect2_clicked();

    void on_pushButtonSelect3_clicked();

    void on_pushButtonSelect4_clicked();

    void on_pushButtonSelect5_clicked();

    void on_refreshInterfaceButton_clicked();

    void refresh_ui_slots(int run_id, QString cmd, QStringList list);

    void on_actionSave_configures_triggered();

    void on_actionClear_configures_triggered();

    void on_checkBoxMacAddr_stateChanged(int arg1);

    void on_lineEditMacPosition_editingFinished();

    void on_start1Button_clicked();

    void on_pushButtonClearLog_clicked();

    void on_start2Button_clicked();

    void on_checkBoxScanMac_stateChanged(int arg1);

    void on_start3Button_clicked();

    void on_lineEditMac1_editingFinished();

    void on_lineEditMac1_selectionChanged();

    void on_lineEditMac1_cursorPositionChanged(int arg1, int arg2);

    void on_start4Button_clicked();

    void on_lineEditMac2_editingFinished();

    void on_lineEditMac3_editingFinished();

    void on_lineEditMac4_editingFinished();


private:
    Ui::MainWindow *ui;

    Controler*   mControler;
};
#endif // MAINWINDOW_H

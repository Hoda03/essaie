#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "transmission_ratio.h"
#include "version.h"
#include <QUdpSocket>
#include <QTcpSocket>
#include <QMainWindow>
#include <QTimer>
#include "sqp.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionPARAM_REQ_triggered();

    void on_actionPARAM_REQ_2_triggered();

    void on_actionUNIT_DATA_REQ_triggered();

    void on_send_clicked();

    void on_actionPWR_SET_REQ_triggered();

    void on_actionMODE_SET_REQ_triggered();

    void on_actionPURGE_RQ_triggered();

    void on_actionMODE_SET_REQ_2_triggered();

    void start_timer();

    void on_STOP_clicked();

    void on_sqp_clicked();

    void on_TRANSMISSION_RATIO_clicked();

    void on_actionVersion_triggered();

    void on_clear_clicked();

    void on_close_clicked();

private:
    Ui::MainWindow *ui;
    QUdpSocket *socketsqp1;
    QUdpSocket *socketTR;
    version version;
    QTcpSocket *socketTcp;
    QTcpSocket *socketTcp1;
    QTimer *timer;
    SQP sqp;
    transmission_ratio transmission_ratio;

};
#endif // MAINWINDOW_H

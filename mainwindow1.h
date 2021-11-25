#ifndef MAINWINDOW1_H
#define MAINWINDOW1_H
#include "sqp1.h"
#include "transmission_ratio1.h"
#include "version.h"
#include <QMainWindow>
#include <QTimer>
#include <QUdpSocket>
#include <QTcpSocket>

#define SIZE_SYNCHRO_VDL2     4
const uint8_t synchrovdl2[SIZE_SYNCHRO_VDL2] = {0xff,0x00,0xf0,0xca};

struct tsPidF0{
    uint8_t synchro[SIZE_SYNCHRO_VDL2];
    uint8_t protocoleCode;
    uint8_t pid;
    uint8_t lengthMsb;
    uint8_t lengthLsb;
    uint8_t mode;
    uint8_t req;
    uint16_t crc;

};
struct tsPid20{
    uint8_t synchro[SIZE_SYNCHRO_VDL2];
    uint8_t mode;
    uint8_t pid;
    uint8_t dfl1_msb;
    uint8_t dfl2_lsb;
    uint8_t freq_msb;
    uint8_t freq_lsb;
    uint8_t tm1;
    uint8_t tm2;
    uint8_t M1_msb;
    uint8_t M1_lsb;
    uint8_t persistance;
    uint16_t crc;


};

namespace Ui {
class MainWindow1;
}

class MainWindow1 : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow1(QWidget *parent = nullptr);
    ~MainWindow1();

private slots:
    void on_actionPARAM_REQ_triggered();

    void on_actionPARAM_REQ_2_triggered();

    void on_UINT_DATA_REQ_clicked();

    void on_actionPWR_SET_REQ_triggered();

    void on_actionMODE_SET_REQ_triggered();

    void on_actionMODE_SET_REQ_2_triggered();

    void on_actionPURGE_REQ_triggered();

    void on_pushButton_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_2_clicked();

    void start_timer1();

    void on_sqp_clicked();

    void on_pushButton_4_clicked();

    void on_actionVersion_triggered();

    void on_pushButton_5_clicked();

    void on_clear_clicked();


private:

    Ui::MainWindow1 *ui;
    QUdpSocket *socketUdp1;
    QUdpSocket *socketUdp1tr1;
    version version;
    QTcpSocket *socketTcp1;
    QTcpSocket *socketTcp1r;
    QTimer *timer1;
    SQP1 sqp1;
    transmission_ratio1 transmission_ratio1;
    QByteArray arr1;

};

#endif // MAINWINDOW1_H

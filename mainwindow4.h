#ifndef MAINWINDOW4_H
#define MAINWINDOW4_H
#include "version.h"

#include <QMainWindow>
#include <QUdpSocket>
#include <QTimer>
#include "sqp4.h"
#include "transmission_ratio4.h"

#define SIZE_SYNCHRO_VDL2     4


struct tsPid20vdl {
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

struct tsPidF0vdl{
    uint8_t synchro[SIZE_SYNCHRO_VDL2];
    uint8_t protocoleCode;
    uint8_t pid;
    uint8_t lengthMsb;
    uint8_t lengthLsb;
    uint8_t mode;
    uint8_t req;
    uint16_t crc;

};
struct tsPidF1vdl {
    uint8_t synchro[SIZE_SYNCHRO_VDL2];
    uint8_t protocoleCode;
    uint8_t pid;
    uint8_t lengthMsb;
    uint8_t lengthLsb;
    uint8_t pwr;
    uint8_t req;
    uint16_t crc;

};
struct tsPid010bvdl {
    uint8_t synchro[SIZE_SYNCHRO_VDL2];
    uint8_t sal;
    uint8_t donnees;
    uint8_t pid;
    uint8_t protocoelCode;
    uint16_t crc;

};
namespace Ui {
class MainWindow4;
}

class MainWindow4 : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow4(QWidget *parent = nullptr);
    ~MainWindow4();

private slots:
    void on_actionPARAM_REQ_triggered();

    void on_actionPARAM_REQ_2_triggered();

    void on_pushButton_clicked();

    void on_actionPWR_SET_REQ_triggered();

    void on_actionMODE_SET_REQ_triggered();

    void on_actionPURGE8req_triggered();

    void on_actionMODE_SET_REQ_2_triggered();

    void on_pushButton_4_clicked();

    void start_timer4();

    void on_sqp_clicked();

    void on_pushButton_3_clicked();

    void on_transmission_ratio_clicked();

    void on_version_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::MainWindow4 *ui;
    QUdpSocket *sockUdp;
    QUdpSocket *sockUdpRecepteur;

    QUdpSocket *sockUdptr;
    sqp4 sqp4;
    QUdpSocket *msock;
    QTimer *timer4;
    transmission_ratio4 transmission_ratio4;
    version version;
    QByteArray arr;

};

#endif // MAINWINDOW4_H

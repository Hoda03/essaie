#ifndef TRANSMISSION_RATIO1_H
#define TRANSMISSION_RATIO1_H

#include <QMainWindow>
#include <QMainWindow>
#include <QUdpSocket>
namespace Ui {
class transmission_ratio1;
}

class transmission_ratio1 : public QMainWindow
{
    Q_OBJECT

public:
    explicit transmission_ratio1(QWidget *parent = nullptr);
    ~transmission_ratio1();

private:
    Ui::transmission_ratio1 *ui;
    QHostAddress sender;
    quint16 senderPort;
    QUdpSocket *mSocketTR1;
    int value;

};

#endif // TRANSMISSION_RATIO1_H

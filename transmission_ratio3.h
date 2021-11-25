#ifndef TRANSMISSION_RATIO3_H
#define TRANSMISSION_RATIO3_H

#include <QMainWindow>
#include <QUdpSocket>

namespace Ui {
class transmission_ratio3;
}

class transmission_ratio3 : public QMainWindow
{
    Q_OBJECT

public:
    explicit transmission_ratio3(QWidget *parent = nullptr);
    ~transmission_ratio3();

private:
    Ui::transmission_ratio3 *ui;
    QHostAddress sender;
    quint16 senderPort;
    QUdpSocket *mSocketTR3;
};

#endif // TRANSMISSION_RATIO3_H

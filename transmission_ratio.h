#ifndef TRANSMISSION_RATIO_H
#define TRANSMISSION_RATIO_H

#include <QMainWindow>
#include <QUdpSocket>

namespace Ui {
class transmission_ratio;
}

class transmission_ratio : public QMainWindow
{
    Q_OBJECT

public:
    explicit transmission_ratio(QWidget *parent = nullptr);
    ~transmission_ratio();

private:
    Ui::transmission_ratio *ui;
    QHostAddress sender;
    quint16 senderPort;
    QUdpSocket *mSocketTR;
};

#endif // TRANSMISSION_RATIO_H

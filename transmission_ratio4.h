#ifndef TRANSMISSION_RATIO4_H
#define TRANSMISSION_RATIO4_H

#include <QMainWindow>
#include <QUdpSocket>

namespace Ui {
class transmission_ratio4;
}

class transmission_ratio4 : public QMainWindow
{
    Q_OBJECT

public:
    explicit transmission_ratio4(QWidget *parent = nullptr);
    ~transmission_ratio4();

private:
    Ui::transmission_ratio4 *ui;
    QHostAddress sender;
    quint16 senderPort;
    QUdpSocket *mSocketW;
};

#endif // TRANSMISSION_RATIO4_H

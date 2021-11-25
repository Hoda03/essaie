#ifndef SQP4_H
#define SQP4_H

#include <QMainWindow>
#include <QtWidgets>
#include <QtCharts>
#include <QHostAddress>
#include <QUdpSocket>
constexpr size_t sizen =100;

namespace Ui {
class sqp4;
}

class sqp4 : public QMainWindow
{
    Q_OBJECT

public:
    explicit sqp4(QWidget *parent = nullptr);
    ~sqp4();

private:
    Ui::sqp4 *ui;
    QUdpSocket *mSocketh;

    QHostAddress sender;
    quint16 senderPort;
    QChartView *graphique; // un widget pour afficher un graphe
    QChart *graphe; // la représentation d'un graphe
    QLineSeries *courbe; // les données

private:
    int numbers[sizen] {0};
    int counter=0;
};

#endif // SQP4_H

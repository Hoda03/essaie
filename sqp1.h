#ifndef SQP1_H
#define SQP1_H

#include <QMainWindow>
#include <QtWidgets>
#include <QtCharts>
#include <QHostAddress>
#include <QUdpSocket>
constexpr size_t sizet =100;

namespace Ui {
class SQP1;
}

class SQP1 : public QMainWindow
{
    Q_OBJECT

public:
    explicit SQP1(QWidget *parent = nullptr);
    ~SQP1();

private:
    Ui::SQP1 *ui;
    QUdpSocket *mSockett;
    QHostAddress sender;
    quint16 senderPort;
    QChartView *graphique; // un widget pour afficher un graphe
    QChart *graphe; // la représentation d'un graphe
    QLineSeries *courbe; // les données
    int numbers[sizet] {0};
    int counter=0;
    int vale;
    int hd;
};

#endif // SQP1_H

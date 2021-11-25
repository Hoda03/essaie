#ifndef SQP_H
#define SQP_H

#include <QMainWindow>
#include <QtWidgets>
#include <QtCharts>
#include <QHostAddress>
#include <QUdpSocket>

constexpr size_t sizez =100;

namespace Ui {
class SQP;
}

class SQP : public QMainWindow
{
    Q_OBJECT

public:
    explicit SQP(QWidget *parent = nullptr);
    ~SQP();

private:
    Ui::SQP *ui;
    QUdpSocket *mSocketz;

    QHostAddress sender;
    quint16 senderPort;
    QChartView *graphique; // un widget pour afficher un graphe
    QChart *graphe; // la reprÃ©sentation d'un graphe
    QLineSeries *courbe; // les donnÃ©es
    int numbers[sizez] {0};
    int counter=0;
};

#endif // SQP_H

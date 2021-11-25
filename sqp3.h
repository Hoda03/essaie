#ifndef SQP3_H
#define SQP3_H

#include <QMainWindow>
#include <QtWidgets>
#include <QtCharts>
#include <QHostAddress>
#include <QUdpSocket>
constexpr size_t sizex =100;
namespace Ui {
class SQP3;
}

class SQP3 : public QMainWindow
{
    Q_OBJECT

public:
    explicit SQP3(QWidget *parent = nullptr);
    ~SQP3();

private:
    Ui::SQP3 *ui;
    QUdpSocket *mSocketx;

    QHostAddress sender;
    quint16 senderPort;
    QChartView *graphique; // un widget pour afficher un graphe
    QChart *graphe; // la reprÃ©sentation d'un graphe
    QLineSeries *courbe; // les donnÃ©es
    int numbers[sizex] {0};
    int counter=0;

};

#endif // SQP3_H

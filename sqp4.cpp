#include "sqp4.h"
#include "ui_sqp4.h"

sqp4::sqp4(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::sqp4)
{
    ui->setupUi(this);
    this->setWindowTitle("Signal Quality");

    mSocketh = new QUdpSocket(this);
    mSocketh->bind(QHostAddress::LocalHost,1478);
    connect(mSocketh,&QUdpSocket::readyRead,[&](){
        while (mSocketh ->hasPendingDatagrams())
        {
            QByteArray data;
            QHostAddress sender;
            quint16 senderPort;
            data.resize(mSocketh->pendingDatagramSize());
            mSocketh->readDatagram(data.data(),data.size(),&sender, &senderPort);
            if( data[5] == 84){
                int hd = (int)data[8];
                if (counter > 10 ){
                    for (int i=0; i<9; i++){
                        numbers[i]= numbers[i+1];
                    }
                    numbers[9] = hd;
                }
                else {
                    numbers[counter] = hd;
                }
                counter++;
                QBarSet *set0 = new QBarSet("Signal Quality");
                *set0 << numbers[0]<< numbers[1]<< numbers[2]<< numbers[3]<< numbers[4]<< numbers[5]<< numbers[6]<< numbers[7]<< numbers[8]<< numbers[9] ;
                QBarSeries *series = new QBarSeries();
                series->append(set0);
                graphe = new QChart();
                graphe->setTitle("Signal Quality graph");
                graphe->addSeries(series);
                QBarCategoryAxis *axisX = new QBarCategoryAxis();
                graphe->addAxis(axisX, Qt::AlignBottom);
                series->attachAxis(axisX);
                QValueAxis *axisY = new QValueAxis();
                axisY->setRange(0,15);
                graphe->addAxis(axisY, Qt::AlignLeft);
                series->attachAxis(axisY);
                graphique = new QChartView;
                graphique->setRenderHint(QPainter::Antialiasing);
                graphique->setChart(graphe);
                setCentralWidget(graphique);
                resize(640, 480);
            }
       }

});
}

sqp4::~sqp4()
{
    delete ui;
}

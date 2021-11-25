#include "sqp3.h"
#include "ui_sqp3.h"
/*--------Dans cette partie on va créer un histogramme qui va afficher la valeur du qualité du signal recue dans la trame sqp_ind  en acars udp-----*/

SQP3::SQP3(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SQP3)
{
    ui->setupUi(this);
    this->setWindowTitle("Signal Quality");

    // création du socket pour se communiquer en localhost pour la réception de la trame sqp_ind
    mSocketx = new QUdpSocket(this);
    mSocketx->bind(QHostAddress::LocalHost,1236);

    //Fonction connect de se connecter et lire la trame sqp

    connect(mSocketx,&QUdpSocket::readyRead,[&](){
          while (mSocketx ->hasPendingDatagrams())
          {
              QByteArray data;
              QHostAddress sender;
              quint16 senderPort;
              data.resize(mSocketx->pendingDatagramSize());
              mSocketx->readDatagram(data.data(),data.size(),&sender, &senderPort);

              if( data[5] == 32 && data[4] == 95){
                  int hd = (int)data[6];
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

              for(int i=0;i<10; i++){
             // *set0 << numbers[0]<< numbers[1]<< numbers[2]<< numbers[3]<< numbers[4]<< numbers[5]<< numbers[6]<< numbers[7]<< numbers[8]<< numbers[9]<< numbers[10] ;
               *set0 << numbers[i];
              }
              QBarSeries *series = new QBarSeries();
              series->append(set0);

              graphe = new QChart();
              graphe->setTitle("Signal Quality graph");
              graphe->addSeries(series);


              QBarCategoryAxis *axisX = new QBarCategoryAxis();
              graphe->addAxis(axisX, Qt::AlignBottom);
              series->attachAxis(axisX);


              QValueAxis *axisY = new QValueAxis();
              axisY->setRange(0,255);
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

SQP3::~SQP3()
{
    delete ui;
}

#include "sqp.h"
#include "ui_sqp.h"


/*--------Dans cette partie on va créer un histogramme qui va afficher la valeur du qualité du signal recue dans la trame sqp_ind en acars tcp-----*/
SQP::SQP(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SQP)
{
    ui->setupUi(this);
    this->setWindowTitle("Signal Quality");

    // création du socket pour se communiquer en localhost pour la réception de la trame sqp_ind
    mSocketz = new QUdpSocket(this);
    mSocketz->bind(QHostAddress::LocalHost,1999);

    //Fonction connect de se connecter et lire la trame sqp
    connect(mSocketz,&QUdpSocket::readyRead,[&](){
          while (mSocketz ->hasPendingDatagrams())
          {
              QByteArray data;
              QHostAddress sender;
              quint16 senderPort;
              data.resize(mSocketz->pendingDatagramSize());
              mSocketz->readDatagram(data.data(),data.size(),&sender, &senderPort);

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
              axisY->setRange(0,511);
              graphe->addAxis(axisY, Qt::AlignLeft);
              series->attachAxis(axisY);

              graphique = new QChartView;
              graphique->setRenderHint(QPainter::Antialiasing);
              graphique->setChart(graphe);
              setCentralWidget(graphique);
              resize(640, 480);


  }

  });
}

SQP::~SQP()
{
    delete ui;
}

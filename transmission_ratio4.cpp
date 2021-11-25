#include "transmission_ratio4.h"
#include "ui_transmission_ratio4.h"

constexpr size_t sizer = 16000;
int bff1[sizer] {0};
int bff2[sizer] {0};
bool isBff1End {false};
bool isBff2End {false};

#include <QMainWindow>
#include <QWidget>
#include <QUdpSocket>

int counter1 =0;
int counter2 =0;
int counter3 =0;

transmission_ratio4::transmission_ratio4(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::transmission_ratio4)
{
    ui->setupUi(this);
    mSocketW = new QUdpSocket(this);
    mSocketW->bind(QHostAddress::LocalHost,5555);
    this->setWindowTitle("Bit Error Rate Test");
    this->setStyleSheet("QWidget { background-color : #ffffff}");

    ui->label_4->setFrameStyle(QFrame::StyledPanel | QFrame::Raised);
    ui->label_4->setStyleSheet("QLabel { background-color : white; color : #0078d7; }");
    ui-> label_4->setLineWidth(3);
    ui-> label_4->setMidLineWidth(3);

    ui->label_3v1_2->setFrameStyle(QFrame::StyledPanel | QFrame::Raised);
    ui-> label_3v1_2->setLineWidth(3);
    ui-> label_3v1_2->setMidLineWidth(3);

    ui->label_5v1_2->setFrameStyle(QFrame::StyledPanel | QFrame::Raised);
    ui-> label_5v1_2->setLineWidth(3);
    ui-> label_5v1_2->setMidLineWidth(3);

    ui->label_7v1_2->setFrameStyle(QFrame::StyledPanel | QFrame::Raised);
    ui-> label_7v1_2->setLineWidth(3);
    ui-> label_7v1_2->setMidLineWidth(3);

    ui->label_4v1_2->setFrameStyle(QFrame::StyledPanel | QFrame::Raised);
    ui-> label_4v1_2->setLineWidth(3);
    ui-> label_4v1_2->setMidLineWidth(3);

    ui->label_8v1_2->setFrameStyle(QFrame::StyledPanel | QFrame::Raised);
    ui-> label_8v1_2->setLineWidth(3);
    ui-> label_8v1_2->setMidLineWidth(3);

    ui->label_6v1_2->setFrameStyle(QFrame::StyledPanel | QFrame::Raised);
    ui-> label_6v1_2->setLineWidth(3);
    ui-> label_6v1_2->setMidLineWidth(3);


    ui->label_4v1_2->setNum(0);
    ui->label_8v1_2->setNum(0);
    ui->label_6v1_2->setNum(0);

    connect(mSocketW,&QUdpSocket::readyRead,[&](){
        while (mSocketW ->hasPendingDatagrams())
        {


            QByteArray data;
            QHostAddress sender;
            quint16 senderPort;
            data.resize(mSocketW->pendingDatagramSize());
            mSocketW->readDatagram(data.data(),data.size(),&sender, &senderPort);

            if (data[5] == 81){
                counter2++ ;
                ui->label_8v1_2->setNum(counter2);

                int longueur = data.size()- 2;
                for(int i = 10; i< longueur ; i++)
                    {
                        int bff = (int) data[i];
                        QString result = QString::number( bff, 16 );
                        ui->  plainTextEdit_2v1->appendPlainText(QString("[%1] : ").arg(i-10) + result);
                        bff1[i-10] = (int) data[i];

                    }
                    isBff1End = true;
                ui->  plainTextEdit_2v1->appendPlainText(QString("----------End Bloc-----------"));


            }
            else{
                    counter1++ ;
                    ui->label_6v1_2->setNum(counter1);

                   int longueur = data.length() ;
                   for(int i = 0; i< longueur ; i++)
                       {
                           int bff = (int) data[i];
                           QString result = QString::number( bff, 16 );
                           ui->  plainTextEditv1->appendPlainText(QString("[%1] : ").arg(i) + result);

                           bff2[i] = (int) data[i];
                       }
                       isBff2End = true;
                   ui->  plainTextEditv1->appendPlainText(QString("----------End Bloc-----------"));


            }
            if (isBff1End && isBff2End)
                {
                 int longueur = data.size();
                 for(int n=0; n< longueur; n++) {

                    if((int) bff2[n] == (int) bff1[n]){
                        ui->label_4v1_2->setNum(0);

                    }
                    else{
                        counter3++;
                        ui->label_4v1_2->setNum(counter3);
                        //ui->  plainTextEdit_3v1->appendPlainText(QString("[%1]  ").arg(n) );

                    }
                }
                }
       /*     if (data[5] == 81 ){
                   int longueur = data.size() -2;
                   for(int z = 10; z< longueur ; z++){
                       bff1[z-10] = (int) data[z];

                   }
                   isBff1End = true;
                }
            if(data[5] != 81){

                    int longueur = data.size();
                    for(int i = 0; i< longueur ; i++){
                        bff2[i] = (int) data[i];
                    }
                    isBff2End = true;
                }

            if (isBff1End && isBff2End)
                {
                 int longueur = data.size();
                 for(int n=0; n< longueur; n++) {

                    if((int) bff2[n] == (int) bff1[n]){
                        ui->label_4v1_2->setNum(0);

                    }
                    else {
                        counter3++;
                        ui->label_4v1_2->setNum(counter3);
                        ui->  plainTextEdit_3v1->appendPlainText(QString("[%1]  ").arg(n) );

                    }
                }
                }*/
        }});
}

transmission_ratio4::~transmission_ratio4()
{
    delete ui;
}

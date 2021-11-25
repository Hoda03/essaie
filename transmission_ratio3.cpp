#include "transmission_ratio3.h"
#include "ui_transmission_ratio3.h"

constexpr size_t sizerTR3 =255;
int bff1TR3[sizerTR3] {0};
int bff2TR3[sizerTR3] {0};
bool isBff1EndTR3 {false};
bool isBff2EndTR3 {false};

#include <QMainWindow>
#include <QWidget>
#include <QUdpSocket>

int counter1TR3 =0;
int counter2TR3 =0;
int counter3TR3 =0;

transmission_ratio3::transmission_ratio3(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::transmission_ratio3)
{
    ui->setupUi(this);
    mSocketTR3 = new QUdpSocket(this);
    mSocketTR3->bind(QHostAddress::LocalHost,7878);
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
    connect(mSocketTR3,&QUdpSocket::readyRead,[&](){
        while (mSocketTR3 ->hasPendingDatagrams())
        {


            QByteArray data;
            QHostAddress sender;
            quint16 senderPort;
            data.resize(mSocketTR3->pendingDatagramSize());
            mSocketTR3->readDatagram(data.data(),data.size(),&sender, &senderPort);
            if (data[5] == 33 ){
                    counter1TR3++ ;
                    ui->label_6v1_2->setNum(counter1TR3);

                   int longueur = data.size() - 2;
                   for(int i = 8; i< longueur ; i++)
                       {
                           int bff = (int) data[i];
                           QString result = QString::number( bff, 16 );
                           ui->  plainTextEditv1->appendPlainText(QString("[%1] : ").arg(i-8) + result);

                       }
                   ui->  plainTextEditv1->appendPlainText(QString("----------End Bloc-----------"));

            }
            if (data[0] == 1){
                counter2TR3++ ;
                ui->label_8v1_2->setNum(counter2TR3);

                int longueur = data.size();
                for(int i = 0; i< longueur ; i++)
                    {
                        int bff = (int) data[i];
                        QString result = QString::number( bff, 16 );
                        ui->  plainTextEdit_2v1->appendPlainText(QString("[%1] : ").arg(i) + result);
                    }
                ui->  plainTextEdit_2v1->appendPlainText(QString("----------End Bloc-----------"));


            }
            if (data[5] == 33 ){
                   int longueur = data.size() -2;
                   for(int z = 8; z< longueur ; z++){
                       bff1TR3[z-8] = (int) data[z];

                   }
                   isBff1EndTR3 = true;
                }
                if (data[0] == 1){

                    int longueur = data.size();
                    for(int i = 0; i< longueur ; i++){
                        bff2TR3[i] = (int) data[i];
                    }
                    isBff2EndTR3 = true;
                }

                if (isBff1EndTR3 && isBff2EndTR3)
                {
                 int longueur = data.size();
                 for(int n=0; n< longueur; n++) {

                    if((int) bff2TR3[n] == (int) bff1TR3[n]){
                        ui->label_4v1_2->setNum(0);

                    }
                    else {
                        counter3TR3++;
                        ui->label_4v1_2->setNum(counter3TR3);
                        ui->  plainTextEdit_2v1->appendPlainText(QString("[%1]  ").arg(n) );

                    }
                }
                }
        }});
}

transmission_ratio3::~transmission_ratio3()
{
    delete ui;
}

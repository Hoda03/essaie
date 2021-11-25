#include "transmission_ratio.h"
#include "ui_transmission_ratio.h"
#include <QMainWindow>
#include <QWidget>
#include <QUdpSocket>

constexpr size_t sizerTR =255;

int bff1TR[sizerTR] {0};
int bff2TR[sizerTR] {0};

bool isBff1EndTR {false};
bool isBff2EndTR {false};

int counter1TR =0;
int counter2TR =0;
int counter3TR =0;

transmission_ratio::transmission_ratio(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::transmission_ratio)
{
    ui->setupUi(this);
    mSocketTR = new QUdpSocket(this);
    mSocketTR->bind(QHostAddress::LocalHost,0505);
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
    connect(mSocketTR,&QUdpSocket::readyRead,[&](){
        while (mSocketTR ->hasPendingDatagrams())
        {


            QByteArray data;
            QHostAddress sender;
            quint16 senderPort;
            data.resize(mSocketTR->pendingDatagramSize());
            mSocketTR->readDatagram(data.data(),data.size(),&sender, &senderPort);
            if (data[5] == 33 ){
                    counter1TR++ ;
                    ui->label_6v1_2->setNum(counter1TR);

                   int longueur = data.size() - 2;
                   for(int i = 8; i< longueur ; i++)
                       {
                           int bff = (int) data[i];
                           QString result = QString::number( bff, 16 );
                           ui->  plainTextEditv1->appendPlainText(QString("[%1] : ").arg(i-8) + result);

                       }
                   ui->  plainTextEditv1->appendPlainText(QString("----------End Bloc-----------"));

            }
            if (data[5] == 81){
                counter2TR++ ;
                ui->label_8v1_2->setNum(counter2TR);

                int longueur = data.size();
                for(int i = 10; i< longueur - 2 ; i++)
                    {
                        int bff = (int) data[i];
                        QString result = QString::number( bff, 16 );
                        ui->  plainTextEdit_2v1->appendPlainText(QString("[%1] : ").arg(i-10) + result);
                    }
                ui->  plainTextEdit_2v1->appendPlainText(QString("----------End Bloc-----------"));


            }
            if (data[5] == 33 ){
                   int longueur = data.size() -2;
                   for(int z = 8; z< longueur ; z++){
                       bff1TR[z-8] = (int) data[z];

                   }
                   isBff1EndTR = true;
                }
                if (data[0] == 1){

                    int longueur = data.size();
                    for(int i = 0; i< longueur ; i++){
                        bff2TR[i] = (int) data[i];
                    }
                    isBff2EndTR = true;
                }
                // comparaison du contenue des tableaux
                if (isBff1EndTR && isBff2EndTR)
                {
                 int longueur = data.size();
                 for(int n=0; n< longueur; n++) {

                    if((int) bff2TR[n] == (int) bff1TR[n]){
                        ui->label_4v1_2->setNum(0);

                    }
                    else {
                        counter3TR++; //s'incrémente à chaque index non-adéquat entre les tableaux
                        ui->label_4v1_2->setNum(counter3TR);
                        ui->  plainTextEdit_2v1->appendPlainText(QString("[%1]  ").arg(n) );

                    }
                }
                }
        }});
}

transmission_ratio::~transmission_ratio()
{
    delete ui;
}

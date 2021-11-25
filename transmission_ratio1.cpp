#include "transmission_ratio1.h"
#include "ui_transmission_ratio1.h"

constexpr size_t sizertr1 =255;
int bff1tr1[sizertr1] {0};
int bff2tr1[sizertr1] {0};
bool isBff1Endtr1 {false};
bool isBff2Endtr1 {false};

#include <QMainWindow>
#include <QWidget>
#include <QUdpSocket>
int counter1tr1 =0;
int counter2tr1 =0;
int counter3tr1 =0;

transmission_ratio1::transmission_ratio1(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::transmission_ratio1)
{
    ui->setupUi(this);
    mSocketTR1 = new QUdpSocket(this);
    mSocketTR1->bind(QHostAddress::LocalHost,0101);
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
    connect(mSocketTR1,&QUdpSocket::readyRead,[&](){
        while (mSocketTR1 ->hasPendingDatagrams())
        {


            QByteArray data;
            QHostAddress sender;
            quint16 senderPort;
            data.resize(mSocketTR1->pendingDatagramSize());
            mSocketTR1->readDatagram(data.data(),data.size(),&sender, &senderPort);


            if(data.length() < 3 ){

                for (int r=0;r<data.length(); r++){
                    uint16_t tab16;
                    tab16 = ( (unsigned char)data[0] << 8 ) | (unsigned char)data[1] ;
                    value = (int) tab16;
                    qDebug()<< "value" <<value;

                }
            }

           else if (data[5] == 81 ){
                counter2tr1++ ;
                ui->label_8v1_2->setNum(counter2tr1);

                if(data.length() == value + 12){

                    int longueur = data.length() -2;
                    for(int i = 10; i< longueur ; i++)
                        {
                            int bff = (int) data[i];
                            QString result = QString::number( bff, 16 );
                            ui->  plainTextEdit_2v1->appendPlainText(QString("[%1] : ").arg(i-10) + result);
                            bff2tr1[i-10] = (int) data[i];
                            value = 0;



                        }
                    isBff2Endtr1 = true;
                    ui->  plainTextEdit_2v1->appendPlainText(QString("----------End Bloc-----------"));

                }
                else if(data.length() == value + 29){

                    int longueur = data.length() -2;
                    for(int i = 10; i< longueur -17 ; i++)
                        {
                            int bff = (int) data[i];
                            QString result = QString::number( bff, 16 );
                            ui->  plainTextEdit_2v1->appendPlainText(QString("[%1] : ").arg(i-10) + result);
                            bff2tr1[i-10] = (int) data[i];
                            value = 0;



                        }
                    isBff2Endtr1 = true;
                    ui->  plainTextEdit_2v1->appendPlainText(QString("----------End Bloc-----------"));

                }
                else if (data[5] == 84 ){
                     counter2tr1++ ;
                     ui->label_8v1_2->setNum(counter2tr1);

                     if(data.length() == value + 12){

                         int longueur = data.length() -2;
                         for(int i = 10; i< longueur ; i++)
                             {
                                 int bff = (int) data[i];
                                 QString result = QString::number( bff, 16 );
                                 ui->  plainTextEdit_2v1->appendPlainText(QString("[%1] : ").arg(i-10) + result);
                                 bff2tr1[i-10] = (int) data[i];
                                 value = 0;



                             }
                         isBff2Endtr1 = true;
                         ui->  plainTextEdit_2v1->appendPlainText(QString("----------End Bloc-----------"));

                     }
                     else if(data.length() == value + 29){

                         int longueur = data.length() -2;
                         for(int i = 10; i< longueur -17 ; i++)
                             {
                                 int bff = (int) data[i];
                                 QString result = QString::number( bff, 16 );
                                 ui->  plainTextEdit_2v1->appendPlainText(QString("[%1] : ").arg(i-10) + result);
                                 bff2tr1[i-10] = (int) data[i];
                                 value = 0;



                             }
                         isBff2Endtr1 = true;
                         ui->  plainTextEdit_2v1->appendPlainText(QString("----------End Bloc-----------"));

                     }

                 }



            }
            else if(data[5] == 33){
                    counter1tr1++ ;
                    ui->label_6v1_2->setNum(counter1tr1);

                   int longueur = data.size() ;
                   for(int i = 9; i< longueur-2 ; i++)
                       {
                           int bff = (int) data[i];
                           QString result = QString::number( bff, 16 );
                           bff1tr1[i-9] = (int) data[i];

                           ui->  plainTextEditv1->appendPlainText(QString("[%1] : ").arg(i-9) + result);
                        }
                           isBff1Endtr1 = true;
                            ui->  plainTextEditv1->appendPlainText(QString("----------End Bloc-----------"));

            }
            else {

                for(int m=0; m < data.length() ;m++){
                    if(data[m] == 81){

                        for(int i = m+5; i< m+ value +5 ; i++)
                            {

                            int bff = (int) data[i];
                            QString result = QString::number( bff, 16 );
                            ui->  plainTextEdit_2v1->appendPlainText(QString("[%1] : ").arg(i-(m+5)) + result);
                            bff2tr1[i-m-5] = (int) data[i];}

                    }


                }
                isBff2Endtr1 = true;
                ui->  plainTextEdit_2v1->appendPlainText(QString("----------End Bloc-----------"));


            }
            if (isBff1Endtr1 && isBff2Endtr1)
            {
             int longueur = data.size();
             for(int n=0; n< longueur; n++) {

                if((int) bff2tr1[n] == (int) bff1tr1[n]){
                    ui->label_4v1_2->setNum(0);

                }
                else {
                    counter3tr1++;
                    ui->label_4v1_2->setNum(counter3tr1);
                    ui->  plainTextEdit_3v1->appendPlainText(QString("[%1]  ").arg(n) );

                }
            }
            }

        }
        isBff2Endtr1 = false;
        isBff2Endtr1 = false;


    });
}

transmission_ratio1::~transmission_ratio1()
{
    delete ui;
}

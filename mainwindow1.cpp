#include "mainwindow1.h"
#include "ui_mainwindow1.h"
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <QTime>
#include <windows.h>

using namespace std;
int cnt=1;
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    #include <QtCore/QTextCodec>
#else
    #include <QtCore5Compat/QTextCodec>
#endif

#ifdef Q_OS_WIN
#define sleeep(msec) Sleep(msec)
#elif defined(Q_OS_UNIX)
#include <unistd.h>
#define sleeep(msec) usleep(msec)
#endif

uint8_t countern =0;


MainWindow1::MainWindow1(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow1)
{
    ui->setupUi(this);
    this->setWindowTitle("MODE2&TCP");
    this->setStyleSheet("QMainWindow { background-color : #ffffff}");

    //création du socket pour la communication en Tcp/vdl2 avec le récepteur
    socketTcp1 = NULL;
    socketTcp1 = new QTcpSocket(this);

    //création du socket pour la communication en Tcp/vdl2 avec l'émetteur
    socketTcp1r = NULL;
    socketTcp1r = new QTcpSocket(this);

    //création du socket en localhost pour envoyer la trame sqp_ind
    socketUdp1 = new QUdpSocket(this);
    socketUdp1 ->bind(QHostAddress::LocalHost,0502);

    //création du socket en localhost pour envoyer la trame unit_data_ind
    socketUdp1tr1 =new QUdpSocket(this);
    socketUdp1tr1 ->bind(QHostAddress::LocalHost,6442);

    /*--------------------Création d'un Timer pour l'envoie succèssives des données en localhost----------------------------------------*/
    timer1 = new QTimer(this);
    connect(timer1, &QTimer::timeout, this, &MainWindow1::start_timer1);

    /*---------------cette fonction connect permet de se connecter avec le récepteur et lire ses réponses-----------------*/
    connect (socketTcp1, &QTcpSocket::readyRead, [&](){

        QByteArray buffer;
        buffer.resize( socketTcp1->bytesAvailable() );
        socketTcp1->read( buffer.data(), buffer.size() );
        QByteArray ba_as_hex_string = buffer.toHex();
        ui->plainTextEdit->setReadOnly( true );


        if ((buffer[5] == 80) && (buffer[8] >= 18))
        {

            int PARAM_REQ = ui-> frequence_2 -> value();
            ui-> plainTextEdit->appendPlainText(QString("PARAM_CONF = %1").arg(PARAM_REQ));
            ui->  plainTextEdit_3->appendPlainText(ba_as_hex_string);

        }

        else if(buffer[5] == 83)
        {

            ui->plainTextEdit->appendPlainText(QString(" VDR_ERROR IND"));
            ui->  plainTextEdit_3->appendPlainText(ba_as_hex_string);

        }
        else if( buffer[9] == 1 && buffer[11] == 119)
        {

            ui->plainTextEdit->appendPlainText(QString("MODE_SET_CONF"));
            ui->  plainTextEdit_3->appendPlainText(ba_as_hex_string);

        }

        else if( buffer.length() < 1460  && buffer[5] == 81  )
        {


              if(buffer.length() == ui->donnees_2->value()+12){

                  ui->plainTextEdit->appendPlainText(QString("UNIT DATA IND"));
                  QByteArray ba_as_hex_string1 = buffer.toHex();
                  ui->  plainTextEdit_3->appendPlainText(ba_as_hex_string1);
                  socketUdp1tr1->writeDatagram(buffer,QHostAddress::LocalHost,0101);

             }
              else {
                  ui->plainTextEdit->appendPlainText(QString("UNIT DATA IND"));
                  ui->plainTextEdit->appendPlainText(QString("SQP IND"));
                  socketUdp1tr1->writeDatagram(buffer,QHostAddress::LocalHost,0101);
                  QByteArray ba_as_hex_string1 = buffer.toHex();
                  ui->  plainTextEdit_3->appendPlainText(ba_as_hex_string1);



             }

        }

        else if( buffer.length() ==  10  && buffer[4] == 95  ){

            ui->plainTextEdit->appendPlainText(QString("CU_IND"));
            ui->  plainTextEdit_3->appendPlainText(ba_as_hex_string);


          }
        else if(buffer.length() == 17 && buffer[5] ==84)
        {
              ui->plainTextEdit->appendPlainText(QString("SQP IND"));
              ui->  plainTextEdit_3->appendPlainText(ba_as_hex_string);
              socketUdp1->writeDatagram(buffer,QHostAddress::LocalHost,6969);
        }
        else {

            if(cnt ==1 ){
                    arr1 = buffer;
                    qDebug() << cnt << "arr";


            }

           else{
                arr1 = arr1.append(buffer);
                qDebug()<<"houda"<< "ct ";

                int val = arr1.length();
                qDebug()<< val;



                if(arr1.length() == ui->donnees_2->value() + 12)
                {
                        ui->plainTextEdit->appendPlainText(QString("UNIT DATA IND"));
                        QByteArray ba_as_hex_string1 = arr1.toHex();

                        ui->  plainTextEdit_3->appendPlainText(ba_as_hex_string1);
                        socketUdp1tr1->writeDatagram(arr1,QHostAddress::LocalHost,0101);
                        arr1.clear();
                        cnt =0;

                        qDebug()<<"append"<< "ct ";


                }
               else if(arr1.length() > ui->donnees_2->value() + 12)
               {

                    ui->plainTextEdit->appendPlainText(QString("UNIT DATA IND"));
                    ui->plainTextEdit->appendPlainText(QString("SQP IND"));
                    QByteArray ba_as_hex_string1 = arr1.toHex();
                    ui->  plainTextEdit_3->appendPlainText(ba_as_hex_string1);
                    socketUdp1tr1->writeDatagram(arr1,QHostAddress::LocalHost,0101);
                    arr1.clear();
                    cnt =0;



                }



            }
            qDebug() << "hi";
            cnt++;

        }


    });
    connect (socketTcp1r, &QTcpSocket::readyRead, [&](){
           QByteArray buffer;
           buffer.resize( socketTcp1r->bytesAvailable() );
           socketTcp1r->read( buffer.data(), buffer.size() );
           ui->plainTextEdit->setReadOnly( true );
           QByteArray ba_as_hex_string = buffer.toHex();

           if ((buffer[5] == 80) && (buffer[8] >= 18))
           {
               int PARAM_REQ = ui-> frequence_2 -> value();
               ui-> plainTextEdit->appendPlainText(QString("PARAM_CONF = %1").arg(PARAM_REQ));
               ui->  plainTextEdit_3->appendPlainText(ba_as_hex_string);

           }
           if (((buffer[8] == 47) || (buffer[8] == 44) || (buffer[8] == 40)) && ( buffer[9] == 1))
           {
               int PWR_SET_REQ = ui -> puissance -> value();
               ui->plainTextEdit->appendPlainText(QString("PWR_SET_CONF = %1").arg(PWR_SET_REQ));
               ui->  plainTextEdit_3->appendPlainText(ba_as_hex_string);
           }

           if( buffer[9] == 1 && buffer[11] == 119)
           {

               ui->plainTextEdit->appendPlainText(QString("MODE_SET_CONF"));
               ui->  plainTextEdit_3->appendPlainText(ba_as_hex_string);

           }
           if(buffer[5] == 83)
           {
               ui->plainTextEdit->appendPlainText(QString("VDR_ERROR IND"));
               ui->  plainTextEdit_3->appendPlainText(ba_as_hex_string);
           }
           if(buffer[5] == 86)
           {
               ui->plainTextEdit->appendPlainText(QString("UNIT_DATA_CONF"));
               ui->  plainTextEdit_3->appendPlainText(ba_as_hex_string);

               // Concaténation des buffer 9 et 10 pour le MacDelay
               uint16_t tab16;
               tab16 = ( (unsigned char)buffer[9] << 8 ) | (unsigned char)buffer[10] ;


               //Convertir les valer en décimal
               int trans_statut =(int) buffer[8]; //
               int PKt_seq =(int) buffer[11];
               int MacDelay =(int) tab16; //MacDelay


               QString s = QString::number(trans_statut);
               QString g = QString::number(MacDelay);
               QString f = QString::number(PKt_seq);

               ui->info->appendPlainText(QString("statut de transmission : %1").arg(s));
               ui->info->appendPlainText(QString("Mac Delay : %1").arg(g));
               ui->info->appendPlainText(QString("Tx Pkt sequence No. : %1").arg(f));
           }
    });


}
/*--------------Cette Fonction nour permet de calculer le CRC16 Kermit---------------------*/

uint16_t CRCVDL2(uint8_t *x, uint16_t len) {
    uint8_t *data = x;
    uint16_t crc=0;

    while (len--) {
        crc ^= *data++;
        for (uint8_t k = 0; k < 8; k++)
            if (crc & 1) crc =(crc >> 1) ^ 0x8408;
            else crc = crc >> 1;
    }

    return crc;
}

MainWindow1::~MainWindow1()
{
    delete ui;
}

/*------------------Cette Fonction nous permet d'envoyer un changement de fréquence à la radios/EM ----------------------*/
void MainWindow1::on_actionPARAM_REQ_triggered()
{
    /*------------------------Se connecter à la radio---------------------------------------------------*/

    QString ip = ui->lineEditip->text();
    quint16 port = ui->spinBox_9->value();
    socketTcp1r->connectToHost(QHostAddress(ip),port);


    /*-----------------------------------------------------------------------------------*/
    // Algo de traitement de la fréquence
    //Il faut créer une liste qui prend la taille de la valeur de fréquence désiré
    //mettre le nombre saisie dans le tableu sous forme de chiffre

    /*-----------------------------------------------------------------------------------------*/


    int T[6];
    int c = 0;
    int msb[2] , lsb[2] ;
    int monnombre = ui->frequence_2->value();
    int mynumber = 0;
    int p =1 , e = 1 , o = 1;
    /*------------------mettre le nmbre saisie dans un tableau en forme de chiffre---------------------------------*/


    while(monnombre!=0) {
        int mod = monnombre%10;
        T[c] = mod;
        monnombre-=mod;
        monnombre/=10;
        c++;
    }

    /*------------------------Reconversion du tableau en chiffre-----------------------------------------------------*/
   for(int i = 1; i<5 ; i++){
        mynumber =  mynumber + T[i]*p;
        p=p*10;
    }

    msb[0] = T[4] ;
    msb[1] = T[3];

    lsb[0] = T[2];
    lsb[1] = T[1];

    uint8_t var = 0 ;
    for(int k=1;k>=0;k--) {
            var = var +msb[k]*e ; //freq_msb
            e = e*10 ;
    }
    uint8_t var2 = 0 ;
    for(int k=1;k>=0;k--) {
            var2 = var2 +lsb[k]*o ; //freq_lsb
            o = o*10 ;
    }

    uint8_t var1 = ui -> timer1 -> value();
    uint8_t var3 =ui -> timer2 -> value();
    uint16_t var4 = ui->counterM1->value();
    uint8_t var5 = ui->persistance->value();

    uint16_t value = var4;
    uint8_t array[2];
    array[0]=value & 0xff;
    array[1]=(value >> 8);


    /*-------------Définition des champs de la trame et son envoie dans un fil------------- */


    if  ( ui -> frequence_2 -> value() >= 118000  &  ui -> frequence_2 -> value() <= 136000 )
            {

                    tsPid20 Pid20;
                    Pid20.synchro [0] = synchrovdl2[0];
                    Pid20.synchro [1] = synchrovdl2[1];
                    Pid20.synchro [2] = synchrovdl2[2];
                    Pid20.synchro [3] = synchrovdl2[3];
                    Pid20.mode = 0xF1;
                    Pid20.pid = 0x20;
                    Pid20.dfl1_msb =0x00;
                    Pid20.dfl2_lsb = 0x07;
                    Pid20.freq_msb=var;
                    Pid20.freq_lsb = var2;
                    Pid20.tm1 =var1;
                    Pid20.tm2 =var3;
                    Pid20.M1_lsb= array[0];
                    Pid20.M1_msb= array[1];
                    Pid20.persistance= var5;

                    /*-------------------------------Calcul du Crc16 Kermit-------------------------------------*/

                    unsigned char myData[] = {0xff, 0x00, 0xf0, 0xca, 0xf1, 0x20, 0x00, 0x07,var,var2,var1,var3,array[1], array[0],var5 };
                    unsigned int myDataLen;
                    unsigned short myCrc;
                    myDataLen = sizeof(myData);
                    myCrc = CRCVDL2(myData, myDataLen);
                    myCrc = ((myCrc << 8) | (myCrc >> 8));
                    Pid20.crc=myCrc;

                    /*-------------------Intégration des champs du buffer dans un fil d'envoie--------------------------*/


                    QByteArray buffer;

                    //  ecrire les données dans le flux d'envoie "s"
                    QDataStream s(&buffer, QIODevice::WriteOnly);
                    if (false){

                        s.setByteOrder(QDataStream::LittleEndian);
                    }
                    s << (uint8_t)Pid20.synchro [0]<< (uint8_t)Pid20.synchro [1]<<(uint8_t)Pid20.synchro [2]<<(uint8_t)Pid20.synchro [3]
                      << (uint8_t)Pid20.mode << (uint8_t)Pid20.pid << (uint8_t)Pid20.dfl1_msb <<(uint8_t)Pid20.dfl2_lsb
                      <<(uint8_t)Pid20.freq_msb<<(uint8_t)Pid20.freq_lsb<<(uint8_t)Pid20.tm1<<(uint8_t)Pid20.tm2
                      <<(uint8_t)Pid20.M1_msb<<(uint8_t)Pid20.M1_lsb<<(uint8_t)Pid20.persistance<<(uint16_t)Pid20.crc;


                    /*--------------------------------Envoie des données--------------------------------------*/
                    socketTcp1r->write(buffer);

                    int PARAM_REQ = ui-> frequence_2 -> value();
                    ui-> plainTextEdit_2->appendPlainText(QString("PARAM_REQ = %1").arg(PARAM_REQ));
                    QByteArray ba_as_hex_string = buffer.toHex();
                    ui->  plainTextEdit_4->appendPlainText(ba_as_hex_string);

            }
}


/*------------------Cette Fonction nous permet d'envoyer un changement de fréquence à la radios/RX ----------------------*/

void MainWindow1::on_actionPARAM_REQ_2_triggered()
{
    /*------------------------Se connecter à la radio---------------------------------------------------*/

    QString ip = ui->lineEditip_2->text();
    quint16 port = ui->spinBox_10->value();
    socketTcp1->connectToHost(QHostAddress(ip),port);


    /*-----------------------------------------------------------------------------------*/
    // Algo de traitement de la fréquence
    //Il faut créer une liste qui prend la taille de la valeur de fréquence désiré
    //mettre le nombre saisie dans le tableu sous forme de chiffre

    /*-----------------------------------------------------------------------------------------*/


    int T[6];
    int c = 0;
    int msb[2] , lsb[2] ;
    int monnombre = ui->frequence_2->value();
    int mynumber = 0;
    int p =1 , e = 1 , o = 1;
    /*------------------mettre le nmbre saisie dans un tableau en forme de chiffre---------------------------------*/


    while(monnombre!=0) {
        int mod = monnombre%10;
        T[c] = mod;
        monnombre-=mod;
        monnombre/=10;
        c++;
    }

    /*------------------------Reconversion du tableau en chiffre-----------------------------------------------------*/
   for(int i = 1; i<5 ; i++){
        mynumber =  mynumber + T[i]*p;
        p=p*10;
    }

    msb[0] = T[4] ;
    msb[1] = T[3];

    lsb[0] = T[2];
    lsb[1] = T[1];

    uint8_t var = 0 ;
    for(int k=1;k>=0;k--) {
            var = var +msb[k]*e ; //freq_msb
            e = e*10 ;
    }
    uint8_t var2 = 0 ;
    for(int k=1;k>=0;k--) {
            var2 = var2 +lsb[k]*o ; //freq_lsb
            o = o*10 ;
    }

    uint8_t var1 = ui -> timer1 -> value();
    uint8_t var3 =ui -> timer2 -> value();
    uint16_t var4 = ui->counterM1->value();
    uint8_t var5 = ui->persistance->value();

    uint16_t value = var4;
    uint8_t array[2];
    array[0]=value & 0xff;
    array[1]=(value >> 8);


    /*-------------Définition des champs de la trame et son envoie dans un fil------------- */


    if  ( ui -> frequence_2 -> value() >= 118000  &  ui -> frequence_2 -> value() <= 136000 )
            {

                    tsPid20 Pid20;
                    Pid20.synchro [0] = synchrovdl2[0];
                    Pid20.synchro [1] = synchrovdl2[1];
                    Pid20.synchro [2] = synchrovdl2[2];
                    Pid20.synchro [3] = synchrovdl2[3];
                    Pid20.mode = 0xF1;
                    Pid20.pid = 0x20;
                    Pid20.dfl1_msb =0x00;
                    Pid20.dfl2_lsb = 0x07;
                    Pid20.freq_msb=var;
                    Pid20.freq_lsb = var2;
                    Pid20.tm1 =var1;
                    Pid20.tm2 =var3;
                    Pid20.M1_lsb= array[0];
                    Pid20.M1_msb= array[1];
                    Pid20.persistance= var5;

                    /*-------------------------------Calcul du Crc16 Kermit-------------------------------------*/

                    unsigned char myData[] = {0xff, 0x00, 0xf0, 0xca, 0xf1, 0x20, 0x00, 0x07,var,var2,var1,var3,array[1], array[0],var5 };
                    unsigned int myDataLen;
                    unsigned short myCrc;
                    myDataLen = sizeof(myData);
                    myCrc = CRCVDL2(myData, myDataLen);
                    myCrc = ((myCrc << 8) | (myCrc >> 8));
                    Pid20.crc=myCrc;

                    /*-------------------Intégration des champs du buffer dans un fil d'envoie--------------------------*/


                    QByteArray buffer;

                    //  ecrire les données dans le flux d'envoie
                    QDataStream s(&buffer, QIODevice::WriteOnly);
                    if (false){

                        s.setByteOrder(QDataStream::LittleEndian);
                    }
                    s << (uint8_t)Pid20.synchro [0]<< (uint8_t)Pid20.synchro [1]<<(uint8_t)Pid20.synchro [2]<<(uint8_t)Pid20.synchro [3]
                      << (uint8_t)Pid20.mode << (uint8_t)Pid20.pid << (uint8_t)Pid20.dfl1_msb <<(uint8_t)Pid20.dfl2_lsb
                      <<(uint8_t)Pid20.freq_msb<<(uint8_t)Pid20.freq_lsb<<(uint8_t)Pid20.tm1<<(uint8_t)Pid20.tm2
                      <<(uint8_t)Pid20.M1_msb<<(uint8_t)Pid20.M1_lsb<<(uint8_t)Pid20.persistance<<(uint16_t)Pid20.crc;


                    /*--------------------------------Envoie des données--------------------------------------*/
                    socketTcp1->write(buffer);
                    int PARAM_REQ = ui-> frequence_2 -> value();
                    ui-> plainTextEdit_2->appendPlainText(QString("PARAM_REQ = %1").arg(PARAM_REQ));
                    QByteArray ba_as_hex_string = buffer.toHex();
                    ui->  plainTextEdit_4->appendPlainText(ba_as_hex_string);


            }
}

/*------------------Cette Fonction nous permet d'envoyerles données vdl2 à la radio/EM ----------------------*/

void MainWindow1::on_UINT_DATA_REQ_clicked()
{
    /*------------------------Se connecter à la radio---------------------------------------------------*/

    QString ip = ui->lineEditip->text();
    quint16 port = ui->spinBox_9->value();
    socketTcp1r->connectToHost(QHostAddress(ip),port);

    /*-----------------------------structure de la PidF0----------------------------------------------------*/

    struct tsPid21{
        uint8_t synchro[SIZE_SYNCHRO_VDL2];
        uint8_t protocoleCode;
        uint8_t pid;
        uint8_t lengthMsb;
        uint8_t lengthLsb;
        uint8_t clic;
        uint16_t crc;
        char str1[16384];
    };

    /*la variable counter change à chaque fois on clique sur le button unit data req */
    countern++;

    /*--------------Définition des 16k valeur à pouvoir envoyer sous le protocole VDL 2------------------------*/

     uint16_t value = ui->donnees_2->value()+1;
     uint8_t array[2];
     array[0]=value & 0xff;
     array[1]=(value >> 8);

    /*----fonction rand()- retourne des entier aléatoire de 0 à 16k----*/

    char table22[ ui->donnees_2->value() ] ;
    unsigned char table222[ui->donnees_2->value()] ;

      for(int i=0; i< (ui->donnees_2->value()); i++){
        QTime time = QTime::currentTime();
        srand((unsigned) time.msec());
        int strn = (rand() % 9) +1 ;
        //qsrand((uint)time.msec());
        //int strn =  (rand() % 9) + 1;
        unsigned char monsef = (unsigned char) strn;
        table22[i] = strn;

        table222[i] = monsef;

        }
         char str[16384];
         str[16383] = '\0';
         strncpy ( str, table22, ui-> donnees_2-> value() );
         QByteArray databuf = QByteArray(reinterpret_cast<char*>(table22), ui->donnees_2->value());


         /*-------------Définition des champs de la trame et l'intégrer dans un flus d'envoie------------- */

         tsPid21 Pid21;
         Pid21.synchro [0] = synchrovdl2[0];
         Pid21.synchro [1] = synchrovdl2[1];
         Pid21.synchro [2] = synchrovdl2[2];
         Pid21.synchro [3] = synchrovdl2[3];
         Pid21.protocoleCode = 0xF1;
         Pid21.pid = 0x21;
         Pid21.lengthMsb=array[1];
         Pid21.lengthLsb=array[0];
         Pid21.clic=countern;

         /*-------------------------------Calcul du Crc16 Kermit-------------------------------------*/


         unsigned char myData3[] ={0xff ,0x00, 0xf0, 0xca,0xf1, 0x21,array[1],array[0],countern};
         unsigned char allstrings[sizeof(table222)+sizeof(myData3)];
         memcpy(allstrings,myData3,sizeof(myData3));
         memcpy(allstrings+sizeof(myData3),table222,sizeof(table222));
         unsigned short myCrc;
         unsigned int myDataLen = sizeof(table222)+ sizeof(myData3);
         myCrc = CRCVDL2(allstrings, myDataLen);
         myCrc = ((myCrc << 8) | (myCrc >> 8));
         Pid21.crc=myCrc;



     /*-------------------Intégration des champs du buffer dans un flux d'envoie--------------------------*/


     QByteArray buffer;
     //  ecrire les données dans le flux d'envoie streamvdldata
     QDataStream streamvdldata(&buffer, QIODevice::WriteOnly);

     if (false){

         streamvdldata.setByteOrder(QDataStream::LittleEndian);
     }
     streamvdldata<< (uint8_t)Pid21.synchro [0]<< (uint8_t)Pid21.synchro [1]<<(uint8_t)Pid21.synchro [2]
                  <<(uint8_t)Pid21.synchro [3]  << (uint8_t)Pid21.protocoleCode << (uint8_t)Pid21.pid
                  << (uint8_t)Pid21.lengthMsb<< (uint8_t)Pid21.lengthLsb<<(uint8_t)Pid21.clic;


        QByteArray buffer3;
        //  ecrire les données dans le flux d'envoie streamCrc
        QDataStream streamCrc(&buffer3, QIODevice::WriteOnly);

        if (false){
            streamCrc.setByteOrder(QDataStream::LittleEndian);
        }
        streamCrc <<(uint16_t) Pid21.crc;
        /*--------------------------------Envoie des données--------------------------------------*/

     QByteArray buffer5 = buffer + databuf + buffer3;

     socketTcp1r->write(buffer5);
     socketUdp1tr1->writeDatagram(buffer5,QHostAddress::LocalHost,0101);


     uint16_t var = ui->donnees_2->value();

     QByteArray varNumData;
     //  ecrire les données dans le flux d'envoie streamCrc
     QDataStream h(&varNumData, QIODevice::WriteOnly);

     if (false){
         h.setByteOrder(QDataStream::LittleEndian);
     }
     h << var;

     socketUdp1tr1->writeDatagram(varNumData,QHostAddress::LocalHost,6969);



     socketUdp1tr1->writeDatagram(varNumData,QHostAddress::LocalHost,0101);

     ui-> plainTextEdit_2 -> appendPlainText("UINT_DATA_REQ");
     QByteArray ba_as_hex_string = buffer5.toHex();
     ui->  plainTextEdit_4->appendPlainText(ba_as_hex_string);

}


void MainWindow1::on_actionPWR_SET_REQ_triggered()
{
    /*------------------------Se connecter à la radio---------------------------------------------------*/

    QString ip = ui->lineEditip->text();
    quint16 port = ui->spinBox_9->value();
    socketTcp1r->connectToHost(QHostAddress(ip),port);

    /*-----------------------------structure de la PidF1----------------------------------------------------*/

    struct tsPidF1 {
        uint8_t synchro[SIZE_SYNCHRO_VDL2];
        uint8_t protocoleCode;
        uint8_t pid;
        uint8_t lengthMsb;
        uint8_t lengthLsb;
        uint8_t pwr;
        uint8_t req;
        uint16_t crc;

    };
    /*----------------instruction conditionnelle pour l'envoie de la puissance à la Radio------------------------*/


    int pwr = ui -> puissance -> value();
    uint8_t var3;

   if(  pwr == 50 ) {
         var3 = 47;
    }
   if ( pwr == 25){
         var3 = 44;
    }
   if ( pwr == 10) {

         var3 = 40;
    }

   /*-------------Définition des champs de la trame ------------- */


    tsPidF1 PidF1;
    PidF1.synchro [0] = synchrovdl2[0];
    PidF1.synchro [1] = synchrovdl2[1];
    PidF1.synchro [2] = synchrovdl2[2];
    PidF1.synchro [3] = synchrovdl2[3];
    PidF1.protocoleCode = 0xF1;
    PidF1.pid=0xF1;
    PidF1.lengthMsb = 0x00;
    PidF1.lengthLsb = 0x02;
    PidF1.pwr=var3;
    PidF1.req=0;

    /*-------------------------------Calcul du Crc16 Kermit-------------------------------------*/

    unsigned char myData[] = {0xff, 0x00, 0xf0, 0xca, 0xf1, 0xf1,0x00,0x02,var3,0x00};
    unsigned int myDataLen;
    unsigned short myCrc;
    myDataLen = sizeof(myData);
    myCrc = CRCVDL2(myData, myDataLen);
    myCrc = ((myCrc << 8) | (myCrc >> 8));
    PidF1.crc=myCrc;

    /*-------------------Intégration des champs du buffer dans un fil d'envoie--------------------------*/


    QByteArray buffer;

    //  ecrire les données dans le flux d'envoie
    QDataStream s(&buffer, QIODevice::WriteOnly);

    if (false){

        s.setByteOrder(QDataStream::LittleEndian);

    }
    s  << (uint8_t)PidF1.synchro [0]<< (uint8_t)PidF1.synchro [1]<<(uint8_t)PidF1.synchro [2]
       <<(uint8_t)PidF1.synchro [3]  << (uint8_t)PidF1.protocoleCode << (uint8_t)PidF1.pid
       << (uint8_t)PidF1.lengthMsb<< (uint8_t)PidF1.lengthLsb<< (uint8_t)PidF1.pwr
       << (uint8_t)PidF1.req   << (uint16_t)PidF1.crc  ;

    /*--------------------------------Envoie des données--------------------------------------*/

    socketTcp1r->write(buffer);
    ui-> plainTextEdit_2 -> appendPlainText("PWR_SET_REQ");

    QByteArray ba_as_hex_string = buffer.toHex();
    ui->  plainTextEdit_4->appendPlainText(ba_as_hex_string);
}


void MainWindow1::on_actionMODE_SET_REQ_triggered()
{
    /*------------------------Se connecter à la radio---------------------------------------------------*/

    QString ip = ui->lineEditip->text();
    quint16 port = ui->spinBox_9->value();

    socketTcp1r->connectToHost(QHostAddress(ip),port);
    /*-----------------------------structure de la PidF0----------------------------------------------------*/

    struct tsPidF0{
        uint8_t synchro[SIZE_SYNCHRO_VDL2];
        uint8_t protocoleCode;
        uint8_t pid;
        uint8_t lengthMsb;
        uint8_t lengthLsb;
        uint8_t mode;
        uint8_t req;
        uint16_t crc;

    };

    /*-------------Définition des champs de la trame ------------- */

    tsPidF0 PidF0;
    PidF0.synchro [0] = synchrovdl2[0];
    PidF0.synchro [1] = synchrovdl2[1];
    PidF0.synchro [2] = synchrovdl2[2];
    PidF0.synchro [3] = synchrovdl2[3];
    PidF0.protocoleCode = 0xF1;
    PidF0.pid=0xF0;
    PidF0.lengthMsb = 0x00;
    PidF0.lengthLsb = 0x02;
    PidF0.mode=0xF2;
    PidF0.req=0;

    /*-------------------------------Calcul du Crc16 Kermit-------------------------------------*/

    unsigned char myData[] = {0xff, 0x00, 0xf0, 0xca, 0xf1, 0xf0,0x00,0x02,0xf2,0x00};
    unsigned int myDataLen;
    unsigned short myCrc;
    myDataLen = sizeof(myData);
    myCrc = CRCVDL2(myData, myDataLen);
    myCrc = ((myCrc << 8) | (myCrc >> 8));
    PidF0.crc= myCrc;

    /*-------------------Intégration des champs du buffer dans un flux d'envoie--------------------------*/

    QByteArray buffer;

    //  ecrire les données dans le flux d'envoie
    QDataStream s(&buffer, QIODevice::WriteOnly);

    if (false){

        s.setByteOrder(QDataStream::LittleEndian);

    }
    s  << (uint8_t)PidF0.synchro [0]<< (uint8_t)PidF0.synchro [1]<<(uint8_t)PidF0.synchro [2]
       <<(uint8_t)PidF0.synchro [3]  << (uint8_t)PidF0.protocoleCode << (uint8_t)PidF0.pid
       << (uint8_t)PidF0.lengthMsb<< (uint8_t)PidF0.lengthLsb<< (uint8_t)PidF0.mode
       << (uint8_t)PidF0.req   << (uint16_t)PidF0.crc  ;


    /*--------------------------------Envoie des données--------------------------------------*/

    socketTcp1r->write(buffer);
    ui->plainTextEdit_2 -> appendPlainText("MODE_SET_REQ");

    QByteArray ba_as_hex_string = buffer.toHex();
    ui->  plainTextEdit_4->appendPlainText(ba_as_hex_string);

}


void MainWindow1::on_actionMODE_SET_REQ_2_triggered()
{
    /*------------------------Se connecter à la radio---------------------------------------------------*/

    QString ip = ui->lineEditip_2->text();
    quint16 port = ui->spinBox_10->value();


    socketTcp1->connectToHost(QHostAddress(ip),port);
    /*-----------------------------structure de la PidF0----------------------------------------------------*/

    struct tsPidF0{
        uint8_t synchro[SIZE_SYNCHRO_VDL2];
        uint8_t protocoleCode;
        uint8_t pid;
        uint8_t lengthMsb;
        uint8_t lengthLsb;
        uint8_t mode;
        uint8_t req;
        uint16_t crc;

    };

    /*-------------Définition des champs de la trame ------------- */

    tsPidF0 PidF0;
    PidF0.synchro [0] = synchrovdl2[0];
    PidF0.synchro [1] = synchrovdl2[1];
    PidF0.synchro [2] = synchrovdl2[2];
    PidF0.synchro [3] = synchrovdl2[3];
    PidF0.protocoleCode = 0xF1;
    PidF0.pid=0xF0;
    PidF0.lengthMsb = 0x00;
    PidF0.lengthLsb = 0x02;
    PidF0.mode=0xF2;
    PidF0.req=0;

    /*-------------------------------Calcul du Crc16 Kermit-------------------------------------*/

    unsigned char myData[] = {0xff, 0x00, 0xf0, 0xca, 0xf1, 0xf0,0x00,0x02,0xf2,0x00};
    unsigned int myDataLen;
    unsigned short myCrc;
    myDataLen = sizeof(myData);
    myCrc = CRCVDL2(myData, myDataLen);
    myCrc = ((myCrc << 8) | (myCrc >> 8));
    PidF0.crc= myCrc;

    /*-------------------Intégration des champs du buffer dans un flux d'envoie streamModeR--------------------------*/

    QByteArray buffer;

    //  ecrire les données dans le flux d'envoie
    QDataStream streamModeR(&buffer, QIODevice::WriteOnly);

    if (false){

        streamModeR.setByteOrder(QDataStream::LittleEndian);

    }
    streamModeR << (uint8_t)PidF0.synchro [0]<< (uint8_t)PidF0.synchro [1]<<(uint8_t)PidF0.synchro [2]
                <<(uint8_t)PidF0.synchro [3]  << (uint8_t)PidF0.protocoleCode << (uint8_t)PidF0.pid
                << (uint8_t)PidF0.lengthMsb<< (uint8_t)PidF0.lengthLsb<< (uint8_t)PidF0.mode
                << (uint8_t)PidF0.req   << (uint16_t)PidF0.crc  ;


    /*--------------------------------Envoie des données--------------------------------------*/

    socketTcp1->write(buffer);
    ui->plainTextEdit_2 -> appendPlainText("MODE_SET_REQ");
    QByteArray ba_as_hex_string = buffer.toHex();
    ui->  plainTextEdit_4->appendPlainText(ba_as_hex_string);
}


void MainWindow1::on_actionPURGE_REQ_triggered()
{

    /*------------------------Se connecter à la radio---------------------------------------------------*/

    QString ip = ui->lineEditip->text();
    quint16 port = ui->spinBox_9->value();
    socketTcp1r->connectToHost(QHostAddress(ip),port);

    /*-------------champs de la trame Pid010b------------- */

    struct tsPid010b{
        uint8_t synchro[SIZE_SYNCHRO_VDL2];
        uint8_t sal;
        uint8_t donnees;
        uint8_t pid;
        uint8_t protocoelCode;
        uint16_t crc;

    };

    /*-------------Définition des champs de la trame ------------- */


    tsPid010b Pid010b;
    Pid010b.synchro [0] = synchrovdl2[0];
    Pid010b.synchro [1] = synchrovdl2[1];
    Pid010b.synchro [2] = synchrovdl2[2];
    Pid010b.synchro [3] = synchrovdl2[3];
    Pid010b.protocoelCode=0x5f;
    Pid010b.pid=0x14;
    Pid010b.donnees=0x00;
    Pid010b.sal=0x00;

    /*-------------------------------Calcul du Crc16 Kermit-------------------------------------*/

    unsigned char myData[] = {0xff, 0x00, 0xf0, 0xca, 0x5f, 0x14,0x00,0x00};
    unsigned int myDataLen;
    unsigned short myCrc;
    myDataLen = sizeof(myData);
    myCrc = CRCVDL2(myData, myDataLen);
    myCrc = ((myCrc << 8) | (myCrc >> 8));
    Pid010b.crc=myCrc;
    /*-------------------Intégration des champs du buffer dans un flux d'envoie  streamPurge--------------------------*/

    QByteArray buffer;
    //  ecrire les données dans le flux d'envoie
    QDataStream streamPurge(&buffer, QIODevice::WriteOnly);

    if (false){

        streamPurge.setByteOrder(QDataStream::LittleEndian);

    }
    streamPurge << (uint8_t)Pid010b.synchro [0]<< (uint8_t)Pid010b.synchro [1]<<(uint8_t)Pid010b.synchro [2]
                <<(uint8_t)Pid010b.synchro [3]<<(uint8_t)Pid010b.protocoelCode<<(uint8_t)Pid010b.pid
                <<(uint8_t)Pid010b.donnees<<(uint8_t)Pid010b.sal<<(uint16_t)Pid010b.crc;

    /*--------------------------------Envoie des données--------------------------------------*/

    socketTcp1r->write(buffer);
    ui-> plainTextEdit_2 -> appendPlainText("PURGE_REQ");
    QByteArray ba_as_hex_string = buffer.toHex();
    ui->  plainTextEdit_4->appendPlainText(ba_as_hex_string);
}

/*-------------Cette action permets de se communiquer avec les deux radios -----------*/
void MainWindow1::on_pushButton_clicked()
{
           /*------------------------Se connecter à la radio---------------------------------------------------*/

            QString ip = ui->lineEditip->text();
            quint16 port = ui->spinBox_9->value();
            socketTcp1r->connectToHost(QHostAddress(ip),port);


            /*------------------------Se connecter à la radio---------------------------------------------------*/

            QString ip1 = ui->lineEditip_2->text();
            quint16 port1 = ui->spinBox_10->value();
            socketTcp1->connectToHost(QHostAddress(ip1),port1);

        /*-----------------------------------------------------------------------------------*/
        // Algo de traitement de la fréquence
        //Il faut créer une liste qui prend la taille de la valeur de fréquence désiré
        //mettre le nombre saisie dans le tableu sous forme de chiffre

        /*-----------------------------------------------------------------------------------------*/


        int T[6];
        int c = 0;
        int msb[2] , lsb[2] ;
        int monnombre = 118025;
        int mynumber = 0;
        int p =1 , e = 1 , o = 1;
        /*------------------mettre le nmbre saisie dans un tableau en forme de chiffre---------------------------------*/


        while(monnombre!=0) {
            int mod = monnombre%10;
            T[c] = mod;
            monnombre-=mod;
            monnombre/=10;
            c++;
        }

        /*------------------------Reconversion du tableau en chiffre-----------------------------------------------------*/
       for(int i = 1; i<5 ; i++){
            mynumber =  mynumber + T[i]*p;
            p=p*10;
        }

        msb[0] = T[4] ;
        msb[1] = T[3];

        lsb[0] = T[2];
        lsb[1] = T[1];

        uint8_t var = 0 ;
        for(int k=1;k>=0;k--) {
                var = var +msb[k]*e ; //freq_msb
                e = e*10 ;
        }
        uint8_t var2 = 0 ;
        for(int k=1;k>=0;k--) {
                var2 = var2 +lsb[k]*o ; //freq_lsb
                o = o*10 ;
        }

        uint8_t var1 = 13;
        uint8_t var3 =6;
        uint16_t var4 =137;
        uint8_t var5 = 9;

        uint16_t value = var4;
        uint8_t array[2];
        array[0]=value & 0xff;
        array[1]=(value >> 8);


        /*-------------Définition des champs de la trame et son envoie dans un fil------------- */


                        tsPid20 Pid20;

                        Pid20.synchro [0] = synchrovdl2[0];
                        Pid20.synchro [1] = synchrovdl2[1];
                        Pid20.synchro [2] = synchrovdl2[2];
                        Pid20.synchro [3] = synchrovdl2[3];
                        Pid20.mode = 0xF1;
                        Pid20.pid = 0x20;
                        Pid20.dfl1_msb =0x00;
                        Pid20.dfl2_lsb = 0x07;
                        Pid20.freq_msb=var;
                        Pid20.freq_lsb = var2;
                        Pid20.tm1 =var1;
                        Pid20.tm2 =var3;
                        Pid20.M1_lsb= array[0];
                        Pid20.M1_msb= array[1];
                        Pid20.persistance= var5;

                        /*-------------------------------Calcul du Crc16 Kermit-------------------------------------*/

                        unsigned char myData[] = {0xff, 0x00, 0xf0, 0xca, 0xf1, 0x20, 0x00, 0x07,var,var2,var1,var3,array[1], array[0],var5 };
                        unsigned int myDataLen;
                        unsigned short myCrc;
                        myDataLen = sizeof(myData);
                        myCrc = CRCVDL2(myData, myDataLen);
                        myCrc = ((myCrc << 8) | (myCrc >> 8));
                        Pid20.crc=myCrc;

                        /*-------------------Intégration des champs du buffer dans un fil d'envoie--------------------------*/


                        QByteArray buffer;

                        //  ecrire les données dans le flux d'envoie
                        QDataStream s(&buffer, QIODevice::WriteOnly);
                        if (false){

                            s.setByteOrder(QDataStream::LittleEndian);
                        }
                        s << (uint8_t)Pid20.synchro [0]<< (uint8_t)Pid20.synchro [1]<<(uint8_t)Pid20.synchro [2]<<(uint8_t)Pid20.synchro [3]
                          << (uint8_t)Pid20.mode << (uint8_t)Pid20.pid << (uint8_t)Pid20.dfl1_msb <<(uint8_t)Pid20.dfl2_lsb
                          <<(uint8_t)Pid20.freq_msb<<(uint8_t)Pid20.freq_lsb<<(uint8_t)Pid20.tm1<<(uint8_t)Pid20.tm2
                          <<(uint8_t)Pid20.M1_msb<<(uint8_t)Pid20.M1_lsb<<(uint8_t)Pid20.persistance<<(uint16_t)Pid20.crc;


                        /*--------------------------------Envoie des données--------------------------------------*/
                        socketTcp1->write(buffer);
                        socketTcp1r->write(buffer);
                        socketTcp1->flush();
                        socketTcp1r->flush();

}

//send n data to transmitter
void MainWindow1::on_pushButton_3_clicked()
{
    timer1->start(3000);

}

//stop the timer
void MainWindow1::on_pushButton_2_clicked()
{
    timer1->stop();

}
// la méthode start_timer qui contient le buffer unit_data_req utilisé dans le timer

void MainWindow1::start_timer1()
{
    /*------------------------Se connecter à la radio---------------------------------------------------*/

   QString ip = ui->lineEditip->text();
    quint16 port = ui->spinBox_9->value();

    socketTcp1r->connectToHost(QHostAddress(ip),port);

    /*-----------------------------structure de la PidF0----------------------------------------------------*/

    struct tsPid21{
        uint8_t synchro[SIZE_SYNCHRO_VDL2];
        uint8_t protocoleCode;
        uint8_t pid;
        uint8_t lengthMsb;
        uint8_t lengthLsb;
        uint8_t clic;
        uint16_t crc;
        char str1[16384];


    };

    /*la variable counter change à chaque fois on clique sur le button unit data req */
    countern++;

    /*--------------Définition des 16k valeur à pouvoir envoyer sous le protocole VDL 2------------------------*/

     uint16_t value = ui->donnees_2->value()+1;
     uint8_t array[2];
     array[0]=value & 0xff;
     array[1]=(value >> 8);


    char table22[ ui->donnees_2->value() ] ;
    unsigned char table222[ui->donnees_2->value()] ;

      for(int i=0; i< (ui->donnees_2->value()); i++){

        srand((unsigned) time(0));
        int strn =  (rand() % 9);
        unsigned char monsef = (unsigned char) strn;
        table22[i] = strn;

        table222[i] = monsef;

        }

         char str[16384];

         str[16383] = '\0';
         strncpy ( str, table22, ui-> donnees_2-> value() );
         QByteArray databuf = QByteArray(reinterpret_cast<char*>(table22), ui->donnees_2->value());


         /*-------------Définition des champs de la trame et l'intégrer dans un flus d'envoie------------- */

         tsPid21 Pid21;

         Pid21.synchro [0] = synchrovdl2[0];
         Pid21.synchro [1] = synchrovdl2[1];
         Pid21.synchro [2] = synchrovdl2[2];
         Pid21.synchro [3] = synchrovdl2[3];
         Pid21.protocoleCode = 0xF1;
         Pid21.pid = 0x21;
         Pid21.lengthMsb=array[1];
         Pid21.lengthLsb=array[0];
         Pid21.clic=countern;

         /*-------------------------------Calcul du Crc16 Kermit-------------------------------------*/


         unsigned char myData3[] ={0xff ,0x00, 0xf0, 0xca,0xf1, 0x21,array[1],array[0],countern};
         unsigned char allstrings[sizeof(table222)+sizeof(myData3)];
         memcpy(allstrings,myData3,sizeof(myData3));
         memcpy(allstrings+sizeof(myData3),table222,sizeof(table222));
         unsigned short myCrc;
         unsigned int myDataLen = sizeof(table222)+ sizeof(myData3);

         myCrc = CRCVDL2(allstrings, myDataLen);
         myCrc = ((myCrc << 8) | (myCrc >> 8));

         Pid21.crc=myCrc;



     /*-------------------Intégration des champs du buffer dans un flux d'envoie--------------------------*/


     QByteArray buffer;

     //  ecrire les données dans le flux d'envoie
     QDataStream s(&buffer, QIODevice::WriteOnly);

     if (false){

         s.setByteOrder(QDataStream::LittleEndian);
     }
     s << (uint8_t)Pid21.synchro [0]<< (uint8_t)Pid21.synchro [1]<<(uint8_t)Pid21.synchro [2]
       <<(uint8_t)Pid21.synchro [3]  << (uint8_t)Pid21.protocoleCode << (uint8_t)Pid21.pid
       << (uint8_t)Pid21.lengthMsb<< (uint8_t)Pid21.lengthLsb<<(uint8_t)Pid21.clic;




        QByteArray buffer3;
        //  ecrire les données dans le flux d'envoie

        QDataStream g(&buffer3, QIODevice::WriteOnly);

        if (false){

            g.setByteOrder(QDataStream::LittleEndian);
        }
        g <<(uint16_t) Pid21.crc;

        /*--------------------------------Envoie des données--------------------------------------*/

     QByteArray buffer5 = buffer + databuf + buffer3;

     socketTcp1r->write(buffer5);
     socketUdp1tr1->writeDatagram(buffer5,QHostAddress::LocalHost,0101);

     ui-> plainTextEdit_2 -> appendPlainText("UINT_DATA_REQ");

     QByteArray ba_as_hex_string = buffer5.toHex();
     ui->  plainTextEdit_4->appendPlainText(ba_as_hex_string);

     //socketd->writeDatagram(buffer5,QHostAddress::LocalHost,1964);
}


void MainWindow1::on_sqp_clicked()
{
    sqp1.show();

}


void MainWindow1::on_pushButton_4_clicked()
{
    transmission_ratio1.show();
}


void MainWindow1::on_actionVersion_triggered()
{
    version.show();
}


void MainWindow1::on_pushButton_5_clicked()
{
    if(socketUdp1tr1 == NULL)
        return;
    socketUdp1tr1 -> disconnectFromHost();
    socketUdp1tr1 -> close();
    socketUdp1tr1 = NULL ;
    //socketTcp1
    if(socketTcp1 == NULL)
        return;
    socketTcp1 -> disconnectFromHost();
    socketTcp1 -> close();
    socketTcp1 = NULL ;
    MainWindow1::close();

}


void MainWindow1::on_clear_clicked()
{
    ui->  plainTextEdit_4->clear();
    ui->  plainTextEdit_3->clear();
    ui->  plainTextEdit_2->clear();
    ui->  plainTextEdit->clear();
    ui->info->clear();
}




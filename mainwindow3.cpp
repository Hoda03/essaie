#include "mainwindow3.h"
#include "ui_mainwindow3.h"
#include <windows.h>

int counter_req=0;
int counter_conf =0;
int counter_ind=0;
int counter_sqp=0;

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
struct tsPidF0{
    uint8_t synchro[SIZE_SYNCHRO_ACARS];
    uint8_t protocoleCode;
    uint8_t pid;
    uint8_t lengthMsb;
    uint8_t lengthLsb;
    uint8_t mode;
    uint8_t req;
    uint16_t crc;

};
struct tsPid20 {

    uint8_t synchro[SIZE_SYNCHRO_ACARS];
    uint8_t protocoleCode;
    uint8_t pid;
    uint8_t lengthMsb;
    uint8_t lengthLsb;
    uint8_t freq_lsb;
    uint8_t freq_msb;
    uint8_t mod;
    uint8_t prekey;
    uint16_t crc;
};

MainWindow3::MainWindow3(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow3)
{
    ui->setupUi(this);    
    this->setWindowTitle("ACARS&UDP");
    this->setStyleSheet("QMainWindow { background-color : #ffffff}");

    //création du socket de communication en udp avec les radios
    socketUdp2 = new QUdpSocket(this);
    socketUdp2->bind(5252,QUdpSocket::ShareAddress);

    //création du socket de communication en localhost/udp pour envoyer les trame unit_data_req
    sockettranRatio = new QUdpSocket(this);
    sockettranRatio->bind(QHostAddress::LocalHost,3380);

    //création du socket pour envoyer la trame sqp_ind
    sockk = new QUdpSocket(this);

    /*--------------------Création d'un Timer pour l'envoie succèssives des données en localhost----------------------------------------*/
    timer3 = new QTimer(this);
    connect(timer3, &QTimer::timeout, this, &MainWindow3::start_timer3);


    /*---------------cette fonction connect permet de se connecter avec l'émetteur et lire ses réponses-----------------*/
    connect(socketUdp2,&QUdpSocket::readyRead,[&](){
       while (socketUdp2 ->hasPendingDatagrams() )
        {

            QByteArray data;
            QHostAddress sender;
            quint16 senderPort;
            data.resize(socketUdp2->pendingDatagramSize());
            socketUdp2->readDatagram(data.data(),data.size(),&sender, &senderPort);
            QByteArray ba_as_hex_string = data.toHex();

            // La réception de la trame de confirmation du changement de fréquence
            if ((data[5] == 80) && (data[8] >= 18))
            {
                   int PARAM_REQ = ui-> frequence_5 -> value();
                   ui-> plainTextEdit->appendPlainText(QString("PARAM CONF = %1").arg(PARAM_REQ));  // Affichage du text PARAM_CONF
                   ui->  plainTextEdit_4->appendPlainText(ba_as_hex_string); //Affichage des trames en héxadécimal


            }

            // La réception de la trame de confirmation du changement de puissance
            if (((data[8] == 47) || (data[8] == 44) || (data[8] == 40)) && ( data[9] == 1))
            {
                  int PWR_SET_REQ = ui -> puissance_5 -> value();
                  ui->plainTextEdit->appendPlainText(QString("PWR SET CONF = %1").arg(PWR_SET_REQ));
                  ui->  plainTextEdit_4->appendPlainText(ba_as_hex_string);

            }

            // La réception de la trame de confirmation de réception des données ACARS

            if( data[5] == 85 )
            {
                // counter pour préciser le nombre des trames Unit_data_conf recue.
                counter_conf++;
                ui->plainTextEdit->appendPlainText(QString("[%1] : UNIT DATA CONF").arg(counter_conf));
                ui->  plainTextEdit_4->appendPlainText(ba_as_hex_string);


            }
            // La réception de la trame de confirmation du changement du mode en acars


            if( data[9] == 1 && data[11] == 93)
            {
                ui->plainTextEdit->appendPlainText(("MODE SET CONF"));
                ui->  plainTextEdit_4->appendPlainText(ba_as_hex_string);
            }

            if(data[5] == 83)
            {
              ui->plainTextEdit->appendPlainText(QString("VDR ERROR IND"));
              ui->  plainTextEdit_4->appendPlainText(ba_as_hex_string);
            }

           if( data[5] == 81 )
           {
                counter_ind++;
                ui->plainTextEdit->appendPlainText(QString("[%1] : UNIT DATA IND").arg(counter_ind));
                ui->  plainTextEdit_4->appendPlainText(ba_as_hex_string);
                QByteArray transRatioData;
                QDataStream s(&transRatioData, QIODevice::WriteOnly);
                  if (false){

                      s.setByteOrder(QDataStream::LittleEndian);
                  }
                  for (int i=10; i < (ui-> donneesAcars -> value()) + 38 ;i++){

                  s << data[i];


              }
              sockettranRatio->writeDatagram(transRatioData,QHostAddress::LocalHost, 7878);

            }

           // récéption de la trame de qualité du signal

           if( data[5] == 32  && data[4] == 95)
           {
              counter_sqp++;
              ui->plainTextEdit->appendPlainText(QString("[%1] : SQP IND").arg(counter_sqp));
              ui->  plainTextEdit_4->appendPlainText(ba_as_hex_string);
              sockk->writeDatagram(data,QHostAddress::LocalHost, 1236);

           }

        }
    });


}

/*--------------Cette Fonction nour permet de calculer le CRC16 Kermit---------------------*/
uint16_t CRC16kK(uint8_t *x, uint8_t len) {
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


MainWindow3::~MainWindow3()
{
    delete ui;
}

/*------------------Cette Fonction nous permet d'envoyer un changement de fréquence à la radios/EM ----------------------*/

void MainWindow3::on_actionPARAM_REQ_triggered()
{
    /*-----------------------------------------------------------------------------------*/
    // Algo de traitement de la fréquence
    //Il faut créer une liste qui prend la taille de la valeur de fréquence désiré
    //mettre le nombre saisie dans le tableu sous forme de chiffre

    /*-----------------------------------------------------------------------------------------*/


    int T[6];
    int c = 0;
    int msb[2] , lsb[2] ;
    int monnombre = ui->frequence_5->value();
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
            var = var +msb[k]*e ;
            e = e*10 ;
    }
    uint8_t var2 = 0 ;
    for(int k=1;k>=0;k--) {
            var2 = var2 +lsb[k]*o ;
            o = o*10 ;
    }

    /*------------------La valeur du Prekey--------------------------*/

    uint8_t var4;
    if (ui -> prekey_5 -> value() >= 0 && ui -> prekey_5 -> value() <= 85 ){
        var4 = ui -> prekey_5-> value();
    }
    else{

        var4 = 37;
    }
    /*-------------------------------Calcul du Crc16 Kermit-------------------------------------*/

    unsigned char myData[] = {0xff, 0x00, 0xf0, 0xca, 0xf2, 0x20, 0x00, 0x04,var,var2,0x00,var4};
    unsigned int myDataLen;
    unsigned short myCrc;
    myDataLen = sizeof(myData);
    myCrc = CRC16kK(myData, myDataLen);
    myCrc = ((myCrc << 8) | (myCrc >> 8));

    /*-------------Définition des champs de la trame et son envoie dans un fil------------- */

    if  ( ui -> frequence_5 -> value() >= 118000  &&  ui -> frequence_5 -> value() <= 136000 )
            {

                tsPid20 Pid20;
                Pid20.synchro [0] = synchroAcars[0];
                Pid20.synchro [1] = synchroAcars[1];
                Pid20.synchro [2] = synchroAcars[2];
                Pid20.synchro [3] = synchroAcars[3];
                Pid20.protocoleCode = 0xF2;
                Pid20.pid = 0x20;
                Pid20.lengthMsb =0x00;
                Pid20.lengthLsb = 0x04;
                Pid20.freq_msb = var2;
                Pid20.freq_lsb = var;
                Pid20.mod = 0x00;
                Pid20.prekey=var4;
                Pid20.crc=myCrc;



                QByteArray bufferAUfreq;
                //  ecrire les données dans le flux d'envoie streamDataUdp
                QDataStream streamfreqUdp(&bufferAUfreq, QIODevice::WriteOnly);
                if (false){

                    streamfreqUdp.setByteOrder(QDataStream::LittleEndian);
                }

                streamfreqUdp << (uint8_t)Pid20.synchro [0]<< (uint8_t)Pid20.synchro [1]<<(uint8_t)Pid20.synchro [2]<<(uint8_t)Pid20.synchro [3]
                              << (uint8_t)Pid20.protocoleCode << (uint8_t)Pid20.pid << (uint8_t)Pid20.lengthMsb <<(uint8_t)Pid20.lengthLsb
                              <<(uint8_t)Pid20.freq_lsb<<(uint8_t)Pid20.freq_msb<<(uint8_t)Pid20.mod<<(uint8_t)Pid20.prekey<<(uint16_t)Pid20.crc;

                /*--------------------------------Envoie des données--------------------------------------*/
                QString ip = ui->hostaddress1_TX->text();
                short port = ui->port1_TX->value();
                socketUdp2->writeDatagram(bufferAUfreq,QHostAddress(ip), port);
                int PARAM_REQ = ui-> frequence_5 -> value();
                ui->  plainTextEdit_2->appendPlainText(QString("PARAM_REQ_TX = %1").arg(PARAM_REQ));
                QByteArray ba_as_hex_string = bufferAUfreq.toHex();
                ui->  plainTextEdit_3->appendPlainText(ba_as_hex_string);

                }
}

/*------------------Cette Fonction nous permet d'envoyer un changement de fréquence à la radios/RX ----------------------*/

void MainWindow3::on_actionPARAM_REQ_2_triggered()
{
    /*-----------------------------------------------------------------------------------*/
    // Algo de traitement de la fréquence
    //Il faut créer une liste qui prend la taille de la valeur de fréquence désiré
    //mettre le nombre saisie dans le tableu sous forme de chiffre
    /*-----------------------------------------------------------------------------------------*/

    int T[6];
    int c = 0;
    int msb[2] , lsb[2] ;
    int monnombre = ui->frequence_5->value();
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
            var = var +msb[k]*e ;
            e = e*10 ;
    }
    uint8_t var2 = 0 ;
    for(int k=1;k>=0;k--) {
            var2 = var2 +lsb[k]*o ;
            o = o*10 ;
    }

    /*------------------La valeur du Prekey--------------------------*/

    uint8_t var4;
    if (ui -> prekey_5 -> value() >= 0 && ui -> prekey_5 -> value() <= 85 ){
        var4 = ui -> prekey_5-> value();
    }
    else{

        var4 = 37;
    }
    /*-------------------------------Calcul du Crc16 Kermit-------------------------------------*/

    unsigned char myData[] = {0xff, 0x00, 0xf0, 0xca, 0xf2, 0x20, 0x00, 0x04,var,var2,0x00,var4};
    unsigned int myDataLen;
    unsigned short myCrc;
    myDataLen = sizeof(myData);
    myCrc = CRC16kK(myData, myDataLen);
    myCrc = ((myCrc << 8) | (myCrc >> 8));

    /*-------------Définition des champs de la trame et son envoie dans un fil------------- */

    if  ( ui -> frequence_5 -> value() >= 118000  &&  ui -> frequence_5 -> value() <= 136000 )
            {

                tsPid20 Pid20;
                Pid20.synchro [0] = synchroAcars[0];
                Pid20.synchro [1] = synchroAcars[1];
                Pid20.synchro [2] = synchroAcars[2];
                Pid20.synchro [3] = synchroAcars[3];
                Pid20.protocoleCode = 0xF2;
                Pid20.pid = 0x20;
                Pid20.lengthMsb =0x00;
                Pid20.lengthLsb = 0x04;
                Pid20.freq_msb = var2;
                Pid20.freq_lsb = var;
                Pid20.mod = 0x00;
                Pid20.prekey=var4;
                Pid20.crc=myCrc;

                QByteArray bufferAcarsUdpfreq;
                //  ecrire les données dans le flux d'envoie streamFreqR
                QDataStream streamFreqR(&bufferAcarsUdpfreq, QIODevice::WriteOnly);
                if (false){

                    streamFreqR.setByteOrder(QDataStream::LittleEndian);
                }

                streamFreqR << (uint8_t)Pid20.synchro [0]<< (uint8_t)Pid20.synchro [1]<<(uint8_t)Pid20.synchro [2]<<(uint8_t)Pid20.synchro [3]
                            << (uint8_t)Pid20.protocoleCode << (uint8_t)Pid20.pid << (uint8_t)Pid20.lengthMsb <<(uint8_t)Pid20.lengthLsb
                            <<(uint8_t)Pid20.freq_lsb<<(uint8_t)Pid20.freq_msb<<(uint8_t)Pid20.mod<<(uint8_t)Pid20.prekey<<(uint16_t)Pid20.crc;

                /*--------------------------------Envoie des données--------------------------------------*/

                QString ip = ui->hostaddress1_RX->text();                      //Get the other party's ip
                short port = ui->port1_RX->value();
                socketUdp2->writeDatagram(bufferAcarsUdpfreq,QHostAddress(ip), port);
                int PARAM_REQ = ui-> frequence_5 -> value();
                ui->  plainTextEdit_2->appendPlainText(QString("PARAM_REQ_TX = %1").arg(PARAM_REQ));
                QByteArray ba_as_hex_string = bufferAcarsUdpfreq.toHex();
                ui->  plainTextEdit_3->appendPlainText(ba_as_hex_string);

            }
}


void MainWindow3::on_actionUNIT_DATA_EQ_triggered()
{

    //counter pour afficher le nombre d'envoie
    counter_req++;

    uint8_t varh = ui-> donneesAcars -> value() + 28;
    unsigned char str[] ={
    0x4c, 0x27, 0x20, 0x41, 0x43, 0x41, 0x52, 0x53 , 0x20, 0x41,
    0x69, 0x72, 0x63, 0x72, 0x61, 0x66, 0x74, 0x20, 0x43, 0x6f,
    0x6d, 0x6d, 0x75,0x6e,  0x69, 0x63, 0x61, 0x74 ,0x69, 0x6f,
    0x6e, 0x20, 0x41 ,0x64, 0x64, 0x72, 0x65, 0x73, 0x73, 0x69,
    0x6e, 0x67, 0x20 ,0x61, 0x6e, 0x64, 0x20 ,0x52, 0x65, 0x70,
    0x6f, 0x72, 0x74, 0x69 ,0x6e, 0x67,  0x20, 0x53 ,0x79, 0x73,
    0x74, 0x65, 0x6d, 0x29, 0x20, 0x65 ,0x73 ,0x74 ,0x20,0x75,
    0x6e, 0x20,  0x73, 0x79, 0x73, 0x74, 0x74, 0x74, 0x74 ,0x74,
    0x6e, 0x20,  0x73, 0x79, 0x73, 0x74, 0x74, 0x74, 0x74 ,0x74,
    0x6e, 0x20,  0x73, 0x79, 0x73, 0x74, 0x74, 0x74, 0x74 ,0x74,
    0x6e, 0x20,  0x73, 0x79, 0x73, 0x74, 0x74, 0x74, 0x74 ,0x74,
    0x6e, 0x20,  0x73, 0x79, 0x73, 0x74, 0x74, 0x74, 0x74 ,0x74,
    0x6e, 0x20,  0x73, 0x79, 0x73, 0x74, 0x74, 0x74, 0x74 ,0x74,
    0x6e, 0x20,  0x73, 0x79, 0x73, 0x74, 0x74, 0x74, 0x74 ,0x74,
    0x6e, 0x20,  0x73, 0x79, 0x73, 0x74, 0x74, 0x74, 0x74 ,0x74,
    0x6e, 0x20,  0x73, 0x79, 0x73, 0x74, 0x74, 0x74, 0x74 ,0x74,
    0x6e, 0x20,  0x73, 0x79, 0x73, 0x74, 0x74, 0x74, 0x74 ,0x74,
    0x6e, 0x20,  0x73, 0x79, 0x73, 0x74, 0x74, 0x74, 0x74 ,0x74,
    0x6e, 0x20,  0x73, 0x79, 0x73, 0x74, 0x74, 0x74, 0x74 ,0x74,
    0x6e, 0x20,  0x73, 0x79, 0x73, 0x74, 0x74, 0x74, 0x74 ,0x74,
    0x6e, 0x20,  0x73, 0x79, 0x73, 0x74, 0x74, 0x74, 0x74 ,0x74,
    0x6c, 0x69, 0x74 ,0x65, 0x20, 0x68,0x7f,0x64,0x61,0x61};


    unsigned char str2[ui-> donneesAcars->value()];
    memcpy(str2, str,sizeof(str2));
    QByteArray databuf = QByteArray((char*)str2, sizeof(str2));

    /*--------------------------- initialisation des valeurs de chaque paramètre de la structure du PARAM_DATA_REQ -------------------------------*/
    tsPid21 Pid21;
    Pid21.synchro [0] = synchroAcars[0];
    Pid21.synchro [1] = synchroAcars[1];
    Pid21.synchro [2] = synchroAcars[2];
    Pid21.synchro [3] = synchroAcars[3];
    Pid21.protocoleCode = 0xF2;
    Pid21.pid = 0x21;
    Pid21.lengthMsb=0x00;
    Pid21.lengthLsb=varh;
    Pid21.soh = 0x01;
    Pid21.mode = 0x02;
    Pid21.address1=0x2e;
    Pid21.address2=0x2e;
    Pid21.address3=0x6e;
    Pid21.address4=0x33;
    Pid21.address5=0x38;
    Pid21.address6=0x37;
    Pid21.address7=0x32;
    Pid21.ack = 0x01;
    Pid21.label1 = 0x01;
    Pid21.label2 = 0x00;
    Pid21.bi = 0x01;
    Pid21.stx = 0x01;
    Pid21.msn=0x31;
    Pid21.flightid=0x58;
    Pid21.suff = 0x03;


    /*---------------------------- Cette Fonction Permettre d'écrire les données dans un flux d'envoie streamDataAU ---------------------------------*/
    QByteArray buffer;
    QDataStream streamDataAU(&buffer, QIODevice::WriteOnly);

    if (false){

        streamDataAU.setByteOrder(QDataStream::LittleEndian);
    }
    streamDataAU  << (uint8_t)Pid21.synchro [0]<< (uint8_t)Pid21.synchro [1]<<(uint8_t)Pid21.synchro [2]
                  <<(uint8_t)Pid21.synchro [3]  << (uint8_t)Pid21.protocoleCode << (uint8_t)Pid21.pid
                  << (uint8_t)Pid21.lengthMsb<< (uint8_t)Pid21.lengthLsb<<(uint8_t)Pid21.soh
                  <<(uint8_t)Pid21.mode<<(uint8_t)Pid21.address1<<(uint8_t)Pid21.address2<<(uint8_t)Pid21.address3
                  <<(uint8_t)Pid21.address4<<(uint8_t)Pid21.address5<<(uint8_t)Pid21.address6
                  <<(uint8_t)Pid21.address7<<(uint8_t)Pid21.ack<<(uint8_t)Pid21.label1<<(uint8_t)Pid21.label2
                  <<(uint8_t)Pid21.bi<<(uint8_t)Pid21.stx<<(uint8_t)Pid21.msn<<(uint8_t)Pid21.msn<<(uint8_t)Pid21.msn<<(uint8_t)Pid21.msn
                  <<(uint8_t)Pid21.flightid<<(uint8_t)Pid21.flightid<<(uint8_t)Pid21.flightid<<(uint8_t)Pid21.flightid<<(uint8_t)Pid21.flightid
                  <<(uint8_t)Pid21.flightid;



    /*-------------------------------Calcul du Crc16 Kermit-------------------------------------*/
    unsigned char myData[] ={
    0xff ,0x00, 0xf0, 0xca, 0xf2, 0x21, 0x00 ,varh , 0x01,0x02,
    0x2e , 0x2e , 0x6e , 0x33 , 0x38 , 0x37,0x32 , 0x01 , 0x01,
    0x00 ,0x01, 0x01,0x31,0x31,0x31,0x31,0x58,0x58,0x58,0x58,
    0x58,0x58,    0x4c, 0x27, 0x20, 0x41, 0x43, 0x41, 0x52, 0x53 , 0x20, 0x41,
    0x69, 0x72, 0x63, 0x72, 0x61, 0x66, 0x74, 0x20, 0x43, 0x6f,
    0x6d, 0x6d, 0x75,0x6e,  0x69, 0x63, 0x61, 0x74 ,0x69, 0x6f,
    0x6e, 0x20, 0x41 ,0x64, 0x64, 0x72, 0x65, 0x73, 0x73, 0x69,
    0x6e, 0x67, 0x20 ,0x61, 0x6e, 0x64, 0x20 ,0x52, 0x65, 0x70,
    0x6f, 0x72, 0x74, 0x69 ,0x6e, 0x67,  0x20, 0x53 ,0x79, 0x73,
    0x74, 0x65, 0x6d, 0x29, 0x20, 0x65 ,0x73 ,0x74 ,0x20,0x75,
    0x6e, 0x20,  0x73, 0x79, 0x73, 0x74, 0x74, 0x74, 0x74 ,0x74,
    0x6e, 0x20,  0x73, 0x79, 0x73, 0x74, 0x74, 0x74, 0x74 ,0x74,
    0x6e, 0x20,  0x73, 0x79, 0x73, 0x74, 0x74, 0x74, 0x74 ,0x74,
    0x6e, 0x20,  0x73, 0x79, 0x73, 0x74, 0x74, 0x74, 0x74 ,0x74,
    0x6e, 0x20,  0x73, 0x79, 0x73, 0x74, 0x74, 0x74, 0x74 ,0x74,
    0x6e, 0x20,  0x73, 0x79, 0x73, 0x74, 0x74, 0x74, 0x74 ,0x74,
    0x6e, 0x20,  0x73, 0x79, 0x73, 0x74, 0x74, 0x74, 0x74 ,0x74,
    0x6e, 0x20,  0x73, 0x79, 0x73, 0x74, 0x74, 0x74, 0x74 ,0x74,
    0x6e, 0x20,  0x73, 0x79, 0x73, 0x74, 0x74, 0x74, 0x74 ,0x74,
    0x6e, 0x20,  0x73, 0x79, 0x73, 0x74, 0x74, 0x74, 0x74 ,0x74,
    0x6e, 0x20,  0x73, 0x79, 0x73, 0x74, 0x74, 0x74, 0x74 ,0x74,
    0x6e, 0x20,  0x73, 0x79, 0x73, 0x74, 0x74, 0x74, 0x74 ,0x74,
    0x6e, 0x20,  0x73, 0x79, 0x73, 0x74, 0x74, 0x74, 0x74 ,0x74,
    0x6e, 0x20,  0x73, 0x79, 0x73, 0x74, 0x74, 0x74, 0x74 ,0x74,
    0x6c, 0x69, 0x74 ,0x65, 0x20, 0x68,0x7f,0x64,0x61,0x61};

   //nouveau atbleau ou on ibnsère le nombre saisie dans l'interface graphique de caractère
   unsigned char myData1[ui-> donneesAcars->value()+32];

   // on copie les valeurs dans le nouveau tableau
   memcpy(myData1, myData,sizeof(myData1));

   // un tableau qui contien la valeur 0x03
   unsigned char myData3[] ={0x03,0x02,0x02,0x7f};
   QByteArray databuf3 = QByteArray((char*)myData3, sizeof(myData3));

   // on conbine entre les deux tableau pour l'intégrer deans la fonction qui calcule le crc
   unsigned char allstrings[sizeof(myData1)+sizeof(myData3)];
   memcpy(allstrings,myData1,sizeof(myData1));
   memcpy(allstrings+sizeof(myData1),myData3,sizeof(myData3));

   unsigned short myCrc;
   int myDataLen = sizeof(myData1)+ sizeof(myData3);
   myCrc = CRC16kK(allstrings, myDataLen);
   myCrc = ((myCrc << 8) | (myCrc >> 8));

   Pid21.crc=myCrc;
   QByteArray bufferCrc;
   QDataStream streamCrc(&bufferCrc, QIODevice::WriteOnly);

   if (false){

       streamCrc.setByteOrder(QDataStream::LittleEndian);
   }
   streamCrc<< (uint16_t)Pid21.crc;


   /*--------------------------------Envoie des données--------------------------------------*/
   QByteArray buffer2 ;
   buffer2 = buffer;
   buffer2 = buffer2.append(databuf);
   buffer2 = buffer2.append(databuf3);
   buffer2 = buffer2 + bufferCrc;

   QByteArray ba_as_hex_string = buffer2.toHex();
   QString ip = ui->hostaddress1_TX->text();
   short port = ui->port1_TX->value();

   socketUdp2->writeDatagram(buffer2,QHostAddress(ip), port);
   sockettranRatio->writeDatagram(buffer2,QHostAddress::LocalHost, 7878);


   ui-> plainTextEdit_2->appendPlainText(QString("[%1] : UNIT_DATA_REQ").arg(counter_req));
   ui->  plainTextEdit_3->appendPlainText(ba_as_hex_string);
}

/*------------------Cette Fonction nous permet d'envoyer un changement de puissance à la radios/EM ----------------------*/

void MainWindow3::on_actionPWR_SET_REQ_triggered()
{
    /*----------------instruction conditionnelle pour l'envoie de la puissance à la Radio------------------------*/
    int pwr = ui -> puissance_5 -> value();
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
    PidF1.synchro [0] = synchroAcars[0];
    PidF1.synchro [1] = synchroAcars[1];
    PidF1.synchro [2] = synchroAcars[2];
    PidF1.synchro [3] = synchroAcars[3];
    PidF1.protocoleCode = 0xF2;
    PidF1.pid = 0xF1;
    PidF1.lengthMsb =0x00;
    PidF1.lengthLsb = 0x02;
    PidF1.pwr = var3;
    PidF1.req = 0x00;

    /*-------------------------------Calcul du Crc16 Kermit-------------------------------------*/
    unsigned char myData[] = {0xff, 0x00, 0xf0, 0xca, 0xf2, 0xf1, 0x00, 0x02,var3,0x00};
    unsigned int myDataLen;
    unsigned short myCrc;
    myDataLen = sizeof(myData);
    myCrc = CRC16kK(myData, myDataLen);
    myCrc = ((myCrc << 8) | (myCrc >> 8));
    PidF1.crc = myCrc;


    /*-------------------Intégration des champs du buffer dans un fil d'envoie streamPwr--------------------------*/
    QByteArray buffer;
    QDataStream streamPwr(&buffer, QIODevice::WriteOnly);

    if (false){

        streamPwr.setByteOrder(QDataStream::LittleEndian);
    }
       streamPwr << (uint8_t)PidF1.synchro [0]<< (uint8_t)PidF1.synchro [1]<<(uint8_t)PidF1.synchro [2]
                 <<(uint8_t)PidF1.synchro [3]  << (uint8_t)PidF1.protocoleCode << (uint8_t)PidF1.pid
                 << (uint8_t)PidF1.lengthMsb <<(uint8_t)PidF1.lengthLsb<<(uint8_t)PidF1.pwr
                 <<(uint8_t)PidF1.req<<(uint16_t)PidF1.crc;


    /*--------------------------------Envoie des données--------------------------------------*/
   QString ip = ui->hostaddress1_TX->text();
   short port = ui->port1_TX->value();
   socketUdp2->writeDatagram(buffer,QHostAddress(ip), port);
   int PWR_SET_REQ = ui -> puissance_5-> value();
   ui->  plainTextEdit_2->appendPlainText(QString("PWR_SET_REQ = %1").arg(PWR_SET_REQ));
   QByteArray ba_as_hex_string = buffer.toHex();
   ui->  plainTextEdit_3->appendPlainText(ba_as_hex_string);

}

/*------------------Cette Fonction nous permet d'envoyer un changement de mode à la radios/EM ----------------------*/

void MainWindow3::on_actionMODE_SET_REQ_triggered()
{
    /*-------------Définition des champs de la trame ------------- */

    tsPidF0 PidF0;
    PidF0.synchro [0] = synchroAcars[0];
    PidF0.synchro [1] = synchroAcars[1];
    PidF0.synchro [2] = synchroAcars[2];
    PidF0.synchro [3] = synchroAcars[3];
    PidF0.protocoleCode = 0xF2;
    PidF0.pid = 0xF0;
    PidF0.lengthMsb =0x00;
    PidF0.lengthLsb = 0x02;
    PidF0.mode = 0xF1;
    PidF0.req = 0x00;

    /*-------------------------------Calcul du Crc16 Kermit-------------------------------------*/


    unsigned char myData[] = {0xff, 0x00, 0xf0, 0xca, 0xf2, 0xf0, 0x00, 0x02,0xF1, 0x00};
    unsigned int myDataLen;
    unsigned short myCrc;
    myDataLen = sizeof(myData);
    myCrc = CRC16kK(myData, myDataLen);
    myCrc = ((myCrc << 8) | (myCrc >> 8));
    PidF0.crc=myCrc;

    /*-------------------Intégration des champs du buffer dans un fil d'envoie streamModeE--------------------------*/
    QByteArray buffer;
    //  ecrire les données dans le flux d'envoie
    QDataStream streamModeE(&buffer, QIODevice::WriteOnly);

    if (false){

        streamModeE.setByteOrder(QDataStream::LittleEndian);
    }
    streamModeE << (uint8_t)PidF0.synchro [0]<< (uint8_t)PidF0.synchro [1]<<(uint8_t)PidF0.synchro [2]<<(uint8_t)PidF0.synchro [3]
                << (uint8_t)PidF0.protocoleCode << (uint8_t)PidF0.pid << (uint8_t)PidF0.lengthMsb <<(uint8_t)PidF0.lengthLsb
                <<(uint8_t)PidF0.mode<<(uint8_t)PidF0.req<<(uint16_t)PidF0.crc;

    /*--------------------------------Envoie des données--------------------------------------*/

    QString ip = ui->hostaddress1_TX->text();                      //Get the other party's ip
    short port = ui->port1_TX->value();
    socketUdp2->writeDatagram(buffer,QHostAddress(ip), port);
    ui->  plainTextEdit_2->appendPlainText("MODE_SET_REQ_TX");
    QByteArray ba_as_hex_string = buffer.toHex();
    ui->  plainTextEdit_3->appendPlainText(ba_as_hex_string);



}

/*------------------Cette Fonction nous permet d'envoyer un changement de mode à la radio/RX ----------------------*/

void MainWindow3::on_actionMODE_SET_REQ_2_triggered()
{
    /*-------------Définition des champs de la trame ------------- */

    tsPidF0 PidF0;
    PidF0.synchro [0] = synchroAcars[0];
    PidF0.synchro [1] = synchroAcars[1];
    PidF0.synchro [2] = synchroAcars[2];
    PidF0.synchro [3] = synchroAcars[3];
    PidF0.protocoleCode = 0xF2;
    PidF0.pid = 0xF0;
    PidF0.lengthMsb =0x00;
    PidF0.lengthLsb = 0x02;
    PidF0.mode = 0xF1;
    PidF0.req = 0x00;

    /*-------------------------------Calcul du Crc16 Kermit-------------------------------------*/


    unsigned char myData[] = {0xff, 0x00, 0xf0, 0xca, 0xf2, 0xf0, 0x00, 0x02,0xF1, 0x00};
    unsigned int myDataLen;
    unsigned short myCrc;
    myDataLen = sizeof(myData);
    myCrc = CRC16kK(myData, myDataLen);
    myCrc = ((myCrc << 8) | (myCrc >> 8));
    PidF0.crc=myCrc;

    /*-------------------Intégration des champs du buffer dans un fil d'envoie streamModeR--------------------------*/


    QByteArray buffer;
    //  ecrire les données dans le flux d'envoie
    QDataStream streamModeR(&buffer, QIODevice::WriteOnly);

    if (false){

        streamModeR.setByteOrder(QDataStream::LittleEndian);
    }
    streamModeR << (uint8_t)PidF0.synchro [0]<< (uint8_t)PidF0.synchro [1]<<(uint8_t)PidF0.synchro [2]<<(uint8_t)PidF0.synchro [3]
                << (uint8_t)PidF0.protocoleCode << (uint8_t)PidF0.pid << (uint8_t)PidF0.lengthMsb <<(uint8_t)PidF0.lengthLsb
                <<(uint8_t)PidF0.mode<<(uint8_t)PidF0.req<<(uint16_t)PidF0.crc;

    /*--------------------------------Envoie des données--------------------------------------*/

    QString ip = ui->hostaddress1_RX->text();
    short port = ui->port1_RX->value();
    socketUdp2->writeDatagram(buffer,QHostAddress(ip), port);
    ui->  plainTextEdit_2->appendPlainText("MODE_SET_REQ_TX");
    QByteArray ba_as_hex_string = buffer.toHex();
    ui->  plainTextEdit_3->appendPlainText(ba_as_hex_string);

}


void MainWindow3::on_actionPURGE_REQ_triggered()
{
    /*-------------Définition dela structure de la trame ------------- */

    struct tsPid25
      {
        uint8_t synchro[SIZE_SYNCHRO_ACARS];
        uint8_t protocoleCode;
        uint8_t pid;
        uint8_t lengthMsb;
        uint8_t lengthLsb;
        uint16_t crc;

      };

    /*-------------Définition des champs de la trame ------------- */
    tsPid25 Pid25;
    Pid25.synchro [0] = synchroAcars[0];
    Pid25.synchro [1] = synchroAcars[1];
    Pid25.synchro [2] = synchroAcars[2];
    Pid25.synchro [3] = synchroAcars[3];
    Pid25.protocoleCode = 0xF2;
    Pid25.pid = 0x25;
    Pid25.lengthLsb =0x00;
    Pid25.lengthMsb = 0x00;

    /*-------------------------------Calcul du Crc16 Kermit-------------------------------------*/
    unsigned char myData[] = {0xff, 0x00, 0xf0, 0xca, 0xf2, 0x25, 0x00, 0x00};
    unsigned int myDataLen;
    unsigned short myCrc;
    myDataLen = sizeof(myData);
    myCrc = CRC16kK(myData, myDataLen);
    myCrc = ((myCrc << 8) | (myCrc >> 8));
    Pid25.crc = myCrc;

    /*-------------------Intégration des champs du buffer dans un flux d'envoie streamPurge--------------------------*/
    QByteArray buffer;
    QDataStream streamPurge(&buffer, QIODevice::WriteOnly);

    if (false){

        streamPurge.setByteOrder(QDataStream::LittleEndian);
    }
    streamPurge << (uint8_t)Pid25.synchro [0]<< (uint8_t)Pid25.synchro [1]<<(uint8_t)Pid25.synchro [2]<<(uint8_t)Pid25.synchro [3]
                << (uint8_t)Pid25.protocoleCode << (uint8_t)Pid25.pid << (uint8_t)Pid25.lengthMsb <<(uint8_t)Pid25.lengthLsb<<(uint16_t)Pid25.crc;

    /*--------------------------------Envoie des données--------------------------------------*/
    QString ip = ui->hostaddress1_TX->text();                      //Get the other party's ip
    short port = ui->port1_TX->value();
    socketUdp2->writeDatagram(buffer,QHostAddress(ip), port);
    ui->  plainTextEdit_2->appendPlainText("Purge_REQ");
    QByteArray ba_as_hex_string = buffer.toHex();
    ui->  plainTextEdit_3->appendPlainText(ba_as_hex_string);
}


void MainWindow3::on_pushButton_clicked()
{
   timer3->start(1000);

}


void MainWindow3::on_SQP_clicked()
{
    sqp3.show();
}

//Arreter l'envoie des donnees
void MainWindow3::on_pushButton_2_clicked()
{
    timer3->stop();
}
// la méthode start_timer qui contient le buffer unit_data_req utilisé dans le timer
void MainWindow3::start_timer3()
{
    counter_req++;
    //uint8_t var6 = ((ui-> spinBox -> value()));
    uint8_t varh = ui-> donneesAcars -> value() + 28;
    unsigned char str[] ={
    0x4c, 0x27, 0x20, 0x41, 0x43, 0x41, 0x52, 0x53 , 0x20, 0x41,
    0x69, 0x72, 0x63, 0x72, 0x61, 0x66, 0x74, 0x20, 0x43, 0x6f,
    0x6d, 0x6d, 0x75,0x6e,  0x69, 0x63, 0x61, 0x74 ,0x69, 0x6f,
    0x6e, 0x20, 0x41 ,0x64, 0x64, 0x72, 0x65, 0x73, 0x73, 0x69,
    0x6e, 0x67, 0x20 ,0x61, 0x6e, 0x64, 0x20 ,0x52, 0x65, 0x70,
    0x6f, 0x72, 0x74, 0x69 ,0x6e, 0x67,  0x20, 0x53 ,0x79, 0x73,
    0x74, 0x65, 0x6d, 0x29, 0x20, 0x65 ,0x73 ,0x74 ,0x20,0x75,
    0x6e, 0x20,  0x73, 0x79, 0x73, 0x74, 0x74, 0x74, 0x74 ,0x74,
    0x6e, 0x20,  0x73, 0x79, 0x73, 0x74, 0x74, 0x74, 0x74 ,0x74,
    0x6e, 0x20,  0x73, 0x79, 0x73, 0x74, 0x74, 0x74, 0x74 ,0x74,
    0x6e, 0x20,  0x73, 0x79, 0x73, 0x74, 0x74, 0x74, 0x74 ,0x74,
    0x6e, 0x20,  0x73, 0x79, 0x73, 0x74, 0x74, 0x74, 0x74 ,0x74,
    0x6e, 0x20,  0x73, 0x79, 0x73, 0x74, 0x74, 0x74, 0x74 ,0x74,
    0x6e, 0x20,  0x73, 0x79, 0x73, 0x74, 0x74, 0x74, 0x74 ,0x74,
    0x6e, 0x20,  0x73, 0x79, 0x73, 0x74, 0x74, 0x74, 0x74 ,0x74,
    0x6e, 0x20,  0x73, 0x79, 0x73, 0x74, 0x74, 0x74, 0x74 ,0x74,
    0x6e, 0x20,  0x73, 0x79, 0x73, 0x74, 0x74, 0x74, 0x74 ,0x74,
    0x6e, 0x20,  0x73, 0x79, 0x73, 0x74, 0x74, 0x74, 0x74 ,0x74,
    0x6e, 0x20,  0x73, 0x79, 0x73, 0x74, 0x74, 0x74, 0x74 ,0x74,
    0x6e, 0x20,  0x73, 0x79, 0x73, 0x74, 0x74, 0x74, 0x74 ,0x74,
    0x6e, 0x20,  0x73, 0x79, 0x73, 0x74, 0x74, 0x74, 0x74 ,0x74,
    0x6c, 0x69, 0x74 ,0x65, 0x20, 0x68,0x7f,0x64,0x61,0x61};

    unsigned char str2[ui-> donneesAcars->value()];

    memcpy(str2, str,sizeof(str2));
    QByteArray databuf = QByteArray((char*)str2, sizeof(str2));

    /*--------------------------- initialisation des valeurs de chaque paramètre de la structure du PARAM_DATA_REQ -------------------------------*/

    tsPid21 Pid21;
    Pid21.synchro [0] = synchroAcars[0];
    Pid21.synchro [1] = synchroAcars[1];
    Pid21.synchro [2] = synchroAcars[2];
    Pid21.synchro [3] = synchroAcars[3];
    Pid21.protocoleCode = 0xF2;
    Pid21.pid = 0x21;
    Pid21.lengthMsb=0x00;
    Pid21.lengthLsb=varh;
    Pid21.soh = 0x01;
    Pid21.mode = 0x02;

    Pid21.address1=0x2e;
    Pid21.address2=0x2e;
    Pid21.address3=0x6e;
    Pid21.address4=0x33;
    Pid21.address5=0x38;
    Pid21.address6=0x37;
    Pid21.address7=0x32;
    Pid21.ack = 0x01;
    Pid21.label1 = 0x01;
    Pid21.label2 = 0x00;
    Pid21.bi = 0x01;
    Pid21.stx = 0x01;
    Pid21.msn=0x31;
    Pid21.flightid=0x58;
    //strncpy ((char *)Pid21.str1,str, longeur );
    Pid21.suff = 0x03;

    /*---------------------------- Cette Fonction Permettre d'écrire les données dans un flus d'envoie ---------------------------------*/

    QByteArray buffer;
    QDataStream s(&buffer, QIODevice::WriteOnly);

    if (false){

        s.setByteOrder(QDataStream::LittleEndian);
    }
    s << (uint8_t)Pid21.synchro [0]<< (uint8_t)Pid21.synchro [1]<<(uint8_t)Pid21.synchro [2]
      <<(uint8_t)Pid21.synchro [3]  << (uint8_t)Pid21.protocoleCode << (uint8_t)Pid21.pid
      << (uint8_t)Pid21.lengthMsb<< (uint8_t)Pid21.lengthLsb<<(uint8_t)Pid21.soh
      <<(uint8_t)Pid21.mode<<(uint8_t)Pid21.address1<<(uint8_t)Pid21.address2<<(uint8_t)Pid21.address3
      <<(uint8_t)Pid21.address4<<(uint8_t)Pid21.address5<<(uint8_t)Pid21.address6
      <<(uint8_t)Pid21.address7<<(uint8_t)Pid21.ack<<(uint8_t)Pid21.label1<<(uint8_t)Pid21.label2
      <<(uint8_t)Pid21.bi<<(uint8_t)Pid21.stx<<(uint8_t)Pid21.msn<<(uint8_t)Pid21.msn<<(uint8_t)Pid21.msn<<(uint8_t)Pid21.msn
      <<(uint8_t)Pid21.flightid<<(uint8_t)Pid21.flightid<<(uint8_t)Pid21.flightid<<(uint8_t)Pid21.flightid<<(uint8_t)Pid21.flightid
      <<(uint8_t)Pid21.flightid;



    /*-------------------------------Calcul du Crc16 Kermit-------------------------------------*/


    unsigned char myData[] ={
    0xff ,0x00, 0xf0, 0xca, 0xf2, 0x21, 0x00 ,varh , 0x01,0x02,
    0x2e , 0x2e , 0x6e , 0x33 , 0x38 , 0x37,0x32 , 0x01 , 0x01,
    0x00 ,0x01, 0x01,0x31,0x31,0x31,0x31,0x58,0x58,0x58,0x58,
    0x58,0x58,    0x4c, 0x27, 0x20, 0x41, 0x43, 0x41, 0x52, 0x53 , 0x20, 0x41,
    0x69, 0x72, 0x63, 0x72, 0x61, 0x66, 0x74, 0x20, 0x43, 0x6f,
    0x6d, 0x6d, 0x75,0x6e,  0x69, 0x63, 0x61, 0x74 ,0x69, 0x6f,
    0x6e, 0x20, 0x41 ,0x64, 0x64, 0x72, 0x65, 0x73, 0x73, 0x69,
    0x6e, 0x67, 0x20 ,0x61, 0x6e, 0x64, 0x20 ,0x52, 0x65, 0x70,
    0x6f, 0x72, 0x74, 0x69 ,0x6e, 0x67,  0x20, 0x53 ,0x79, 0x73,
    0x74, 0x65, 0x6d, 0x29, 0x20, 0x65 ,0x73 ,0x74 ,0x20,0x75,
    0x6e, 0x20,  0x73, 0x79, 0x73, 0x74, 0x74, 0x74, 0x74 ,0x74,
    0x6e, 0x20,  0x73, 0x79, 0x73, 0x74, 0x74, 0x74, 0x74 ,0x74,
    0x6e, 0x20,  0x73, 0x79, 0x73, 0x74, 0x74, 0x74, 0x74 ,0x74,
    0x6e, 0x20,  0x73, 0x79, 0x73, 0x74, 0x74, 0x74, 0x74 ,0x74,
    0x6e, 0x20,  0x73, 0x79, 0x73, 0x74, 0x74, 0x74, 0x74 ,0x74,
    0x6e, 0x20,  0x73, 0x79, 0x73, 0x74, 0x74, 0x74, 0x74 ,0x74,
    0x6e, 0x20,  0x73, 0x79, 0x73, 0x74, 0x74, 0x74, 0x74 ,0x74,
    0x6e, 0x20,  0x73, 0x79, 0x73, 0x74, 0x74, 0x74, 0x74 ,0x74,
    0x6e, 0x20,  0x73, 0x79, 0x73, 0x74, 0x74, 0x74, 0x74 ,0x74,
    0x6e, 0x20,  0x73, 0x79, 0x73, 0x74, 0x74, 0x74, 0x74 ,0x74,
    0x6e, 0x20,  0x73, 0x79, 0x73, 0x74, 0x74, 0x74, 0x74 ,0x74,
    0x6e, 0x20,  0x73, 0x79, 0x73, 0x74, 0x74, 0x74, 0x74 ,0x74,
    0x6e, 0x20,  0x73, 0x79, 0x73, 0x74, 0x74, 0x74, 0x74 ,0x74,
    0x6e, 0x20,  0x73, 0x79, 0x73, 0x74, 0x74, 0x74, 0x74 ,0x74,
    0x6c, 0x69, 0x74 ,0x65, 0x20, 0x68,0x7f,0x64,0x61,0x61};

   //nouveau atbleau ou on ibnsère le nombre saisie dans l'interface graphique de caractère
   unsigned char myData1[ui-> donneesAcars->value()+32];
    // on copie les valeurs dans le nouveau tableau
   memcpy(myData1, myData,sizeof(myData1));
   // un tableau qui contien la valeur 0x03
   unsigned char myData3[] ={0x03,0x02,0x02,0x7f};
   QByteArray databuf3 = QByteArray((char*)myData3, sizeof(myData3));
   // on conbine entre les deux tableau pour l'intégrer deans la fonction qui calcule le crc
   unsigned char allstrings[sizeof(myData1)+sizeof(myData3)];
   memcpy(allstrings,myData1,sizeof(myData1));
   memcpy(allstrings+sizeof(myData1),myData3,sizeof(myData3));
   unsigned short myCrc;
   int myDataLen = sizeof(myData1)+ sizeof(myData3);
   myCrc = CRC16kK(allstrings, myDataLen);
   myCrc = ((myCrc << 8) | (myCrc >> 8));


   Pid21.crc=myCrc;
   QByteArray bufferCrc;
   QDataStream streamData(&bufferCrc, QIODevice::WriteOnly);

   if (false){

       streamData.setByteOrder(QDataStream::LittleEndian);
   }
   streamData<< (uint16_t)Pid21.crc;

   /*--------------------------------Envoie des données--------------------------------------*/
   QByteArray buffer2 ;
   buffer2 = buffer;
   buffer2 = buffer2.append(databuf);
   buffer2 = buffer2.append(databuf3);
   buffer2 = buffer2 + bufferCrc;


   QString ip = ui->hostaddress1_TX->text();                      //Get the other party's ip
   short port = ui->port1_TX->value();
   socketUdp2->writeDatagram(buffer2,QHostAddress(ip), port);
   sockettranRatio->writeDatagram(buffer2,QHostAddress::LocalHost, 7878);


   ui-> plainTextEdit_2->appendPlainText(QString("[%1] : UNIT_DATA_REQ").arg(counter_req));
   QByteArray ba_as_hex_string = buffer2.toHex();
   ui->  plainTextEdit_3->appendPlainText(ba_as_hex_string);


}

void MainWindow3::on_TRANSMISSION_RATIO_clicked()
{
    transmission_ratio3.show();
}


void MainWindow3::on_actionVersion_triggered()
{
    version.show();
}

//Fermer la fenetre
void MainWindow3::on_close_clicked()
{
    MainWindow3::close();
}

// effacer le contenu des PlainEdit
void MainWindow3::on_Clear_clicked()
{
    ui->  plainTextEdit_4->clear();
    ui->  plainTextEdit_3->clear();
    ui->  plainTextEdit_2->clear();
    ui->  plainTextEdit->clear();
}


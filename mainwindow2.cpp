#include "mainwindow2.h"
#include "ui_mainwindow2.h"
int counter_req=0;
int counter_conf =0;
int counter_ind=0;
int counter_sqp=0;
MainWindow2::MainWindow2(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow2)
{
    ui->setupUi(this);
    ui->setupUi(this);
    this->setWindowTitle("ACARS&UDP");

    this->setStyleSheet("QMainWindow { background-color : #ffffff}");

    socketUdp2 = new QUdpSocket(this);

    socketUdp2->bind(1234,QUdpSocket::ShareAddress);

    connect(socketUdp2,&QUdpSocket::readyRead,[&](){


        while (socketUdp2 ->hasPendingDatagrams() )
        {

            QByteArray data;
            QHostAddress sender;
            quint16 senderPort;
            data.resize(socketUdp2->pendingDatagramSize());
            socketUdp2->readDatagram(data.data(),data.size(),&sender, &senderPort);
            QByteArray ba_as_hex_string = data.toHex();
            ui->  plainTextEdit_4->appendPlainText(ba_as_hex_string);
            if ((data[5] == 80) && (data[8] >= 18)){

                   int PARAM_REQ = ui-> frequence_5 -> value();
                   ui-> plainTextEdit->appendPlainText(QString("PARAM CONF = %1").arg(PARAM_REQ));

                   ui->  plainTextEdit_4->appendPlainText(ba_as_hex_string);

               }
            if (((data[8] == 47) || (data[8] == 44) || (data[8] == 40)) && ( data[9] == 1)){

                  int PWR_SET_REQ = ui -> puissance_5 -> value();
                  ui->plainTextEdit->appendPlainText(QString("PWR SET CONF = %1").arg(PWR_SET_REQ));
                  ui->  plainTextEdit_4->appendPlainText(ba_as_hex_string);

                }

            if( data[5] == 85 ){
                counter_conf++;

                  ui->plainTextEdit->appendPlainText(QString("[%1] : UNIT DATA CONF").arg(counter_conf));
                  ui->  plainTextEdit_4->appendPlainText(ba_as_hex_string);


              }
            if( data[9] == 1 && data[11] == 93){
                ui->plainTextEdit->appendPlainText(("MODE SET CONF"));
                ui->  plainTextEdit_4->appendPlainText(ba_as_hex_string);
            }
            if(data[5] == 83){


              ui->plainTextEdit->appendPlainText(QString("VDR ERROR IND"));
              ui->  plainTextEdit_4->appendPlainText(ba_as_hex_string);

            }

           if( data[5] == 81 ){

                 counter_ind++;
                 ui->plainTextEdit->appendPlainText(QString("[%1] : UNIT DATA IND").arg(counter_ind));
                  ui->  plainTextEdit_4->appendPlainText(ba_as_hex_string);

                 /* QByteArray houda;
                  QDataStream s(&houda, QIODevice::WriteOnly);

                  if (false){

                      s.setByteOrder(QDataStream::LittleEndian);
                  }
                  for (int i=10; i < (ui-> spinBox_2 -> value()) + 38 ;i++){

                  s << data[i];


              }*/
              // socketUdp22->writeDatagram(houda,QHostAddress::LocalHost, 6666);




                   }
           if( data[5] == 32  && data[4] == 95){
              counter_sqp++;
              ui->plainTextEdit->appendPlainText(QString("[%1] : SQP IND").arg(counter_sqp));
              ui->  plainTextEdit_4->appendPlainText(ba_as_hex_string);
            //  socketUdp22->writeDatagram(data,QHostAddress::LocalHost, 1236);


           }

}});

}

uint16_t CRC16K(uint8_t *x, uint8_t len) {
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

MainWindow2::~MainWindow2()
{
    delete ui;
}

void MainWindow2::on_actionPARAM_RES_triggered()
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
    myCrc = CRC16K(myData, myDataLen);
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

                QByteArray buffer;


                //  ecrire les données dans le flux d'envoie
                QDataStream s(&buffer, QIODevice::WriteOnly);
                if (false){

                    s.setByteOrder(QDataStream::LittleEndian);
                }

                s << (uint8_t)Pid20.synchro [0]<< (uint8_t)Pid20.synchro [1]<<(uint8_t)Pid20.synchro [2]<<(uint8_t)Pid20.synchro [3]
                << (uint8_t)Pid20.protocoleCode << (uint8_t)Pid20.pid << (uint8_t)Pid20.lengthMsb <<(uint8_t)Pid20.lengthLsb
               <<(uint8_t)Pid20.freq_lsb<<(uint8_t)Pid20.freq_msb<<(uint8_t)Pid20.mod<<(uint8_t)Pid20.prekey<<(uint16_t)Pid20.crc;

                /*--------------------------------Envoie des données--------------------------------------*/

                QString ip = ui->hostaddress1_10->text();                      //Get the other party's ip
                short port = ui->port1_11->value();


                socketUdp2->writeDatagram(buffer,QHostAddress(ip), port);



                int PARAM_REQ = ui-> frequence_5 -> value();
                ui->  plainTextEdit_2->appendPlainText(QString("PARAM_REQ_TX = %1").arg(PARAM_REQ));

                QByteArray ba_as_hex_string = buffer.toHex();
                ui->  plainTextEdit_3->appendPlainText(ba_as_hex_string);






            }
}


void MainWindow2::on_actionparam8req_triggered()
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
    myCrc = CRC16K(myData, myDataLen);
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

                QByteArray buffer;


                //  ecrire les données dans le flux d'envoie
                QDataStream s(&buffer, QIODevice::WriteOnly);
                if (false){

                    s.setByteOrder(QDataStream::LittleEndian);
                }

                s << (uint8_t)Pid20.synchro [0]<< (uint8_t)Pid20.synchro [1]<<(uint8_t)Pid20.synchro [2]<<(uint8_t)Pid20.synchro [3]
                << (uint8_t)Pid20.protocoleCode << (uint8_t)Pid20.pid << (uint8_t)Pid20.lengthMsb <<(uint8_t)Pid20.lengthLsb
               <<(uint8_t)Pid20.freq_lsb<<(uint8_t)Pid20.freq_msb<<(uint8_t)Pid20.mod<<(uint8_t)Pid20.prekey<<(uint16_t)Pid20.crc;

                /*--------------------------------Envoie des données--------------------------------------*/

                QString ip = ui->hostaddress1_11->text();                      //Get the other party's ip
                short port = ui->port1_10->value();


                socketUdp2->writeDatagram(buffer,QHostAddress(ip), port);



                int PARAM_REQ = ui-> frequence_5 -> value();
                ui->  plainTextEdit_2->appendPlainText(QString("PARAM_REQ_TX = %1").arg(PARAM_REQ));

                QByteArray ba_as_hex_string = buffer.toHex();
                ui->  plainTextEdit_3->appendPlainText(ba_as_hex_string);



}
}


void MainWindow2::on_actionUNIT_DATA_REQ_triggered()
{
    counter_req++;
    //uint8_t var6 = ((ui-> spinBox -> value()));
    uint8_t varh = ui-> spinBox_5 -> value() + 28;
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
    unsigned char str2[ui-> spinBox_5->value()];
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
   unsigned char myData1[ui-> spinBox_5->value()+32];

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
   myCrc = CRC16K(allstrings, myDataLen);
   myCrc = ((myCrc << 8) | (myCrc >> 8));

   Pid21.crc=myCrc;

   QByteArray bufferCrc;
   QDataStream g(&bufferCrc, QIODevice::WriteOnly);

   if (false){

       g.setByteOrder(QDataStream::LittleEndian);
   }
   g<< (uint16_t)Pid21.crc;




   /*--------------------------------Envoie des données--------------------------------------*/
   QByteArray buffer2 ;



   buffer2 = buffer;
   buffer2 = buffer2.append(databuf);
   buffer2 = buffer2.append(databuf3);

   buffer2 = buffer2 + bufferCrc;
   int longueur = sizeof(buffer2);


   QString ip = ui->hostaddress1_10->text();                      //Get the other party's ip
   short port = ui->port1_11->value();


   socketUdp2->writeDatagram(buffer,QHostAddress(ip), port);

   //mSocket1->writeDatagram(buffer2,QHostAddress::LocalHost, 6666);


   ui-> plainTextEdit_2->appendPlainText(QString("[%1] : UNIT_DATA_REQ").arg(counter_req));

   QByteArray ba_as_hex_string = buffer2.toHex();
   ui->  plainTextEdit_3->appendPlainText(ba_as_hex_string);


}


void MainWindow2::on_actionPURGE_REQ_triggered()
{

}


void MainWindow2::on_actionPWR_SET_REQ_triggered()
{

}


void MainWindow2::on_actionMODE_SET_REQ_triggered()
{

}


void MainWindow2::on_actionMODE_SET_REQ_2_triggered()
{

}


void MainWindow2::on_SQP_clicked()
{
}


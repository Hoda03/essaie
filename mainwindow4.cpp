#include "mainwindow4.h"
#include "ui_mainwindow4.h"
int ct =1;
int counter_confvdl = 0;
int counter_indvdl = 0;
uint8_t counterm =0;
uint8_t counternd =0;

#define SIZE_SYNCHRO_VDL2     4
const uint8_t synchrovdl2[SIZE_SYNCHRO_VDL2] = {0xff,0x00,0xf0,0xca};


#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    #include <QtCore/QTextCodec>
#else
    #include <QtCore5Compat/QTextCodec>
#endif
#ifdef Q_OS_WIN
#include <windows.h>
#define sleeep(msec) Sleep(msec)
#elif defined(Q_OS_UNIX)
#include <unistd.h>
#define sleeep(msec) usleep(msec)
#endif

bool isBffPARAM_REQ {false};
bool isBff1PWR_SET_REQ {false};
bool isBff1Mode_Set_REQ {false};
bool isBffPurge_REQ {false};
bool isBffUnit_data_req {false};
bool isBffPARAM_REQ_R {false};
bool isBff1Mode_Set_REQ_R {false};


MainWindow4::MainWindow4(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow4)
{
    ui->setupUi(this);
    this->setWindowTitle("MODE2&UDP");
    this->setStyleSheet("QMainWindow { background-color : #ffffff}");

   // Create a UDP Socket to communicate with telerad's Radio
   sockUdp = new QUdpSocket(this);
   sockUdptr = new QUdpSocket(this);
   sockUdp->bind(1235,QUdpSocket::ShareAddress);
   sockUdptr->bind(QHostAddress::LocalHost,0202);

   sockUdpRecepteur = new QUdpSocket(this);
   sockUdpRecepteur->bind(1963,QUdpSocket::ShareAddress);

   /*--------------------Création d'un Timer pour l'envoie succèssives des données en localhost----------------------------------------*/
   timer4 = new QTimer(this);
   connect(timer4, &QTimer::timeout, this, &MainWindow4::start_timer4);


   /*---------------cette fonction connect permet de se connecter avec les radios et lire ses réponses-----------------*/

   connect(sockUdpRecepteur,&QUdpSocket::readyRead,[&](){
     while (sockUdpRecepteur ->hasPendingDatagrams())
         {

               QByteArray data;
               QHostAddress sender;
               quint16 senderPort;
               data.resize(sockUdpRecepteur->pendingDatagramSize());

               sockUdpRecepteur->readDatagram(data.data(),data.size(),&sender, &senderPort);

               QByteArray ba_as_hex_string = data.toHex();

               if (data.length() == 17  && (data[5] == 80) && (data[8] >= 18)){

                             int PARAM_REQ = ui-> frequence_3 -> value();
                             ui-> plainTextEdit-> appendPlainText(QString("PARAM_CONF = %1").arg(PARAM_REQ));
                             ui->  plainTextEdit_4->appendPlainText(ba_as_hex_string);



                        }

               else if(data.length() == 12 &&  data[9] == 1 && data[11] == 119){

                           ui->plainTextEdit->appendPlainText(QString("MODE_SET_CONF"));
                           ui->  plainTextEdit_4->appendPlainText(ba_as_hex_string);

                       }
               else if( data[5] == 84){

                      ui->plainTextEdit->appendPlainText(QString("SQP IND"));
                      ui->  plainTextEdit_4->appendPlainText(ba_as_hex_string);
                      sockUdp->writeDatagram(data,QHostAddress::LocalHost, 1478);

                  }
                else if( data.length() ==  10  && data[4] == 95  ){

                    ui->plainTextEdit->appendPlainText(QString("CU_IND"));
                    ui->  plainTextEdit_4->appendPlainText(ba_as_hex_string);


                  }

                else if( data.length() < 9216  && data[5] == 81  ){

                    ui->plainTextEdit->appendPlainText(QString("[%1] : UNIT DATA IND").arg(counter_indvdl));
                    ui->  plainTextEdit_4->appendPlainText(ba_as_hex_string);
                    sockUdptr->writeDatagram(data,QHostAddress::LocalHost, 5555);


                  }


               else {

                   if(ct ==1 ){
                           arr = data;
                           qDebug() << ct << "arr";

                   }

                  else{
                       counter_indvdl++;
                       arr = arr.append(data);
                       qDebug()<<"houda"<< "ct ";

                       int val = arr.length();
                       qDebug()<< val;



                       if(arr.length() == ui->donnees_3->value() + 12)
                       {
                               ui->plainTextEdit->appendPlainText(QString("[%1] : UNIT DATA IND").arg(counter_indvdl));
                               QByteArray ba_as_hex_string1 = arr.toHex();

                               ui->  plainTextEdit_4->appendPlainText(ba_as_hex_string1);
                               sockUdptr->writeDatagram(arr,QHostAddress::LocalHost, 5555);
                               arr.clear();

                               qDebug()<<"append"<< "ct ";


                       }



                   }
                   qDebug() << "hi";
                   ct++;

               }

     }


   });

   connect(sockUdp,&QUdpSocket::readyRead,[&](){
     while (sockUdp ->hasPendingDatagrams())
         {

               QByteArray data;
               QHostAddress sender;
               quint16 senderPort;
               data.resize(sockUdp->pendingDatagramSize());

               sockUdp->readDatagram(data.data(),data.size(),&sender, &senderPort);

               QByteArray ba_as_hex_string = data.toHex();


             if (data.length() == 17  && (data[5] == 80) && (data[8] >= 18)){

                           int PARAM_REQ = ui-> frequence_3 -> value();
                           ui-> plainTextEdit-> appendPlainText(QString("PARAM_CONF = %1").arg(PARAM_REQ));
                           ui->  plainTextEdit_4->appendPlainText(ba_as_hex_string);



                      }



                else if (data.length() == 12 && ((data[8] == 47) || (data[8] == 44) || (data[8] == 40)) && ( data[9] == 1)){

                           int PWR_SET_REQ = ui -> puissance_3 -> value();
                            ui->plainTextEdit->appendPlainText(QString("PWR_SET_CONF = %1").arg(PWR_SET_REQ));
                            ui->  plainTextEdit_4->appendPlainText(ba_as_hex_string);

                       }




              else if(data.length() == 12 &&  data[9] == 1 && data[11] == 119){

                          ui->plainTextEdit->appendPlainText(QString("MODE_SET_CONF"));
                          ui->  plainTextEdit_4->appendPlainText(ba_as_hex_string);

                      }


               else if(data[5] == 83){


                   ui->plainTextEdit->appendPlainText(QString(" VDR_ERROR IND"));
                   ui->  plainTextEdit_4->appendPlainText(ba_as_hex_string);

               }


               else if( data[5] == 86 ){
                             counter_confvdl++;
                             ui->plainTextEdit->appendPlainText(QString("[%1] : UNIT DATA CONF").arg(counter_confvdl));
                             ui->  plainTextEdit_4->appendPlainText(ba_as_hex_string);
                             uint16_t tab16;
                             tab16 = ( (unsigned char)data[9] << 8 ) | (unsigned char)data[10] ;
                             int a_as_int =(int) data[8];
                             int a_as_int2 =(int) data[11];
                             int z =(int) tab16;



                             QString s = QString::number(a_as_int);
                             QString g = QString::number(z);
                             QString f = QString::number(a_as_int2 );

                             ui->info->appendPlainText(QString("statut de transmission : %1").arg(s));
                             ui->info->appendPlainText(QString("Mac Delay : %1").arg(g));
                             ui->info->appendPlainText(QString("Tx Pkt sequence No. : %1").arg(f));



                         }







               }





     qDebug()<< "clear";


    // ct = 1;

});
}

/*--------------Cette Fonction nour permet de calculer le CRC16 Kermit---------------------*/

uint16_t CRC16(uint8_t *x, uint16_t len) {
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

MainWindow4::~MainWindow4()
{
    delete ui;
}

/*------------------Cette Fonction nous permet d'envoyer un changement de fréquence à la radios/EM ----------------------*/

void MainWindow4::on_actionPARAM_REQ_triggered()
{
    /*-----------------------------------------------------------------------------------*/
      // Algo de traitement de la fréquence
      //Il faut créer une liste qui prend la taille de la valeur de fréquence désiré
      //mettre le nombre saisie dans le tableu sous forme de chiffre

      /*-----------------------------------------------------------------------------------------*/


      int T[6];
      int c = 0;
      int msb[2] , lsb[2] ;
      int monnombre = ui->frequence_3->value();
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



      uint8_t  var1 = ui -> spinBox_9 -> value();   //Valeur du TIMER1
      uint8_t  var3 = ui -> spinBox_10 -> value();   //Valeur du TIMER2
      uint8_t  var5 = ui->spinBox_12->value();         //Valeur de la persistance

      uint16_t var4 = ui->spinBox_11->value();       //Valeur du compteur
      uint16_t value = var4;
      uint8_t array[2];
      array[0]=value & 0xff;
      array[1]=(value >> 8);

      /*-------------Définition des champs de la trame et son envoie dans un fil------------- */

      if  (  ui -> frequence_3 -> value() >= 118000  &  ui -> frequence_3 -> value() <= 136000 )
              {


                      tsPid20vdl Pid20;
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
                      myCrc = CRC16(myData, myDataLen);
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

                      QString ip = ui->hostaddress1_TX->text();                      //Get the other party's ip
                      short port = ui->port1_TX->value();

                      sockUdp->writeDatagram(buffer,QHostAddress(ip), port);

                      int PARAM_REQ = ui-> frequence_3 -> value();
                      ui-> plainTextEdit_2 -> appendPlainText(QString("PARAM_CONF = %1").arg(PARAM_REQ));

                      QByteArray ba_as_hex_string = buffer.toHex();
                      ui->  plainTextEdit_3->appendPlainText(ba_as_hex_string);

    } isBffPARAM_REQ = true;
    }

/*------------------Cette Fonction nous permet d'envoyer un changement de fréquence à la radios/RX ----------------------*/

void MainWindow4::on_actionPARAM_REQ_2_triggered()
{
    /*-----------------------------------------------------------------------------------*/
      // Algo de traitement de la fréquence
      //Il faut créer une liste qui prend la taille de la valeur de fréquence désiré
      //mettre le nombre saisie dans le tableu sous forme de chiffre

      /*-----------------------------------------------------------------------------------------*/


      int T[6];
      int c = 0;
      int msb[2] , lsb[2] ;
      int monnombre = ui->frequence_3->value();
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



      uint8_t  var1 = ui -> spinBox_9 -> value();   //Valeur du TIMER1
      uint8_t  var3 = ui -> spinBox_10 -> value();   //Valeur du TIMER2
      uint8_t  var5 = ui->spinBox_12->value();         //Valeur de la persistance

      uint16_t var4 = ui->spinBox_11->value();       //Valeur du compteur
      uint16_t value = var4;
      uint8_t array[2];
      array[0]=value & 0xff;
      array[1]=(value >> 8);

      /*-------------Définition des champs de la trame et son envoie dans un fil------------- */

      if  (  ui -> frequence_3 -> value() >= 118000  &  ui -> frequence_3 -> value() <= 136000 )
              {


                      tsPid20vdl Pid20;
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
                      myCrc = CRC16(myData, myDataLen);
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

                      QString ip = ui->hostaddress1_RX->text();                      //Get the other party's ip
                      short port = ui->port1_RX->value();

                      sockUdpRecepteur->writeDatagram(buffer,QHostAddress(ip), port);

                      int PARAM_REQ = ui-> frequence_3 -> value();
                      ui-> plainTextEdit_2 -> appendPlainText(QString("PARAM_CONF = %1").arg(PARAM_REQ));

                      QByteArray ba_as_hex_string = buffer.toHex();
                      ui->  plainTextEdit_3->appendPlainText(ba_as_hex_string);



              }



       isBffPARAM_REQ_R = true;
}



/*------------------Cette Fonction nous permet d'envoyerles données vdl2 à la radio/EM ----------------------*/


void MainWindow4::on_pushButton_clicked()
{
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
       counterm++;

       /*--------------Définition des 16k valeur à pouvoir envoyer sous le protocole VDL 2------------------------*/

            uint16_t value = ui->donnees_3->value()+1;
            uint8_t array[2];
            array[0]=value & 0xff;
            array[1]=(value >> 8);

            /*----fonction rand()- retourne des entier aléatoire de 0 à 16k----*/

           char table22[ ui->donnees_3->value() ] ;
           unsigned char table222[ui->donnees_3->value()] ;
             for(int i=0; i< (ui->donnees_3->value()); i++){
               srand((unsigned) time(0));
               int strn =  (rand() % 256);
               unsigned char monsef = (unsigned char) strn;
               table22[i] = strn;

               table222[i] = monsef;

               }

            char str[16384];

            str[16383] = '\0';
            strncpy ( str, table22, ui->donnees_3->value() );
            QByteArray databuf = QByteArray(reinterpret_cast<char*>(table22), ui->donnees_3->value());


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
            Pid21.clic=counterm;

            /*-------------------------------Calcul du Crc16 Kermit-------------------------------------*/


            unsigned char myData3[] ={0xff ,0x00, 0xf0, 0xca,0xf1, 0x21,array[1],array[0],counterm};
            unsigned char allstrings[sizeof(table222)+sizeof(myData3)];
            memcpy(allstrings,myData3,sizeof(myData3));
            memcpy(allstrings+sizeof(myData3),table222,sizeof(table222));
            unsigned short myCrc;
            unsigned int myDataLen = sizeof(table222)+ sizeof(myData3);

            myCrc = CRC16(allstrings, myDataLen);
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
           << (uint8_t)Pid21.lengthMsb<< (uint8_t)Pid21.lengthLsb
           << (uint8_t)Pid21.clic ;



           QByteArray buffer3;
           //  ecrire les données dans le flux d'envoie

           QDataStream g(&buffer3, QIODevice::WriteOnly);

           if (false){

               g.setByteOrder(QDataStream::LittleEndian);
           }
           g <<(uint16_t) Pid21.crc;


           QByteArray buffer5 = buffer +databuf + buffer3;


        /*--------------------------------Envoie des données--------------------------------------*/

           QString ip = ui->hostaddress1_TX->text();                      //Get the other party's ip
           short port = ui->port1_TX->value();

       sockUdp->writeDatagram(buffer5,QHostAddress(ip), port);
       QByteArray buffer6 = buffer + databuf;

       sockUdptr->writeDatagram(databuf,QHostAddress::LocalHost, 5555);
       ui->  plainTextEdit_2 -> appendPlainText(QString("[%1] : UINT_DATA_REQ").arg(counterm));

       QByteArray ba_as_hex_string = buffer5.toHex();
       ui->  plainTextEdit_3->appendPlainText(ba_as_hex_string);


        isBffUnit_data_req = true;


}


void MainWindow4::on_actionPWR_SET_REQ_triggered()
{
    /*----------------instruction conditionnelle pour l'envoie de la puissance à la Radio------------------------*/


          int pwr = ui -> puissance_3 -> value();
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


          tsPidF1vdl PidF1;

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
          myCrc = CRC16(myData, myDataLen);
          myCrc = ((myCrc << 8) | (myCrc >> 8));
          PidF1.crc=myCrc ;

          /*-------------------Intégration des champs du buffer dans un fil d'envoie "s"--------------------------*/

          QByteArray buffer;
          QDataStream s(&buffer, QIODevice::WriteOnly);

          if (false){

              s.setByteOrder(QDataStream::LittleEndian);
          }
             s << (uint8_t)PidF1.synchro [0]<< (uint8_t)PidF1.synchro [1]<<(uint8_t)PidF1.synchro [2]
               <<(uint8_t)PidF1.synchro [3]  << (uint8_t)PidF1.protocoleCode << (uint8_t)PidF1.pid
               << (uint8_t)PidF1.lengthMsb <<(uint8_t)PidF1.lengthLsb<<(uint8_t)PidF1.pwr
               <<(uint8_t)PidF1.req<<(uint16_t)PidF1.crc;


             /*--------------------------------Envoie des données--------------------------------------*/

            QString ip = ui->hostaddress1_TX->text();                      //Get the other party's ip
            short port = ui->port1_TX->value();

            sockUdp->writeDatagram(buffer,QHostAddress(ip), port);
            int PWR_SET_REQ = ui -> puissance_3 -> value();
            ui->  plainTextEdit_2 -> appendPlainText(QString("PWR_SET_REQ = %1").arg(PWR_SET_REQ));

            QByteArray ba_as_hex_string = buffer.toHex();
            ui->  plainTextEdit_3->appendPlainText(ba_as_hex_string);

             isBff1PWR_SET_REQ = true;



}


void MainWindow4::on_actionMODE_SET_REQ_triggered()
{
    /*-------------Définition des champs de la trame ------------- */

       tsPidF0vdl PidF0;
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
       myCrc = CRC16(myData, myDataLen);
       myCrc = ((myCrc << 8) | (myCrc >> 8));
       PidF0.crc= myCrc;

       /*-------------------Intégration des champs du buffer dans un fil d'envoie--------------------------*/

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


       QString ip = ui->hostaddress1_TX->text();                      //Get the other party's ip
       short port = ui->port1_TX->value();

       sockUdp->writeDatagram(buffer,QHostAddress(ip), port);
       ui->  plainTextEdit_2 -> appendPlainText("MODE_SET_REQ");

       QByteArray ba_as_hex_string = buffer.toHex();
       ui->  plainTextEdit_3->appendPlainText(ba_as_hex_string);


        isBff1Mode_Set_REQ = true;
 }



void MainWindow4::on_actionPURGE8req_triggered()
{
    /*-------------Définition des champs de la trame ------------- */


       tsPid010bvdl Pid010b;

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
       myCrc = CRC16(myData, myDataLen);
       myCrc = ((myCrc << 8) | (myCrc >> 8));
       Pid010b.crc=myCrc;

       /*-------------------Intégration des champs du buffer dans un flux d'envoie--------------------------*/


       QByteArray buffer;
       //  ecrire les données dans le flux d'envoie
       QDataStream s(&buffer, QIODevice::WriteOnly);

       if (false){

           s.setByteOrder(QDataStream::LittleEndian);

       }
       s  << (uint8_t)Pid010b.synchro [0]<< (uint8_t)Pid010b.synchro [1]<<(uint8_t)Pid010b.synchro [2]
          <<(uint8_t)Pid010b.synchro [3]<<(uint8_t)Pid010b.protocoelCode<<(uint8_t)Pid010b.pid
          <<(uint8_t)Pid010b.donnees<<(uint8_t)Pid010b.sal<<(uint16_t)Pid010b.crc;


       /*--------------------------------Envoie des données--------------------------------------*/


       QString ip = ui->hostaddress1_TX->text();                      //Get the other party's ip
       short port = ui->port1_TX->value();

       sockUdp->writeDatagram(buffer,QHostAddress(ip), port);
       ui->  plainTextEdit_2 -> appendPlainText("PURGE_REQ");

       QByteArray ba_as_hex_string = buffer.toHex();
       ui->  plainTextEdit_3->appendPlainText(ba_as_hex_string);
}


void MainWindow4::on_actionMODE_SET_REQ_2_triggered()
{
    /*-------------Définition des champs de la trame ------------- */

       tsPidF0vdl PidF0;
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
       myCrc = CRC16(myData, myDataLen);
       myCrc = ((myCrc << 8) | (myCrc >> 8));
       PidF0.crc= myCrc;

       /*-------------------Intégration des champs du buffer dans un fil d'envoie--------------------------*/

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


       QString ip = ui->hostaddress1_RX->text();                      //Get the other party's ip
       short port = ui->port1_RX->value();

       sockUdpRecepteur->writeDatagram(buffer,QHostAddress(ip), port);
       ui->  plainTextEdit_2 -> appendPlainText("MODE_SET_REQ");

       QByteArray ba_as_hex_string = buffer.toHex();
       ui->  plainTextEdit_3->appendPlainText(ba_as_hex_string);
}

// ENVOYER N DONNEES
void MainWindow4::on_pushButton_4_clicked()
{
     if (ui->donnees_3-> value() >= 5000 ){
         timer4->start(4000);

     }
     if (ui->donnees_3-> value() < 5000 && ui->donnees_3-> value() >= 1000 ){

         timer4->start(3000);

     }
     if (ui->donnees_3-> value() < 1000 ){

         timer4->start(500);

     }


}

// la méthode start_timer qui contient le buffer unit_data_req utilisé dans le timer

void MainWindow4::start_timer4(){

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
           counterm++;

           /*--------------Définition des 16k valeur à pouvoir envoyer sous le protocole VDL 2------------------------*/

                uint16_t value = ui->donnees_3->value()+1;
                uint8_t array[2];
                array[0]=value & 0xff;
                array[1]=(value >> 8);


               char table22[ ui->donnees_3->value() ] ;
               unsigned char table222[ui->donnees_3->value()] ;
                 for(int i=0; i< (ui->donnees_3->value()); i++){
                   srand((unsigned) time(0));
                   int strn =  (rand() % 255);
                   unsigned char monsef = (unsigned char) strn;
                   table22[i] = strn;

                   table222[i] = monsef;

                   }

                char str[16384];

                str[16383] = '\0';
                strncpy ( str, table22, ui->donnees_3->value() );
                QByteArray databuf = QByteArray(reinterpret_cast<char*>(table22), ui->donnees_3->value());


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
                Pid21.clic=counterm;

                /*-------------------------------Calcul du Crc16 Kermit-------------------------------------*/


                unsigned char myData3[] ={0xff ,0x00, 0xf0, 0xca,0xf1, 0x21,array[1],array[0],counterm};
                unsigned char allstrings[sizeof(table222)+sizeof(myData3)];
                memcpy(allstrings,myData3,sizeof(myData3));
                memcpy(allstrings+sizeof(myData3),table222,sizeof(table222));
                unsigned short myCrc;
                unsigned int myDataLen = sizeof(table222)+ sizeof(myData3);

                myCrc = CRC16(allstrings, myDataLen);
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
               << (uint8_t)Pid21.lengthMsb<< (uint8_t)Pid21.lengthLsb
               << (uint8_t)Pid21.clic ;



               QByteArray buffer3;
               //  ecrire les données dans le flux d'envoie

               QDataStream g(&buffer3, QIODevice::WriteOnly);

               if (false){

                   g.setByteOrder(QDataStream::LittleEndian);
               }
               g <<(uint16_t) Pid21.crc;


               QByteArray buffer5 = buffer +databuf + buffer3;


               QString ip = ui->hostaddress1_TX->text();
               short port = ui->port1_TX->value();

               sockUdp->writeDatagram(buffer5,QHostAddress(ip), port);

               sockUdptr->writeDatagram(databuf,QHostAddress::LocalHost, 5555);

               ui->  plainTextEdit_2 -> appendPlainText(QString("[%1] : UINT_DATA_REQ").arg(counterm));

               QByteArray ba_as_hex_string = buffer5.toHex();
              ui->  plainTextEdit_3->appendPlainText(ba_as_hex_string);

}

void MainWindow4::on_sqp_clicked()
{
    sqp4.show();
}

//stop the timer
void MainWindow4::on_pushButton_3_clicked()
{
    timer4->stop();

}


void MainWindow4::on_transmission_ratio_clicked()
{
transmission_ratio4.show();
}


void MainWindow4::on_version_clicked()
{
    version.show();

}


void MainWindow4::on_pushButton_2_clicked()
{
    ui->  plainTextEdit_4->clear();
    ui->  plainTextEdit_3->clear();
    ui->  plainTextEdit_2->clear();
    ui->  plainTextEdit->clear();
    ui->info->clear();
}



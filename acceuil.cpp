#include "acceuil.h"
#include "ui_acceuil.h"
#include <QMessageBox>

acceuil::acceuil(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::acceuil)
{
    ui->setupUi(this);

    /*-----------------------Titre de la fenetre d'acceuil------------------------*/
    this->setWindowTitle("X&S");

    /*-----------------------------------Partie de style de la fenetre----------------------------------------*/
    this->setStyleSheet("QMainWindow { background-color : #ffffff}");

    ui->label_2->setFrameStyle(QFrame::Box | QFrame::Sunken);
    ui-> label_2->setLineWidth(3);
    ui-> label_2->setMidLineWidth(2);

    ui->label->setFrameStyle(QFrame::StyledPanel | QFrame::Raised);
    ui-> label->setLineWidth(3);
    ui-> label->setMidLineWidth(3);

    ui->label_3->setFrameStyle(QFrame::StyledPanel | QFrame::Raised);
    ui-> label_3->setLineWidth(3);
    ui-> label_3->setMidLineWidth(3);


    ui->label_4->setFrameStyle(QFrame::StyledPanel | QFrame::Raised);
    ui->label_4->setStyleSheet("QLabel { background-color : white; color : #0078d7; }");
    ui-> label_4->setLineWidth(3);
    ui-> label_4->setMidLineWidth(3);


}

acceuil::~acceuil()
{
    delete ui;
}

void acceuil::on_acarsvdl2_clicked()
{
    /*-----------Ouverture d'une fenetre pour la communication acars en UDP--------------*/
    if ((ui -> acars -> isChecked() & ui -> radioButton_5 -> isChecked()))
     {
         acceuil::close();
         mainwindow3.show();
     }

    /*-----------Ouverture d'une fenetre pour la communication acars en TCP--------------*/

    else if ((ui -> acars -> isChecked() & ui -> radioButton_6 -> isChecked()))
     {
         acceuil::close();
         mainwindow.show();
     }

    /*-----------Ouverture d'une fenetre pour la communication VDL2 en UDP--------------*/

    else if ((ui -> vdl2 -> isChecked() & ui -> radioButton_5 -> isChecked()))
     {
         acceuil::close();
         mainwindow4.show();
     }

    /*-----------Ouverture d'une fenetre pour la communication VDL2 en TCP--------------*/

    else if ((ui -> vdl2 -> isChecked() & ui -> radioButton_6 -> isChecked()))
     {
         acceuil::close();
         mainwindow1.show();
     }
     else
     {
         QMessageBox::information(this, "Erreur", "Attention aucun port ni protocole sélectionnés !");

     }
}


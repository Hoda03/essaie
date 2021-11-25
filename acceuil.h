#ifndef ACCEUIL_H
#define ACCEUIL_H

#include <QMainWindow>
#include "mainwindow.h"    // ACARS&TCP
#include "mainwindow1.h"   //MODE2&TCP
#include "mainwindow3.h"   //ACARS&UDP
#include "mainwindow4.h"   //MODE2&UDP


/*---------Header de la Page d'acceuil du logiciel X&S----------------*/


namespace Ui {
class acceuil;
}

class acceuil : public QMainWindow
{
    Q_OBJECT

public:
    explicit acceuil(QWidget *parent = nullptr);
    ~acceuil();

private slots:

    void on_acarsvdl2_clicked();

private:

    Ui::acceuil *ui;
    MainWindow mainwindow;
    MainWindow1 mainwindow1;
    MainWindow3 mainwindow3;
    MainWindow4 mainwindow4;
};

#endif // ACCEUIL_H


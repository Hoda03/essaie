#include "version.h"
#include "ui_version.h"

version::version(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::version)
{
    ui->setupUi(this);
    this->setWindowTitle("Version");

    ui->label_2->setFrameStyle(QFrame::Box | QFrame::Sunken);
    ui-> label_2->setLineWidth(3);
    ui-> label_2->setMidLineWidth(2);

    ui->label->setFrameStyle(QFrame::StyledPanel | QFrame::Raised);
    ui-> label->setLineWidth(3);
    ui-> label->setMidLineWidth(3);

    ui->label_5->setFrameStyle(QFrame::StyledPanel | QFrame::Raised);
    ui->label_5->setStyleSheet("QLabel { background-color : white; color : #0078d7; }");
    ui-> label_5->setLineWidth(3);
    ui-> label_5->setMidLineWidth(3);
}

version::~version()
{
    delete ui;
}

void version::on_pushButton_clicked()
{
    version::close();
}


#ifndef VERSION_H
#define VERSION_H

#include <QMainWindow>

namespace Ui {
class version;
}

class version : public QMainWindow
{
    Q_OBJECT

public:
    explicit version(QWidget *parent = nullptr);
    ~version();

private slots:
    void on_pushButton_clicked();

private:
    Ui::version *ui;
};

#endif // VERSION_H

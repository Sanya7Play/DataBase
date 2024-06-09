#ifndef BDRZHD_H
#define BDRZHD_H
#include <QMainWindow>
#include <QDebug>
#include <QMessageBox>
#include "glavnaya.h"
QT_BEGIN_NAMESPACE
namespace Ui {
class BDRZHD;
}
QT_END_NAMESPACE

class BDRZHD : public QMainWindow
{
    Q_OBJECT

public:
    BDRZHD(QWidget *parent = nullptr);
    ~BDRZHD();

private slots:

    void UserLoggedIn();
private:
    Ui::BDRZHD *ui;
    glavnaya *glavnayaWindow;


};

#endif // BDRZHD_H

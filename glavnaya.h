#ifndef GLAVNAYA_H
#define GLAVNAYA_H
#include <QDialog>
#include <QMessageBox>
#include <QtSql>
#include <QSqlDatabase>
#include <QSqlError>
#include <QInputDialog>
#include <QTableWidgetItem>
namespace Ui {
class glavnaya;
}
class glavnaya : public QDialog
{
    Q_OBJECT
public:
    explicit glavnaya(QWidget *parent = nullptr);
    ~glavnaya();

private slots:

    void on_View_clicked();
    void on_Delete_clicked();
    void on_Edit_clicked();
    void on_Add_clicked();


private:
    Ui::glavnaya *ui;
    bool createTables();
    void viewData();
    void deleteData();
    void editData();
    void addData();

};

#endif // GLAVNAYA_H

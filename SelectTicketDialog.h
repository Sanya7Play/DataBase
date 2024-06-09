#ifndef SELECTTICKETDIALOG_H
#define SELECTTICKETDIALOG_H

#include <QDialog>
#include <QListWidget>
#include <QPushButton>

class SelectTicketDialog : public QDialog {
    Q_OBJECT

public:
    explicit SelectTicketDialog(QWidget *parent = nullptr);
    int getSelectedTicketId() const;

private slots:
    void onItemSelected();

private:
    QListWidget *listWidget;
    QPushButton *okButton;
    int selectedTicketId;
};

#endif // SELECTTICKETDIALOG_H

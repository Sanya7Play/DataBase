#ifndef EDITTICKETDIALOG_H
#define EDITTICKETDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QTimeEdit>
#include <QPushButton>

class EditTicketDialog : public QDialog {
    Q_OBJECT

public:
    explicit EditTicketDialog(int ticketId, QWidget *parent = nullptr);
    int getSeatNumber() const;
    QTime getDepartureTime() const;

private:
    int ticketId;
    QLineEdit *seatNumberEdit;
    QTimeEdit *departureTimeEdit;
    QPushButton *saveButton;
};

#endif // EDITTICKETDIALOG_H

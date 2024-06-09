// #include <EditTicketDialog.h>
// #include <QVBoxLayout>
// #include <QSqlQuery>
// #include <QLabel>

// EditTicketDialog::EditTicketDialog(int ticketId, QWidget *parent) : QDialog(parent), ticketId(ticketId) {
//     QVBoxLayout *layout = new QVBoxLayout(this);

//     seatNumberEdit = new QLineEdit(this);
//     departureTimeEdit = new QTimeEdit(this);
//     saveButton = new QPushButton(tr("Сохранить"), this);

//     layout->addWidget(new QLabel(tr("Номер места:"), this));
//     layout->addWidget(seatNumberEdit);
//     layout->addWidget(new QLabel(tr("Время отправления:"), this));
//     layout->addWidget(departureTimeEdit);
//     layout->addWidget(saveButton);

//     connect(saveButton, &QPushButton::clicked, this, &EditTicketDialog::accept);

//     QSqlQuery query(QSqlDatabase::database("postgres"));
//     query.prepare("SELECT seat_number, departure_time FROM tickets WHERE ticket_id = :ticket_id");
//     query.bindValue(":ticket_id", ticketId);
//     if (query.exec() && query.next()) {
//         seatNumberEdit->setText(query.value("seat_number").toString());
//         departureTimeEdit->setTime(query.value("departure_time").toTime());
//     }
// }

// int EditTicketDialog::getSeatNumber() const {
//     return seatNumberEdit->text().toInt();
// }

// QTime EditTicketDialog::getDepartureTime() const {
//     return departureTimeEdit->time();
// }

#include "selectticketdialog.h"
#include <QVBoxLayout>
#include <QSqlQuery>

SelectTicketDialog::SelectTicketDialog(QWidget *parent) : QDialog(parent), selectedTicketId(-1) {
    QVBoxLayout *layout = new QVBoxLayout(this);
    listWidget = new QListWidget(this);
    okButton = new QPushButton(tr("OK"), this);
    layout->addWidget(listWidget);
    layout->addWidget(okButton);

    connect(okButton, &QPushButton::clicked, this, &SelectTicketDialog::accept);
    connect(listWidget, &QListWidget::itemClicked, this, &SelectTicketDialog::onItemSelected);

    QSqlQuery query("SELECT ticket_id, passenger_name FROM tickets", QSqlDatabase::database("postgres"));
    while (query.next()) {
        QString itemText = QString("Ticket ID: %1, Passenger: %2").arg(query.value(0).toInt()).arg(query.value(1).toString());
        QListWidgetItem *item = new QListWidgetItem(itemText, listWidget);
        item->setData(Qt::UserRole, query.value(0).toInt());
    }
}

int SelectTicketDialog::getSelectedTicketId() const {
    return selectedTicketId;
}

void SelectTicketDialog::onItemSelected() {
    QListWidgetItem *selectedItem = listWidget->currentItem();
    if (selectedItem) {
        selectedTicketId = selectedItem->data(Qt::UserRole).toInt();
    }
}

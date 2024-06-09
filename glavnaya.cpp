#include "glavnaya.h"
#include "ui_glavnaya.h"
#include <QInputDialog>
#include <QTableWidgetItem>
#include <QSqlQuery>
#include <QMessageBox>
#include <QSqlError>

glavnaya::glavnaya(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::glavnaya)
{
    ui->setupUi(this);
    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL");
    qDebug() << db.drivers();
    db.setDatabaseName("postgres");
    db.setPort(5432);
    db.setUserName("postgres");
    db.setPassword("Ivan1234");

    if (!db.open())
    {
        QMessageBox::warning(0, "Ошибка БД", db.lastError().databaseText());
    }
    else
    {
        QMessageBox::information(0, "Успешно", "Соединение с БД Установлено");
        createTables();
    }

}

glavnaya::~glavnaya()
{
    delete ui;
}

bool glavnaya::createTables()
{
    QSqlQuery query(QSqlDatabase::database("postgres"));

    // Сброс счетчика ticket_id
    if (!query.exec("ALTER SEQUENCE IF EXISTS tickets_ticket_id_seq RESTART WITH 1")) {
        QMessageBox::warning(0, "Ошибка", query.lastError().databaseText());
        qDebug() << query.lastError().databaseText();
    }

    // Создаем таблицу routes
    if (!query.exec("CREATE TABLE IF NOT EXISTS routes ("
                    "route_id SERIAL PRIMARY KEY, "
                    "route_name VARCHAR NOT NULL)")) {
        QMessageBox::warning(0, "Ошибка", query.lastError().databaseText());
        qDebug() << query.lastError().databaseText();
    }

    // Создаем таблицу trains с каскадным удалением маршрутов
    if (!query.exec("CREATE TABLE IF NOT EXISTS trains ("
                    "train_id SERIAL PRIMARY KEY, "
                    "route_id INT NOT NULL, "
                    "train_number INT NOT NULL, "
                    "departure_time TIME, "
                    "arrival_time TIME, "
                    "FOREIGN KEY (route_id) REFERENCES routes(route_id) ON DELETE CASCADE)")) {
        QMessageBox::warning(0, "Ошибка", query.lastError().databaseText());
        qDebug() << query.lastError().databaseText();
    }

    // Создаем таблицу tickets с каскадным удалением маршрутов и поездов
    if (!query.exec("CREATE TABLE IF NOT EXISTS tickets ("
                    "ticket_id SERIAL PRIMARY KEY, "
                    "route_id INT NOT NULL, "
                    "train_id INT NOT NULL, "
                    "seat_number INT NOT NULL, "
                    "passenger_name VARCHAR, "
                    "departure_time TIME, "
                    "arrival_time TIME, "
                    "FOREIGN KEY (route_id) REFERENCES routes(route_id) ON DELETE CASCADE, "
                    "FOREIGN KEY (train_id) REFERENCES trains(train_id) ON DELETE CASCADE)")) {
        QMessageBox::warning(0, "Ошибка", query.lastError().databaseText());
        qDebug() << query.lastError().databaseText();
    }

    return true;
}

void glavnaya::viewData()
{
    QSqlQuery query(QSqlDatabase::database("postgres"));

    if (!query.exec("SELECT * FROM tickets"))
    {
        QMessageBox::warning(0, "Ошибка", query.lastError().databaseText());
        return;
    }

    // Устанавливаем заголовки таблицы
    ui->tableWidget->setColumnCount(7);
    QStringList headers;
    headers << "Билет ID" << "Маршрут" << "Поезд ID" << "Номер места" << "Имя пассажира" << "Время отправления" << "Время прибытия";
    ui->tableWidget->setHorizontalHeaderLabels(headers);

    // Заполняем таблицу данными
    ui->tableWidget->setRowCount(0);  // Очистить таблицу перед заполнением
    int row = 0;
    while (query.next()) {
        ui->tableWidget->insertRow(row);
        ui->tableWidget->setItem(row, 0, new QTableWidgetItem(query.value("ticket_id").toString()));
        ui->tableWidget->setItem(row, 1, new QTableWidgetItem(query.value("route_id").toString()));
        ui->tableWidget->setItem(row, 2, new QTableWidgetItem(query.value("train_id").toString()));
        ui->tableWidget->setItem(row, 3, new QTableWidgetItem(query.value("seat_number").toString()));
        ui->tableWidget->setItem(row, 4, new QTableWidgetItem(query.value("passenger_name").toString()));
        ui->tableWidget->setItem(row, 5, new QTableWidgetItem(query.value("departure_time").toString()));
        ui->tableWidget->setItem(row, 6, new QTableWidgetItem(query.value("arrival_time").toString()));
        row++;
    }
}

void glavnaya::deleteData()
{
    QList<QTableWidgetItem *> selectedItems = ui->tableWidget->selectedItems();
    if (selectedItems.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Выберите билет для удаления");
        return;
    }

    int row = selectedItems.first()->row();
    int ticket_id = ui->tableWidget->item(row, 0)->text().toInt();

    // Получаем номер билета для подтверждения
    QString ticket_number = ui->tableWidget->item(row, 0)->text();

    // Показываем диалоговое окно для подтверждения удаления
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Подтверждение удаления",
                                  QString("Вы действительно хотите удалить билет с номером %1?").arg(ticket_number),
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        QSqlQuery query(QSqlDatabase::database("postgres"));
        query.prepare("DELETE FROM tickets WHERE ticket_id = :ticket_id");
        query.bindValue(":ticket_id", ticket_id);

        if (!query.exec()) {
            QMessageBox::warning(0, "Ошибка", query.lastError().databaseText());
        } else {
            QMessageBox::information(0, "Успешно", "Билет успешно удален.");
            ui->tableWidget->removeRow(row); // Удалить строку из таблицы
        }
    } else {
        QMessageBox::information(0, "Отмена", "Удаление билета отменено.");
    }
}

void glavnaya:: editData()
{
    // Создание запроса для получения всех билетов
    QSqlQuery query(QSqlDatabase::database("postgres"));
    if (!query.exec("SELECT ticket_id, route_id, train_id, seat_number, passenger_name, departure_time, arrival_time FROM tickets")) {
        QMessageBox::warning(0, "Ошибка", query.lastError().databaseText());
        return;
    }

    // Создание списка билетов для выбора
    QStringList ticketList;
    while (query.next()) {
        QString ticket = QString("Билет ID: %1, Маршрут ID: %2, Поезд ID: %3, Номер места: %4, Имя пассажира: %5, Время отправления: %6, Время прибытия: %7")
                             .arg(query.value("ticket_id").toString())
                             .arg(query.value("route_id").toString())
                             .arg(query.value("train_id").toString())
                             .arg(query.value("seat_number").toString())
                             .arg(query.value("passenger_name").toString())
                             .arg(query.value("departure_time").toString())
                             .arg(query.value("arrival_time").toString());
        ticketList << ticket;
    }

    // Диалог для выбора билета
    bool ok;
    QString selectedTicket = QInputDialog::getItem(this, tr("Редактирование билета"), tr("Выберите билет для редактирования:"), ticketList, 0, false, &ok);
    if (!ok || selectedTicket.isEmpty()) {
        return; // Если не выбран билет, выходим из функции
    }

    // Получение ID выбранного билета
    QStringList selectedTicketDetails = selectedTicket.split(", ");
    int selectedTicketId = selectedTicketDetails[0].split(" ").last().toInt();

    // Получение новых данных
    int newSeatNumber = QInputDialog::getInt(this, tr("Редактирование билета"), tr("Введите новый номер места:"), 1, 1, 2147483647, 1, &ok);
    if (!ok) return;

    QString newDepartureTime = QInputDialog::getText(this, tr("Редактирование билета"), tr("Введите новое время отправления (формат HH:MM:SS):"), QLineEdit::Normal, QString(), &ok);
    if (!ok || newDepartureTime.isEmpty()) return;

    QString newArrivalTime = QInputDialog::getText(this, tr("Редактирование билета"), tr("Введите новое время прибытия (формат HH:MM:SS):"), QLineEdit::Normal, QString(), &ok);
    if (!ok || newArrivalTime.isEmpty()) return;

    // Обновление данных билета в базе данных
    query.prepare("UPDATE tickets SET seat_number = :seat_number, departure_time = :departure_time, arrival_time = :arrival_time WHERE ticket_id = :ticket_id");
    query.bindValue(":seat_number", newSeatNumber);
    query.bindValue(":departure_time", newDepartureTime);
    query.bindValue(":arrival_time", newArrivalTime);
    query.bindValue(":ticket_id", selectedTicketId);

    if (!query.exec()) {
        QMessageBox::warning(0, "Ошибка", query.lastError().databaseText());
    } else {
        QMessageBox::information(0, "Успешно", "Билет успешно отредактирован.");
        viewData(); // Обновить таблицу
    }

}

void glavnaya::addData()
{
    bool ok;

    // Получение максимального значения ticket_id из таблицы tickets
    int maxTicketId = 0;
    QSqlQuery maxIdQuery(QSqlDatabase::database("postgres"));
    if (maxIdQuery.exec("SELECT MAX(ticket_id) FROM tickets")) {
        if (maxIdQuery.next()) {
            maxTicketId = maxIdQuery.value(0).toInt();
        }
    } else {
        QMessageBox::warning(0, "Ошибка", maxIdQuery.lastError().databaseText());
        return;
    }

    // Начальное значение для нового билета
    int ticket_id = maxTicketId + 1;

    // Запрос для получения доступных маршрутов
    QSqlQuery routeQuery(QSqlDatabase::database("postgres"));
    if (!routeQuery.exec("SELECT route_id FROM routes")) {
        QMessageBox::warning(0, "Ошибка", routeQuery.lastError().databaseText());
        return;
    }
    QStringList routeList;
    while (routeQuery.next()) {
        routeList << routeQuery.value("route_id").toString();
    }
    QString selectedRoute = QInputDialog::getItem(this, tr("Добавление билета"), tr("Выберите ID маршрута:"), routeList, 0, false, &ok);
    if (!ok || selectedRoute.isEmpty()) {
        return;
    }
    int route_id = selectedRoute.toInt();

    // Запрос для получения доступных поездов для выбранного маршрута
    QSqlQuery trainQuery(QSqlDatabase::database("postgres"));
    trainQuery.prepare("SELECT train_id, departure_time, arrival_time FROM trains WHERE route_id = :route_id");
    trainQuery.bindValue(":route_id", route_id);
    if (!trainQuery.exec()) {
        QMessageBox::warning(0, "Ошибка", trainQuery.lastError().databaseText());
        return;
    }
    QStringList trainList;
    QMap<int, QPair<QTime, QTime>> trainScheduleMap; // Для хранения расписания поездов
    while (trainQuery.next()) {
        int train_id = trainQuery.value("train_id").toInt();
        QTime departureTime = trainQuery.value("departure_time").toTime();
        QTime arrivalTime = trainQuery.value("arrival_time").toTime();
        trainScheduleMap[train_id] = qMakePair(departureTime, arrivalTime);
        trainList << QString::number(train_id);
    }
    QString selectedTrain = QInputDialog::getItem(this, tr("Добавление билета"), tr("Выберите ID поезда:"), trainList, 0, false, &ok);
    if (!ok || selectedTrain.isEmpty()) {
        return;
    }
    int train_id = selectedTrain.toInt();

    // Установка времени отправления и прибытия на основе расписания выбранного поезда
    if (trainScheduleMap.contains(train_id)) {
        QPair<QTime, QTime> schedule = trainScheduleMap[train_id];
        QString departure_time = schedule.first.toString("HH:mm:ss");
        QString arrival_time = schedule.second.toString("HH:mm:ss");

        // Запрос остальных данных для добавления билета
        int seat_number = QInputDialog::getInt(this, tr("Добавление билета"), tr("Введите номер места:"), 1, 1, 2147483647, 1, &ok);
        if (!ok) {
            return;
        }
        QString passenger_name = QInputDialog::getText(this, tr("Добавление билета"), tr("Введите имя пассажира:"), QLineEdit::Normal, QString(), &ok);
        if (!ok || passenger_name.isEmpty()) {
            return;
        }

        // Добавление билета с установленными временем отправления и прибытия
        QSqlQuery query(QSqlDatabase::database("postgres"));
        query.prepare("INSERT INTO tickets (ticket_id, route_id, train_id, seat_number, passenger_name, departure_time, arrival_time) VALUES (:ticket_id, :route_id, :train_id, :seat_number, :passenger_name, :departure_time, :arrival_time)");
        query.bindValue(":ticket_id", ticket_id);
        query.bindValue(":route_id", route_id);
        query.bindValue(":train_id", train_id);
        query.bindValue(":seat_number", seat_number);
        query.bindValue(":passenger_name", passenger_name);
        query.bindValue(":departure_time", departure_time);
        query.bindValue(":arrival_time", arrival_time);
        if (!query.exec()) {
            QMessageBox::warning(0, "Ошибка", query.lastError().databaseText());
        } else {
            QMessageBox::information(0, "Успешно", "Билет успешно добавлен.");
            viewData(); // Обновить таблицу
        }
    }
    else {
        QMessageBox::warning(0, "Ошибка", "Расписание для выбранного поезда не найдено.");
        return;
    }
}

void glavnaya::on_View_clicked()
{
    viewData();
}

void glavnaya::on_Delete_clicked()
{
    deleteData();
}

void glavnaya::on_Edit_clicked()
{
    editData();
}

void glavnaya::on_Add_clicked()
{
    addData();
}

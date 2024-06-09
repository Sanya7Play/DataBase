#include "bdrzhd.h"
#include "ui_bdrzhd.h"
#include <QCryptographicHash>
BDRZHD::BDRZHD(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::BDRZHD)
{
    ui->setupUi(this);

    // Устанавливаем режим отображения пароля для поля ввода
    ui->passwordLineEdit->setEchoMode(QLineEdit::Password);

    // Подключаем сигнал нажатия кнопки к слоту для обработки входа
    connect(ui->loginButton, &QPushButton::clicked, this, &BDRZHD::UserLoggedIn);
}
BDRZHD::~BDRZHD()
{
    delete ui;
}

void BDRZHD::UserLoggedIn()
{
    QString username = ui->loginLineEdit->text(); // Получаем логин из поля ввода
    QString password = ui->passwordLineEdit->text(); // Получаем пароль из поля ввода

    // Здесь вы можете выполнить проверку логина и пароля
    // Например, сравнить их с данными из базы данных или другого источника

    if (username == "1" && password == "1") {
        // Вход выполнен успешно
        qDebug() << "Вход выполнен успешно!";
        QMessageBox::information(this, "Молодец", "Правильный логин или пароль!");

        // Создаем и отображаем объект класса glavnaya
        glavnaya *glavnayaWindow = new glavnaya;
        glavnayaWindow->show();

        // Здесь можно выполнить действия при успешном входе, например, открыть главное окно приложения
    } else {
        // Неправильный логин или пароль
        qDebug() << "Неправильный логин или пароль!";
        // Здесь можно вывести сообщение об ошибке пользователю
        QMessageBox::warning(this, "Ошибка", "Неправильный логин или пароль!");
    }
}



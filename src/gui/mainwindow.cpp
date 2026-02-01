#include "mainwindow.h"
#include "add_company_dialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QLabel>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QApplication>
#include "../utils/logger.h"

MainWindow::MainWindow(DbManager& db, QWidget *parent) 
    : QMainWindow(parent), m_db(db) 
{
    setupUI();
    loadData();
}

MainWindow::~MainWindow() {
}

void MainWindow::setupUI() {
    createMenuBar();

    QWidget* central = new QWidget(this);
    setCentralWidget(central);
    resize(900, 600);
    setWindowTitle("Реестр ИБ-Компаний");

    QVBoxLayout* mainLayout = new QVBoxLayout(central);

    // Панель поиска
    QHBoxLayout* searchLayout = new QHBoxLayout();
    
    m_searchEdit = new QLineEdit(this);
    m_searchEdit->setPlaceholderText("Поиск по названию...");
    connect(m_searchEdit, &QLineEdit::textChanged, this, &MainWindow::onSearchChanged);

    m_filterCombo = new QComboBox(this);
    m_filterCombo->addItem("Все");
    m_filterCombo->addItems({"Вендор (Разработчик)", "Интегратор", "Дистрибьютор", "Аудитор/Консалтинг"});
    connect(m_filterCombo, &QComboBox::currentTextChanged, this, &MainWindow::onSearchChanged);

    searchLayout->addWidget(new QLabel("Поиск:", this));
    searchLayout->addWidget(m_searchEdit, 1);
    searchLayout->addWidget(new QLabel("Тип:", this));
    searchLayout->addWidget(m_filterCombo);
    
    mainLayout->addLayout(searchLayout);

    m_table = new QTableWidget(this);
    m_table->setColumnCount(3); 
    m_table->setHorizontalHeaderLabels({"Название", "Тип", "Дата лицензии"});
    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    mainLayout->addWidget(m_table);

    // Кнопки
    QHBoxLayout* btnLayout = new QHBoxLayout();
    m_btnAdd = new QPushButton("Добавить компанию", this);
    m_btnDelete = new QPushButton("Удалить выбранное", this);
    
    connect(m_btnAdd, &QPushButton::clicked, this, &MainWindow::onAddClicked);
    connect(m_btnDelete, &QPushButton::clicked, this, &MainWindow::onDeleteClicked);

    btnLayout->addWidget(m_btnAdd);
    btnLayout->addWidget(m_btnDelete);
    mainLayout->addLayout(btnLayout);
    //настройки таблицы
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    connect(m_table, &QTableWidget::cellDoubleClicked, this, &MainWindow::onTableDoubleClicked);

    mainLayout->addWidget(m_table);
}

void MainWindow::createMenuBar() {
    QMenuBar* menuBar = new QMenuBar(this);
    setMenuBar(menuBar);

    QMenu* fileMenu = menuBar->addMenu("Файл");
    QAction* exitAction = new QAction("Выход", this);
    connect(exitAction, &QAction::triggered, this, &MainWindow::onActionExit);
    fileMenu->addAction(exitAction);

    QMenu* helpMenu = menuBar->addMenu("Справка");
    QAction* aboutAction = new QAction("О программе", this);
    connect(aboutAction, &QAction::triggered, this, &MainWindow::onActionAbout);
    helpMenu->addAction(aboutAction);
}

void MainWindow::onActionExit() {
    QApplication::quit();
}

void MainWindow::onActionAbout() {
    QString info = "Реестр компаний информационной безопасности\n\n"
                   "Курсовой проект\n"
                   "Стек технологий: C++, Qt, PostgreSQL, Docker\n"
                   "Версия: 1.1.0 (No IDs)";
    QMessageBox::about(this, "О программе", info);
}

void MainWindow::loadData() {
    m_table->setRowCount(0);
    QString name = m_searchEdit->text();
    QString type = m_filterCombo->currentText();
    
    //поиск возвращает и описание
    std::vector<Company> companies = m_db.searchCompanies(name, type);

    for (const auto& c : companies) {
        int row = m_table->rowCount();
        m_table->insertRow(row);

        // Имя + скрытое описание
        QTableWidgetItem* nameItem = new QTableWidgetItem(c.name);
        nameItem->setData(Qt::UserRole, c.description); // Прячем описание тут
        
        m_table->setItem(row, 0, nameItem);
        m_table->setItem(row, 1, new QTableWidgetItem(c.type));
        m_table->setItem(row, 2, new QTableWidgetItem(c.licenseDate.toString("dd.MM.yyyy")));
    }
}
void MainWindow::onSearchChanged() {
    loadData();
}

void MainWindow::onAddClicked() {
    AddCompanyDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        if (m_db.addCompany(dialog.getCompanyData())) {
            loadData();
            LOG_INFO("Новая компания добавлена");
        } else {
            QMessageBox::critical(this, "Ошибка", "Не удалось сохранить в БД. Возможно, такая компания уже есть.");
        }
    }
}

void MainWindow::onDeleteClicked() {
    int currentRow = m_table->currentRow();
    if (currentRow < 0) {
        QMessageBox::warning(this, "Внимание", "Выберите строку для удаления");
        return;
    }

    // Берем название компании из 0-й колонки 
    QString companyName = m_table->item(currentRow, 0)->text();

    auto reply = QMessageBox::question(this, "Удаление", 
                                     "Удалить запись '" + companyName + "' навсегда?", 
                                     QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        if (m_db.removeCompany(companyName)) { // Передаем имя
            loadData();
            LOG_INFO("Компания удалена: {}", companyName.toStdString());
        } else {
            QMessageBox::critical(this, "Ошибка", "Ошибка при удалении из БД");
        }
    }
}
void MainWindow::onTableDoubleClicked(int row, int column) {
    // Собираем данные из строки
    Company c;
    c.name = m_table->item(row, 0)->text();
    c.description = m_table->item(row, 0)->data(Qt::UserRole).toString(); // Достаем скрытое
    c.type = m_table->item(row, 1)->text();
    c.licenseDate = QDate::fromString(m_table->item(row, 2)->text(), "dd.MM.yyyy");

    // Открываем диалог
    AddCompanyDialog dialog(this);
    dialog.setCompanyData(c); // Заполняем поля

    if (dialog.exec() == QDialog::Accepted) {
        // Получаем новые данные
        Company newC = dialog.getCompanyData();
        
        // Отправляем в базу
        if (m_db.updateCompany(c.name, newC.licenseDate, newC.description)) {
            loadData(); // Обновляем таблицу
            LOG_INFO("Компания обновлена: {}", c.name.toStdString());
        } else {
            QMessageBox::critical(this, "Ошибка", "Не удалось обновить запись");
        }
    }
}

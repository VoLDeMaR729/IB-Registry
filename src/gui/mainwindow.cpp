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
    resize(1200, 600);
    setWindowTitle("Реестр ИБ-Компаний");

    QVBoxLayout* mainLayout = new QVBoxLayout(central);
    QHBoxLayout* searchLayout = new QHBoxLayout();
    
    m_searchEdit = new QLineEdit(this);
    m_searchEdit->setPlaceholderText("Поиск по названию, ИНН или № лицензии...");
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

    m_table->setColumnCount(7); 
    m_table->setHorizontalHeaderLabels({"Имя", "ИНН", "Адрес", "Тип", "Лицензия №", "Дата", "Описание"});

    m_table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    m_table->horizontalHeader()->setSectionResizeMode(6, QHeaderView::Stretch);
    
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    
    connect(m_table, &QTableWidget::cellDoubleClicked, this, &MainWindow::onTableDoubleClicked);

    mainLayout->addWidget(m_table);

    QHBoxLayout* btnLayout = new QHBoxLayout();
    m_btnAdd = new QPushButton("Добавить компанию", this);
    m_btnDelete = new QPushButton("Удалить выбранное", this);
    
    connect(m_btnAdd, &QPushButton::clicked, this, &MainWindow::onAddClicked);
    connect(m_btnDelete, &QPushButton::clicked, this, &MainWindow::onDeleteClicked);

    btnLayout->addWidget(m_btnAdd);
    btnLayout->addWidget(m_btnDelete);
    mainLayout->addLayout(btnLayout);
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
                   "Версия: 2.0.0 (Extended)";
    QMessageBox::about(this, "О программе", info);
}

void MainWindow::loadData() {
    m_table->setRowCount(0);
    QString name = m_searchEdit->text();
    QString type = m_filterCombo->currentText();
    
    std::vector<Company> companies = m_db.searchCompanies(name, type);

    for (const auto& c : companies) {
        int row = m_table->rowCount();
        m_table->insertRow(row);

        QTableWidgetItem* nameItem = new QTableWidgetItem(c.name);
        nameItem->setData(Qt::UserRole, c.ogrn);
        m_table->setItem(row, 0, nameItem);

        m_table->setItem(row, 1, new QTableWidgetItem(c.inn));

        m_table->setItem(row, 2, new QTableWidgetItem(c.address));

        m_table->setItem(row, 3, new QTableWidgetItem(c.type));

        m_table->setItem(row, 4, new QTableWidgetItem(c.licenseNum));

        m_table->setItem(row, 5, new QTableWidgetItem(c.licenseDate.toString("dd.MM.yyyy")));

        m_table->setItem(row, 6, new QTableWidgetItem(c.description));
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
            QMessageBox::critical(this, "Ошибка", "Не удалось сохранить в БД. Проверьте уникальность Имени или ИНН.");
        }
    }
}

void MainWindow::onDeleteClicked() {
    int currentRow = m_table->currentRow();
    if (currentRow < 0) {
        QMessageBox::warning(this, "Внимание", "Выберите строку для удаления");
        return;
    }

    QString companyName = m_table->item(currentRow, 0)->text();

    auto reply = QMessageBox::question(this, "Удаление", 
                                     "Удалить запись '" + companyName + "' навсегда?", 
                                     QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        if (m_db.removeCompany(companyName)) { 
            loadData();
            LOG_INFO("Компания удалена: {}", companyName.toStdString());
        } else {
            QMessageBox::critical(this, "Ошибка", "Ошибка при удалении из БД");
        }
    }
}

void MainWindow::onTableDoubleClicked(int row, int column) {
    // Собираем данные из строки таблицы обратно в структуру Company
    Company c;
    c.name = m_table->item(row, 0)->text();
    c.ogrn = m_table->item(row, 0)->data(Qt::UserRole).toString();
    c.inn = m_table->item(row, 1)->text();
    c.address = m_table->item(row, 2)->text();
    c.type = m_table->item(row, 3)->text();
    c.licenseNum = m_table->item(row, 4)->text();
    c.licenseDate = QDate::fromString(m_table->item(row, 5)->text(), "dd.MM.yyyy");
    c.description = m_table->item(row, 6)->text();

    // Открываем диалог в режиме редактирования
    AddCompanyDialog dialog(this);
    dialog.setCompanyData(c); 

    if (dialog.exec() == QDialog::Accepted) {
        Company newC = dialog.getCompanyData();
        if (m_db.updateCompany(c.name, newC.address, newC.licenseNum, newC.licenseDate, newC.description)) {
            loadData(); 
            LOG_INFO("Компания обновлена: {}", c.name.toStdString());
        } else {
            QMessageBox::critical(this, "Ошибка", "Не удалось обновить запись");
        }
    }
}

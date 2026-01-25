#include "mainwindow.h"
#include "add_company_dialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QLabel>
#include <QMenuBar>     // Для полоски меню
#include <QMenu>        // Для выпадающих списков
#include <QAction>      // Для действий (кнопок в меню)
#include <QApplication> // Для выхода из программы
#include "../utils/logger.h"

// Конструктор: инициализация и запуск интерфейса
MainWindow::MainWindow(DbManager& db, QWidget *parent) 
    : QMainWindow(parent), m_db(db) 
{
    setupUI();
    loadData(); // Загрузка данных сразу после открытия
}

MainWindow::~MainWindow() {
}

// Настройка внешнего вида
void MainWindow::setupUI() {
    // 1. Создаем верхнее меню
    createMenuBar();

    QWidget* central = new QWidget(this);
    setCentralWidget(central);
    resize(900, 600);
    setWindowTitle("Реестр ИБ-Компаний");

    QVBoxLayout* mainLayout = new QVBoxLayout(central);

    // 2. Панель поиска
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

    // 3. Таблица
    m_table = new QTableWidget(this);
    m_table->setColumnCount(4);
    m_table->setHorizontalHeaderLabels({"ID", "Название", "Тип", "Дата лицензии"});
    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    mainLayout->addWidget(m_table);

    // 4. Нижние кнопки
    QHBoxLayout* btnLayout = new QHBoxLayout();
    m_btnAdd = new QPushButton("Добавить компанию", this);
    m_btnDelete = new QPushButton("Удалить выбранное", this);
    
    connect(m_btnAdd, &QPushButton::clicked, this, &MainWindow::onAddClicked);
    connect(m_btnDelete, &QPushButton::clicked, this, &MainWindow::onDeleteClicked);

    btnLayout->addWidget(m_btnAdd);
    btnLayout->addWidget(m_btnDelete);
    mainLayout->addLayout(btnLayout);
}

//меню
void MainWindow::createMenuBar() {
    QMenuBar* menuBar = new QMenuBar(this);
    setMenuBar(menuBar);

    // Меню "Файл"
    QMenu* fileMenu = menuBar->addMenu("Файл");
    
    // Пункт "Выход"
    QAction* exitAction = new QAction("Выход", this);
    connect(exitAction, &QAction::triggered, this, &MainWindow::onActionExit);
    fileMenu->addAction(exitAction);

    // Меню "Справка"
    QMenu* helpMenu = menuBar->addMenu("Справка");
    
    // Пункт "О программе"
    QAction* aboutAction = new QAction("О программе", this);
    connect(aboutAction, &QAction::triggered, this, &MainWindow::onActionAbout);
    helpMenu->addAction(aboutAction);
}

// Слот: Нажатие "Файл -> Выход"
void MainWindow::onActionExit() {
    QApplication::quit();
}

// Слот: Нажатие "Справка -> О программе"
void MainWindow::onActionAbout() {
    QString info = "Реестр компаний информационной безопасности\n\n"
                   "Курсовой проект\n"
                   "Стек технологий: C++, Qt, PostgreSQL, Docker\n"
                   "Версия: 1.0.0";
    QMessageBox::about(this, "О программе", info);
}

//Стандартные методы (поиск, добавление, удаление)
void MainWindow::loadData() {
    m_table->setRowCount(0);
    QString name = m_searchEdit->text();
    QString type = m_filterCombo->currentText();

    std::vector<Company> companies = m_db.searchCompanies(name, type);

    for (const auto& c : companies) {
        int row = m_table->rowCount();
        m_table->insertRow(row);
        m_table->setItem(row, 0, new QTableWidgetItem(QString::number(c.id)));
        m_table->setItem(row, 1, new QTableWidgetItem(c.name));
        m_table->setItem(row, 2, new QTableWidgetItem(c.type));
        m_table->setItem(row, 3, new QTableWidgetItem(c.licenseDate.toString("dd.MM.yyyy")));
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
            QMessageBox::critical(this, "Ошибка", "Не удалось сохранить в БД");
        }
    }
}

void MainWindow::onDeleteClicked() {
    int currentRow = m_table->currentRow();
    if (currentRow < 0) {
        QMessageBox::warning(this, "Внимание", "Выберите строку для удаления");
        return;
    }

    int id = m_table->item(currentRow, 0)->text().toInt();

    auto reply = QMessageBox::question(this, "Удаление", 
                                     "Удалить запись навсегда?", 
                                     QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        if (m_db.removeCompany(id)) {
            loadData();
            LOG_INFO("Компания удалена (ID: {})", id);
        } else {
            QMessageBox::critical(this, "Ошибка", "Ошибка при удалении из БД");
        }
    }
}

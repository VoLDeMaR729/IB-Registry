#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QLabel>
#include <QMenuBar>
#include <QStatusBar>
#include <QApplication>
#include "add_company_dialog.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setupUi();
    setupModel();
}

MainWindow::~MainWindow() {}

void MainWindow::setupUi() {
    resize(1200, 700);
    setWindowTitle("Реестр Лицензиатов ИБ");

    m_centralWidget = new QWidget(this);
    setCentralWidget(m_centralWidget);
    QVBoxLayout *layout = new QVBoxLayout(m_centralWidget);

    QMenuBar *menu = new QMenuBar(this);
    setMenuBar(menu);
    QMenu *file = menu->addMenu("Файл");
    file->addAction("Выход", qApp, &QApplication::quit);
    QMenu *help = menu->addMenu("Справка");
    help->addAction("О программе", [this](){
        QMessageBox::about(this, "Инфо", "Курсовой проект: Реестр ИБ");
    });

    QHBoxLayout *top = new QHBoxLayout();
    m_searchEdit = new QLineEdit();
    m_searchEdit->setPlaceholderText("Поиск...");
    connect(m_searchEdit, &QLineEdit::textChanged, this, &MainWindow::onSearchChanged);
    top->addWidget(new QLabel("Поиск:"));
    top->addWidget(m_searchEdit, 1);
    layout->addLayout(top);

    m_tableView = new QTableView(this);
    m_tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    connect(m_tableView, &QTableView::doubleClicked, this, &MainWindow::onTableDoubleClicked);
    
    layout->addWidget(m_tableView);

    QHBoxLayout *btns = new QHBoxLayout();
    m_addBtn = new QPushButton("Добавить");
    m_deleteBtn = new QPushButton("Удалить");
    m_refreshBtn = new QPushButton("Обновить");

    connect(m_addBtn, &QPushButton::clicked, this, &MainWindow::onAddClicked);
    connect(m_deleteBtn, &QPushButton::clicked, this, &MainWindow::onDeleteClicked);
    connect(m_refreshBtn, &QPushButton::clicked, this, &MainWindow::onRefreshClicked);

    btns->addWidget(m_addBtn);
    btns->addWidget(m_deleteBtn);
    btns->addStretch();
    btns->addWidget(m_refreshBtn);
    layout->addLayout(btns);
}

void MainWindow::setupModel() {
    m_model = m_db.getTableModel(this);
    m_tableView->setModel(m_model);
    configureTable();
}

void MainWindow::configureTable() {
    m_model->setHeaderData(1, Qt::Horizontal, "Название");
    m_model->setHeaderData(3, Qt::Horizontal, "ИНН");
    m_model->setHeaderData(6, Qt::Horizontal, "Адрес");
    m_model->setHeaderData(7, Qt::Horizontal, "Сайт");

    m_tableView->hideColumn(0); m_tableView->hideColumn(2); m_tableView->hideColumn(4);
    m_tableView->hideColumn(5); m_tableView->hideColumn(8);

    m_tableView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_tableView->horizontalHeader()->setSectionResizeMode(6, QHeaderView::Stretch);
}

void MainWindow::onSearchChanged(const QString &text) {
    if (text.isEmpty()) m_model->setFilter("");
    else m_model->setFilter(QString("name ILIKE '%%1%' OR inn LIKE '%%1%'").arg(text));
}

void MainWindow::onAddClicked() {
    AddCompanyDialog d(this);
    if (d.exec() == QDialog::Accepted) {
        if (m_db.addCompany(d.getCompanyData())) {
            QMessageBox::information(this, "Успех", "Запись добавлена");
            onRefreshClicked();
        } else QMessageBox::critical(this, "Ошибка", "Сбой БД (возможен дубликат)");
    }
}

void MainWindow::onTableDoubleClicked(const QModelIndex &index) {
    int row = index.row();
    QString inn = m_model->data(m_model->index(row, 3)).toString();
    Company c = m_db.getCompanyFullData(inn);

    AddCompanyDialog d(this);
    d.setCompanyData(c);
    d.setReadOnly(true); 

    if (d.exec() == QDialog::Accepted) {
        Company updated = d.getCompanyData();
        updated.inn = inn; 
        if (m_db.updateCompany(updated)) {
            QMessageBox::information(this, "Успех", "Обновлено");
            onRefreshClicked();
        } else QMessageBox::critical(this, "Ошибка", "Не удалось обновить");
    }
}

void MainWindow::onDeleteClicked() {
    auto sel = m_tableView->selectionModel()->selectedRows();
    if(sel.isEmpty()) return;
    QString name = m_model->data(m_model->index(sel.first().row(), 1)).toString();
    if(m_db.removeCompany(name)) onRefreshClicked();
}

void MainWindow::onRefreshClicked() {
    m_model->select();
}

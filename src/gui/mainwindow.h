#pragma once
#include <QMainWindow>
#include <QTableView>
#include <QPushButton>
#include <QLineEdit>
#include <QSqlTableModel>
#include "../core/db_manager.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onAddClicked();
    void onRefreshClicked();
    void onSearchChanged(const QString &text);
    void onDeleteClicked();
    void onTableDoubleClicked(const QModelIndex &index);

private:
    void setupUi();
    void setupModel();
    void configureTable();

    DBManager m_db;
    QSqlTableModel *m_model;

    QWidget *m_centralWidget;
    QTableView *m_tableView;
    QLineEdit *m_searchEdit;
    QPushButton *m_addBtn;
    QPushButton *m_refreshBtn;
    QPushButton *m_deleteBtn;
};

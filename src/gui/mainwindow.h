#pragma once

#include <QMainWindow>
#include <QTableWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include "../core/db_manager.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(DbManager& db, QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // Слоты кнопок
    void onAddClicked();
    void onDeleteClicked();
    
    // Слоты поиска
    void onSearchChanged();

    //Обработка двойного клика по таблице
    void onTableDoubleClicked(int row, int column);

    // Слоты меню
    void onActionAbout();
    void onActionExit();

private:
    void setupUI();
    void createMenuBar();
    void loadData();

    DbManager& m_db;
    
    QTableWidget* m_table;
    QPushButton* m_btnAdd;
    QPushButton* m_btnDelete;
    
    QLineEdit* m_searchEdit;
    QComboBox* m_filterCombo;
};

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
    // Конструктор принимает ссылку на подключенную базу
    explicit MainWindow(DbManager& db, QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    //Слоты кнопок
    void onAddClicked();
    void onDeleteClicked();
    
    //Слоты поиска
    void onSearchChanged();

    //Слоты меню
    void onActionAbout(); // О программе
    void onActionExit();  // Выход

private:
    void setupUI();
    void createMenuBar(); // Метод создания верхнего меню
    void loadData();

    DbManager& m_db;
    
    // Элементы таблицы и кнопок
    QTableWidget* m_table;
    QPushButton* m_btnAdd;
    QPushButton* m_btnDelete;
    
    // Элементы поиска
    QLineEdit* m_searchEdit;
    QComboBox* m_filterCombo;
};

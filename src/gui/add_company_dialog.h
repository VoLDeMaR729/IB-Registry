#pragma once
#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QTabWidget>
#include <QTableWidget>
#include <QTreeWidget>
#include <QDateEdit>
#include <QPushButton>
#include <QLabel>
#include "../core/domain_types.h"

class AddCompanyDialog : public QDialog {
    Q_OBJECT

public:
    explicit AddCompanyDialog(QWidget *parent = nullptr);
    Company getCompanyData() const;           
    void setCompanyData(const Company &c); 
    void setReadOnly(bool readOnly); 

private slots:
    void addLicenseRow();
    void removeLicenseRow();
    void onLicenseTypeChanged(int row);
    void onEditClicked();

private:
    void setupUi();
    void setupGeneralTab();
    void setupLicensesTab();
    void setupServicesTab();
    void populateServicesTree();
    void loadCities();

    QTabWidget *m_tabs;
    bool m_isViewMode = false;

    // Кнопки управления
    QPushButton *m_saveBtn;
    QPushButton *m_cancelBtn;
    QPushButton *m_editBtn;

    // Вкладка 1
    QComboBox *m_orgTypeCombo; 
    QLineEdit *m_nameEdit;
    QLineEdit *m_fullNameEdit;
    QLineEdit *m_innEdit;
    QLineEdit *m_ogrnEdit;
    QComboBox *m_cityCombo;
    QLineEdit *m_websiteEdit;
    
    QLineEdit *m_streetEdit; 
    QLineEdit *m_houseEdit;
    QLineEdit *m_officeEdit;
    
    // Вкладка 2
    QTableWidget *m_licensesTable;
    QPushButton *m_addLicenseBtn;
    QPushButton *m_delLicenseBtn;

    // Вкладка 3
    QTreeWidget *m_servicesTree;
};

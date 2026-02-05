#pragma once
#include <QDialog>
#include <QLineEdit>
#include <QDateEdit>
#include <QTextEdit>
#include <QComboBox>
#include <QRegularExpressionValidator>
#include "../core/domain_types.h"

class AddCompanyDialog : public QDialog {
    Q_OBJECT

public:
    explicit AddCompanyDialog(QWidget *parent = nullptr);
    
    Company getCompanyData() const;
    void setCompanyData(const Company& c);

private:
    void setupUI();

    QComboBox* m_nameEdit; 
    
    QLineEdit* m_innEdit;
    QLineEdit* m_ogrnEdit;
    
    QComboBox* m_cityCombo;
    QLineEdit* m_addressEdit;
    
    QLineEdit* m_licenseNumEdit;
    QComboBox* m_typeCombo;
    QDateEdit* m_dateEdit;
    QTextEdit* m_descEdit;
};

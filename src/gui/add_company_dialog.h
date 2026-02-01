#pragma once
#include <QDialog>
#include <QLineEdit>
#include <QDateEdit>
#include <QTextEdit>
#include <QComboBox>
#include "../core/domain_types.h"

class AddCompanyDialog : public QDialog {
    Q_OBJECT

public:
    explicit AddCompanyDialog(QWidget *parent = nullptr);
    
    Company getCompanyData() const;

    //Заполнить поля для редактирования
    void setCompanyData(const Company& c);

private:
    void setupUI();
    QLineEdit* m_nameEdit;
    QComboBox* m_typeCombo;
    QDateEdit* m_dateEdit;
    QTextEdit* m_descEdit;
};

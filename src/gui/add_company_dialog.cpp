#include "add_company_dialog.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QDate>

AddCompanyDialog::AddCompanyDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle("Добавление компании");
    setupUI();
}

void AddCompanyDialog::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    QFormLayout* formLayout = new QFormLayout();

    // поля ввода
    m_nameEdit = new QLineEdit(this);
    
    m_typeCombo = new QComboBox(this);
    m_typeCombo->addItems({"Вендор (Разработчик)", "Интегратор", "Дистрибьютор", "Аудитор/Консалтинг"});

    m_dateEdit = new QDateEdit(QDate::currentDate(), this);
    m_dateEdit->setCalendarPopup(true);

    m_descEdit = new QTextEdit(this);
    m_descEdit->setMaximumHeight(100);

    formLayout->addRow("Название:", m_nameEdit);
    formLayout->addRow("Тип деятельности:", m_typeCombo);
    formLayout->addRow("Дата лицензии:", m_dateEdit);
    formLayout->addRow("Описание:", m_descEdit);

    mainLayout->addLayout(formLayout);

    // кнопки ок/отмена
    QDialogButtonBox* btnBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
        Qt::Horizontal, this);
    
    connect(btnBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(btnBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    mainLayout->addWidget(btnBox);
}

Company AddCompanyDialog::getCompanyData() const {
    Company c;
    c.name = m_nameEdit->text();
    c.type = m_typeCombo->currentText();
    c.licenseDate = m_dateEdit->date();
    c.description = m_descEdit->toPlainText();
    return c;
}

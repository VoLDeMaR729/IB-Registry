#include "add_company_dialog.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QDialogButtonBox>
#include <QDate>
#include <QRegularExpression>
#include <QValidator>

AddCompanyDialog::AddCompanyDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle("Карточка компании");
    setupUI();
}

void AddCompanyDialog::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    QFormLayout* formLayout = new QFormLayout();

    m_nameEdit = new QComboBox(this);
    m_nameEdit->setEditable(true);
    m_nameEdit->addItems({
        "ООО \"Лаборатория Касперского\"",
        "ООО \"Позитив Текнолоджиз\"",
        "АО \"ИнфоТеКС\"",
        "ООО \"Код Безопасности\"",
        "ООО \"УЦСБ\"",
        "АО \"НПО \"Эшелон\"",
        "ООО \"Газинформсервис\""
    });
    m_nameEdit->setCurrentIndex(-1);
    m_nameEdit->setPlaceholderText("Выберите или введите название");

    // 2. ИНН
    m_innEdit = new QLineEdit(this);
    m_innEdit->setPlaceholderText("10 или 12 цифр");
    m_innEdit->setMaxLength(12);
    m_innEdit->setValidator(new QRegularExpressionValidator(QRegularExpression("[0-9]*"), this));

    // 3. ОГРН
    m_ogrnEdit = new QLineEdit(this);
    m_ogrnEdit->setPlaceholderText("13 или 15 цифр");
    m_ogrnEdit->setMaxLength(15);
    m_ogrnEdit->setValidator(new QRegularExpressionValidator(QRegularExpression("[0-9]*"), this));

    // 4. Тип
    m_typeCombo = new QComboBox(this);
    m_typeCombo->addItems({"Вендор (Разработчик)", "Интегратор", "Дистрибьютор", "Аудитор/Консалтинг"});

    // 5. Адрес
    QWidget* addressWidget = new QWidget(this);
    QHBoxLayout* addressLayout = new QHBoxLayout(addressWidget);
    addressLayout->setContentsMargins(0, 0, 0, 0);

    m_cityCombo = new QComboBox(this);
    m_cityCombo->setEditable(false); // ЗАПРЕЩАЕМ писать бред в городе, только выбор!
    m_cityCombo->addItems({"Москва", "Санкт-Петербург", "Новосибирск", "Екатеринбург", "Казань", "Нижний Новгород", "Челябинск", "Самара", "Омск", "Владивосток"});
    
    m_addressEdit = new QLineEdit(this);
    m_addressEdit->setPlaceholderText("Улица, дом, офис");
    m_addressEdit->setValidator(new QRegularExpressionValidator(QRegularExpression("[а-яА-Яa-zA-Z0-9\\s\\.,-]*"), this));

    addressLayout->addWidget(m_cityCombo, 1);
    addressLayout->addWidget(m_addressEdit, 2);

    // 6. Номер лицензии
    m_licenseNumEdit = new QLineEdit(this);
    m_licenseNumEdit->setInputMask("nnnn-00000-00/00000000"); 
    m_licenseNumEdit->setPlaceholderText("Л024-...");

    // 7. Дата
    m_dateEdit = new QDateEdit(QDate::currentDate(), this);
    m_dateEdit->setCalendarPopup(true);

    // 8. Описание
    m_descEdit = new QTextEdit(this);
    m_descEdit->setMaximumHeight(100);

    formLayout->addRow("Название:", m_nameEdit);
    formLayout->addRow("ИНН:", m_innEdit);
    formLayout->addRow("ОГРН:", m_ogrnEdit);
    formLayout->addRow("Тип:", m_typeCombo);
    formLayout->addRow("Адрес:", addressWidget);
    formLayout->addRow("Лицензия:", m_licenseNumEdit);
    formLayout->addRow("Дата:", m_dateEdit);
    formLayout->addRow("Описание:", m_descEdit);

    mainLayout->addLayout(formLayout);

    QDialogButtonBox* btnBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
        Qt::Horizontal, this);
    
    connect(btnBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(btnBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    mainLayout->addWidget(btnBox);
}

Company AddCompanyDialog::getCompanyData() const {
    Company c;
    c.name = m_nameEdit->currentText(); // Берем текст из Combo
    c.inn = m_innEdit->text();
    c.ogrn = m_ogrnEdit->text();
    c.type = m_typeCombo->currentText();
    
    c.address = m_cityCombo->currentText() + ", " + m_addressEdit->text();
    
    c.licenseNum = m_licenseNumEdit->text();
    c.licenseDate = m_dateEdit->date();
    c.description = m_descEdit->toPlainText();
    return c;
}

void AddCompanyDialog::setCompanyData(const Company& c) {
    m_nameEdit->setCurrentText(c.name);
    m_nameEdit->setEnabled(false);
    
    m_innEdit->setText(c.inn);
    m_innEdit->setReadOnly(true);
    
    m_ogrnEdit->setText(c.ogrn);
    m_ogrnEdit->setReadOnly(true);

    m_typeCombo->setCurrentText(c.type);
    m_typeCombo->setEnabled(false);

    QString fullAddress = c.address;
    int commaIndex = fullAddress.indexOf(",");

    if (commaIndex != -1) {
        QString city = fullAddress.left(commaIndex).trimmed(); 
        QString street = fullAddress.mid(commaIndex + 1).trimmed();
        
        m_cityCombo->setCurrentText(city);
        m_addressEdit->setText(street);
    } else {
        m_addressEdit->setText(fullAddress);
    }
    
    m_licenseNumEdit->setText(c.licenseNum);
    m_dateEdit->setDate(c.licenseDate);
    m_descEdit->setText(c.description);

    setWindowTitle("Редактирование компании");
}

#include "add_company_dialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QHeaderView>
#include <QFormLayout>
#include <QMessageBox>
#include <QRegularExpressionValidator>
#include <QSqlQuery>
#include <QDebug>

AddCompanyDialog::AddCompanyDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle("Карточка организации");
    resize(1000, 750);
    setupUi();
    loadCities();
    populateServicesTree();
}

void AddCompanyDialog::setupUi() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    m_tabs = new QTabWidget(this);
    
    setupGeneralTab();
    setupLicensesTab();
    setupServicesTab();

    mainLayout->addWidget(m_tabs);

    QHBoxLayout *btnLayout = new QHBoxLayout();
    
    // Кнопки
    m_saveBtn = new QPushButton("Сохранить", this);
    m_cancelBtn = new QPushButton("Закрыть", this);
    m_editBtn = new QPushButton("Редактировать", this);
    m_editBtn->setVisible(false);

    connect(m_saveBtn, &QPushButton::clicked, this, &QDialog::accept);
    connect(m_cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
    connect(m_editBtn, &QPushButton::clicked, this, &AddCompanyDialog::onEditClicked);

    btnLayout->addWidget(m_editBtn);
    btnLayout->addStretch();
    btnLayout->addWidget(m_cancelBtn);
    btnLayout->addWidget(m_saveBtn);
    mainLayout->addLayout(btnLayout);
}

void AddCompanyDialog::loadCities() {
    m_cityCombo->clear();
    QSqlQuery q("SELECT name FROM cities ORDER BY name");
    while (q.next()) {
        m_cityCombo->addItem(q.value(0).toString());
    }
}

void AddCompanyDialog::populateServicesTree() {
    m_servicesTree->clear();
    
    // 1. Грузим категории
    QMap<int, QTreeWidgetItem*> categories;
    QSqlQuery qCat("SELECT id, name FROM service_categories ORDER BY name");
    while(qCat.next()) {
        int id = qCat.value(0).toInt();
        QString name = qCat.value(1).toString();
        
        QTreeWidgetItem *item = new QTreeWidgetItem(m_servicesTree);
        item->setText(0, name);
        item->setExpanded(true);
        categories[id] = item;
    }

    // 2. Грузим услуги и кладем в категории
    QSqlQuery qServ("SELECT category_id, name FROM services ORDER BY name");
    while(qServ.next()) {
        int catId = qServ.value(0).toInt();
        QString name = qServ.value(1).toString();

        if (categories.contains(catId)) {
            QTreeWidgetItem *child = new QTreeWidgetItem(categories[catId]);
            child->setText(0, name);
            child->setCheckState(0, Qt::Unchecked);
        }
    }
}

void AddCompanyDialog::setupGeneralTab() {
    QWidget *tab = new QWidget();
    QFormLayout *layout = new QFormLayout(tab);

    QHBoxLayout *nameLayout = new QHBoxLayout();
    m_orgTypeCombo = new QComboBox();
    m_orgTypeCombo->addItems({"ООО", "АО", "ПАО", "ИП", "ФГУП", "ГК"});
    m_nameEdit = new QLineEdit();
    m_nameEdit->setPlaceholderText("Название компании");
    nameLayout->addWidget(m_orgTypeCombo);
    nameLayout->addWidget(m_nameEdit);

    m_fullNameEdit = new QLineEdit(); m_fullNameEdit->setReadOnly(true);
    m_innEdit = new QLineEdit(); m_innEdit->setInputMask("999999999999"); m_innEdit->setPlaceholderText("ИНН");
    m_ogrnEdit = new QLineEdit(); m_ogrnEdit->setInputMask("9999999999999"); m_ogrnEdit->setPlaceholderText("ОГРН");

    m_streetEdit = new QLineEdit(); m_streetEdit->setPlaceholderText("Улица");
    m_streetEdit->setValidator(new QRegularExpressionValidator(QRegularExpression("^[A-Za-zА-Яа-я \\-\\.]+$"), this));
    
    m_houseEdit = new QLineEdit(); m_houseEdit->setPlaceholderText("Дом");
    m_houseEdit->setValidator(new QRegularExpressionValidator(QRegularExpression("^[0-9]+$"), this));
    m_houseEdit->setFixedWidth(80);

    m_officeEdit = new QLineEdit(); m_officeEdit->setPlaceholderText("Оф.");
    m_officeEdit->setFixedWidth(60);

    QWidget *addrW = new QWidget();
    QHBoxLayout *al = new QHBoxLayout(addrW);
    al->setContentsMargins(0,0,0,0);
    al->addWidget(m_streetEdit); al->addWidget(m_houseEdit); al->addWidget(m_officeEdit);

    m_cityCombo = new QComboBox();
    m_websiteEdit = new QLineEdit(); m_websiteEdit->setPlaceholderText("сайт.рф");

    layout->addRow("Название:", nameLayout);
    layout->addRow("Полное:", m_fullNameEdit);
    layout->addRow("ИНН:", m_innEdit);
    layout->addRow("ОГРН:", m_ogrnEdit);
    layout->addRow("Город:", m_cityCombo);
    layout->addRow("Адрес:", addrW);
    layout->addRow("Сайт:", m_websiteEdit);

    m_tabs->addTab(tab, "1. Реквизиты");
}

void AddCompanyDialog::setupLicensesTab() {
    QWidget *tab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(tab);
    m_licensesTable = new QTableWidget(0, 3);
    m_licensesTable->setHorizontalHeaderLabels({"Вид деятельности", "Номер (Серия)", "Дата"});
    m_licensesTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    layout->addWidget(m_licensesTable);

    QHBoxLayout *bl = new QHBoxLayout();
    m_addLicenseBtn = new QPushButton("Добавить");
    m_delLicenseBtn = new QPushButton("Удалить");
    connect(m_addLicenseBtn, &QPushButton::clicked, this, &AddCompanyDialog::addLicenseRow);
    connect(m_delLicenseBtn, &QPushButton::clicked, this, &AddCompanyDialog::removeLicenseRow);
    bl->addWidget(m_addLicenseBtn); bl->addWidget(m_delLicenseBtn);
    layout->addLayout(bl);
    m_tabs->addTab(tab, "2. Лицензии");
}

void AddCompanyDialog::addLicenseRow() {
    int row = m_licensesTable->rowCount();
    m_licensesTable->insertRow(row);
    
    QComboBox *type = new QComboBox();
    type->addItems({"ТЗКИ (ФСТЭК)", "Разработка СЗИ (ФСТЭК)", "Криптография (ФСБ)", "Гостайна (ФСБ)"});
    connect(type, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int){ onLicenseTypeChanged(row); });
    
    m_licensesTable->setCellWidget(row, 0, type);
    m_licensesTable->setCellWidget(row, 1, new QLineEdit());
    QDateEdit *de = new QDateEdit(QDate::currentDate());
    de->setCalendarPopup(true);
    m_licensesTable->setCellWidget(row, 2, de);
    onLicenseTypeChanged(row);
}

void AddCompanyDialog::onLicenseTypeChanged(int row) {
    auto *c = qobject_cast<QComboBox*>(m_licensesTable->cellWidget(row, 0));
    auto *e = qobject_cast<QLineEdit*>(m_licensesTable->cellWidget(row, 1));
    if(!c || !e) return;

    QString t = c->currentText();
    e->clear();

    if(t.contains("ФСТЭК")) {
        e->setInputMask("Л000-00000-000000");
        e->setPlaceholderText("Л024-...");
    } else {
        e->setInputMask(""); 
        QRegularExpression rx("^[A-Za-zА-Яа-я0-9-]+$");
        e->setValidator(new QRegularExpressionValidator(rx, this));
        e->setPlaceholderText("ЛСЗ...");
    }
}

void AddCompanyDialog::removeLicenseRow() {
    int r = m_licensesTable->currentRow();
    if(r>=0) m_licensesTable->removeRow(r);
}

void AddCompanyDialog::setupServicesTab() {
    QWidget *tab = new QWidget();
    QVBoxLayout *l = new QVBoxLayout(tab);
    m_servicesTree = new QTreeWidget();
    m_servicesTree->setHeaderLabel("Выберите направления деятельности:");
    l->addWidget(m_servicesTree);
    m_tabs->addTab(tab, "3. Услуги");
}

Company AddCompanyDialog::getCompanyData() const {
    Company c;
    c.name = m_nameEdit->text();
    c.fullName = QString("%1 «%2»").arg(m_orgTypeCombo->currentText(), c.name);
    c.inn = m_innEdit->text();
    c.ogrn = m_ogrnEdit->text();
    c.city = m_cityCombo->currentText();
    c.website = m_websiteEdit->text();
    
    c.address = QString("ул. %1, д. %2").arg(m_streetEdit->text(), m_houseEdit->text());
    if(!m_officeEdit->text().isEmpty()) c.address += ", оф. " + m_officeEdit->text();

    QSet<QString> used;
    for(int i=0; i<m_licensesTable->rowCount(); ++i) {
        auto *cb = qobject_cast<QComboBox*>(m_licensesTable->cellWidget(i,0));
        auto *ed = qobject_cast<QLineEdit*>(m_licensesTable->cellWidget(i,1));
        auto *dt = qobject_cast<QDateEdit*>(m_licensesTable->cellWidget(i,2));
        if(cb && ed && dt) {
            QString n = ed->text();
            if(!n.isEmpty() && !used.contains(n)) {
                c.licenses.append({cb->currentText(), n, dt->date()});
                used.insert(n);
            }
        }
    }

    QTreeWidgetItemIterator it(m_servicesTree);
    while (*it) {
        if ((*it)->checkState(0) == Qt::Checked) c.services.append((*it)->text(0));
        ++it;
    }
    return c;
}

void AddCompanyDialog::setCompanyData(const Company &c) {
    m_nameEdit->setText(c.name);
    m_innEdit->setText(c.inn); 
    m_ogrnEdit->setText(c.ogrn);
    m_websiteEdit->setText(c.website);
    m_cityCombo->setCurrentText(c.city);
    m_fullNameEdit->setText(c.fullName);
    m_streetEdit->setText(c.address); 

    m_licensesTable->setRowCount(0);
    for(const auto &lic : c.licenses) {
        addLicenseRow();
        int r = m_licensesTable->rowCount()-1;
        auto *cb = qobject_cast<QComboBox*>(m_licensesTable->cellWidget(r,0));
        auto *ed = qobject_cast<QLineEdit*>(m_licensesTable->cellWidget(r,1));
        auto *dt = qobject_cast<QDateEdit*>(m_licensesTable->cellWidget(r,2));
        if(cb) cb->setCurrentText(lic.activityType);
        if(ed) ed->setText(lic.number);
        if(dt) dt->setDate(lic.date);
    }
    QTreeWidgetItemIterator it(m_servicesTree);
    while (*it) {
        if (c.services.contains((*it)->text(0))) (*it)->setCheckState(0, Qt::Checked);
        ++it;
    }
}

void AddCompanyDialog::setReadOnly(bool readOnly) {
    m_isViewMode = readOnly;
    
    // Блокируем поля
    m_nameEdit->setReadOnly(readOnly);
    m_orgTypeCombo->setEnabled(!readOnly);
    m_innEdit->setReadOnly(readOnly);
    m_ogrnEdit->setReadOnly(readOnly);
    m_streetEdit->setReadOnly(readOnly);
    m_houseEdit->setReadOnly(readOnly);
    m_officeEdit->setReadOnly(readOnly);
    m_cityCombo->setEnabled(!readOnly);
    m_websiteEdit->setReadOnly(readOnly);

    // Блокируем кнопки
    m_addLicenseBtn->setVisible(!readOnly);
    m_delLicenseBtn->setVisible(!readOnly);
    m_licensesTable->setEditTriggers(readOnly ? QAbstractItemView::NoEditTriggers : QAbstractItemView::AllEditTriggers);
    
    // Блокируем дерево
    m_servicesTree->setEnabled(!readOnly);

    // Управление кнопками
    if (readOnly) {
        m_saveBtn->setVisible(false);
        m_editBtn->setVisible(true);
        setWindowTitle("Просмотр информации");
    } else {
        m_saveBtn->setVisible(true);
        m_editBtn->setVisible(false);
        setWindowTitle("Редактирование");
    }
}

void AddCompanyDialog::onEditClicked() {
    // Включаем режим редактирования
    setReadOnly(false);
    // ИНН все равно менять нельзя, он ключ
    m_innEdit->setReadOnly(true); 
}

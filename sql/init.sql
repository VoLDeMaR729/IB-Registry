DROP TABLE IF EXISTS company_services, services, service_categories, licenses, companies, cities, users CASCADE;

CREATE TABLE users (
    id SERIAL PRIMARY KEY,
    username VARCHAR(50) UNIQUE NOT NULL,
    password_hash VARCHAR(255) NOT NULL
);

CREATE TABLE cities (
    id SERIAL PRIMARY KEY,
    name VARCHAR(100) UNIQUE NOT NULL
);

INSERT INTO cities (name) VALUES 
('Москва'), ('Санкт-Петербург'), ('Новосибирск'), ('Екатеринбург'), ('Казань'),
('Нижний Новгород'), ('Челябинск'), ('Самара'), ('Омск'), ('Ростов-на-Дону'),
('Уфа'), ('Красноярск'), ('Воронеж'), ('Пермь'), ('Волгоград'), ('Краснодар'),
('Саратов'), ('Тюмень'), ('Тольятти'), ('Ижевск'), ('Барнаул'), ('Ульяновск'),
('Иркутск'), ('Хабаровск'), ('Ярославль'), ('Владивосток'), ('Махачкала'),
('Томск'), ('Оренбург'), ('Кемерово'), ('Новокузнецк'), ('Рязань'), ('Астрахань'),
('Набережные Челны'), ('Пенза'), ('Киров'), ('Липецк'), ('Чебоксары'),
('Балашиха'), ('Калининград'), ('Тула'), ('Курск'), ('Севастополь'),
('Сочи'), ('Ставрополь'), ('Тверь'), ('Магнитогорск'), ('Иваново'),
('Брянск'), ('Белгород'), ('Сургут'), ('Владимир'), ('Иннополис');

CREATE TABLE companies (
    id SERIAL PRIMARY KEY,
    name VARCHAR(255) NOT NULL,
    full_name VARCHAR(255),
    inn VARCHAR(12) UNIQUE,
    ogrn VARCHAR(15),
    city_id INTEGER REFERENCES cities(id),
    address TEXT,
    website VARCHAR(255),
    description TEXT
);

CREATE TABLE licenses (
    id SERIAL PRIMARY KEY,
    company_id INTEGER REFERENCES companies(id) ON DELETE CASCADE,
    activity_type VARCHAR(255),
    license_number VARCHAR(100),
    issue_date DATE,
    issuer VARCHAR(50)
);

CREATE TABLE service_categories (
    id SERIAL PRIMARY KEY,
    name VARCHAR(100) UNIQUE
);

CREATE TABLE services (
    id SERIAL PRIMARY KEY,
    category_id INTEGER REFERENCES service_categories(id),
    name VARCHAR(200)
);

CREATE TABLE company_services (
    company_id INTEGER REFERENCES companies(id) ON DELETE CASCADE,
    service_id INTEGER REFERENCES services(id) ON DELETE CASCADE,
    PRIMARY KEY (company_id, service_id)
);

INSERT INTO service_categories (name) VALUES 
('Сетевая безопасность и Шлюзы'), 
('Защита конечных точек и Серверов'), 
('Криптография и Электронная подпись'),
('Мониторинг, SOC и Расследования'),
('Прикладная безопасность и Анализ кода'),
('Консалтинг, Аудит и Интеграция');

-- 1. Сетевая безопасность (Код Безопасности, ИнфоТеКС, Солар)
WITH cat AS (SELECT id FROM service_categories WHERE name = 'Сетевая безопасность и Шлюзы')
INSERT INTO services (category_id, name) SELECT id, unnest(ARRAY[
    'Межсетевые экраны нового поколения (напр. Solar, Континент 4)',
    'Индустриальные шлюзы (защита АСУ ТП)',
    'Виртуальные шлюзы безопасности',
    'Защита каналов связи (VPN-шлюзы)',
    'Системы обнаружения вторжений (СОВ/IDS)',
    'Защита от DDoS-атак',
    'Защита веб-приложений (WAF)',
    'Контроль сетевого трафика (NTA)'
]) FROM cat;

-- 2. Защита конечных точек (Secret Net, Kaspersky, Dr.Web)
WITH cat AS (SELECT id FROM service_categories WHERE name = 'Защита конечных точек и Серверов')
INSERT INTO services (category_id, name) SELECT id, unnest(ARRAY[
    'Средства защиты от несанкционированного доступа (НСД)',
    'Модули доверенной загрузки (Плата/BIOS)',
    'Антивирусная защита рабочих мест',
    'Защита виртуальных сред (напр. vGate)',
    'Защита мобильных устройств (MDM)',
    'Защита от утечек информации (Контроль флешек/почты)'
]) FROM cat;

-- 3. Криптография (ИнфоТеКС, КриптоПро)
WITH cat AS (SELECT id FROM service_categories WHERE name = 'Криптография и Электронная подпись')
INSERT INTO services (category_id, name) SELECT id, unnest(ARRAY[
    'Средства криптографической защиты информации (СКЗИ)',
    'Клиенты для электронной подписи',
    'Удостоверяющие центры (PKI)',
    'Квантовые криптографические системы',
    'Шифрование файлов и баз данных'
]) FROM cat;

-- 4. Мониторинг и SOC (Solar, Jet)
WITH cat AS (SELECT id FROM service_categories WHERE name = 'Мониторинг, SOC и Расследования')
INSERT INTO services (category_id, name) SELECT id, unnest(ARRAY[
    'Центр мониторинга и реагирования (SOC 24/7)',
    'Системы сбора и анализа событий (Сбор логов)',
    'Киберразведка (Поиск угроз)',
    'Компьютерная криминалистика (Расследование инцидентов)',
    'Защита от корпоративного мошенничества'
]) FROM cat;

-- 5. Прикладная безопасность (Solar AppScreener, PT)
WITH cat AS (SELECT id FROM service_categories WHERE name = 'Прикладная безопасность и Анализ кода')
INSERT INTO services (category_id, name) SELECT id, unnest(ARRAY[
    'Статический анализ кода (Поиск уязвимостей в ПО)',
    'Динамический анализ приложений',
    'Контроль прав доступа и учетных записей',
    'Контроль привилегированных пользователей (Админов)'
]) FROM cat;

-- 6. Консалтинг и Интеграция (Джет, Софтмолл)
WITH cat AS (SELECT id FROM service_categories WHERE name = 'Консалтинг, Аудит и Интеграция')
INSERT INTO services (category_id, name) SELECT id, unnest(ARRAY[
    'Аттестация объектов по требованиям ФСТЭК',
    'Категорирование объектов КИИ (187-ФЗ)',
    'Тестирование на проникновение (Имитация хакеров)',
    'Построение комплексных систем защиты (Интеграция)',
    'Аутсорсинг информационной безопасности',
    'Обучение сотрудников киберграмотности'
]) FROM cat;

-- 1. ТАБЛИЦЫ (СТРУКТУРА 3NF)

-- Таблица пользователей (Админы)
CREATE TABLE IF NOT EXISTS users (
    id SERIAL PRIMARY KEY, -- Технический ID
    login VARCHAR(50) UNIQUE NOT NULL,-- Логин должен быть уникальным
    password_hash VARCHAR(64) NOT NULL
);

-- Таблица логов (Безопасность/Аудит)
CREATE TABLE IF NOT EXISTS audit_logs (
    id SERIAL PRIMARY KEY,
    user_id INT REFERENCES users(id) ON DELETE SET NULL, 
    action VARCHAR(255) NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- Справочник типов компаний (Устраняет дублирование текста)
CREATE TABLE IF NOT EXISTS company_types (
    id SERIAL PRIMARY KEY,
    name VARCHAR(100) UNIQUE NOT NULL
);

-- Заполняем справочник (чтобы программа знала эти типы)
INSERT INTO company_types (name) VALUES 
('Вендор (Разработчик)'), 
('Интегратор'), 
('Дистрибьютор'), 
('Аудитор/Консалтинг')
ON CONFLICT (name) DO NOTHING;

-- Таблица компаний (Бизнес-данные)
CREATE TABLE IF NOT EXISTS companies (
    id SERIAL PRIMARY KEY,
    name VARCHAR(255) UNIQUE NOT NULL,-- Уникальное имя (для поиска/удаления)
    type_id INT REFERENCES company_types(id), -- Ссылка на справочник
    license_date DATE NOT NULL,
    description TEXT
);

-- 2. ХРАНИМЫЕ ФУНКЦИИ (ЛОГИКА)

-- Функция авторизации
CREATE OR REPLACE FUNCTION auth_user_func(_login VARCHAR, _hash VARCHAR) 
RETURNS BOOLEAN AS $$
DECLARE
    stored_hash VARCHAR;
BEGIN
    -- Ищем хеш пароля по логину
    SELECT password_hash INTO stored_hash FROM users WHERE login = _login;
    
    -- Сравниваем
    IF found AND stored_hash = _hash THEN
        RETURN TRUE;
    ELSE
        RETURN FALSE;
    END IF;
END;
$$ LANGUAGE plpgsql;


-- Функция добавления компании 
CREATE OR REPLACE FUNCTION add_company_func(
    _name VARCHAR, 
    _type_str VARCHAR, 
    _date DATE, 
    _desc TEXT
) RETURNS VOID AS $$
DECLARE
    _found_type_id INT;
BEGIN
    -- 1. Ищем ID типа по его названию
    SELECT id INTO _found_type_id FROM company_types WHERE name = _type_str;

    -- Если такого типа нет (на всякий случай)
    IF _found_type_id IS NULL THEN
        RAISE EXCEPTION 'Неизвестный тип компании: %', _type_str;
    END IF;

    -- 2. Вставляем запись, используя найденный ID
    INSERT INTO companies (name, type_id, license_date, description)
    VALUES (_name, _found_type_id, _date, _desc)
    ON CONFLICT (name) DO NOTHING;
END;
$$ LANGUAGE plpgsql;


-- Функция удаления (По названию)
CREATE OR REPLACE FUNCTION delete_company_func(_name VARCHAR) RETURNS VOID AS $$
BEGIN
    -- Удаляем компанию, где имя совпадает с переданным
    DELETE FROM companies WHERE name = _name;
END;
$$ LANGUAGE plpgsql;


-- Функция поиска
-- Соединяет таблицы companies и company_types, чтобы вернуть название типа
CREATE OR REPLACE FUNCTION search_companies_func(
    _name_filter VARCHAR, 
    _type_filter VARCHAR
) RETURNS TABLE (
    out_name VARCHAR,
    out_type VARCHAR, -- Возвращаем строку, а не ID
    out_date DATE,
    out_desc TEXT
) AS $$
BEGIN
    RETURN QUERY
    SELECT 
        c.name, 
        t.name as type, -- Берем название из справочника
        c.license_date, 
        c.description
    FROM companies c
    JOIN company_types t ON c.type_id = t.id -- Соединяем таблицы
    WHERE 
        -- Фильтр по имени
        (_name_filter = '' OR c.name ILIKE CONCAT('%', _name_filter, '%'))
        AND
        -- Фильтр по типу (сравниваем строки)
        (_type_filter = 'Все' OR t.name = _type_filter);
END;
$$ LANGUAGE plpgsql;

-- Функция обновления (Описание и Дата)
CREATE OR REPLACE FUNCTION update_company_func(
    _name VARCHAR, 
    _new_date DATE, 
    _new_desc TEXT
) RETURNS VOID AS $$
BEGIN
    UPDATE companies
    SET license_date = _new_date, description = _new_desc
    WHERE name = _name;
END;
$$ LANGUAGE plpgsql;

-- Создаем админа (пароль: admin)
INSERT INTO users (login, password_hash)
VALUES ('admin', '8c6976e5b5410415bde908bd4dee15dfb167a9c873fc4bb8a81f6f2ab448a918')
ON CONFLICT (login) DO NOTHING;

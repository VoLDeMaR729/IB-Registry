-- 1. Таблица пользователей
CREATE TABLE IF NOT EXISTS users (
    id SERIAL PRIMARY KEY,
    login VARCHAR(50) UNIQUE NOT NULL,
    password_hash VARCHAR(64) NOT NULL
);

-- 2. Таблица компаний
CREATE TABLE IF NOT EXISTS companies (
    id SERIAL PRIMARY KEY,
    name VARCHAR(255) NOT NULL,
    type VARCHAR(100) NOT NULL,
    license_date DATE NOT NULL,
    description TEXT
);

-- 3. Функция добавления компании (Хранимая процедура)
CREATE OR REPLACE FUNCTION add_company_func(
    _name VARCHAR, 
    _type VARCHAR, 
    _date DATE, 
    _desc TEXT
) RETURNS VOID AS $$
BEGIN
    INSERT INTO companies (name, type, license_date, description)
    VALUES (_name, _type, _date, _desc);
END;
$$ LANGUAGE plpgsql;

-- 4. Функция удаления компании
CREATE OR REPLACE FUNCTION delete_company_func(_id INT) RETURNS VOID AS $$
BEGIN
    DELETE FROM companies WHERE id = _id;
END;
$$ LANGUAGE plpgsql;

-- 5. Функция поиска (Возвращает таблицу)
CREATE OR REPLACE FUNCTION search_companies_func(
    _name_filter VARCHAR, 
    _type_filter VARCHAR
) RETURNS TABLE (
    out_id INT,
    out_name VARCHAR,
    out_type VARCHAR,
    out_date DATE,
    out_desc TEXT
) AS $$
BEGIN
    RETURN QUERY
    SELECT id, name, type, license_date, description
    FROM companies
    WHERE 
        (_name_filter = '' OR name ILIKE '%' || _name_filter || '%')
        AND
        (_type_filter = 'Все' OR type = _type_filter);
END;
$$ LANGUAGE plpgsql;

-- 6. Функция авторизации (Проверка хеша)
CREATE OR REPLACE FUNCTION auth_user_func(_login VARCHAR, _hash VARCHAR) 
RETURNS BOOLEAN AS $$
DECLARE
    stored_hash VARCHAR;
BEGIN
    SELECT password_hash INTO stored_hash FROM users WHERE login = _login;
    
    IF found AND stored_hash = _hash THEN
        RETURN TRUE;
    ELSE
        RETURN FALSE;
    END IF;
END;
$$ LANGUAGE plpgsql;

-- 7. Создаем админа по умолчанию (если нет)
INSERT INTO users (login, password_hash)
VALUES ('admin', '8c6976e5b5410415bde908bd4dee15dfb167a9c873fc4bb8a81f6f2ab448a918') -- sha256("admin")
ON CONFLICT (login) DO NOTHING;

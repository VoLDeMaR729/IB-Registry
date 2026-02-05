CREATE TABLE IF NOT EXISTS users (
    id SERIAL PRIMARY KEY,
    login VARCHAR(50) NOT NULL UNIQUE,
    password_hash VARCHAR(64) NOT NULL
);

CREATE TABLE IF NOT EXISTS company_types (
    id SERIAL PRIMARY KEY,
    name VARCHAR(100) NOT NULL UNIQUE
);

INSERT INTO company_types (name) VALUES 
('Вендор (Разработчик)'), 
('Интегратор'), 
('Дистрибьютор'), 
('Аудитор/Консалтинг')
ON CONFLICT (name) DO NOTHING;

CREATE TABLE IF NOT EXISTS companies (
    id SERIAL PRIMARY KEY,
    name VARCHAR(255) NOT NULL UNIQUE,
    inn VARCHAR(12) NOT NULL,
    ogrn VARCHAR(15),
    address TEXT NOT NULL,
    license_num VARCHAR(50) NOT NULL,
    type_id INT REFERENCES company_types(id),
    license_date DATE NOT NULL,
    description TEXT
);

CREATE OR REPLACE FUNCTION auth_user_func(p_login VARCHAR, p_hash VARCHAR) 
RETURNS BOOLEAN AS $$
BEGIN
    RETURN EXISTS(SELECT 1 FROM users WHERE login = p_login AND password_hash = p_hash);
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION add_company_func(
    p_name VARCHAR, 
    p_inn VARCHAR,
    p_ogrn VARCHAR,
    p_address VARCHAR,
    p_license_num VARCHAR,
    p_type_name VARCHAR, 
    p_date DATE, 
    p_desc TEXT
)
RETURNS VOID AS $$
DECLARE
    v_type_id INT;
BEGIN
    SELECT id INTO v_type_id FROM company_types WHERE name = p_type_name;
    
    INSERT INTO companies (name, inn, ogrn, address, license_num, type_id, license_date, description)
    VALUES (p_name, p_inn, p_ogrn, p_address, p_license_num, v_type_id, p_date, p_desc);
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION search_companies_func(p_name VARCHAR, p_type VARCHAR)
RETURNS TABLE(
    out_name VARCHAR, 
    out_inn VARCHAR, 
    out_ogrn VARCHAR, 
    out_addr TEXT, 
    out_lnum VARCHAR,
    out_type VARCHAR, 
    out_date DATE, 
    out_desc TEXT
) AS $$
BEGIN
    RETURN QUERY 
    SELECT c.name, c.inn, c.ogrn, c.address, c.license_num, t.name, c.license_date, c.description
    FROM companies c
    JOIN company_types t ON c.type_id = t.id
    WHERE (p_name = '' OR c.name ILIKE '%' || p_name || '%' OR c.inn LIKE '%' || p_name || '%')
      AND (p_type = 'Все' OR t.name = p_type);
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION update_company_func(
    p_name VARCHAR, 
    p_address VARCHAR,
    p_license_num VARCHAR,
    p_date DATE, 
    p_desc TEXT
)
RETURNS VOID AS $$
BEGIN
    UPDATE companies 
    SET address = p_address,
        license_num = p_license_num,
        license_date = p_date, 
        description = p_desc
    WHERE name = p_name;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION delete_company_func(p_name VARCHAR)
RETURNS VOID AS $$
BEGIN
    DELETE FROM companies WHERE name = p_name;
END;
$$ LANGUAGE plpgsql;

INSERT INTO users (login, password_hash) 
VALUES ('admin', '8c6976e5b5410415bde908bd4dee15dfb167a9c873fc4bb8a81f6f2ab448a918')
ON CONFLICT (login) DO NOTHING;

CREATE DATABASE KeyStorage;
USE KeyStorage;

CREATE USER 'kms'@'172.17.0.1' IDENTIFIED BY 'YourStrongPassword';
GRANT ALL PRIVILEGES ON . TO 'kms'@'172.17.0.1' WITH GRANT OPTION;
FLUSH PRIVILEGES;

CREATE TABLE key_store_symmetric (
    ksid INT,
    id INT,
    hash VARCHAR(255),
    expiration_timestamp DATETIME,
    suspended BIT,
    creation_timestamp DATETIME,
    used BIT,
    key_material VARCHAR(2048),
    PRIMARY KEY (ksid, id)
);

CREATE TABLE key_store_oblivious (
    ksid INT,
    id INT,
    hash VARCHAR(255),
    expiration_timestamp DATETIME,
    suspended BIT,
    creation_timestamp DATETIME,
    used BIT,
    key_material VARCHAR(2048),
    PRIMARY KEY (ksid, id)
);

CREATE TABLE raw_key_store_symmetric (
    seq INT,
    id INT,
    sync BIT,
    timestamp DATETIME(3),
    size INT,
    size_used INT,
    key_material VARCHAR(2048),
    PRIMARY KEY (seq, id)
);

CREATE TABLE raw_key_store_oblivious (
    seq INT,
    id INT,
    sync BIT,
    timestamp DATETIME(3),
    size INT,
    size_used INT,
    key_material VARCHAR(2048),
    PRIMARY KEY (seq, id)
);

-- INSERT INTO key_store_symmetric (ksid, id, hash, expiration_timestamp, suspended, creation_timestamp, used, key_material) 
-- VALUES (1, 1, 'hash_value_1', '2024-05-10 12:00:00', 0, '2024-05-10 10:00:00', 0, 'key_material_1'),
--        (2, 2, 'hash_value_2', '2024-05-11 12:00:00', 1, '2024-05-11 10:00:00', 1, 'key_material_2');

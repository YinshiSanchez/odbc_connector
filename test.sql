CREATE TABLE employees ( employee_id SERIAL PRIMARY KEY, first_name VARCHAR(50), last_name VARCHAR(50), department_id INT REFERENCES departments(department_id), salary NUMERIC(10, 2));

INSERT INTO departments (department_name) VALUES ('HR'), ('IT'), ('Finance'), ('Marketing');

INSERT INTO employees (first_name, last_name, department_id, salary) VALUES ('John', 'Doe', 1, 60000), ('Jane', 'Smith', 2, 75000), ('Alice', 'Johnson', 3, 50000), ('Bob', 'Brown', 4, 55000);


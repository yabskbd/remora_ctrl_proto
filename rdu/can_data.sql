CREATE TABLE can_data (
	time TIMESTAMP WITHOUT TIME ZONE NOT NULL,
	metric varchar(200) NOT NULL,
	value double PRECISION NOT NULL
);


SELECT create_hypertable('can_data', 'time');

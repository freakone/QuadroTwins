void sensors_init();
int extern sensors_error_flag;
void sensors_read();
uint8_t i2c_read2(uint8_t adres, uint8_t reg_adres);
void i2c_read( uint8_t adres, uint8_t reg_adres, uint8_t * dane, uint8_t len );
void i2c_write(uint8_t address, uint8_t* data, uint32_t length);
void i2c_write2(uint8_t address, uint8_t reg, uint8_t data);
void i2c_config();

typedef struct {

	int16_t x;
	int16_t y;
	int16_t z;
	uint8_t address;
	float scale;

} Accelerometer;

typedef struct {

	int16_t x;
	int16_t y;
	int16_t z;
	uint8_t address;
	float scale;

} Gyroscope;

typedef struct {

	int16_t x;
	int16_t y;
	int16_t z;
	uint8_t address;

} Magnetometer;


Accelerometer acc;
Gyroscope gyro;
Magnetometer magneto;

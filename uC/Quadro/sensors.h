void sensors_init();
uint8_t i2c_read2(uint8_t adres, uint8_t reg_adres);
int extern sensors_error_flag;


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

void EXTI3_IRQHandler(void);
void EXTI4_IRQHandler(void);
void EXTI9_5_IRQHandler(void);

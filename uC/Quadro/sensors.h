void sensors_init();
uint8_t i2c_read2(uint8_t adres, uint8_t reg_adres);

typedef struct {

	float p;
	float i;
	float d;
	float dt;

} PIDStruct;

typedef struct {

	int16_t x;
	int16_t y;
	int16_t z;
	uint8_t address;
	float scale;
	PIDStruct pid;

} Accelerometer;

typedef struct {

	int16_t x;
	int16_t y;
	int16_t z;
	uint8_t address;
	float scale;
	PIDStruct pid;

} Gyroscope;

typedef struct {

	int16_t x;
	int16_t y;
	int16_t z;
	uint8_t address;
	PIDStruct pid;

} Magnetometer;


Accelerometer acc;
Gyroscope gyro;
Magnetometer magneto;

void EXTI3_IRQHandler(void);
void EXTI4_IRQHandler(void);
void EXTI9_5_IRQHandler(void);

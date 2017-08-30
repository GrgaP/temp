
#define SHT20_I2C_ADDRESS			0x0A
#define SHT20_I2C_INST	I2C_DEFAULT_INSTANCE
#define SHT20_I2C_SPEED	100000

uint8_t sht20_io_init(void);

/*uint8_t sht20_io_open();

uint8_t sht20_io_close();*/

uint8_t sht20_io_write(uint8_t Reg, uint8_t Value);

uint8_t sht20_io_read(uint8_t Reg, uint8_t* rVal);
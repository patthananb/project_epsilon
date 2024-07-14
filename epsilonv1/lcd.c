/*
 * CFile1.c
 *
 * Created: 04/04/2024 13:57:45
 *  Author: patth
 */
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
 
 
 // LCD functions with I2C communication
 uint8_t _addr = 0x27;
 uint8_t p_write;


// I2C communication functions prototype
void i2c_write(unsigned char data);
void i2c_start(void);
void i2c_stop(void);
void i2c_init(void);

// I2C communication functions
void i2c_write(unsigned char data) {
	TWDR = data;
	TWCR = (1 << TWINT) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)));
}

void i2c_start(void) {
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
	while ((TWCR & (1 << TWINT)) == 0);
}

void i2c_stop(void) {
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
	_delay_us(10); // Ensure STOP condition is executed properly
}

void i2c_init(void) {
	TWSR = 0x00;              // Set prescaler bits to zero
	TWBR = 0x48;              // Set bit rate register for 100 kHz at 16 MHz clock
	TWCR = (1 << TWEN);       // Enable the TWI module
}

char lcdbegin() {
	i2c_init(); // Initialize I2C

	write(0x00);
	write4Bit(0x03);
	_delay_us(4500);
	write4Bit(0x03);
	_delay_us(4500);
	write4Bit(0x03);
	_delay_us(150);
	write4Bit(0x02);
	write4Bit(0x02);
	write4Bit(0x0C);
	write4Bit(0x00);
	write4Bit(0x08);
	write4Bit(0x00);
	write4Bit(0x01);
	write4Bit(0x00);
	write4Bit(0x06);
	write4Bit(0x00);
	write4Bit(0x0F);
	setBacklight(1);
	return 1;
}

void write(uint8_t data) {
	i2c_start(); // Transmit START condition
	i2c_write(_addr << 1); // Transmit SLA + W(0)
	i2c_write(data); // Transmit data
	i2c_stop(); // Transmit STOP condition
	// Handle I2C errors if needed
}

void latch() {
	p_write = p_write & 0xF9;
	write(p_write);
	_delay_ms(1);
	p_write = p_write | 0x04;
	write(p_write);
	_delay_ms(1);
	p_write = p_write & 0xF9;
	write(p_write);
	_delay_us(100);
}

void write4Bit(uint8_t data) {
	p_write &= 0x0F;
	p_write |= data << 4;
	latch();
}

void writeByte(uint8_t data) {
	write4Bit(data >> 4);
	write4Bit(data);
}

void writeCommand(uint8_t data) {
	p_write = p_write & 0xF8;  // RS LOW
	write(p_write);
	writeByte(data);
}

void writeData(uint8_t data) {
	p_write = p_write | 0x01;  // RS HIGH
	write(p_write);
	writeByte(data);
}

void setPosition(uint8_t x, uint8_t y) {
	if (y == 0) {
		writeCommand(0x80 | x);
		} else if (y == 1) {
		writeCommand(0x80 | 0x40 | x);
	}
}

void print(const char *str) {
	while (*str) {
		writeData(*str);
		str += 1;
	}
}

void lcdclear() {
	_delay_ms(100);
	writeCommand(0x01);
	_delay_ms(100);
}

void setBacklight(uint8_t data) {
	p_write = 0;
	if (data) {
		p_write |= 0x08;
		write(p_write);
		} else {
		p_write &= 0xF7;
		write(p_write);
	}
	_delay_ms(50);
}
/*
 * IncFile1.h
 *
 * Created: 05/04/2024 02:54:20
 *  Author: patth
 */ 


#ifndef INCFILE1_H_
#define INCFILE1_H_

// LCD functions with I2C communication
char lcdbegin(void);
void write(uint8_t data);
void latch(void);
void write4Bit(uint8_t data);
void writeByte(uint8_t data);
void writeCommand(uint8_t data);
void writeData(uint8_t data);
void setPosition(uint8_t x, uint8_t y);
void print(const char *str);
void lcdclear(void);
void setBacklight(uint8_t data);




#endif /* INCFILE1_H_ */
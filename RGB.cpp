#include <stdio.h>
#include <pigpio.h>


// define pins
#define sw_1 1
#define sw_2 2
#define sw_3 3
#define sw_4 4


//Global Variables
int mode = 0;
bool color_mode = 0;

//I2C set up
#define ADS7830 0x48
#define PCF8591 0x4b


//I2C Initilization
int I2C_init(int& i2c_handler1, int& i2c_handler2)
{
	i2c_handler1 = i2cOpen(1, ADS7830, 0);
	i2c_handler2 = i2cOpen(1, PCF8591, 0);
	
	if (i2c_handler1<0 || i2c_handler2<0) 
	{
		printf("i2C initialization failed!\n");
		return -1;
	}
	else 
	{
		printf("i2c initializaion succeed!\n");
		return 1;
	}
	
}

//ISR
void Mode_1_Switch(int gpio, int level, unsigned int i)
{
	mode  = 1;
}

void Mode_2_Switch(int gpio, int level, unsigned int i)
{
	mode = 2;
}

void Mode_3_Switch(int gpio, int level, unsigned int i)
{
	mode = 3;
}

void Color_Mode_Switch(int gpio, int level, unsigned int i)
{
	color_mode = color_mode ? 0 : 1;
}

//mode 1 breathing
void Mode_1()
{
}

//mode 2 swaping
void Mode_2()
{
}

//mode 3 normal
void Mode_3()
{
}

//value check
void value_check(float* const RGB, const int* handler)
{
	i2cWriteByte(handler[0], 0x84);
	RGB[0] = i2cReadByte(handler[0]);
	i2cWriteByte(handler[0], 0xD4);
	RGB[1] = i2cReadByte(handler[0]);
	i2cWriteByte(handler[0], 0x94);
	RGB[2] = i2cReadByte(handler[0]);
	
}

int main()
{
	gpioInitialise();
	int i2c_handler[2];
	
	float RGB[3];
	
	
	//initialize ISR
	gpioSetISRFunc(sw_1, FALLING_EDGE, 0, Mode_1_Switch);
	gpioSetISRFunc(sw_2, FALLING_EDGE, 0, Mode_2_Switch);
	gpioSetISRFunc(sw_3, FALLING_EDGE, 0, Mode_3_Switch);
	gpioSetISRFunc(sw_4, FALLING_EDGE, 0, Color_Mode_Switch);
	
	//main coding
	while(I2C_init(i2c_handler[0], i2c_handler[1])>=0)
	{
		value_check(RGB, i2c_handler);
		
	}
	return 0;
}



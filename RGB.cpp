#include <stdio.h>
#include <pigpio.h>
#include <unistd.h>


// define pins
#define sw_1 1
#define sw_2 2
#define sw_3 3
#define sw_4 4

// define pwm outputs
#define r_light 5
#define g_light 5
#define b_light 5


//Global Variables
int mode = 0;
bool color_mode = 0;
float cycle = 0;

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
void Mode_1 (const float* const RGB)
{
	if (color_mode == 0)
	{
		for (i=0; i<75; ++i)
		{
			gpioPWM(r_lights, cycle/75*255*RGB[1]);
			gpioPWM(b_lights, cycle/75*255*RGB[]);
			gpioPWM(g_lights, cycle/75*255*RGB[]);
			delay()
		}
		for (i=0; i<75; ++i)
		{
			gpioPWM(r_lights, (149-cycle)/74*255*RGB[0]);
			gpioPWM(b_lights, (149-cycle)/74*255*RGB[1]);
			gpioPWM(g_lights, (149-cycle)/74*255*RGB[2]);
		}
	}
	else
	{
		if (RGB[cycle]==0) return;
		for (i=0; i<75; ++i)
		{
			gpioPWM(cycle, (149-cycle)/74*255*RGB[1]);
		}
		for (i=0; i<75; ++i)
		{
			gpioPWM(cycle, (149-cycle)/74*255*RGB[1]);
		}
		cycle = (cycle<7? ++cycle: 0);
	}
}

//mode 2 slowly cycling, mode 2 will be RGB as background alternatively, while another 2 strobing
void Mode_2()
{
}

//mode 3 normal constant light
void Mode_3(const float* const RGB)
{
	if (!color_mode)
	{
		gpioPWM(r_lights, RGB[0]*255);
		gpioPWM(g_lights, RGB[1]*255);
		gpioPWM(b_lights, RGB[2]*255);
		sleep(3);
	}
	else
	{
		if(!RGB[cycle]) return;
		gpioPWM(cycle, RGB[cycle-4]*255)
		cycle = (cycle<7? ++cycle: 0);
	}
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

	//set gpio frequency
	gpioSetPWMfrequency(r_light, 50);
	gpioSetPWMfrequency(b_light, 50);
	gpioSetPWMfrequency(g_light, 50);
	
	//main coding
	while(I2C_init(i2c_handler[0], i2c_handler[1])>=0)
	{
		value_check(RGB, i2c_handler);
		switch (mode)
		{
			case 1:
			case 2:
			case 3:
			default
			    printf("An error occured! \n");
			    break;
		}
	}
	return 0;
}



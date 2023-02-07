#include <atmel_start.h>
#include <stdlib.h>
#include "ftn_vip_lib/usbUART.h"
#include "ftn_vip_lib/debugUART.h"
#include "ftn_vip_lib/nbiotUART.h"
#include "ftn_vip_lib/timer_1ms.h"
#include "ftn_vip_lib/Quectel_BC68.h"

//on-board sensors:
#include "ftn_vip_lib/SHTC3.h"
#include "ftn_vip_lib/bmp280.h"
#include "ftn_vip_lib/BH1750FVI.h"
#include "ftn_vip_lib/LIS2DE12.h"

typedef struct  
{
	//SHTC3
	uint32_t shtc3_hum;
	uint32_t shtc3_temp;
	//BMP280
	int32_t bmp280_temp;
	int32_t bmp280_pres;
	//BH1750FVI
	uint32_t lum;
}sensor_data;

//parametri servera
#define ServerIP	"199.247.17.15"
#define ServerPort	50067

void getSensorData(sensor_data *sd)
{
	char str[64];
	//SHTC3
	SHTC3_update();
	sd -> shtc3_hum = SHTC3_raw2Percent() * 100;
	sd -> shtc3_temp = SHTC3_raw2DegC() * 100;
	sprintf(str, "SHTC3 ->\tT = %d.%d C\tH = %d.%d \%\r\n", sd -> shtc3_temp / 100, sd -> shtc3_temp % 100, sd -> shtc3_hum / 100, sd -> shtc3_hum % 100);
	usbUARTputString(str);
		
	//BMP280
	bmp280_measure(&(sd -> bmp280_temp), &(sd -> bmp280_pres));
	sprintf(str, "BMP280 ->\tT = %d.%d C\tP = %d.%d mBar\r\n", sd -> bmp280_temp / 100, sd -> bmp280_temp % 100, sd -> bmp280_pres / 100, sd -> bmp280_pres % 100);
	usbUARTputString(str);
		
	//BH1750FVI
	sd -> lum = BH1750FVI_GetLightIntensity();
	sprintf(str, "BH1750FVI ->\tL = %ld lux\r\n\r\n", sd -> lum);
	usbUARTputString(str);
}

void getAccelData(accel_3axis *accel)
{
	char str[32];
	//sprintf(str, "usao sam u accel data");
	usbUARTputString(str);
	
	LIS2DE12_getAccel(accel);

	sprintf(str, "%d,%d,%d\r\n", accel -> accel_X, accel -> accel_Y, accel -> accel_Z);
	usbUARTputString(str);
		
	delay(5);
}

int getAccelDataX(accel_3axis *accel)
{
	LIS2DE12_getAccel(accel);
	return accel -> accel_X ;
	
}

int getAccelDataY(accel_3axis *accel)
{
	LIS2DE12_getAccel(accel);
	return accel -> accel_Y;
	
}

int getAccelDataZ(accel_3axis *accel)
{
	LIS2DE12_getAccel(accel);
	return accel -> accel_Z;
	
}

/*int doorOpened(accel_3axis *accel)
{
	
}*/

void echoTest(uint8_t protocol, char *payload)
{
	char str[128], response[128];

	/*sprintf(str, "Pritisni taster za slanje...\r\n");
	usbUARTputString(str);
	while (gpio_get_pin_level(BUTTON));*/

	char socket = BC68_openSocket(1, protocol);
	int16_t rxBytes = BC68_tx(protocol, ServerIP, ServerPort, payload, strlen(payload), socket);
	BC68_rx(response, rxBytes, socket);
	sprintf(str, "Odgovor servera -> %s\r\n", response);
	usbUARTputString(str);
	BC68_closeSocket(socket);
}

int main(void)
{
	//Inicijalizacija
	atmel_start_init();
	debugUARTdriverInit();
	usbUARTinit();
	nbiotUARTinit();
	timer_1ms_init();

	
	char str[256];
	delay(3000);
	sprintf(str, "--- FTN-VIP NB-IoT ---\r\n");
	usbUARTputString(str);
	setLEDfreq(FREQ_1HZ);
	enableLED();

	//init sensors
	//SHTC3_begin();
	//bmp280_init();
	//BH1750FVI_begin();
	LIS2DE12_init();
	
	//NB-IoT connect
	BC68_debugEnable(true, DEBUG_USB);
	BC68_connect();
	/*char resp[1024];
	getBC68response("AT+CSIM=10,00B2010426\r\n", "OK", resp, 3000);*/

	setLEDfreq(FREQ_1HZ);
	sensor_data sd;
	accel_3axis ad;
	int x,y,z;
	x = getAccelDataX(&ad);
	y = getAccelDataY(&ad);
	z = getAccelDataZ(&ad);
	char response[1024];
	while (1)
	{
		//getAccelData(&ad);
		/*getBC68response("AT+CSMS=1\r\n", "OK", response, 3000);
		getBC68response("AT+CNMA=1\r\n", "OK", response, 3000);
		getBC68response("AT+CSCA=+38163537882,145\r\n", "OK", response, 3000);
		getBC68response("AT+CMGS=+38163537882\r\n", "OK", response, 3000);*/
		/*getBC68response("AT+CSCS=GSM\r\n", "OK", response, 300);
		getBC68response("AT+CMGSW=+38163537882\r\n", "OK", response, 300);
		getBC68response("AT+CMSS=54\r\n", "OK", response, 300);
		getBC68response("AT+CMSS=54,+38163537882\r\n", "OK", response, 300);*/
		
		//getSensorData(&sd);
		//echoTest(UDP, "UDP test data...");
		/*x = getAccelDataX(&ad);
		y = getAccelDataY(&ad);
		z = getAccelDataZ(&ad);*/
		/*sprintf(str, "x: %d, y: %d, z: %d\r\n", x,y,z);
		usbUARTputString(str);*/
		if((y - getAccelDataY(&ad) > 5) | (y - getAccelDataY(&ad) < -5) | (z - getAccelDataZ(&ad) > 5) | (z - getAccelDataZ(&ad) < -5))
		{
			echoTest(TCP, "Vrata su se otvorila");
		}
		
		if((x - getAccelDataX(&ad) > 10) | (x - getAccelDataX(&ad) < -10))
		{
			echoTest(TCP, "Senzor se ne nalazi na vratima");
		}
		
		
		//delay(5000);
	}
}


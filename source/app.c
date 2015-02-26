#include "digitdisplay.h"
#include "mmd.h"
#include "string.h"
#include "app.h"
#include "typedefs.h"
#include "mb.h"


/*
*------------------------------------------------------------------------------
* app - the app structure. 
XX ABCDEFGH XXXXXXXX XX.X XX.X XXX.X
*------------------------------------------------------------------------------
*/
typedef struct _APP
{
	//Variables to handle dot matrix display
	UINT8 F2[MSG_LENGTH];
	UINT8 eepUpdate;

	//Variables to handle seven segment display
    UINT8 F1[2];
    UINT8 F3[8];
    UINT8 F4[3];
    UINT8 F5[3];
    UINT8 F6[4];
	UINT8 eMBdata[28];

}APP;

#pragma idata APP_DATA
APP app = {{0},0};
MMD_Config mmdConfig = {0};
UINT8 data[8] = "ABCDEFGH";
#pragma idata



/*
*------------------------------------------------------------------------------
* void APP_init(void)
*
* Summary	: Initialize application
*
* Input		: None
*
* Output	: None
*------------------------------------------------------------------------------
*/

void APP_init(void)
{
	UINT8 i;
	UINT16 sbaudrate , saddress;
	eMBErrorCode    eStatus;

	
	sbaudrate = 19200;
	saddress = 1;
	//modbus configuration
	eStatus = eMBInit( MB_RTU, ( UCHAR )saddress, 0, sbaudrate, MB_PAR_NONE);
	eStatus = eMBEnable(  );	/* Enable the Modbus Protocol Stack. */



}

/*
*------------------------------------------------------------------------------
* void APP_task(void)
*
* Summary	: 
*
* Input		: None
*
* Output	: None
*------------------------------------------------------------------------------
*/

void APP_task(void)
{

	UINT8 i;
	volatile UINT16 temp;



}



/*
*------------------------------------------------------------------------------
* MODBUS CALLBACK
*------------------------------------------------------------------------------
*/

eMBErrorCode
eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs )
{

    eMBErrorCode    eStatus = MB_ENOERR;


    return eStatus;

}

eMBErrorCode
eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs,
                 eMBRegisterMode eMode )
{

	UINT8	starting_add = usAddress;
	UINT8	no_regs		 = usNRegs * 2;
	eMBErrorCode    eStatus = MB_ENOERR;
	UINT8 i = 0;

	switch(eMode)
	{
	case MB_REG_WRITE:

    
	while( no_regs > 0)
	{

		app.eMBdata[i++] = * pucRegBuffer++;

		starting_add++;
		no_regs	--;
	}
//	app.valueBuffer[i++] = 0;
    break;

 	case MB_REG_READ: 

	while(no_regs > 0)
	{

			* pucRegBuffer++ =	'A';
			* pucRegBuffer++ =	'B';		
			
			* pucRegBuffer++ = 'C';
			* pucRegBuffer++ = 'D';

						



		starting_add++;
		no_regs	--;	
	}
   	 break;
	}
/*--------------------------------------------------------------------
update the individual buffer for display from input buffer	
----------------------------------------------------------------------*/

	for(i = 0; i < 8; i++)
	{
		app.F2[i] = app.eMBdata[(i+2)];
		app.F3[i] = app.eMBdata[(i+10)];

		if( i < 4)
		{
			app.F6[i] = app.eMBdata[(i+24)];
		}
		if( i < 3)
		{
			app.F4[i] = app.eMBdata[(i+18)];
			app.F5[i] = app.eMBdata[(i+21)];
		}
		if( i < 2)
		{
			app.F1[i] = app.eMBdata[(i)];
		}
	}
	

	MMD_clearSegment(0);
	mmdConfig.startAddress = 0;
	mmdConfig.length = MMD_MAX_CHARS;
	mmdConfig.symbolCount = strlen(app.F2);
	mmdConfig.symbolBuffer = app.F2;
	mmdConfig.scrollSpeed = 0;//SCROLL_SPEED_LOW;
	MMD_configSegment( 0 , &mmdConfig);

	DigitDisplay_updateBufferPartial(app.F1,12,2);
	DigitDisplay_updateBufferPartial(app.F3,0,8);  
	DigitDisplay_updateBufferPartial(app.F4,14,3);
	DigitDisplay_updateBufferPartial(app.F5,17,3);  
	DigitDisplay_updateBufferPartial(app.F6,20,4); 
	DigitDisplay_DotOn(15,1);
	DigitDisplay_DotOn(18,1);
	DigitDisplay_DotOn(21,1);

	return eStatus;
  }


eMBErrorCode
eMBRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils,
               eMBRegisterMode eMode )
{
    return MB_ENOREG;
}

eMBErrorCode
eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete )
{
    return MB_ENOREG;
}


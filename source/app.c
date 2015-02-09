#include "digitdisplay.h"
#include "mmd.h"
#include "string.h"
#include "app.h"
#include "typedefs.h"
#include "mb.h"


/*
*------------------------------------------------------------------------------
* app - the app structure. 
*------------------------------------------------------------------------------
*/
typedef struct _APP
{
	//Variables to handle dot matrix display
	UINT8 model[MSG_LENGTH];
	UINT8 eepUpdate;

	//Variables to handle seven segment display
    UINT8 Actual[NO_OF_DIGITS];
    UINT8 Plan[NO_OF_DIGITS];

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

	for(i = 0; i < 8; i++)
	{
		app.model[i] = data[i];

		app.Plan[i] = i + '0';
		app.Actual[i] = i + '0';
	}

	
	//Dot matrix initialization
	MMD_clearSegment(0);
	mmdConfig.startAddress = 0;
	mmdConfig.length = MMD_MAX_CHARS;
	mmdConfig.symbolCount = strlen(app.model);
	mmdConfig.symbolBuffer = app.model;
	mmdConfig.scrollSpeed = 0;//SCROLL_SPEED_LOW;
	MMD_configSegment( 0 , &mmdConfig);


	DigitDisplay_updateBufferPartial(app.Plan,0,8);
	DigitDisplay_updateBufferPartial(app.Actual,8,8);  


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

		app.model[i++] = * pucRegBuffer++;

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

	DigitDisplay_updateBufferPartial(app.model, 0, 2);
	//DigitDisplay_updateBuffer(app.model);

	MMD_clearSegment(0);
	mmdConfig.startAddress = 0;
	mmdConfig.length = MMD_MAX_CHARS;
	mmdConfig.symbolCount = 8;//strlen(app.model);
	mmdConfig.symbolBuffer = app.model+2;
	mmdConfig.scrollSpeed = 0;//SCROLL_SPEED_LOW;
	MMD_configSegment( 0 , &mmdConfig);

	DigitDisplay_updateBufferPartial((app.model+10), 2, 8);

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


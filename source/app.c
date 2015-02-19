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
	
	UINT8 data[NO_OF_DATA];				//Variables to handle input data
	UINT8 model[MSG_LENGTH + 1];			//Variables to handle dot matrix display
	UINT8 digit[NO_OF_DIGITS];			//Variables to handle seven segment display
	UINT8 mmdupdateFlag;


}APP;

#pragma idata APP_DATA
APP app = {{0},{0},{0},0};
MMD_Config mmdConfig = {0};
UINT8 temp[9] = "IDEONICS";
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
		app.model[i] = temp[i];

	}
	for(i = 0; i < 20; i++)
	{
		app.digit[i] = '0';
	}

	
	//Dot matrix initialization
	MMD_clearSegment(0);
	mmdConfig.startAddress = 0;
	mmdConfig.length = MMD_MAX_CHARS;
	mmdConfig.symbolCount = strlen(app.model);
	mmdConfig.symbolBuffer = app.model;
	mmdConfig.scrollSpeed = 0;//SCROLL_SPEED_LOW;
	MMD_configSegment( 0 , &mmdConfig);

	//Digit initialization
	DigitDisplay_updateBuffer(app.digit);
	DigitDisplay_DotOn(11,1);
	DigitDisplay_DotOn(14,1);
	DigitDisplay_DotOn(18,1);


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

//	app.updateFlag = TRUE ;
	while( no_regs > 0)
	{

		app.data[i++] = * pucRegBuffer++;

		starting_add++;
		no_regs	--;

	}

	for(i = 0; i < 8; i++)
	{
		if(app.model[i] != app.data[i + 2])
		{
			app.mmdupdateFlag = TRUE;
		}
	}


	if ( (app.data[0] != 0xFF ) && (app.data[1] != 0xFF ))
	{
		for(i = 0; i < 8; i++)
		{
			app.model[i] = app.data[i + 2];
	
		}
		for(i = 0;i < 2; i++)
		{
			app.digit[i] = app.data[i];
		}
		for(i = 2;i < 20 ; i++)
		{
			app.digit[i] = app.data[ i + 8];
		}
	}

		if(app.mmdupdateFlag == TRUE )
		{
			app.mmdupdateFlag = FALSE;		
			MMD_clearSegment(0);
			mmdConfig.startAddress = 0;
			mmdConfig.length = MMD_MAX_CHARS;
			mmdConfig.symbolCount = strlen(app.model);
			mmdConfig.symbolBuffer = app.model;
			mmdConfig.scrollSpeed = 0;//SCROLL_SPEED_LOW;
			MMD_configSegment( 0 , &mmdConfig);
		}

		DigitDisplay_updateBuffer(app.digit);
		DigitDisplay_DotOn(11,1);
		DigitDisplay_DotOn(14,1);
		DigitDisplay_DotOn(18,1);


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


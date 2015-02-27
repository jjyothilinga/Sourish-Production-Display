#include "digitdisplay.h"
#include "mmd.h"
#include "string.h"
#include "app.h"
#include "typedefs.h"
#include "mb.h"


/*
*------------------------------------------------------------------------------
* Data Format
XX ABCDEFGH XXXXXXXX XX.X XX.X XXX.X
*------------------------------------------------------------------------------
*/
typedef struct _APP
{
	//Buffer for Dot Matrix data
	UINT8 F2[MSG_LENGTH];
	UINT8 Update;

	//Buffers for Seven Segment Data
    UINT8 F1[F1_LENGTH];
    UINT8 F3[F3_LENGTH];
    UINT8 F4[F4_LENGTH];
    UINT8 F5[F5_LENGTH];
    UINT8 F6[F6_LENGTH];

//Modbus buffer
	UINT8 eMBdata[NO_OF_DIGITS];

}APP;

#pragma idata APP_DATA
APP app = {{0},0};
MMD_Config mmdConfig = {0};
#pragma idata
UINT8 data[10] = "12345678";

void updateDigits(void);
void updateMMD(void);

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
	UINT8 i,j;
	UINT16 sbaudrate , saddress;
	eMBErrorCode    eStatus;

	
	sbaudrate = 19200;	//set baudrate
	saddress = 1;		//slave address

	//modbus configuration
	eStatus = eMBInit( MB_RTU, ( UCHAR )saddress, 0, sbaudrate, MB_PAR_NONE);
	eStatus = eMBEnable(  );	/* Enable the Modbus Protocol Stack. */

//	updateMMD();
	

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
 DISABLE_UART_RX_INTERRUPT();
	if(app.Update == TRUE )
	{
		ENABLE_UART_RX_INTERRUPT();
		for(i = 0; i < 8; i++)
		{
			app.F2[i] = app.eMBdata[(i+2)];
			app.F3[i] = app.eMBdata[(i+10)];
	
			if( i < 4)
			{
				app.F6[i] = app.eMBdata[(i+26)];
			}
			if( i < 3)
			{
				app.F4[i] = app.eMBdata[(i+18)];
				app.F5[i] = app.eMBdata[(i+22)];
			}
			if( i < 2)
			{
				app.F1[i] = app.eMBdata[i];
			}

		}
	
	updateDigits();
	updateMMD();
	DISABLE_UART_RX_INTERRUPT();
	app.Update = FALSE;

	ENABLE_UART_RX_INTERRUPT();

	}
	
	ENABLE_UART_RX_INTERRUPT();


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

 DISABLE_UART_RX_INTERRUPT();
	app.Update = TRUE;
ENABLE_UART_RX_INTERRUPT();
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
/*
	for(i = 0; i < 8; i++)
	{
		app.F2[i] = app.eMBdata[(i+2)];
		app.F3[i] = app.eMBdata[(i+10)];

		if( i < 4)
		{
			app.F6[i] = app.eMBdata[(i+26)];
		}
		if( i < 3)
		{
			app.F4[i] = app.eMBdata[(i+18)];
			app.F5[i] = app.eMBdata[(i+22)];
		}
		if( i < 2)
		{
			app.F1[i] = app.eMBdata[i];
		}
	}
	
	updateDigits();
	updateMMD();
*/
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



void updateDigits(void)
{

	DigitDisplay_updateFields(app.F3,12,8);
	DigitDisplay_updateFields(app.F4,0,3);
	DigitDisplay_updateFields(app.F5,3,3);  
	DigitDisplay_updateFields(app.F1,6,2);
  	DigitDisplay_updateFields(app.F6,8,4); 


	DigitDisplay_DotOn(1,1);
	DigitDisplay_DotOn(4,1);
	DigitDisplay_DotOn(10,1);
}

void updateMMD(void)
{
	MMD_clearSegment(0);
	mmdConfig.startAddress = 0;
	mmdConfig.length = MMD_MAX_CHARS;
	mmdConfig.symbolCount = strlen(app.F2); 
	mmdConfig.symbolBuffer = app.F2;
	mmdConfig.scrollSpeed = 0;//SCROLL_SPEED_LOW;
	MMD_configSegment( 0 , &mmdConfig);
}
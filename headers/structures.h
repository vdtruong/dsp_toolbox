/* 10/31/09	Structures headers. */

typedef struct Coord 
{
	uint16 xCoordArry[SETCOEFFSIZE];
	uint16 yCoordArry[SETCOEFFSIZE];
} lcdCoord;

typedef struct plotXHYH
{
	uint8 cmnd;
	uint8 xmsb; /* x most sig. byte */
	uint8 xlsb; /* x least sig. byte */
	uint8 ymsb;
	uint8 ylsb;
} plotXhyh;

typedef struct cmndXHYH
{
	uint8 cmnd; /* use cmnd needed */
	uint8 xmsb; /* x most sig. byte */
	uint8 xlsb; /* x least sig. byte */
	uint8 ymsb;
	uint8 ylsb;
} cmndXhyh;

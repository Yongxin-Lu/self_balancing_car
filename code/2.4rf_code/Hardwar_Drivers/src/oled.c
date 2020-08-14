/*
 * oled.c
 *
 *  Created on: 2020年7月14日
 *      Author: simonlu
 */

#include "oled.h"
#include "oledfont.h"
#include "systick.h"

#define SOFT_SPI

int16_t cursor_x;
int16_t cursor_y;
uint8_t textsize=1;
uint8_t textcolor=1;
uint8_t textbg=0;

//oled显存映射
u8 OLED_GRAM[OLED_WIDTH*OLED_HEIGHT/8];	

static const uint32_t imgFont[] = {
    0x00000000, 0x00017000, 0x000C00C0, 0x0A7CA7CA, 0x0855F542, 0x19484253, 0x1251F55E, 0x00003000,
    0x00452700, 0x001C9440, 0x0519F314, 0x0411F104, 0x00000420, 0x04104104, 0x00000400, 0x01084210,
    0x0F45145E, 0x0001F040, 0x13555559, 0x0D5D5551, 0x087C928C, 0x0D555557, 0x0D55555E, 0x010C5251,
    0x0F55555E, 0x0F555556, 0x0000A000, 0x0000A400, 0x0028C200, 0x0028A280, 0x00086280, 0x000D5040,
    0x0018E300, 0x1F24929C, 0x0D5D555F, 0x1145149C, 0x0725145F, 0x1155555F, 0x0114515F, 0x1D55545E,
    0x1F10411F, 0x0045F440, 0x07210410, 0x1D18411F, 0x1041041F, 0x1F04F05E, 0x1F04109C, 0x0F45545E,
    0x0314925F, 0x1F45D45E, 0x1B34925F, 0x0D555556, 0x0105F041, 0x0721041F, 0x0108421F, 0x0F41E41F,
    0x1D184317, 0x0109C107, 0x114D5651, 0x0045F000, 0x0001F000, 0x0001F440, 0x000C1080, 0x10410410,
};

/********** 禁用半主机模式 **********/
#pragma import(__use_no_semihosting)
 
struct __FILE
{
	int a;
};
 
FILE __stdout;
 
void _sys_exit(int x)
{
}

//Source: https://blog.csdn.net/qq_32108893/article/details/78682531
int fputc(int ch, FILE *f)
{
	char temp_ch=(char)ch;
	static uint8_t x = 0, y = 0;
    if(ch == '\n')    		//换行 
    { 
        x = 0; 
        y += 8; 
        return ch; 
					
    }
    if(x + 6 > 128)  			//宽不能超过128
    { 
        x = 0;          	//x置零 
        y += 8; 					//y移至下一行 			
    } 
    if(y + 6 > 64)  			//行不超过7
    { 
        y=0;
        OLED_ClearDisplay(); 		//直接清屏 重头开始 
        x=0;
    }
    OLED_PrintCharEx(x, y, &temp_ch);		//打印字符ch 
		OLED_Refresh_Gram();
    x += 6;               //跳转到下一个位置, 是否越界有上面函数判断
    return ch; 
}

//OLED的初始化
void OLED_Init(void)
{
	OLED_GPIO_Init();
	
#ifndef SOFT_SPI
	OLED_SPI_Init();
#endif
	
	OLED_RST_Set();
	Delay_ms(100);
	OLED_RST_Clr();
	Delay_ms(100);
	OLED_RST_Set(); 
  //init OLED
  OLED_W_CMD(0xae);//--turn off oled panel
  OLED_W_CMD(0x00);//---set low column address
  OLED_W_CMD(0x10);//---set high column address
  OLED_W_CMD(0x40);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
  OLED_W_CMD(0x81);//--set contrast control register
  OLED_W_CMD(0xcf); // Set SEG Output Current Brightness
   
  OLED_W_CMD(0xa1);//--Set SEG/Column Mapping     
  OLED_W_CMD(0xc8);//Set COM/Row Scan Direction   
  OLED_W_CMD(0xa6);//--set normal display
  OLED_W_CMD(0xa8);//--set multiplex ratio(1 to 64)
  OLED_W_CMD(0x3f);//--1/64 duty
  OLED_W_CMD(0xd3);//-set display offset  Shift Mapping RAM Counter (0x00~0x3F)
  OLED_W_CMD(0x00);//-not offset
  OLED_W_CMD(0xd5);//--set display clock divide ratio/oscillator frequency
  OLED_W_CMD(0x80);//--set divide ratio, Set Clock as 100 Frames/Sec
  OLED_W_CMD(0xd9);//--set pre-charge period
  OLED_W_CMD(0xf1);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
  OLED_W_CMD(0xda);//--set com pins hardware configuration
  OLED_W_CMD(0x12);
  OLED_W_CMD(0xdb);//--set vcomh
  OLED_W_CMD(0x40);//Set VCOM Deselect Level
  OLED_W_CMD(0x20);//-Set Page Addressing Mode (0x00/0x01/0x02)
  OLED_W_CMD(0x02);//
  OLED_W_CMD(0x8d);//--set Charge Pump enable/disable
  OLED_W_CMD(0x14);//--set(0x10) disable
  OLED_W_CMD(0xa4);// Disable Entire Display On (0xa4/0xa5)
  OLED_W_CMD(0xa6);// Disable Inverse Display On (0xa6/a7) 
  OLED_ClearDisplay();
  OLED_Refresh_Gram();
  OLED_W_CMD(0xaf);//--turn on oled panel
}

void OLED_GPIO_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable,ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);
	
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;           //flash chip csn pin
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOC, &GPIO_InitStructure);
 	GPIO_SetBits(GPIOC,GPIO_Pin_13);

#ifndef SOFT_SPI
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_5;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
#else
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_5;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
 	GPIO_SetBits(GPIOB,GPIO_Pin_3|GPIO_Pin_5);
#endif
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_9;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
 	GPIO_SetBits(GPIOB,GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_9);
}

#ifndef SOFT_SPI

//ssd1306 support cpol low, cpha 1edge
//prescaler(72mhz) 2;  can't set too low

void OLED_SPI_Init(void)
{	
	SPI_InitTypeDef SPI_InitStructure;
	
	GPIO_PinRemapConfig(GPIO_Remap_SPI1,ENABLE);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	
	SPI_Cmd(SPI1,DISABLE); //<<--------------------------------------------------important
	
	SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;   //Can't select two lines mode
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPI1,&SPI_InitStructure);

	SPI_Cmd(SPI1,ENABLE);

}
#endif

//向OLED写命令。
void OLED_W_CMD(u8 cmd)
{	
  OLED_DC_Clr();
	OLED_CS_Clr();
	#ifndef SOFT_SPI
	//Delay_us(1);  //<<--------------------------------------------must wait a few time, 1us maybe too long
	
	while(!SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE));
	SPI_I2S_SendData(SPI1,cmd);
	//while(!SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE));
#else
	u8 i;
	for(i=0;i<8;i++)
	{			  
		OLED_SCL_Clr();
		if(cmd&0x80)
		   OLED_SDA_Set();
		else 
		   OLED_SDA_Clr();
		OLED_SCL_Set();
		cmd<<=1;   
	}	
#endif
	OLED_CS_Set();
   	  
} 
//向OLED写数据。
void OLED_W_DATA(u8 data)
{	
  OLED_DC_Set();
	OLED_CS_Clr();
	#ifndef SOFT_SPI
	//Delay_us(1);  //<<--------------------------------------------must wait a few time, 1us maybe too long
	
	while(!SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE));
	SPI_I2S_SendData(SPI1,data);
	//while(!SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE));
#else
	u8 i;
	for(i=0;i<8;i++)
	{			  
		OLED_SCL_Clr();
		if(data&0x80)
		   OLED_SDA_Set();
		else 
		   OLED_SDA_Clr();
		OLED_SCL_Set();
		data<<=1;   
	}	
#endif
	OLED_CS_Set();	
} 

//更新显存到OLED
void OLED_Refresh_Gram(void)
{
  OLED_W_CMD(0x20);
  OLED_W_CMD(0);
	
  OLED_W_CMD(0x21);
  OLED_W_CMD(0);   // Column start address (0 = reset)
  OLED_W_CMD(OLED_WIDTH-1); // Column end address (127 = reset)

  OLED_W_CMD(0x22);
  OLED_W_CMD(0); // Page start address (0 = reset)
  OLED_W_CMD(7); // Page end address

  for(int i=0; i<OLED_WIDTH*OLED_HEIGHT/8; i++)
	{
    OLED_W_DATA(OLED_GRAM[i]);
  } 
}

//屏幕旋转180度
void OLED_DisplayTurn(u8 i)
{
	if(i==0)
		{
			OLED_W_CMD(0xC8);//正常显示
			OLED_W_CMD(0xA1);
		}
	if(i==1)
		{
			OLED_W_CMD(0xC0);//反转显示
			OLED_W_CMD(0xA0);
		}
}

//OLED整屏填充
void OLED_FillScreen(u8 color)
{
  if (color) color = 0xFF;  
  for(int16_t i=0; i<OLED_WIDTH*OLED_HEIGHT/8; i++)  
  { 
    OLED_GRAM[i] = color; 
  }
}

//OLED读点
u8 OLED_GetPixel(u8 x, u8 y)
{
  uint8_t row = y / 8;
  uint8_t bit_position = y % 8;
  return (OLED_GRAM[(row*OLED_WIDTH) + x] & _BV(bit_position)) >> bit_position;
}

//OLED画点
void OLED_DrawPixel(u8 x, u8 y, u8 color)
{
  if ( x > (OLED_WIDTH-1) ||  y > (OLED_HEIGHT-1))
  {
    return;
  }

  u8 row = (u8)y / 8;
  if (color)
  {
    OLED_GRAM[(row*OLED_WIDTH) + (u8)x] |=   _BV((u8)y % 8);
  }
  else
  {
    OLED_GRAM[(row*OLED_WIDTH) + (u8)x] &= ~ _BV((u8)y % 8);
  }
}

//画圆
void OLED_DrawCircle(int16_t x0, int16_t y0, uint8_t r, uint8_t color)
{
  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x = 0;
  int16_t y = r;

  OLED_DrawPixel(x0, y0+r, color);
  OLED_DrawPixel(x0, y0-r, color);
  OLED_DrawPixel(x0+r, y0, color);
  OLED_DrawPixel(x0-r, y0, color);

  while (x<y)
  {
    if (f >= 0)
    {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }

    x++;
    ddF_x += 2;
    f += ddF_x;

    OLED_DrawPixel(x0 + x, y0 + y, color);
    OLED_DrawPixel(x0 - x, y0 + y, color);
    OLED_DrawPixel(x0 + x, y0 - y, color);
    OLED_DrawPixel(x0 - x, y0 - y, color);
    OLED_DrawPixel(x0 + y, y0 + x, color);
    OLED_DrawPixel(x0 - y, y0 + x, color);
    OLED_DrawPixel(x0 + y, y0 - x, color);
    OLED_DrawPixel(x0 - y, y0 - x, color);
  }
}

//画垂直线
void OLED_DrawFastVLine(int16_t x, int16_t y, int8_t h, uint8_t color)
{
  int end = y+h;
	
    /*  Check parameters  */
    if (y < 0) {
        if (h <= -y) return;
        h += y;
        y = 0;
    }
    if (h <= 0 || y >= OLED_HEIGHT || x <= 0 || x >= OLED_WIDTH) return;
    if (y + h > OLED_HEIGHT) h = OLED_HEIGHT - y;
	
  for (int a = max(0,y); a < min(end,OLED_HEIGHT); a++)
  {
    OLED_DrawPixel(x,a,color);
  }
}

//画水平线
void OLED_DrawFastHLine(int16_t x, int16_t y, uint8_t w, uint8_t color)
{	
  int end = x+w;
    /*  Check parameters  */
    if (x < 0) {
        if (w <= -x) return;
        w += x;
        x = 0;
    }
    if (w <= 0 || x >= OLED_WIDTH || y <= 0 || y >= OLED_HEIGHT) return;
    if (x + w > OLED_WIDTH) w = OLED_WIDTH - x;
	
  for (int a = max(0,x); a < min(end,OLED_WIDTH); a++)
  {
    OLED_DrawPixel(a,y,color);
  }
}

//画图
void OLED_DrawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, uint8_t w, uint8_t h, uint8_t color)
{
 int yOffset = ABS(y) % 8;
 int sRow = y / 8;
 if (y < 0) {
   sRow--;
   yOffset = 8 - yOffset;
 }
 int rows = h/8;
 if (h%8!=0) rows++;
 for (int a = 0; a < rows; a++) {
   int bRow = sRow + a;
   if (bRow > (OLED_HEIGHT/8)-1) break;
   if (bRow > -2) {
     for (int iCol = 0; iCol<w; iCol++) {
       if (iCol + x > (OLED_WIDTH-1)) break;
       if (iCol + x >= 0) {
         if (bRow >= 0) {
           if      (color == WHITE) OLED_GRAM[ (bRow*OLED_WIDTH) + x + iCol ] |= (pgm_read_byte(bitmap+(a*w)+iCol) << yOffset);
           else if (color == BLACK) OLED_GRAM[ (bRow*OLED_WIDTH) + x + iCol ] &= (pgm_read_byte(bitmap+(a*w)+iCol) << yOffset);
           else                     OLED_GRAM[ (bRow*OLED_WIDTH) + x + iCol ] ^= pgm_read_byte(bitmap+(a*w)+iCol) << yOffset;
         }
         if (yOffset && bRow<(OLED_HEIGHT/8)-1 && bRow > -2) {
           if      (color == WHITE) OLED_GRAM[ ((bRow+1)*OLED_WIDTH) + x + iCol ] |= pgm_read_byte(bitmap+(a*w)+iCol) >> (8-yOffset);
           else if (color == BLACK) OLED_GRAM[ ((bRow+1)*OLED_WIDTH) + x + iCol ] &= ~(pgm_read_byte(bitmap+(a*w)+iCol) >> (8-yOffset));
           else                     OLED_GRAM[ ((bRow+1)*OLED_WIDTH) + x + iCol ] ^= pgm_read_byte(bitmap+(a*w)+iCol) >> (8-yOffset);
         }
       }
     }
   }
 }
}

//画实心矩形
void FillRect(int16_t x, int16_t y, uint8_t w, uint8_t h, uint8_t color)
{
  for (int16_t i=x; i<x+w; i++)
  {
    OLED_DrawFastVLine(i, y, h, color);
  }
}
//void FillBeltBlack(uint8_t *p, uint8_t d, uint8_t w)
//{
//    d = ~d;
//    for (; w > 0; w--) {
//        *p++ &= d;
//    }
//}
//void FillBeltWhite(uint8_t *p, uint8_t d, uint8_t w)
//{
//    for (; w > 0; w--) {
//        *p++ |= d;
//    }
//}

//画实心矩形 带输入参数保护
void FillRect2(int16_t x, int16_t y, uint8_t w, int8_t h, uint8_t color)
{
    /*  Check parameters  */
    if (x < 0) {
        if (w <= -x) return;
        w += x;
        x = 0;
    }
    if (y < 0) {
        if (h <= -y) return;
        h += y;
        y = 0;
    }
    if (w <= 0 || x >= OLED_WIDTH || h <= 0 || y >= OLED_HEIGHT) return;
    if (x + w > OLED_WIDTH) w = OLED_WIDTH - x;
    if (y + h > OLED_HEIGHT) h = OLED_HEIGHT - y;

    /*  Draw a filled rectangle  */
	  for (int16_t i=x; i<x+w; i++)
	  {
		OLED_DrawFastVLine(i, y, h, color);
	  }
//	/*  Draw a filled rectangle2  */
//    uint8_t yOdd = y & 7;
//    uint8_t d = 0xFF << yOdd;
//    y -= yOdd;
//    h += yOdd;
//    for (uint8_t *p = OLED_GRAM + x + (y / 8) * OLED_WIDTH; h > 0; h -= 8, p += OLED_WIDTH) {
//        if (h < 8) d &= 0xFF >> (8 - h);
//        if (color == BLACK) {
//            FillBeltBlack(p, d, w);
//        } else {
//            FillBeltWhite(p, d, w);
//        }
//        d = 0xFF;
//    }
}

void DrawChar(int16_t x, int16_t y, unsigned char c, uint8_t color, uint8_t bg, uint8_t size)
{
	  //color=1;
    u8 draw_bg = bg != color;

    if (x >= OLED_WIDTH || y >= OLED_HEIGHT || x + 5 * size < 0 || y + 6 * size < 0) return;
    uint32_t ptn = pgm_read_dword(imgFont + (c - ' '));
    if (size == 1) {
        for (int8_t i = 0; i < 6; i++) {
            for (int8_t j = 0; j < 6; j++) {
                u8 draw_fg = ptn & 0x1;
                if (draw_fg || draw_bg) {
                    OLED_DrawPixel(x + i, y + j, (draw_fg) ? color : bg);
                }
                ptn >>= 1;
            }
        }
    } else {
        for (int8_t i = 0; i < 6; i++) {
            for (int8_t j = 0; j < 6; j++) {
                u8 draw_fg = ptn & 0x1;
                if (draw_fg || draw_bg) {
                    FillRect(x + i * size, y + j * size, size, size, (draw_fg) ? color : bg);
                }
                ptn >>= 1;
            }
        }
    }
}

//显示一个字符号串
void OLED_ShowString(u8 x,u8 y,char *chr)
{
	unsigned char j=0;
	while (chr[j]!='\0')
	{		DrawChar(x,y,chr[j],1,0,1);
			x+=8;
		if(x>(OLED_WIDTH-8)){x=0;y+=6;}
			j++;
	}
}

//画空心矩形
void DrawRect(int16_t x, int16_t y, uint8_t w, int8_t h, uint8_t color)
{
    OLED_DrawFastHLine(x, y, w, color);
    OLED_DrawFastHLine(x, y + h - 1, w, color);
    OLED_DrawFastVLine(x, y + 1, h - 2, color);
    OLED_DrawFastVLine(x + w - 1, y + 1, h - 2, color);
}

 void FillPatternedRect(int16_t x, int16_t y, uint8_t w, int8_t h, const uint8_t *ptn)
{
    /*  Check parameters  */
    if (x < 0) {
        if (w <= -x) return;
        w += x;
        x = 0;
    }
    if (y < 0) {
        if (h <= -y) return;
        h += y;
        y = 0;
    }
    if (w <= 0 || x >= OLED_WIDTH || h <= 0 || y >= OLED_HEIGHT) return;
    if (x + w > OLED_WIDTH) w = OLED_WIDTH - x;
    if (y + h > OLED_HEIGHT) h = OLED_HEIGHT - y;

    /*  Draw a patterned rectangle  */
    uint8_t yOdd = y & 7;
    uint8_t d = 0xFF << yOdd;
    y -= yOdd;
    h += yOdd;
    for (uint8_t *p = OLED_GRAM + x + (y / 8) * OLED_WIDTH; h > 0; h -= 8, p += OLED_WIDTH - w) {
        if (h < 8) d &= 0xFF >> (8 - h);
        for (uint8_t i = w; i > 0; i--, p++) {
            *p = (*p & ~d)|(pgm_read_byte(ptn + (int) p % 4) & d);
        }
        d = 0xFF;
    }
}

void swap(int16_t *a, int16_t *b)
{
  int temp = *a;
  *a = *b;
  *b = temp;
}

//OLED画线
void OLED_DrawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint8_t color)
{
  // bresenham's algorithm - thx wikpedia
  uint8_t steep = ABS(y1 - y0) > ABS(x1 - x0);
  if (steep) {
    swap(&x0, &y0);
    swap(&x1, &y1);
  }

  if (x0 > x1) {
    swap(&x0, &x1);
    swap(&y0, &y1);
  }

  int16_t dx, dy;
  dx = x1 - x0;
  dy = ABS(y1 - y0);

  int16_t err = dx / 2;
  int8_t ystep;

  if (y0 < y1)
  {
    ystep = 1;
  }
  else
  {
    ystep = -1;
  }

  for (; x0 <= x1; x0++)
  {
    if (steep)
    {
      OLED_DrawPixel(y0, x0, color);
    }
    else
    {
      OLED_DrawPixel(x0, y0, color);
    }

    err -= dy;
    if (err < 0)
    {
      y0 += ystep;
      err += dx;
    }
  }
}

void OLED_Write(uint8_t c)
{
    if (c == '\n') {
        cursor_y += textsize * 6;
        cursor_x = 0;
    } else if (c >= ' ' && c <= '_') {
        DrawChar(cursor_x, cursor_y, c, textcolor, textbg, textsize);
        cursor_x += textsize * 6;
        if ( (cursor_x > (OLED_WIDTH - textsize * 6))) OLED_Write('\n');
    }
  
}

//设置坐标
void SetCursor(int16_t x, int16_t y)
{
  cursor_x = x;
  cursor_y = y;
}

//设置字符大小
void SetTextSize(uint8_t s)
{
  textsize = max(1,s); 
}

void OLED_PrintChar( char *chr)
{
	unsigned char j=0;
	while (chr[j]!='\0')
	{		
		OLED_Write(chr[j]);
		j++;
	}
}

void OLED_PrintNum(u16 num)
{
	unsigned char j=0;
	char chr[6];
	sprintf(chr, "%d", num); 
	while (chr[j]!='\0')
	{		
		OLED_Write(chr[j]);
		j++;
	}
}

//带坐标打印字符串
size_t OLED_PrintCharEx(int16_t x, int16_t y,char *chr)
{
	//size_t n = 0;
	SetCursor(x,y);
	unsigned char j=0;
	while (chr[j]!='\0')
	{		
		OLED_Write(chr[j]);
		j++;
	}
	return j;
}

//OLED清屏
void OLED_ClearDisplay(void) 
{
  OLED_FillScreen(BLACK);
}

//开启OLED显示    
void OLED_Display_On(void)
{
	OLED_W_CMD(0X8D);  //SET DCDC命令
	OLED_W_CMD(0X14);  //DCDC ON
	OLED_W_CMD(0XAF);  //DISPLAY ON
}

//关闭OLED显示     
void OLED_Display_Off(void)
{
	OLED_W_CMD(0X8D);  //SET DCDC命令
	OLED_W_CMD(0X10);  //DCDC OFF
	OLED_W_CMD(0XAE);  //DISPLAY OFF
} 

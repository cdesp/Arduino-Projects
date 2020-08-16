

//----------------------------------------------------------------------
//EASTRISING TECHNOLOGY CO,.LTD.//
// Module    : ER-TFTM070-4V2.1  800*480
// Lanuage   : C51 Code
// Create    : JAVEN LIU
// Date      : Dec-11-2013
// Drive IC  : SSD1963
// INTERFACE : 16BIT 8080
// MCU 		 : STC12LE5C60S2
// VDD		 : 5V   
//----------------------------------------------------------------------
 


//=============================================================

//=============================================================
//LCM INTERFACE

#define  GPIO2_PREFER_SPEED    1
//#include <digitalWriteFast.h>
//#include <arduino2.h>

#define LCD_cs  40 //A0  
#define rs      38 //A1
#define wrb     39 //A2  pc2
#define rdb     3 //A3

#define D0 22 //D2
#define D1 23 //D3
#define D2 24 //D4
#define D3 25 //D5
#define D4 26 //D6
#define D5 27 //D7
#define D6 28 //D8
#define D7 29 //D9  pb1

#define wid 800
#define hei 480
#define extw 0      //horiz area to skip
#define exth 0    //vert area to skip

//*********************************
// COLORS RGB 888
//*********************************
#define DSP_YELLOW 0xFFFF00
#define DSP_RED 0xFF0000
#define DSP_GREEN 0x006400
#define DSP_BLUE 0x0000FF

#define PORTRAIT 0
#define LANDSCAPE 1

//byte      fch, fcl, bch, bcl,fcr,fcg,fcb;
word fcol,bcol;
byte orient=PORTRAIT;


 void byteOut(int bt);

 void SSD1963_Initial(void);
 void LCD_clear(unsigned long  i);
 void LCD_SetPos(unsigned int xs,unsigned int xe,unsigned int ys,unsigned int ye);
 void write_command(unsigned char c);
 void Write_Data_byte(unsigned char DH,unsigned char DL);
 void Write_Data_int(unsigned char d);
 void clrXY();
 void setColor(word color);
 void setColor(byte r, byte g, byte b);
 void setRGBColor(unsigned long color);

char col1=0;

void loop() {

 int buf[798];
  int x, x2;
  int y, y2;
  int r;
  
  Serial.println("Start");
  setRGBColor(DSP_YELLOW); 

  Serial.println("Clear Screen");
 //LCD_CLS();
  LCD_FASTCLS();
  Serial.println("DRAW LINE");
  setRGBColor(DSP_RED);
  drawLine(10,10,200,10);//horiz
  drawLine(10,10,10,200);//vert
  drawLine(10,10,200,200);//diag

  setRGBColor(DSP_YELLOW); 
  drawLine(399, 15, 399, 464);
  drawLine(1, 239, 798, 239);
  for (int i=9; i<790; i+=10)
    drawLine(i, 237, i, 242);
  for (int i=19; i<470; i+=10)
    drawLine(397, i, 402, i);




// Draw sin-, cos- and tan-lines  
  setColor(0,255,0);
 // myGLCD.print("Sin", 5, 15);
  for (int i=1; i<798; i++)
  {
    drawPixel(i,239+(sin(((i*1.13)*3.14)/180)*200));
  }
  
  setColor(255,0,0);
  //myGLCD.print("Cos", 5, 27);
  for (int i=1; i<798; i++)
  {
    drawPixel(i,239+(cos(((i*1.13)*3.14)/180)*200));
  }

  setColor(0,0,255);
  //myGLCD.print("Tan", 5, 39);
  for (int i=1; i<798; i++)
  {
    drawPixel(i,239+(tan(((i*0.9)*3.14)/180)));
  }

delay(2000);

  setColor(0,0,0);
  fillRect(1,15,798,464);
  setColor(0, 0, 255);
//  setBackColor(0, 0, 0);
  drawLine(399, 15, 399, 464);
  drawLine(1, 239, 798, 239);

// Draw a moving sinewave
Serial.println("Moving sinewavw");
  x=1;
  for (int i=1; i<(798*20); i++) 
  {
    x++;
    if (x==799)
      x=1;
    if (i>799)
    {
      if ((x==399)||(buf[x-1]==239))
        setColor(0,0,255);
      else
        setColor(0,0,0);
      drawPixel(x,buf[x-1]);
    }
    setColor(0,255,255);
    y=239+(sin(((i*1.65)*3.14)/180)*(200-(i / 100)));
    drawPixel(x,y);
    buf[x-1]=y;
  }

  delay(2000);
  
  setColor(0,0,0);
  fillRect(1,15,798,464);

// Draw some random filled rectangles
Serial.println("Random Filled rects");
  for (int i=0; i<50; i++)
  {
    setColor(random(255), random(255), random(255));
    x=2+random(746);
    y=16+random(397);
    x2=x+50;
    y2=y+50;
    fillRect(x, y, x2, y2);
  }

  delay(2000);
  
  setColor(0,0,0);
  fillRect(1,15,798,464);

// Draw some random filled, rounded rectangles
Serial.println("Random Filled round rects");
  for (int i=0; i<50; i++)
  {
    setColor(random(255), random(255), random(255));
    x=2+random(746);
    y=16+random(397);
    x2=x+50;
    y2=y+50;
    fillRoundRect(x, y, x2, y2);
  }
  
  delay(2000);
  
  setColor(0,0,0);
  fillRect(1,15,798,464);

// Draw some random filled circles
Serial.println("Random Filled Circles");
  for (int i=0; i<50; i++)
  {
    setColor(random(255), random(255), random(255));
    x=27+random(746);
    y=41+random(397);
    fillCircle(x, y, 25);
  }
  
  delay(2000);
  
  setColor(0,0,0);
  fillRect(1,15,798,464);

// Draw some lines in a pattern
Serial.println("Pattern Lines");
  setColor (255,255,0);
  for (int i=15; i<463; i+=5)
  {
    drawLine(1, i, (i*1.66)-10, 463);
  }
  setColor (255,255,0);
  for (int i=463; i>15; i-=5)
  {
    drawLine(798, i, (i*1.66)+30, 15);
  }
  setColor (0,255,255);
  for (int i=463; i>15; i-=5)
  {
    drawLine(1, i, 770-(i*1.66), 15);
  }
  setColor (0,255,255);
  for (int i=15; i<463; i+=5)
  {
    drawLine(798, i, 810-(i*1.66), 463);
  }
  
  delay(2000);
  
  setColor(0,0,0);
  fillRect(1,15,798,464);

test:

// Draw some random circles
Serial.println("Random Circles");
  for (int i=0; i<250; i++)
  {
    setColor(random(255), random(255), random(255));
    x=32+random(700);
    y=45+random(340);
    r=random(30);
    drawCircle(x, y, r);
  }

  delay(2000);
  
  setColor(0,0,0);
  fillRect(1,15,798,464);

// Draw some random rectangles
Serial.println("Random rects");
  for (int i=0; i<250; i++)
  {
    setColor(random(255), random(255), random(255));
    x=2+random(796);
    y=16+random(447);
    x2=2+random(796);
    y2=16+random(447);
    drawRect(x, y, x2, y2);
  }

  delay(2000);
  
  setColor(0,0,0);
  fillRect(1,15,798,464);

// Draw some random rounded rectangles
Serial.println("Random round REcts");
  for (int i=0; i<250; i++)
  {
    setColor(random(255), random(255), random(255));
    x=2+random(796);
    y=16+random(447);
    x2=2+random(796);
    y2=16+random(447);
    drawRoundRect(x, y, x2, y2);
  }

  delay(2000);
  
  setColor(0,0,0);
  fillRect(1,15,798,464);

  Serial.println("Random Lines");
  for (int i=0; i<250; i++)
  {
    setColor(random(255), random(255), random(255));
    x=2+random(796);
    y=16+random(447);
    x2=2+random(796);
    y2=16+random(447);
    drawLine(x, y, x2, y2);
  }

  delay(2000);
  Serial.println("Random Pixels");
  
  setColor(0,0,0);
  fillRect(1,15,798,464);

  for (int i=0; i<10000; i++)
  {
    setColor(random(255), random(255), random(255));
    drawPixel(2+random(796), 16+random(447));
  }


  
  Serial.println("PROGAM END");
  while (true);
 
  
}

void setup() {
  Serial.begin (9600);
 
  pinMode(LCD_cs, OUTPUT);
  pinMode(rs, OUTPUT);
  pinMode(wrb, OUTPUT);
  pinMode(rdb, OUTPUT);
  digitalWrite(rdb, HIGH);//off

  pinMode(D0, OUTPUT);
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(D3, OUTPUT);
  pinMode(D4, OUTPUT);
  pinMode(D5, OUTPUT);
  pinMode(D6, OUTPUT);
  pinMode(D7, OUTPUT);

    pinMode(8, OUTPUT);  //backlight 
  digitalWrite(8, HIGH);//on
  
  SSD1963_Initial(); 
  
  
}

void byteOut(int bt){
  digitalWrite(D0, bt & B00000001);
  digitalWrite(D1, bt & B00000010);
  digitalWrite(D2, bt & B00000100);
  digitalWrite(D3, bt & B00001000);
  digitalWrite(D4, bt & B00010000);
  digitalWrite(D5, bt & B00100000);
  digitalWrite(D6, bt & B01000000);
  digitalWrite(D7, bt & B10000000);
//  Serial.print(bt & B10000000);
  
}

void write_command(unsigned char c)
{
 //  digitalWrite(LCD_cs, LOW);	
   digitalWrite(rs, LOW);	
   SendByte(c);
   digitalWrite(rs, HIGH);//allways on data
//   digitalWrite(LCD_cs, HIGH);	

}

void Write_Data_byte(unsigned char DH,unsigned char DL)
{
 //  digitalWrite(LCD_cs, LOW);  
  // digitalWrite(rs, HIGH);  
   SendByte(DH);      
   SendByte(DL);   
 //  digitalWrite(wrb, LOW);  
  // digitalWrite(wrb, HIGH);    
//   digitalWrite(LCD_cs, HIGH);  
  
}

void SendByte(unsigned char d)
{   
 byteOut(d);   
 digitalWrite(wrb, LOW);  
 digitalWrite(wrb, HIGH);    
}
    
void Write_Data_int(unsigned char d)
{
  // digitalWrite(LCD_cs, LOW);	
  // digitalWrite(rs, HIGH);      
   SendByte(d);
 //  digitalWrite(LCD_cs, HIGH);	
}


word rgb888to666(byte r,byte g,byte b)
{
  return rgb888to666((r<<16)+(g<<8)+b);
}

word rgb888to666(unsigned long  d)
{
 byte dr,dg,db,rnew,gnew,bnew;
 word rgb;

 dr=d>>16;
 dg=d>>8;
 db=d;

 //r5,r4,r3,r2,r1,r0  r5 allways 0
 //g5,g4,g3,g2,g1,g0  g2 allways 0
 //b5,b4,b3,b2,b1,b0

 rnew= dr & 0b11111000; //shift left to upperbyte
 //gnew= (dg & 0b11101100); //omit bit 2 of 6pack
 gnew= (dg & 0b11100000); 
 gnew+= ((dg & 0b00001100)<<1); 
 bnew= db & 0b11111100; //shift 2 right to lower byte

 rgb=(rnew<<8)+(gnew<<3)+(bnew>>2);

 return rgb;
}

void Sendpixel(word col)
{
  SendByte(col>>8);
  SendByte(col);
}

void LCD_CLS()
{
  //long sz=wid*hei;
  long sz=384000;
  Serial.println("CLEARING...");
 
 digitalWrite(LCD_cs, LOW);        
 clrXY();
 //digitalWrite(rs, HIGH); 

 Serial.println(sz); 
 for (long i=0;i<sz; i++)
 {
  Sendpixel(fcol);
 }
  
digitalWrite(LCD_cs, HIGH);  
Serial.println("CLEAR OK!"); 
}

void LCD_FASTCLS()
{
  //long sz=wid*hei;
  long sz=384000;
  Serial.println("CLEARING...");
 
 digitalWrite(LCD_cs, LOW);        
 clrXY();
 //digitalWrite(rs, HIGH); 
 byteOut(0xFF);   
 
 for (long i=0;i<sz; i++)
 {
  digitalWrite(wrb, LOW);  
  digitalWrite(wrb, HIGH);    
  digitalWrite(wrb, LOW);  
  digitalWrite(wrb, HIGH);      
 }
  
digitalWrite(LCD_cs, HIGH);  
Serial.println("CLEAR OK!"); 
}

void LCD_SetPos(unsigned int xs,unsigned int xe,unsigned int ys,unsigned int ye)
{
    write_command(0x002A);	
	Write_Data_int(xs>>8);	    
	Write_Data_int(xs&0x00ff);
	Write_Data_int(xe>>8);	    
	Write_Data_int(xe&0x00ff);
    write_command(0x002b);	
	Write_Data_int(ys>>8);	    
	Write_Data_int(ys&0x00ff);
	Write_Data_int(ye>>8);	    
	Write_Data_int(ye&0x00ff);
	write_command(0x002c); 
}

void delayus(int us){
  
 delayMicroseconds(us); 
}

void SSD1963_Initial(void)
{
  digitalWrite(LCD_cs, LOW);      
	write_command(0x00E2);	//PLL multiplier, set PLL clock to 120M
	Write_Data_int(0x0023);	    //23//N=0x36 for 6.5M, 0x23 for 10M crystal
	Write_Data_int(0x0002);
	Write_Data_int(0x0054);
	write_command(0x00E0);  // PLL enable
	Write_Data_int(0x0001);
	delayus(10);
	write_command(0x00E0);
	Write_Data_int(0x0003);		// now, use PLL output as system clock
	delayus(10);
	write_command(0x0001);  // software reset
	delayus(20);
	write_command(0x00E6);	//PLL setting for PCLK, depends on resolution
	Write_Data_int(0x0003);
	Write_Data_int(0x0033); //33
	Write_Data_int(0x0033); //33

	write_command(0x00B0);	//LCD SPECIFICATION
	Write_Data_int(0x0020); //24 bit TFT panel
        //Write_Data_int(0x0080); //TFT Mode
	Write_Data_int(0x0000); //Hsync+Vsync +DE mode  TFT mode
	Write_Data_int(((wid-1)>>8));  //Set HDP
	Write_Data_int(wid-1);
    Write_Data_int((hei-1)>>8);  //Set VDP
	Write_Data_int(hei-1);
    Write_Data_int(0x0000);

	write_command(0x00B4);	//HSYNC
	Write_Data_int(0x04);  //Set HT
	Write_Data_int(0x1f);
	Write_Data_int(0x00);  //Set HPS
	Write_Data_int(0xd2);
	Write_Data_int(0x00);			   //Set HPW
	Write_Data_int(0x00);  //Set HPS
	Write_Data_int(0x00);
	Write_Data_int(0x00);

	write_command(0x00B6);	//VSYNC
	Write_Data_int(0x02);   //Set VT
	Write_Data_int(0x0c);
	Write_Data_int(0x00);  //Set VPS
	Write_Data_int(0x22);
	Write_Data_int(0x00);		//Set VPW
	Write_Data_int(0x00);  //Set FPS
	Write_Data_int(0x00);

  write_command(0x00B8);
  Write_Data_int(0x000f); //F    //GPIO is controlled by host GPIO[3:0]=output   GPIO[0]=1  LCD ON  GPIO[0]=1  LCD OFF 
  Write_Data_int(0x0001);    //GPIO0 normal


  write_command(0x00BA);
  Write_Data_int(0x0001); //1   //GPIO[0] out 1 --- LCD display on/off control PIN


	

	write_command(0x0036); //rotation
	Write_Data_int(0x0008);  // 00 //RGB=BGR

	//write_command(0x003A); //Set the current pixel format for RGB image data
//	Write_Data_int(0x0070);//8-bit/pixel 50=16bit  20=8bit 0f=3bit 70=24bit

	write_command(0x00F0); //Pixel Data Interface Format
	//Write_Data_int(0x0000);//8-bit 8R,8G,8B 3bytes data //3= 16packed  5=24bit 0=8bit 6=9bit 2=16bit
  Write_Data_int(0x0006);//9-bit 6R,6G,6B 2bytes data //3= 16packed  5=24bit 0=8bit 6=9bit 2=16bit
  
  delayus(5);
  

	write_command(0x00BC); 
	Write_Data_int(0x0040);//contrast value
	Write_Data_int(0x0080);//brightness value
	Write_Data_int(0x0040);//saturation value
	Write_Data_int(0x0001);//Post Processor Enable

	delayus(5);

      //  write_command(0x0030); //set partial area
	//Write_Data_int(0x0000);//
	//Write_Data_int(0x0000);//
	//Write_Data_int(0x0001);//
	//Write_Data_int(0x00E0);//

        //write_command(0x0012);//enter partial mode

	//write_command(0x0029); //display on


 // LCD_SetPos(0, 799,0, 479);
  write_command(0x29);    //display on

	write_command(0x00BE); //set PWM for B/L
	Write_Data_int(0x0006);
	Write_Data_int(0x00f0); //80
	Write_Data_int(0x0001);
	Write_Data_int(0x00f0);
	Write_Data_int(0x0000);
	Write_Data_int(0x0000);

	write_command(0x00d0); 
	Write_Data_int(0x000d);

  //write_command(0x2C); 
	digitalWrite(LCD_cs, HIGH);  
}


void setColor(byte r, byte g, byte b)
{
 // fch=((r&248)|g>>5);
//  fcl=((g&28)<<3|b>>3);

 // fcr=r;
  //fcg=g;
  //fcb=b;

 fcol=rgb888to666(r,g,b);
  
//  fch=((r&0x1f<<3)|(g&0x3f>>3));
//  fcl=((g&0x3f)<<5|(b&0x1f));
}

void setRGBColor(unsigned long  color)
{
  fcol=rgb888to666(color);  
}

void setColor(word color)
{
  //fch=byte(color>>8);
  //fcl=byte(color & 0xFF);
 fcol=color; 
}

word getColor()
{
 // return (fch<<8) | fcl;
 return fcol;
}


void drawPixel(int x, int y)
{
  digitalWrite(LCD_cs, LOW);    
  LCD_SetPos(x, x, y, y);
 // digitalWrite(rs, HIGH);
  Sendpixel(fcol);  
  clrXY();
  digitalWrite(LCD_cs, HIGH);    
}


void drawLine(int x1, int y1, int x2, int y2)
{
  if (y1==y2)
    drawHLine(x1, y1, x2-x1);
  else if (x1==x2)
    drawVLine(x1, y1, y2-y1);
  else
  {
    unsigned int  dx = (x2 > x1 ? x2 - x1 : x1 - x2);
    short     xstep =  x2 > x1 ? 1 : -1;
    unsigned int  dy = (y2 > y1 ? y2 - y1 : y1 - y2);
    short     ystep =  y2 > y1 ? 1 : -1;
    int       col = x1, row = y1;

    digitalWrite(LCD_cs, LOW);    
    if (dx < dy)
    {
      int t = - (dy >> 1);
      while (true)
      {
        LCD_SetPos (col,  col,row,  row);
       // digitalWrite(rs, HIGH);
        Sendpixel(fcol);
        if (row == y2)
          return;
        row += ystep;
        t += dx;
        if (t >= 0)
        {
          col += xstep;
          t   -= dy;
        }
      } 
    }
    else
    {
      int t = - (dx >> 1);
      while (true)
      {
        LCD_SetPos (col,  col,row, row);
      //  digitalWrite(rs, HIGH);
        Sendpixel(fcol);
        if (col == x2)
          return;
        col += xstep;
        t += dy;
        if (t >= 0)
        {
          row += ystep;
          t   -= dx;
        }
      } 
    }
    
  }
  clrXY();
  digitalWrite(LCD_cs, HIGH);
}

void drawHLine(int x, int y, int l)
{
  if (l<0)
  {
    l = -l;
    x -= l;
  }
  
 
 digitalWrite(LCD_cs, LOW);
 LCD_SetPos(x,  x+l,y, y);
// digitalWrite(rs, HIGH);

    for (int i=0; i<l+1; i++)
    {
      Sendpixel(fcol);
    }

  clrXY();
   digitalWrite(LCD_cs, HIGH);
}

void drawVLine(int x, int y, int l)
{
  if (l<0)
  {
    l = -l;
    y -= l;
  }
   
 
  digitalWrite(LCD_cs, LOW);
   LCD_SetPos(x,x, y, y+l);
  //digitalWrite(rs, HIGH);

    for (int i=0; i<l+1; i++)
    {
     Sendpixel(fcol);
    }

   clrXY();
   digitalWrite(LCD_cs, HIGH);
 
}

void clrXY()
{
  if (orient==PORTRAIT)
    LCD_SetPos(0,wid-1,0,hei-1);
  else
    LCD_SetPos(0,hei-1,0,wid-1);
}

void setXY(int x1,int y1,int x2,int y2)
{
  LCD_SetPos(x1,x2,y1,y2);   
}

void swap(int &t1,int &t2)
{
  int t=t2;
  t2=t1;
  t1=t;
}

void drawRect(int x1, int y1, int x2, int y2)
{
  if (x1>x2)
  {
    swap(x1, x2);
  }
  if (y1>y2)
  {
    swap(y1, y2);
  }

  drawHLine(x1, y1, x2-x1);
  drawHLine(x1, y2, x2-x1);
  drawVLine(x1, y1, y2-y1);
  drawVLine(x2, y1, y2-y1);
}

void drawRoundRect(int x1, int y1, int x2, int y2)
{
  if (x1>x2)
  {
    swap(x1, x2);
  }
  if (y1>y2)
  {
    swap(y1, y2);
  }
  if ((x2-x1)>4 && (y2-y1)>4)
  {
    drawPixel(x1+1,y1+1);
    drawPixel(x2-1,y1+1);
    drawPixel(x1+1,y2-1);
    drawPixel(x2-1,y2-1);
    drawHLine(x1+2, y1, x2-x1-4);
    drawHLine(x1+2, y2, x2-x1-4);
    drawVLine(x1, y1+2, y2-y1-4);
    drawVLine(x2, y1+2, y2-y1-4);
  }
}

void fillRect(int x1, int y1, int x2, int y2)
{
  if (x1>x2)
  {
    swap( x1, x2);
  }
  if (y1>y2)
  {
    swap( y1, y2);
  }
 
    if (orient==PORTRAIT)
    {
      for (int i=0; i<((y2-y1)/2)+1; i++)
      {
        drawHLine(x1, y1+i, x2-x1);
        drawHLine(x1, y2-i, x2-x1);
      }
    }
    else
    {
      for (int i=0; i<((x2-x1)/2)+1; i++)
      {
        drawVLine(x1+i, y1, y2-y1);
        drawVLine(x2-i, y1, y2-y1);
      }
    }
 
}

void fillRoundRect(int x1, int y1, int x2, int y2)
{
  if (x1>x2)
  {
    swap( x1, x2);
  }
  if (y1>y2)
  {
    swap( y1, y2);
  }

  if ((x2-x1)>4 && (y2-y1)>4)
  {
    for (int i=0; i<((y2-y1)/2)+1; i++)
    {
      switch(i)
      {
      case 0:
        drawHLine(x1+2, y1+i, x2-x1-4);
        drawHLine(x1+2, y2-i, x2-x1-4);
        break;
      case 1:
        drawHLine(x1+1, y1+i, x2-x1-2);
        drawHLine(x1+1, y2-i, x2-x1-2);
        break;
      default:
        drawHLine(x1, y1+i, x2-x1);
        drawHLine(x1, y2-i, x2-x1);
      }
    }
  }
}

void drawCircle(int x, int y, int radius)
{
  int f = 1 - radius;
  int ddF_x = 1;
  int ddF_y = -2 * radius;
  int x1 = 0;
  int y1 = radius;
 
  digitalWrite(LCD_cs, LOW);   
  setXY(x, y + radius, x, y + radius);
  Sendpixel(fcol);
  setXY(x, y - radius, x, y - radius);
  Sendpixel(fcol);
  setXY(x + radius, y, x + radius, y);
  Sendpixel(fcol);
  setXY(x - radius, y, x - radius, y);
  Sendpixel(fcol);
 
  while(x1 < y1)
  {
    if(f >= 0) 
    {
      y1--;
      ddF_y += 2;
      f += ddF_y;
    }
    x1++;
    ddF_x += 2;
    f += ddF_x;    
    setXY(x + x1, y + y1, x + x1, y + y1);
    Sendpixel(fcol);
    setXY(x - x1, y + y1, x - x1, y + y1);
    Sendpixel(fcol);
    setXY(x + x1, y - y1, x + x1, y - y1);
    Sendpixel(fcol);
    setXY(x - x1, y - y1, x - x1, y - y1);
    Sendpixel(fcol);
    setXY(x + y1, y + x1, x + y1, y + x1);
    Sendpixel(fcol);
    setXY(x - y1, y + x1, x - y1, y + x1);
    Sendpixel(fcol);
    setXY(x + y1, y - x1, x + y1, y - x1);
    Sendpixel(fcol);
    setXY(x - y1, y - x1, x - y1, y - x1);
    Sendpixel(fcol);
  }  
  clrXY();
  digitalWrite(LCD_cs, HIGH); 
}

void fillCircle(int x, int y, int radius)
{
  for(int y1=-radius; y1<=0; y1++) 
    for(int x1=-radius; x1<=0; x1++)
      if(x1*x1+y1*y1 <= radius*radius) 
      {
        drawHLine(x+x1, y+y1, 2*(-x1));
        drawHLine(x+x1, y-y1, 2*(-x1));
        break;
      }
}

#include <TimerOne.h>
#include <police.h>

//Pin connected to ST_CP of 74HC595
int latchPin = 12;
//Pin connected to SH_CP of 74HC595
int clockPin = 8;
////Pin connected to DS of 74HC595
int dataPin = 11;

const uint8_t fix_sin[] = {0, 1, 3, 4, 6, 7, 9, 10, 12, 14, 15, 17, 18, 20, 21, 23, 25, 26, 28, 29, 31, 32, 34, 36, 37, 39, 40, 42, 43, 45, 46, 48, 49, 51, 53, 54, 56, 57, 59, 60, 62, 63, 65, 66, 68, 69, 71, 72, 74, 75, 77, 78, 80, 81, 83, 84, 86, 87, 89, 90, 92, 93, 95, 96, 97, 99, 100, 102, 103, 105, 106, 108, 109, 110, 112, 113, 115, 116, 117, 119, 120, 122, 123, 124, 126, 127, 128, 130, 131, 132, 134, 135, 136, 138, 139, 140, 142, 143, 144, 146, 147, 148, 149, 151, 152, 153, 155, 156, 157, 158, 159, 161, 162, 163, 164, 166, 167, 168, 169, 170, 171, 173, 174, 175, 176, 177, 178, 179, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 211, 212, 213, 214, 215, 216, 217, 217, 218, 219, 220, 221, 221, 222, 223, 224, 225, 225, 226, 227, 227, 228, 229, 230, 230, 231, 232, 232, 233, 234, 234, 235, 235, 236, 237, 237, 238, 238, 239, 239, 240, 241, 241, 242, 242, 243, 243, 244, 244, 244, 245, 245, 246, 246, 247, 247, 247, 248, 248, 249, 249, 249, 250, 250, 250, 251, 251, 251, 251, 252, 252, 252, 252, 253, 253, 253, 253, 254, 254, 254, 254, 254, 254, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 256};


int data[8][8];
int inc[8][8];
uint8_t place[8];

boolean drawing[8][8][8];
boolean maison[8][8][24];

int rangee[8] = {38,40,42,44,46,48,50,52};

int col = 0;

int trans = 0;
int sens = 1;

int16_t sin_fix10_8(int16_t in)
{
  int16_t out = 0;
  int16_t x = in % 1024;
  if(x>=0 && x<256) out = fix_sin[x];
  if(x>=256 && x<512) out = fix_sin[512-x];
  if(x>=512 && x<768) out = -fix_sin[x-512];
  if(x>=768 && x<1024) out = -fix_sin[1024-x];
  if (out>=1024) out = 1024;
  return out;
}

int16_t cos_fix10_8(int16_t in)
{
  return sin_fix10_8(256+in);
}


void setup() {
  
  Serial.begin(9600);
  
  Timer1.initialize(20000); 
  Timer1.attachInterrupt( draw_loop );
  //set pins to output so you can control the shift register
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  
  for(int i = 0; i<8; i++)
  {
    pinMode(rangee[i],OUTPUT);
    digitalWrite(rangee[i],LOW);
  }
  
  //digitalWrite(rangee[3],HIGH);
  
  
  
  
  inc[0][2] = 0;
  data[0][2] = 0b10000000;
  inc[2][2] = 0;
  data[2][2] = 0b01000000;
  inc[4][2] = 0;
  data[4][2] = 0b00100000;
  inc[6][2] = 0;
  data[6][2] = 0b00010000;
  inc[0][3] = 0;
  data[0][3] = 0b00001000;
  inc[2][3] = 0;
  data[2][3] = 0b00000100;
  inc[4][3] = 0;
  data[4][3] = 0b00000010;
  inc[6][3] = 0;
  data[6][3] = 0b00000001;
  
  inc[0][6] = 1;
  data[0][6] = 0b10000000;
  inc[2][6] = 1;
  data[2][6] = 0b01000000;
  inc[4][6] = 1;
  data[4][6] = 0b00100000;
  inc[6][6] = 1;
  data[6][6] = 0b00010000;
  inc[0][7] = 1;
  data[0][7] = 0b00001000;
  inc[2][7] = 1;
  data[2][7] = 0b00000100;
  inc[4][7] = 1;
  data[4][7] = 0b00000010;
  inc[6][7] = 1;
  data[6][7] = 0b00000001;
  
  inc[7][7] = 2;
  data[7][7] = 0b10000000;
  inc[5][7] = 2;
  data[5][7] = 0b01000000;
  inc[3][7] = 2;
  data[3][7] = 0b00100000;
  inc[1][7] = 2;
  data[1][7] = 0b00010000;
  inc[7][6] = 2;
  data[7][6] = 0b00001000;
  inc[5][6] = 2;
  data[5][6] = 0b00000100;
  inc[3][6] = 2;
  data[3][6] = 0b00000010;
  inc[1][6] = 2;
  data[1][6] = 0b00000001;
  
  inc[7][3] = 3;
  data[7][3] = 0b10000000;
  inc[5][3] = 3;
  data[5][3] = 0b01000000;
  inc[3][3] = 3;
  data[3][3] = 0b00100000;
  inc[1][3] = 3;
  data[1][3] = 0b00010000;
  inc[7][2] = 3;
  data[7][2] = 0b00001000;
  inc[5][2] = 3;
  data[5][2] = 0b00000100;
  inc[3][2] = 3;
  data[3][2] = 0b00000010;
  inc[1][2] = 3;
  data[1][2] = 0b00000001;
  
  inc[7][1] = 4;
  data[7][1] = 0b10000000;
  inc[5][1] = 4;
  data[5][1] = 0b01000000;
  inc[3][1] = 4;
  data[3][1] = 0b00100000;
  inc[1][1] = 4;
  data[1][1] = 0b00010000;
  inc[7][0] = 4;
  data[7][0] = 0b00001000;
  inc[5][0] = 4;
  data[5][0] = 0b00000100;
  inc[3][0] = 4;
  data[3][0] = 0b00000010;
  inc[1][0] = 4;
  data[1][0] = 0b00000001;
  
  inc[7][5] = 5;
  data[7][5] = 0b10000000;
  inc[5][5] = 5;
  data[5][5] = 0b01000000;
  inc[3][5] = 5;
  data[3][5] = 0b00100000;
  inc[1][5] = 5;
  data[1][5] = 0b00010000;
  inc[7][4] = 5;
  data[7][4] = 0b00001000;
  inc[5][4] = 5;
  data[5][4] = 0b00000100;
  inc[3][4] = 5;
  data[3][4] = 0b00000010;
  inc[1][4] = 5;
  data[1][4] = 0b00000001;
  
  inc[0][4] = 6;
  data[0][4] = 0b10000000;
  inc[2][4] = 6;
  data[2][4] = 0b01000000;
  inc[4][4] = 6;
  data[4][4] = 0b00100000;
  inc[6][4] = 6;
  data[6][4] = 0b00010000;
  inc[0][5] = 6;
  data[0][5] = 0b00001000;
  inc[2][5] = 6;
  data[2][5] = 0b00000100;
  inc[4][5] = 6;
  data[4][5] = 0b00000010;
  inc[6][5] = 6;
  data[6][5] = 0b00000001;
  
  inc[0][0] = 7;
  data[0][0] = 0b10000000;
  inc[2][0] = 7;
  data[2][0] = 0b01000000;
  inc[4][0] = 7;
  data[4][0] = 0b00100000;
  inc[6][0] = 7;
  data[6][0] = 0b00010000;
  inc[0][1] = 7;
  data[0][1] = 0b00001000;
  inc[2][1] = 7;
  data[2][1] = 0b00000100;
  inc[4][1] = 7;
  data[4][1] = 0b00000010;
  inc[6][1] = 7;
  data[6][1] = 0b00000001;
  
  cube_refresh();
  
  init_maison();
  //ecriture(lettre_G, 0, 0, 0, 0);

}

void loop() {
  for(trans=0; trans<16; trans++)
  {
  cube_refresh();
  for(int i =0; i<8; i++)
  {
    for(int j =0; j<8; j++)
    {
      for(int k =0; k<8; k++)
      {
        drawing[i][j][k] = maison[i][j][k+trans];
      }
    }        
  }
  delay(150);
  }
  for(trans=15; trans>=0; trans--)
  {
  cube_refresh();
  for(int i =0; i<8; i++)
  {
    for(int j =0; j<8; j++)
    {
      for(int k =0; k<8; k++)
      {
        drawing[i][j][k] = maison[i][j][k+trans];
      }
    }        
  }
  delay(100);
  }
  
  for(int i =16; i>-56; i--)
  {
     ecriture(lettre_G, i, 0, 0, 0);
     ecriture(lettre_G, 7, i-8, 0, 1);
     
     ecriture(lettre_R, i+7, 0, 0, 0);
     ecriture(lettre_R, 7, i-1, 0, 1);
     
     ecriture(lettre_A, i+14, 0, 0, 0);
     ecriture(lettre_A, 7, i+6, 0, 1);
     
     ecriture(lettre_O, i+21, 0, 0, 0);
     ecriture(lettre_O, 7, i+13, 0, 1);
     
     ecriture(lettre_U, i+28, 0, 0, 0);
     ecriture(lettre_U, 7, i+20, 0, 1);
     
     ecriture(lettre_L, i+35, 0, 0, 0);
     ecriture(lettre_L, 7, i+27, 0, 1);
     
     ecriture(lettre_A, i+42, 0, 0, 0);
     ecriture(lettre_A, 7, i+34, 0, 1);
     
     ecriture(lettre_B, i+49, 0, 0, 0);
     ecriture(lettre_B, 7, i+41, 0, 1);
     
     delay(60);
     cube_refresh();
  }
  
  moveletter(lettre_G);
  moveletter(lettre_R);
  moveletter(lettre_A);
  moveletter(lettre_O);
  moveletter(lettre_U);
  moveletter(lettre_L);
  moveletter(lettre_A);
  moveletter(lettre_B);
  
  
}

void moveletter(boolean lettre[8][8])
{
  for(int i =8; i>=0; i--)
  {
     cube_refresh();
     ecriture(lettre, i, 0, 0, 0);
     delay(30);
     
  }
  delay(200);
  for(int i =0; i<8; i++)
  {
    cube_refresh();
     ecriture(lettre, 0, i, 0, 0);
     delay(40);
     
  }
  for(int i =7; i>=0; i--)
  {
    cube_refresh();
     ecriture(lettre, 0, i, 0, 0);
     delay(40);
     
  }
}

void draw_loop()
{
  draw();
}

void ecriture(boolean lettre[8][8], int posx, int posy, int posz, int direction)
{
  if(direction==0) // axe X, lettre tournée à gauche
  {
    if(posx>-8 && posx<8 && posy>=0 && posy <8 && posz>-8 && posz<8)
	{
	  for(int i = max(0,posx); i<min(8,(posx+8)); i++)
	  {
	    for(int j = max(0,posz); j<min(8,(posz+8)); j++)
		{
	      //drawing[i][posy][j] = lettre[i-posx][7+posz-j];
                drawing[i][posy][j] = lettre[7+posz-j][i-posx];
		}
	  }
	}
  }
  
  if(direction==1) // axe Y, lettre tournée à gauche
  {
    if(posx>=0 && posx<8 && posy>-8 && posy <8 && posz>-8 && posz<8)
	{
	  for(int i = max(0,posy); i<min(8,(posy+8)); i++)
	  {
	    for(int j = max(0,posz); j<min(8,(posz+8)); j++)
		{
	      drawing[posx][i][j] = lettre[7+posz-j][i-posy];
		}
	  }
	}
  }
}


void cube_refresh()
{
  for(int i = 0; i<8; i++)
  {
    for(int j = 0; j<8; j++)
    {
	  for(int k = 0; k<8; k++)
      {
	    drawing[i][j][k] = false;
	  }
	}
  }
}

void cube_arretes()
{
  for(int i = 0; i<8; i++)
  {
    drawing[0][0][i] = true;
	drawing[7][0][i] = true;
	drawing[0][7][i] = true;
	drawing[7][7][i] = true;
	//drawing[0][i][0] = true;
	//drawing[7][i][0] = true;
        for(int j = 0; j<8; j++)
        {
	  drawing[j][i][7] = true;
        }
	//drawing[7][i][7] = true;
	//drawing[i][0][0] = true;
	//drawing[i][7][0] = true;
	//drawing[i][0][7] = true;
	//drawing[i][7][7] = true;
  }
}

void face_porte()
{
  
 /*boolean porte[8][8] = {1,1,1,1,1,1,1,1,
  
                1,0,0,0,0,0,0,1,
				1,0,0,0,0,0,0,1,
				1,0,0,1,1,0,0,1,
				1,0,1,0,0,1,0,1,
				1,0,1,0,0,1,0,1,
				1,0,1,0,0,1,0,1,
				1,1,1,0,0,1,1,1};

drawing[0] = porte;*/
  /*for(int i =0; i<8; i++)
  {
  drawing[2][i][0] = true;
  drawing[2][i][1] = true;
  drawing[2][i][2] = true;
  drawing[2][i][3] = true;
  drawing[3][i][4] = true;
  drawing[4][i][4] = true;
  drawing[5][i][3] = true;
  drawing[5][i][2] = true;
  drawing[5][i][1] = true;
  drawing[5][i][0] = true;
  }*/
  
  drawing[2][0][0] = true;
  drawing[2][0][1] = true;
  drawing[2][0][2] = true;
  drawing[2][0][3] = true;
  drawing[3][0][4] = true;
  drawing[4][0][4] = true;
  drawing[5][0][3] = true;
  drawing[5][0][2] = true;
  drawing[5][0][1] = true;
  drawing[5][0][0] = true;
  
  
}

void draw()
{
  for(int lig = 0; lig<8; lig++)
  {
    

    for(int i=0; i<8; i++)
    {
      for(int j=0; j<8; j++)
      {
        if(drawing[i][j][lig]) place[inc[i][j]] |= data[i][j];
      }
    }
    digitalWrite(latchPin, LOW);
    for(int i=0; i<8; i++)
    {
      shiftOut(dataPin, clockPin, MSBFIRST,place[i]);
      place[i] = 0;
    }
    if(lig==0) digitalWrite(rangee[7],LOW);
    else digitalWrite(rangee[lig-1],LOW);
    digitalWrite(rangee[lig],HIGH);
    digitalWrite(latchPin, HIGH);
  }
}






//murs

void init_maison()
{

  for (int i=0; i<16; i++)
  {
    maison[0][0][i] = true;
    maison[0][7][i] = true;
    maison[7][0][i] = true;
    maison[7][7][i] = true;
  }


  // planchers

  for (int i=0; i<8; i++)
  {
    for (int j=0; j<8; j++)
    {
      maison[i][j][7] = true;
    }
  }

  for (int i=0; i<8; i++)
  {
    for (int j=0; j<8; j++)
    {
      maison[i][j][15] = true;
    }
  }


  // toit

  for(int i=0; i<8; i++)
  {
    trait(0,i,15,3,i,19);
    trait(7,i,15,4,i,19);
  }


  // fenêtre

  trait(2,0,9,5,0,9);
  trait(5,0,9,5,0,12);
  trait(5,0,12,2,0,12);
  trait(2,0,9,2,0,12);
  
  
  //porte
  maison[2][0][0] = true;
  maison[2][0][1] = true;
  maison[2][0][2] = true;
  maison[2][0][3] = true;
  maison[3][0][4] = true;
  maison[4][0][4] = true;
  maison[5][0][3] = true;
  maison[5][0][2] = true;
  maison[5][0][1] = true;
  maison[5][0][0] = true;
}


void trait(int xa, int ya, int za, int xb, int yb, int zb)
{
  int distancex, distancey, distancez;
  
  if(xa>xb) distancex = xa-xb;
  else distancex = xb-xa;
  
  if(ya>yb) distancey = ya-yb;
  else distancey = yb-ya;
  
  if(za>zb) distancez = za-zb;
  else distancez = zb-za;
  
  if(distancex >= distancey && distancex >= distancez)
  {
    if(xb>xa)
	{
	  for(int i = xa; i<=xb; i++)
	  {
	    maison[i][ya + (i-xa)*(yb-ya)/(xb-xa)][za + (i-xa)*(zb-za)/(xb-xa)] = true;
	  }
	}
	if(xb<xa)
	{
	  for(int i = xb; i<=xa; i++)
	  {
	    maison[i][yb + (i-xb)*(ya-yb)/(xa-xb)][zb + (i-xb)*(za-zb)/(xa-xb)] = true;
	  }
	}
  }
  
  if(distancey > distancex && distancey >= distancez)
  {
    if(yb>ya)
	{
	  for(int i = ya; i<=yb; i++)
	  {
	    maison[xa + (i-ya)*(xb-xa)/(yb-ya)][i][za + (i-ya)*(zb-za)/(yb-ya)] = true;
	  }
	}
	if(yb<ya)
	{
	  for(int i = yb; i<=ya; i++)
	  {
	    maison[xb + (i-yb)*(xa-xb)/(ya-yb)][i][zb + (i-yb)*(za-zb)/(ya-yb)] = true;
	  }
	}
  }
  
  if(distancez > distancex && distancez > distancey)
  {
    if(zb>za)
	{
	  for(int i = za; i<=zb; i++)
	  {
	    maison[xa + (i-za)*(xb-xa)/(zb-za)][ya + (i-za)*(yb-ya)/(zb-za)][i] = true;
	  }
	}
	if(zb<za)
	{
	  for(int i = zb; i<=za; i++)
	  {
	    maison[xb + (i-zb)*(xa-xb)/(za-zb)][yb + (i-zb)*(ya-yb)/(za-zb)][i] = true;
	  }
	}
  }
}

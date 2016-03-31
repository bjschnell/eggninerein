 /* To call the keyboard, first call:
 
 drawKeyBoard(false);

 Then call:
 
 readKeyBoard(bool SHIFT,bool PASSWORD,int X,int Y);

 The first two parameters are booleans. The first should always be false
 since it is handled internally when you push the shift button.
 The second corresponds to if you are typing a password and the font
 should be displayed as: "*******" the last two are the coordinates of
 where you want your writing to start.
 
 */
 char font[95] = {'1','2','3','4','5','6','7','8','9','0','q','w','e','r','t','y','u','i','o','p','a','s','d','f','g','h','j','k','l','z','x','c','v','b','n','m'};
  char upper_font[95] = {'1','2','3','4','5','6','7','8','9','0','Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', 'Z', 'X', 'C', 'V', 'B', 'N', 'M'};
  //'!','-','+','$','%','^','=','*','(',')','A','B','C','D','E','F','G','H','I','J'};
 // extern uint8_t BigFont[];
  extern uint8_t SmallFont[];

void drawKeyBoard(bool shift)
{
  
  myGLCD.setFont(SmallFont);
   /*Create Keyboard Matrix*/
 
  int i = 0;

  //Top two rows of Keyboard
  for(i=0; i<10; i++)
  {
    myGLCD.setColor(0, 0, 0);
  
    myGLCD.fillRoundRect (10+(i*30), 90, 30+(i*30), 113);
    myGLCD.fillRoundRect (10+(i*30), 120, 30+(i*30), 143);

    myGLCD.setColor(0, 0, 255);
  
    myGLCD.drawRoundRect (10+(i*30), 90, 30+(i*30), 113);
    myGLCD.drawRoundRect (10+(i*30), 120, 30+(i*30), 143);

    if(shift == false)
    {
      myGLCD.print(String(font[i]), 18+(i*30), 95);
      myGLCD.print(String(font[i+10]), 18+(i*30), 125);
    }
    else
    {
    
      
      myGLCD.print(String(upper_font[i]), 18+(i*30), 95);
      myGLCD.print(String(upper_font[i+10]), 18+(i*30), 125);
    }
    
  }


//Third row of keyboard
  for(i=0;i<9;i++)
  {
    
    if(i==0)
    {
      myGLCD.setColor(0,0,0);
      myGLCD.fillRoundRect(25+(i*30), 150, 45+(i*30), 173);
      myGLCD.setColor(0,0,255);
      myGLCD.drawRoundRect(25+(i*30), 150, 45+(i*30), 173);
    }
    else
    {
      myGLCD.setColor(0,0,0);
      myGLCD.fillRoundRect (25+(i*30), 150, 45+(i*30), 173);
      myGLCD.setColor(0,0,255);
      myGLCD.drawRoundRect (25+(i*30), 150, 45+(i*30), 173);
    }
    myGLCD.setColor(0,0,255);

    if(shift == false)
    {
      myGLCD.print(String(font[i+20]), 33+(i*30), 155);    
    }
    else
    {
      myGLCD.print(String(upper_font[i+20]), 33+(i*30), 155);
    }
  }

  for(i=0;i<7;i++)    //Fourth Row
  {
    
    if(i==0)
    {
      myGLCD.setColor(0,0,0);
      myGLCD.fillRoundRect(55+(i*30), 180, 75+(i*30), 203);
      myGLCD.setColor(0,0,255);
      myGLCD.drawRoundRect(55+(i*30), 180, 75+(i*30), 203);
    }
    else
    {
      myGLCD.setColor(0,0,0);
      myGLCD.fillRoundRect (55+(i*30), 180, 75+(i*30), 203);
      myGLCD.setColor(0,0,255);
      myGLCD.drawRoundRect (55+(i*30), 180, 75+(i*30), 203);
    }
    if(shift == false)
    {
      myGLCD.print(String(font[i+29]), 63+(i*30), 185);    
    }
    else
    {
      myGLCD.print(String(upper_font[i+29]), 63+(i*30), 185);
    }
  }
/*Create the Space Bar*/
  myGLCD.setColor(0,0,0);
  myGLCD.fillRoundRect(85,210,225,233);
  myGLCD.setColor(0,0,255);
  myGLCD.drawRoundRect(85,210,225,233);
  myGLCD.print("SPACE", 135,215);

/*Create the New Line Button*/
  myGLCD.setColor(0,0,0);
  myGLCD.fillRoundRect(265,210,300,233);
  myGLCD.setColor(0,0,255);
  myGLCD.drawRoundRect(265,210,300,233);
  myGLCD.print("\\n", 277,215);  

/*Create Symbols Button*/
  myGLCD.setColor(0,0,0);
  myGLCD.fillRoundRect(10,210,75,233);
  myGLCD.setColor(0,0,255);
  myGLCD.drawRoundRect(10,210,75,233);
  myGLCD.print("Symbols", 17,215);

/*Create Symbols Button*/
  myGLCD.setColor(0,0,0);
  myGLCD.fillRoundRect(235,210,255,233);
  myGLCD.setColor(0,0,255);
  myGLCD.drawRoundRect(235,210,255,233);
  myGLCD.print(".", 244,215);
  
/*Create Shift Button*/
  myGLCD.setColor(0,0,0);
  myGLCD.fillRoundRect(10,180,45,203);
  myGLCD.setColor(0,0,255);
  myGLCD.drawRoundRect(10,180,45,203);
  myGLCD.print(">>", 23,185);

  /*Create Back Button*/
  myGLCD.setColor(0,0,0);
  myGLCD.fillRoundRect(265,180,300,203);
  myGLCD.setColor(0,0,255);
  myGLCD.drawRoundRect(265,180,300,203);
  myGLCD.print("Back", 268,185);
}



void updateStr(int val, int x1, int y1, bool pass, char stCurrent[],int* stCurrentLen )
{
  if(pass == true)
  {
      char passString[20]="";
      if(val == 8)
      {
        if (*stCurrentLen > 0) {
          stCurrent[(*stCurrentLen)-1]=' ';
          stCurrent[*stCurrentLen]='\0';
          myGLCD.setColor(0, 0, 255);
          myGLCD.setFont(BigFont);
         // myGLCD.print(stCurrent, x1, y1);
          (*stCurrentLen)--;
          stCurrent[*stCurrentLen]='\0';
          for(int i = 0; i<*stCurrentLen;i++)
          {
            passString[i] = '*';
          }
          myGLCD.print(passString, x1, y1);
          
          myGLCD.setFont(SmallFont);
        }
        
      }
      else
      {
        if (*stCurrentLen<20)
        {
          stCurrent[*stCurrentLen]=val;
          stCurrent[(*stCurrentLen)+1]='\0';
          (*stCurrentLen)++;
          myGLCD.setColor(0, 0, 255);
          myGLCD.setFont(BigFont);
         // myGLCD.print(stCurrent, 180, 10);
         for(int i = 0; i<*stCurrentLen;i++)
          {
            passString[i] = '*';
          }
          myGLCD.print(passString, x1, y1);
          myGLCD.setFont(SmallFont);
        }
        else
        {
          myGLCD.setColor(255, 0, 0);
          myGLCD.print("BUFFER FULL!", CENTER, 70);
          delay(500);
          myGLCD.print("            ", CENTER, 70);
          delay(500);
          myGLCD.print("BUFFER FULL!", CENTER, 70);
          delay(500);
          myGLCD.print("            ", CENTER, 70);
          myGLCD.setColor(0, 255, 0);
        }
      }
    
  }
  else
  {
      if(val == 8)
      {
        if (*stCurrentLen > 0) {
          stCurrent[(*stCurrentLen)-1]=' ';
          stCurrent[*stCurrentLen]='\0';
          myGLCD.setColor(0, 0, 255);
          myGLCD.setFont(BigFont);
          myGLCD.print(stCurrent, x1, y1);
          (*stCurrentLen)--;
          stCurrent[*stCurrentLen]='\0';
          myGLCD.setFont(SmallFont);
        }
      }
      else
      {
        if (*stCurrentLen<20)
        {
          stCurrent[*stCurrentLen]=val;
          stCurrent[(*stCurrentLen)+1]='\0';
          (*stCurrentLen)++;
          myGLCD.setColor(0, 0, 255);
          myGLCD.setFont(BigFont);
          myGLCD.print(stCurrent, 180, 10);
          myGLCD.setFont(SmallFont);
        }
        else
        {
          myGLCD.setColor(255, 0, 0);
          myGLCD.print("BUFFER FULL!", CENTER, 70);
          delay(500);
          myGLCD.print("            ", CENTER, 70);
          delay(500);
          myGLCD.print("BUFFER FULL!", CENTER, 70);
          delay(500);
          myGLCD.print("            ", CENTER, 70);
          myGLCD.setColor(0, 255, 0);
        }
      }
  }
}


char* readKeyBoard(bool shift, bool password, int textX, int textY)
{
  int i;
  char stCurrent[20];
  int stCurrentLen = 0;
  while(true)
  {
    if(myTouch.dataAvailable())
    {
      myTouch.read();
      x = myTouch.getX();
      y = myTouch.getY();
  
      if(shift == false)
      {
        if ((y>=90) && (y<=113))  // Upper row
        {
          for(i = 0; i<10; i++)
          {
            if((x>=10+(i*30)) && (x<=30+(i*30)))
            {
              waitForIt(10+(i*30), 90, 30+(i*30), 113);
              updateStr(font[i],textX, textY, password, stCurrent, &stCurrentLen);
            }
          }
        }
        if ((y>=120) && (y<=143))  // Second row
        {
          for(i = 0; i<10; i++)
          {
            if((x>=10+(i*30)) && (x<=30+(i*30)))
            {
              waitForIt(10+(i*30), 120, 30+(i*30), 143);
              updateStr(font[i+10],textX, textY, password, stCurrent, &stCurrentLen);
            }
          }
        }

        if ((y>=150) && (y<=173))  // Third row
        {
          for(i = 0; i<9; i++)
          {
            if((x>=25+(i*30)) && (x<=45+(i*30)))
            {
              waitForIt(25+(i*30), 150, 45+(i*30), 173);
              updateStr(font[i+20],textX, textY, password, stCurrent, &stCurrentLen);
            }
          }
        }
        if ((y>=180) && (y<=203))  // Fourth row
        {
          for(i = 0; i<10; i++)
          {
            if((x>=55+(i*30)) && (x<=75+(i*30)))
            {
              waitForIt(55+(i*30), 180, 75+(i*30), 203);
              updateStr(font[i+29],textX, textY, password, stCurrent, &stCurrentLen);
            }
          }
        }

       //shift button pushed
       if ((y>=180) && (y<=203))
       {
          if((x>=10)&&(x<=45))
          {
            waitForIt(10, 180, 45, 203);
            if(shift == false)
            {
              shift = true;
              drawKeyBoard(true);
            }            
          }
        
       }

      

      //Back Button Pushed
      if ((y>=180) && (y<=203))
       {
          if((x>=265)&&(x<=300))
          {
            int t = 8;
            //waitForIt(265, 180, 300, 203);
            updateStr(t,textX, textY, password, stCurrent, &stCurrentLen);
          }
       }


       //Space Button Pushed
      if ((y>=210) && (y<=233))
       {
          if((x>=85)&&(x<=225))
          {
            waitForIt(85, 210, 225, 233);
            updateStr(' ',textX, textY, password, stCurrent, &stCurrentLen);
          }
       }

      //Enter Button Pushed
      if ((y>=210) && (y<=233))
       {
          if((x>=265)&&(x<=300))
          {
            waitForIt(265, 210, 300, 233);
            break;
          }
       }
      }
      else
      {

          if ((y>=90) && (y<=113))  // Upper row
        {
          for(i = 0; i<10; i++)
          {
            if((x>=10+(i*30)) && (x<=30+(i*30)))
            {
              waitForIt(10+(i*30), 90, 30+(i*30), 113);
              updateStr(upper_font[i],textX, textY, password, stCurrent, &stCurrentLen);
            }
          }
        }
        if ((y>=120) && (y<=143))  // Second row
        {
          for(i = 0; i<10; i++)
          {
            if((x>=10+(i*30)) && (x<=30+(i*30)))
            {
              waitForIt(10+(i*30), 120, 30+(i*30), 143);
              updateStr(upper_font[i+10],textX, textY, password, stCurrent, &stCurrentLen);
            }
          }
        }

        if ((y>=150) && (y<=173))  // Third row
        {
          for(i = 0; i<9; i++)
          {
            if((x>=25+(i*30)) && (x<=45+(i*30)))
            {
              waitForIt(25+(i*30), 150, 45+(i*30), 173);
              updateStr(upper_font[i+20],textX, textY, password, stCurrent, &stCurrentLen);
            }
          }
        }
        if ((y>=180) && (y<=203))  // Fourth row
        {
          for(i = 0; i<10; i++)
          {
            if((x>=55+(i*30)) && (x<=75+(i*30)))
            {
              waitForIt(55+(i*30), 180, 75+(i*30), 203);
              updateStr(upper_font[i+29],textX, textY, password, stCurrent, &stCurrentLen);
            }
          }
        }



        
        //shift button pushed
       if ((y>=180) && (y<=203))
       {
          if((x>=10)&&(x<=45))
          {
            waitForIt(10, 180, 45, 203);
            if(shift == true)
            {
              shift = false;
              drawKeyBoard(false);
            }            
          }
        
       }

       //Back Button Pushed
        if ((y>=180) && (y<=203))
       {
          if((x>=265)&&(x<=300))
          {
            int t = 8;
            //waitForIt(265, 180, 300, 203);
            updateStr(t,textX, textY, password, stCurrent, &stCurrentLen);
          }
       }

        //Space Button Pushed
      if ((y>=210) && (y<=233))
       {
          if((x>=85)&&(x<=225))
          {
            waitForIt(85, 210, 225, 233);
            updateStr(' ',textX, textY, password, stCurrent, &stCurrentLen);
          }
       }     
      }  
      }
    }
  return stCurrent;
}

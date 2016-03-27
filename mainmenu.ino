#include <UTFT.h>
#include <ITDB02_Touch.h>
#include <List.h>

// Smart beverage Dispenser 


// This code block is only needed to support multiple
// MCU architectures in a single sketch.
#if defined(__AVR__)
  #define imagedatatype  unsigned int
#elif defined(__PIC32MX__)
  #define imagedatatype  unsigned short
#elif defined(__arm__)
  #define imagedatatype  unsigned short
#endif
// End of multi-architecture block

#define FONT_SIZE 16

extern uint8_t BigFont[];

UTFT myGLCD(SSD1289, 38, 39, 40, 41); 
ITDB02_Touch myTouch(6, 5, 4, 3, 2);

int x, y;

// Draw a red frame while a button is touched
void waitForIt(int x1, int y1, int x2, int y2)
{
  myGLCD.setColor(255, 0, 0);
  myGLCD.drawRoundRect (x1, y1, x2, y2);
  while (myTouch.dataAvailable())
    myTouch.read();
  myGLCD.setColor(255, 255, 255);
  myGLCD.drawRoundRect (x1, y1, x2, y2);
}

class Button {
  public: 
    int x1, x2, y1, y2;
    String text; 
    int fontx, fonty;
    int buttontype;

  Button(int x1, int y1, int x2, int y2, String text) {
     this->x1 = x1;
     this->x2 = x2;
     this->y1 = y1;
     this->y2 = y2; 
     this->text = text;

     fontx = x1 + (((x2 - x1) / 2 ) - ((text.length() * (FONT_SIZE / 2)))); 
     fonty = y1 + (((y2 - y1) / 2 ) - (FONT_SIZE/2));
    
  }
  
  void display_button() {
    myGLCD.setColor(0, 0, 255);
    myGLCD.fillRoundRect (x1,y1, x2, y2);
    myGLCD.setColor(255, 255, 255);
    myGLCD.drawRoundRect (x1, y1, x2, y2);
    myGLCD.print(text, fontx, fonty);
  }

  void is_pressed(int x, int y) {
    if (( y >= y1 && y <= y2)) {
        if ((x>= x1 && x <= x2)) {
          waitForIt(x1,y1,x2,y2);
        }
      }
  }
};

class Screen {
  public: 
    String screenName;
    List<Button*>* buttonslist;
     
  Screen(String Name) { 
    this->screenName = Name;
    this->buttonslist = new List<Button*>();
  }

  void display_screen() { 
    Node<Button*>* curr = buttonslist->getHead();

    for (; curr; curr = curr->getNext()) {
      curr->getData()->display_button();
    }
  }

  void add_button(int x1,int y1,int x2,int y2, String text) {
    buttonslist->append( new Button(x1,y1,x2,y2,text));
  }

  void check(int x, int y) {
    Node<Button*>* curr = buttonslist->getHead();

    for (; curr; curr = curr->getNext()) {
      curr->getData()->is_pressed(x,y);
    }
  }
  
};


Button* beverage1;
Screen* main_menu;
Screen* currScreen;
void setup() {
  // Inital setup code of the 

  myGLCD.InitLCD(LANDSCAPE);
  myGLCD.clrScr();

  myTouch.InitTouch(LANDSCAPE);
  myTouch.setPrecision(PREC_MEDIUM);

  myGLCD.setFont(BigFont);
  myGLCD.setBackColor(0,0,255);

  main_menu = new Screen("Main menu");
  main_menu->add_button(5,5,245,50,"Phillips Beer");
  main_menu->add_button(5,55,245,95, "Smirnoff");
  main_menu->add_button(5,100,245,140, "Steamwhistle");
  main_menu->add_button(5,145,245,185, "Jagermeister");
  main_menu->add_button(170,190,310,230, "Options");
  main_menu->add_button(5,190,135,230, "Logout");
}

void loop() {
  // put your main code here, to run repeatedly:
  currScreen = main_menu;
  currScreen->display_screen();
  myGLCD.setBackColor (0, 0, 0);
  while (true) 
  {
    if (myTouch.dataAvailable()) {
      myTouch.read();
      x=myTouch.getX();
      y=myTouch.getY();

      currScreen->check(x,y);
    }
  }
}

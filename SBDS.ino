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
#define HEIGHT 240
#define WIDTH 320

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

class Screen;

List<Screen*>* screen_stack;

/*****************************************
 * Generic button class 
 * Used to go to different screens screen.
 *****************************************/
class GenericButton {
    public: 
      int x1, x2, y1, y2;
      String text; 
      int fontx, fonty;
      int buttontype;

    GenericButton(int x1, int y1, int x2, int y2, String text) {
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

    bool is_pressed(int x, int y) {
      if (( y >= y1 && y <= y2)) {
          if ((x>= x1 && x <= x2)) {
            waitForIt(x1,y1,x2,y2);
            return on_press();
          }
        }
        return false;
      }

    virtual bool on_press() = 0;
};

/*****************************************
 * Generic button class 
 * Used to go a different screen.
 * Inherits from GenericButton
 *****************************************/
class Button : public GenericButton {
public:
  Screen* next_screen;

  Button (int x1, int y1, int x2, int y2, String text, Screen* next_screen) : GenericButton(x1,y1,x2,y2,text) {
    this->next_screen = next_screen;
  }
  
  bool on_press() {
    screen_stack->push(next_screen);
    return true;
  }
};

/*****************************************
 * Back button class 
 * Used to go to the previous screen.
 * Inherits from GenericButton
 *****************************************/
class BackButton: public GenericButton {
public:
  BackButton(int x1, int y1, int x2, int y2, String text) : GenericButton(x1,y1,x2,y2,text) {}

  bool on_press() {
    screen_stack->pop();
    return true;
  }
};

/**************************************
 * Dispense Button Class
 * Dispenses the beverage based
 * on the volume specified.
 **************************************/
class DispenseButton: public GenericButton {
public:
  int dispensevol;
  Screen* next_screen;
  DispenseButton(int x1, int y1, int x2, int y2, String text, Screen* next_screen) : GenericButton(x1,y1,x2,y2,text) {
    this->next_screen = next_screen; 
  }

  bool on_press() {
    // Dispense the beverage based 
    screen_stack->push(next_screen);
    return true;
  }
};

/**************************************
 * Free pour Button Class
 * Dispenses the beverage based
 * on the volume specified.
 **************************************/
class FreePourButton: public GenericButton {
public:
  FreePourButton(int x1, int y1, int x2, int y2, String text) : GenericButton(x1,y1,x2,y2,text) {}

  bool on_press() {
    // Dispense the beverage based 
      while (myTouch.dataAvailable()) {
        myTouch.read();
        // send signal to pour the beverage
      }
      // send signal to stop pouring
     return false;
  }
};

/**************************************
 * Disable button class
 * Disables pouring for the current beverage
 * by sending a 
 **************************************/
class DisableButton: public GenericButton {
public:
  DisableButton(int x1, int y1, int x2, int y2, String text) : GenericButton(x1,y1,x2,y2,text) {}

  bool on_press() {
    // Stop the pouring process
    screen_stack->pop();
    return true;
  }
};

/**************************************
 * Screen Text Class
 * 
 * 
 **************************************/
class ScreenText {
  public:
    int x, y;
    String text;

  ScreenText(String input, int x, int y) {
    this->text = input;
    this->x = x;
    this->y = y;
  }

  void display_text() {
    myGLCD.setBackColor (0, 0, 0);
    myGLCD.print(text, x, y);
    myGLCD.setBackColor (0, 0, 255);
  }
  
};

/*****************************************
 * Screen class
 * Each new screen is treated as an object
 * When a button is pressed the current 
 * screen is updated to be that target screen
 ******************************************/
class Screen {
  public: 
    
    String screenName;
    List<GenericButton*>* buttonslist;
    List<ScreenText*>* screentext;
    
  Screen(String Name) { 
    this->screenName = Name;
    this->buttonslist = new List<GenericButton*>();
    this->screentext = new List<ScreenText*>();
  }

  void display_screen() { 
    Node<GenericButton*>* curr = buttonslist->getHead();
    Node<ScreenText*>* point = screentext->getHead();
    
    // Draw the text for the screen
    for (; point; point = point->getNext()) {
      point->getData()->display_text();
    }

    // Draw the buttons for the screen
    for (; curr; curr = curr->getNext()) {
      curr->getData()->display_button();
    }


  }

  void add_button(int x1,int y1,int x2,int y2, String text, Screen* next_screen) {
    buttonslist->append(new Button(x1,y1,x2,y2,text,next_screen));
  }

  void add_text(int x, int y, String text) {
    screentext->append(new ScreenText(text,x,y));
  }
  
  bool check(int x, int y) {
    Node<GenericButton*>* curr = buttonslist->getHead();

    for (; curr; curr = curr->getNext()) {
      if (curr->getData()->is_pressed(x,y)) {
        return true;
      }
    }
    return false;
  }
  
};

Screen* main_menu;
Screen* dispense_one;
Screen* dispense_two;
Screen* dispensing;
Screen* options_menu;
Screen* admin_menu;
Screen* beverage_options_menu;
Screen* user_profile_menu;
Screen* blank_screen;
Screen* currScreen;

void setup() {
  // Inital setup code of the 

  myGLCD.InitLCD(LANDSCAPE);
  myGLCD.clrScr();

  myTouch.InitTouch(LANDSCAPE);
  myTouch.setPrecision(PREC_MEDIUM);

  myGLCD.setFont(BigFont);
  myGLCD.setBackColor(0,0,255);

  screen_stack = new List<Screen*>();

  blank_screen = new Screen("Blank screen");
  blank_screen->buttonslist->append(new BackButton(5,5,245,50,"Back..."));
  
  admin_menu = new Screen("Admin menu");
  
  // *************************************************************
  // IMPORTANT NOTE: Target screens must be initalized before they
  //                 are used as the destination screen
  //************************************************************** 

  // Admin menu screen 
  int num_admin_buttons = 7;
  int admin_button_height = (HEIGHT - ((num_admin_buttons + 1) * 5))/num_admin_buttons;
  admin_menu->add_button(5,5,315,5+admin_button_height, "Create user", blank_screen);
  admin_menu->add_button(5,5+(5+admin_button_height),315,(5+admin_button_height)*2, "Create admin", blank_screen);
  admin_menu->add_button(5,5+(5+admin_button_height)*2,315,(5+admin_button_height)*3, "Remove user", blank_screen);
  admin_menu->add_button(5,5+(5+admin_button_height)*3,315,(5+admin_button_height)*4, "Revoke privileges", blank_screen);
  admin_menu->add_button(5,5+(5+admin_button_height)*4,315,(5+admin_button_height)*5, "Transfer ownership", blank_screen);
  admin_menu->add_button(5,5+(5+admin_button_height)*5,315,(5+admin_button_height)*6, "Reset password", blank_screen);
  admin_menu->buttonslist->append(new BackButton(170,5+(5+admin_button_height)*6,315,(5+admin_button_height)*7,"Back..."));

  // Beverage options screen
  beverage_options_menu = new Screen("Beverage options menu");
  beverage_options_menu->add_button(5,5,315,50, "Add beverage", blank_screen);
  beverage_options_menu->add_button(5,55,315,95, "Remove beverage", blank_screen);
  beverage_options_menu->add_button(5,100,315,140, "Beverage info", blank_screen);
  beverage_options_menu->buttonslist->append(new BackButton(170,190,315,230,"Back..."));

  // User profile options menu
  user_profile_menu = new Screen("User profile menu");
  user_profile_menu->add_button(5,5,315,50, "Consumption report", blank_screen);
  user_profile_menu->add_button(5,55,315,95, "View quotas", blank_screen);
  user_profile_menu->add_button(5,100,315,140, "Set quota", blank_screen);
  user_profile_menu->add_button(5,145,315,185, "Change password", blank_screen);
  user_profile_menu->buttonslist->append(new BackButton(170,190,315,230,"Back..."));

  // Main options menu screen
  options_menu = new Screen("Options menu");
  options_menu->add_button(5,5,315,50, "Admin Options", admin_menu);
  options_menu->add_button(5,55,315,95, "Beverage Options", beverage_options_menu);
  options_menu->add_button(5,100,315,140, "User Profile", user_profile_menu);
  options_menu->add_button(5,145,315,185, "Enable Party Mode", blank_screen);
  options_menu->buttonslist->append(new BackButton(170,190,315,230,"Back..."));

  // Dispensing beverage screen
  dispensing = new Screen("Dispensing beverage screen");
  String beverage1_text = "Phillips Beer";
  String dispenseMessage = "Now dispensing";
  String dispenseMessage2 = "your beverage!";
  dispensing->add_text( (((315 - 5) / 2 ) - ((beverage1_text.length() * (FONT_SIZE / 2)))) , 10, beverage1_text);
  dispensing->add_text( (((315 - 5) / 2 ) - ((dispenseMessage.length() * (FONT_SIZE / 2)))) , 75, dispenseMessage);
  dispensing->add_text( (((315 - 5) / 2 ) - ((dispenseMessage2.length() * (FONT_SIZE / 2)))) , 91, dispenseMessage2);
  dispensing->buttonslist->append(new DisableButton(70,140,(320-70),180, "Stop"));
  
  // Dispensing beverage type 1 screen
  dispense_one = new Screen("Dispense Beer beverage");
  dispense_one->add_text( (((315 - 5) / 2 ) - ((beverage1_text.length() * (FONT_SIZE / 2)))) , 10, beverage1_text);
  dispense_one->buttonslist->append(new DispenseButton(5,55,315,95, "8 0z", dispensing));
  dispense_one->buttonslist->append(new DispenseButton(5,100,315,140, "16 Oz", dispensing));
  dispense_one->buttonslist->append(new FreePourButton(5,145,315,185, "Free pour"));
  dispense_one->buttonslist->append(new BackButton(70,190,(320-70),230,"Back..."));

  // Dispensing beverage type 1 screen
  dispense_two = new Screen("Dispense Alchohol beverage");
  String beverage2_text = "Smirnoff";
  dispense_two->add_text( (((315 - 5) / 2 ) - ((beverage2_text.length() * (FONT_SIZE / 2)))) , 10, beverage2_text);
  dispense_two->buttonslist->append(new DispenseButton(5,55,315,95, "Single Shot", dispensing));
  dispense_two->buttonslist->append(new DispenseButton(5,100,315,140, "Double Shot", dispensing));
  dispense_two->buttonslist->append(new FreePourButton(5,145,315,185, "Free pour"));
  dispense_two->buttonslist->append(new DisableButton(170,190,315,230, "Disable"));
  dispense_two->buttonslist->append(new BackButton(5,190,150,230,"Back..."));
  
  // Setup the main menu screen and objects
  main_menu = new Screen("Main menu");
  main_menu->add_button(5,5,245,50,"Phillips Beer", dispense_one);
  main_menu->add_button(5,55,245,95, "Smirnoff", dispense_two);
  main_menu->add_button(5,100,245,140, "Steamwhistle", blank_screen);
  main_menu->add_button(5,145,245,185, "Jagermeister", blank_screen);
  main_menu->add_button(170,190,315,230, "Options", options_menu);
  main_menu->add_button(5,190,135,230, "Logout", blank_screen);

  
  
  screen_stack->push(main_menu);  // Set start screen.

}

void loop() {
  // put your main code here, to run repeatedly:
  myGLCD.clrScr();
  myGLCD.setBackColor(0,0,255);
  currScreen = screen_stack->getHead()->getData();
  currScreen->display_screen();
  myGLCD.setBackColor (0, 0, 0);
  while (true) 
  {
    if (myTouch.dataAvailable()) {
      myTouch.read();
      x=myTouch.getX();
      y=myTouch.getY();

      if(currScreen->check(x,y)){
        break;
      }
    }
  }
}
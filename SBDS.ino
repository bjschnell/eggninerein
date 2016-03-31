#include <UTFT.h>
#include <ITDB02_Touch.h>
#include <List.h>
#include <Beverage.h>
#include <Servo.h>

String username = "BILL";
String password = "BIRD";

Servo servo1;
Servo servo2;
//Set the starting positions of each servo
int position1 = 0;
int position2 = 180;

// Smart beverage Dispenser

#define FONT_SIZE 16
#define HEIGHT 240
#define WIDTH 320

enum DisplayMode { VISIBLE, HIDDEN };
DisplayMode keyboard_display_mode;

extern uint8_t BigFont[];
extern uint8_t SmallFont[];

UTFT myGLCD(SSD1289, 38, 39, 40, 41);
ITDB02_Touch myTouch(6, 5, 4, 3, 2);

class Screen;
class MainMenu;
class Form;
class RemoveBevScreen;
class Beverage;

int x, y;
Screen* login;
MainMenu* main_menu;
Screen* dispense_one;
Screen* dispense_two;
Screen* dispensing;
Screen* options_menu;
Screen* admin_menu;
Screen* beverage_options_menu;
Screen* user_profile_menu;
Screen* blank_screen;
Screen* currScreen;
Screen* add_beverage;
RemoveBevScreen* remove_beverage;
Screen* keyboard_screen;
Screen* confirm;

String keyboard_string = "";

String input_username = "";
String input_password = "";

Beverage* remove_bev;

bool bev_mod = false;

Form* current_form; // The current active form the user is using.

List<Screen*>* screen_stack;
List<Beverage*>* beverage_list;

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

/**************************************
   Screen Text Class


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
   Generic button class
   Used to go to different screens screen.
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
      fonty = y1 + (((y2 - y1) / 2 ) - (FONT_SIZE / 2));

    }

    virtual void display_button() {
      myGLCD.fillRoundRect (x1, y1, x2, y2);
      myGLCD.setColor(255, 255, 255);
      myGLCD.drawRoundRect (x1, y1, x2, y2);
      myGLCD.print(text, fontx, fonty);
    }

    /* IF YOU ARE CHANGING SCREENS RETURN
        TRUE, OTHERWISE FALSE
    */
    bool is_pressed(int x, int y) {
      if (( y >= y1 && y <= y2)) {
        if ((x >= x1 && x <= x2)) {
          waitForIt(x1, y1, x2, y2);
          return on_press();
        }
      }
      return false;
    }

    virtual bool on_press() = 0;
};

/*****************************************
   Generic button class
   Used to go a different screen.
   Inherits from GenericButton
 *****************************************/
class Button : public GenericButton {
  public:
    Screen* next_screen;

    Button (int x1, int y1, int x2, int y2, String text, Screen* next_screen) : GenericButton(x1, y1, x2, y2, text) {
      this->next_screen = next_screen;
    }

    void display_button() {
      myGLCD.setBackColor (0, 0, 255);
      myGLCD.setColor(0, 0, 255);
      GenericButton::display_button();
    }

    bool on_press() {
      screen_stack->push(next_screen);
      return true;
    }
};

/*****************************************
   Back button class
   Used to go to the previous screen.
   Inherits from GenericButton
 *****************************************/
class BackButton: public GenericButton {
  public:
    BackButton(int x1, int y1, int x2, int y2, String text) : GenericButton(x1, y1, x2, y2, text) {}

    void display_button() {
      myGLCD.setBackColor (0, 0, 255);
      myGLCD.setColor(0, 0, 255);
      GenericButton::display_button();
    }

    bool on_press() {
      screen_stack->pop();
      return true;
    }
};
/*****************************************
   Back button class
   Used to go to the previous screen.
   Inherits from GenericButton
 *****************************************/
class LoginButton: public GenericButton {
  public:
    Screen* next_screen;

    LoginButton(int x1, int y1, int x2, int y2, String text, Screen* next_screen) : GenericButton(x1, y1, x2, y2, text) {
      this->next_screen = next_screen;
    }

    void display_button() {
      myGLCD.setBackColor (0, 0, 255);
      myGLCD.setColor(0, 0, 255);
      GenericButton::display_button();
    }

    bool on_press() {
      if (input_username == username && password == input_password) {
        screen_stack->push(next_screen);
        keyboard_string = "";
        input_username = "";
        input_password = "";
        return true;
      }
      return false;
    }
};

/**************************************
   Dispense Button Class
   Dispenses the beverage based
   on the volume specified.
 **************************************/
class DispenseButton: public GenericButton {
  public:
    int dispensevol;
    Screen* next_screen;
    DispenseButton(int x1, int y1, int x2, int y2, String text, Screen* next_screen) : GenericButton(x1, y1, x2, y2, text) {
      this->next_screen = next_screen;
    }

    void display_button() {
      myGLCD.setBackColor (0, 0, 255);
      myGLCD.setColor(0, 0, 255);
      GenericButton::display_button();
    }

    bool on_press() {
      // Dispense the beverage based
      pinMode(8, OUTPUT);
      screen_stack->push(next_screen);
      return true;
    }
};

/**************************************
   Free pour Button Class
   Dispenses the beverage based
   on the volume specified.
 **************************************/
class FreePourButton: public GenericButton {
  public:
    FreePourButton(int x1, int y1, int x2, int y2, String text) : GenericButton(x1, y1, x2, y2, text) {}

    void display_button() {
      myGLCD.setBackColor (0, 0, 255);
      myGLCD.setColor(0, 0, 255);
      GenericButton::display_button();
    }

    bool on_press() {
      // Dispense the beverage based
      pinMode(8, OUTPUT);
      while (myTouch.dataAvailable()) {
        myTouch.read(); // THIS LINE MIGHT BE OPTIONAL
        // send signal to pour the beverage

      }
      pinMode(8, INPUT);
      // send signal to stop pouring
      return false;
    }
};

/**************************************
   Disable button class
   Disables pouring for the current beverage
   by sending a
 **************************************/
class DisableButton: public GenericButton {
  public:
    DisableButton(int x1, int y1, int x2, int y2, String text) : GenericButton(x1, y1, x2, y2, text) {}

    void display_button() {
      myGLCD.setBackColor (0, 0, 255);
      myGLCD.setColor(0, 0, 255);
      GenericButton::display_button();
    }

    bool on_press() {
      // Stop the pouring process
      screen_stack->pop();
      return true;
    }
};

/**************************************
 * Remove beverage button class
 * Removes the beverage that the buttons represents
 * by sending a 
 **************************************/
class RemoveButton: public GenericButton {
public:
  Beverage* beverageID;
  Screen* next_screen;
  
  RemoveButton(int x1, int y1, int x2, int y2, String text, Screen* next_screen, Beverage* bevID ) : GenericButton(x1,y1,x2,y2,text) {
    this->beverageID = bevID;
    this->next_screen = next_screen; 
  }

    void display_button() {
      myGLCD.setBackColor (0, 0, 255);
      myGLCD.setColor(0, 0, 255);
      myGLCD.fillRoundRect (x1,y1, x2, y2);
      myGLCD.setColor(255, 255, 255);
      myGLCD.drawRoundRect (x1, y1, x2, y2);
      myGLCD.print(text, fontx, fonty);
    }

  bool on_press() {
    remove_bev = this->beverageID;
    screen_stack->push(next_screen);
    return true;
  }
};


/**************************************
 * Confirm button class
 * Removes the beverage that the buttons represents
 * by sending a 
 **************************************/
class ConfirmButton: public GenericButton {
public:
  
  ConfirmButton(int x1, int y1, int x2, int y2, String text) : GenericButton(x1,y1,x2,y2,text) {}

    void display_button() {
      myGLCD.setBackColor (255, 0, 0);
      myGLCD.setColor(255, 0, 0);
      myGLCD.fillRoundRect (x1,y1, x2, y2);
      myGLCD.setColor(255, 255, 255);
      myGLCD.drawRoundRect (x1, y1, x2, y2);
      myGLCD.print(text, fontx, fonty);
    }

  bool on_press() {
    screen_stack->pop();
    beverage_list->remove(remove_bev);
    bev_mod = true;
    return true;
  }
};

/**************************************
   Form class
   Allows for user input via keyboard.

**************************************/
class Form : public GenericButton {
  public:
    int string_length;
    String* what_it_sets;
    DisplayMode display_mode;

    Form (int x1, int y1, int x2, int y2, String text, int string_length, String* what_it_sets, DisplayMode display_mode) : GenericButton(x1, y1, x2, y2, text) {
      this->string_length = string_length;
      this->what_it_sets = what_it_sets;
      this->display_mode = display_mode;
    }

    void display_button() {
      myGLCD.setBackColor (0, 0, 0);
      myGLCD.setColor(0, 0, 0);
      GenericButton::display_button();
    }

    bool on_press() {

      if (display_mode == HIDDEN) {
        keyboard_display_mode = HIDDEN;
      } else {
        keyboard_display_mode = VISIBLE;
      }

      screen_stack->push(keyboard_screen); // Always brings up keyboard screen.
      current_form = this;
      return true;
    }

    void set_text(String text) {
      if (text.length() > string_length) {
        this->text = text.substring(0, string_length);
      } else {
        this->text = text;
      }
      *what_it_sets = this->text;

      if (display_mode == HIDDEN) {
        this->text = "****";
      }

      fontx = x1 + (((x2 - x1) / 2 ) - ((this->text.length() * (FONT_SIZE / 2))));
      fonty = y1 + (((y2 - y1) / 2 ) - (FONT_SIZE / 2));
    }
};

/**************************************
   AlphanumButton Class
   Represents the numeric (0..9) and alphabetical
   ('A'..'Z') buttons on the keyboard.
**************************************/
class AlphanumKey : public GenericButton {
  public:
    char character;

    AlphanumKey(int x1, int y1, int x2, int y2, String text, char character) : GenericButton(x1, y1, x2, y2, text) {
      this->character = character;
    }
    // Override the display_button method to use green as the keyboard colour.
    void display_button() {
      myGLCD.setBackColor (0, 200, 0);
      myGLCD.setColor(0, 200, 0);
      GenericButton::display_button();
    }

    bool on_press() {
      keyboard_string += character;
      show_keyboard_text();
      return false;
    }

};

// Prints inputted text onto keyboard screen.
// If the keyboard display mode is set to HIDDEN,
// print 4 asterisks instead.
void show_keyboard_text() {
  if (keyboard_display_mode == HIDDEN) {

    myGLCD.setColor(255, 255, 255);
    String hidden_string = "";

    for (int i = 0; i < keyboard_string.length() - 1; i++) {
      hidden_string += '*';
    }
    hidden_string += keyboard_string.charAt(keyboard_string.length() - 1);

    myGLCD.print(hidden_string, 5, 5);
  } else {
    myGLCD.setColor(255, 255, 255);
    myGLCD.print(keyboard_string, 5, 5);
  }

}

/**************************************
   BackspaceKey Class
   Functions as a backspace key for the keyboard.
**************************************/
class BackspaceKey : public GenericButton {
  public:
    BackspaceKey(int x1, int y1, int x2, int y2, String text) : GenericButton(x1, y1, x2, y2, text) {}

    void display_button() {
      myGLCD.setBackColor (0, 200, 0);
      myGLCD.setColor(0, 200, 0);
      GenericButton::display_button();
    }

    bool on_press() {
      keyboard_string.remove(keyboard_string.length() - 1);
      myGLCD.print("                   ", 5, 5);
      show_keyboard_text();
      return false;
    }

};

/**************************************
   EnterKey Class
   Functions as a backspace key for the keyboard.
**************************************/
class EnterKey : public GenericButton {
  public:
    EnterKey(int x1, int y1, int x2, int y2, String text) : GenericButton(x1, y1, x2, y2, text) {}

    void display_button() {
      myGLCD.setBackColor (0, 255, 0);
      myGLCD.setColor(0, 200, 0);
      GenericButton::display_button();
    }

    bool on_press() {
      current_form->set_text(keyboard_string);
      keyboard_string = "";
      screen_stack->pop();
      return true;
    }
};

/*****************************************
   Screen class
   Each new screen is treated as an object
   When a button is pressed the current
   screen is updated to be that target screen
 ******************************************/
class Screen {
  public:

    String screenName;
    List<GenericButton*>* buttonslist;
    List<ScreenText*>* screentext;
    List<Form*>* formlist;

    Screen(String Name) {
      this->screenName = Name;
      this->buttonslist = new List<GenericButton*>();
      this->screentext = new List<ScreenText*>();
      this->formlist = new List<Form*>();
    }

    void display_screen() {
      Node<GenericButton*>* curr = buttonslist->getHead();
      Node<Form*>* form_pointer = formlist->getHead();
      Node<ScreenText*>* point = screentext->getHead();
      
      // Draw the text for the screen
      for (; point; point = point->getNext()) {
        point->getData()->display_text();
      }

      // Draw the buttons for the screen
      for (; curr; curr = curr->getNext()) {
        curr->getData()->display_button();
      }

      // Draw the forms on the screen
      for (; form_pointer; form_pointer = form_pointer->getNext()) {
        form_pointer->getData()->display_button();
      }
    }

    void add_button(int x1, int y1, int x2, int y2, String text, Screen* next_screen) {
      buttonslist->append(new Button(x1, y1, x2, y2, text, next_screen));
    }

    void add_text(int x, int y, String text) {
      screentext->append(new ScreenText(text, x, y));
    }

    bool check(int x, int y) {
      Node<GenericButton*>* curr = buttonslist->getHead();
      Node<Form*>* form_pointer = formlist->getHead();

      for (; curr; curr = curr->getNext()) {
        if (curr->getData()->is_pressed(x, y)) {
          return true;
        }
      }
      for (; form_pointer; form_pointer = form_pointer->getNext()) {
        if (form_pointer->getData()->is_pressed(x, y)) {
          return true;
        }
      }
      return false;
    }

};

/*****************************************
   Main Menu class
 ******************************************/
class MainMenu : public Screen {
  public:
    MainMenu(String Name) : Screen(Name) {};

    void createButtons() {
      Node<Beverage*>* curr = beverage_list->getHead();
      this->buttonslist = new List<GenericButton*>(); // WRITE A DELETE LATER YOU SCRUBS
      int height = 45;
      int width = 240;
      int padding = 5;
      int yloc = padding + height;

      for (; curr; curr = curr->getNext()) {
        if (curr->getData()->getType() == BEER) {
          main_menu->add_button(5, ((yloc - 45) + 5), 245, yloc, curr->getData()->getName(), dispense_one);
        } else {
          main_menu->add_button(5, ((yloc - 45) + 5), 245, yloc, curr->getData()->getName(), dispense_two);
        }
        yloc += height;
      }

      this->add_button(170, 190, 315, 230, "Options", options_menu);
      this->buttonslist->append(new BackButton(5, 190, 135, 230, "Logout"));
    }

};

/*****************************************
 * Remove beverage Screen
 * Automatically creates buttons based upon
 * how many beverages are contained in the list.
 ******************************************/
class RemoveBevScreen : public Screen {
  public:
   RemoveBevScreen(String Name) : Screen(Name) {};
   
  void createButtons() {
    Node<Beverage*>* curr = beverage_list->getHead();
    this->buttonslist = new List<GenericButton*>(); // WRITE A DELETE LATER YOU SCRUBS
    int height = 38; 
    int width = 315;
    int padding = 4;
    int startpos = 41 + padding;
    int yloc = padding + height;
    String removeText = "Select the beverage";
    String text2 = "you wish to remove";
    // Going to add text to the top of the list.
    this->add_text( (((315 - 5) / 2 ) - ((removeText.length() * (FONT_SIZE / 2)))) , 5, removeText);
     this->add_text( (((315 - 5) / 2 ) - ((text2.length() * (FONT_SIZE / 2)))) ,25 , text2);
    for (; curr; curr = curr->getNext()) {
        this->buttonslist->append(new RemoveButton(5,(((startpos+yloc)-38)+4),315,startpos+yloc,curr->getData()->getName(), confirm, curr->getData()));
      yloc += height;
    }
    
    this->buttonslist->append(new BackButton(170, 205, 315, 235,"Back"));
  }
  
};

void setup() {
  // Inital setup code of the

  myGLCD.InitLCD(LANDSCAPE);
  myGLCD.clrScr();

  myTouch.InitTouch(LANDSCAPE);
  myTouch.setPrecision(PREC_MEDIUM);

  myGLCD.setFont(BigFont);
  myGLCD.setBackColor(0, 0, 255);

  screen_stack = new List<Screen*>();
  beverage_list = new List<Beverage*>();

  beverage_list->append(new Beverage("Phillips Beer", BEER, 37.50, 5000, SLOT_1));
  beverage_list->append(new Beverage("BANANANAS", LIQUOR, 37.50, 5000, SLOT_2));
  beverage_list->append(new Beverage("Poonanny", BEER, 37.50, 5000, SLOT_3));
  beverage_list->append(new Beverage("Semen", LIQUOR, 37.50, 5000, SLOT_4));

  blank_screen = new Screen("Blank screen");
  blank_screen->buttonslist->append(new BackButton(5, 5, 245, 50, "Back..."));

  admin_menu = new Screen("Admin menu");

  // *************************************************************
  // IMPORTANT NOTE: Target screens must be initalized before they
  //                 are used as the destination screen
  //**************************************************************

  //**************************************************************
  // INITIALIZE KEYBOARD

  keyboard_screen = new Screen("Keyboard");
  {
    char alphanum_keys[36] = {'1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
                              'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', 
                              'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', 'Z', 
                              'X', 'C', 'V', 'B', 'N', 'M'
                             };

    int start_x = 5;
    int key_gap = 0;
    int key_width = (WIDTH - start_x - key_gap * 11) / 10;
    int start_y = 240 - key_width*4 - key_gap*4 - 5;
    int max_keyboard_x = WIDTH - key_width;
    int max_keyboard_y = HEIGHT - 5 - key_width;
    int key_count = 0;
    int alphanum_index = 0;

    int keyboard_x = start_x;
    int keyboard_y = start_y;

    for (; keyboard_y <= max_keyboard_y; keyboard_y += key_width + key_gap) {
      keyboard_x = start_x;
      for (; keyboard_x < max_keyboard_x; keyboard_x += key_width + key_gap, key_count++) {
        if (key_count == 29) {  // Backspace key
          keyboard_screen->buttonslist->append(new BackspaceKey(keyboard_x, keyboard_y, keyboard_x + key_width, keyboard_y + key_width, "<"));
          continue;
          
        }
        if (key_count == 37) {  // Spacebar
          keyboard_screen->buttonslist->append(new AlphanumKey(keyboard_x, keyboard_y, keyboard_x + key_gap + (key_width * 2), keyboard_y + key_width, "_", ' '));
          keyboard_x += key_width + key_gap;
          continue;
        }
        if (key_count == 38) {  // Enter button
          keyboard_screen->buttonslist->append(new EnterKey(keyboard_x, keyboard_y, keyboard_x + key_width, keyboard_y + key_width, "#"));
          continue;
        }
        keyboard_screen->buttonslist->append(new AlphanumKey(keyboard_x, keyboard_y, keyboard_x + key_width, keyboard_y + key_width,
                                             String(alphanum_keys[alphanum_index]), alphanum_keys[alphanum_index]));
        alphanum_index++;
      }
    }
  }
  //**************************************************************

  // Admin menu screen
  int num_admin_buttons = 7;
  int admin_button_height = (HEIGHT - ((num_admin_buttons + 1) * 5)) / num_admin_buttons;
  admin_menu->add_button(5, 5, 315, 5 + admin_button_height, "Create user", blank_screen);
  admin_menu->add_button(5, 5 + (5 + admin_button_height), 315, (5 + admin_button_height) * 2, "Create admin", blank_screen);
  admin_menu->add_button(5, 5 + (5 + admin_button_height) * 2, 315, (5 + admin_button_height) * 3, "Remove user", blank_screen);
  admin_menu->add_button(5, 5 + (5 + admin_button_height) * 3, 315, (5 + admin_button_height) * 4, "Revoke privileges", blank_screen);
  admin_menu->add_button(5, 5 + (5 + admin_button_height) * 4, 315, (5 + admin_button_height) * 5, "Transfer ownership", blank_screen);
  admin_menu->add_button(5, 5 + (5 + admin_button_height) * 5, 315, (5 + admin_button_height) * 6, "Reset password", blank_screen);
  admin_menu->buttonslist->append(new BackButton(170, 5 + (5 + admin_button_height) * 6, 315, (5 + admin_button_height) * 7, "Back..."));
  
  // Confirm beverage Screen
  confirm = new Screen("Confirm choice screen");
  String confirmationText = "Are you sure?";
  confirm->add_text(((315 - 5) / 2 ) - ((confirmationText.length() * (FONT_SIZE / 2))), 5, confirmationText);
  confirm->buttonslist->append(new ConfirmButton(5,100,150,140,"Confirm"));
  confirm->buttonslist->append(new BackButton(170, 100, 315, 140, "Cancel"));
  
  // Remove beverage screen initialization 
  remove_beverage = new RemoveBevScreen("Remove beverage screen");
  remove_beverage->createButtons();
  
  // Beverage options screen
  beverage_options_menu = new Screen("Beverage options menu");
  beverage_options_menu->add_button(5, 5, 315, 50, "Add beverage", blank_screen);
  beverage_options_menu->add_button(5, 55, 315, 95, "Remove beverage", remove_beverage);
  beverage_options_menu->add_button(5, 100, 315, 140, "Beverage info", blank_screen);
  beverage_options_menu->buttonslist->append(new BackButton(170, 190, 315, 230, "Back..."));

  // User profile options menu
  user_profile_menu = new Screen("User profile menu");
  user_profile_menu->add_button(5, 5, 315, 50, "Consumption report", blank_screen);
  user_profile_menu->add_button(5, 55, 315, 95, "View quotas", blank_screen);
  user_profile_menu->add_button(5, 100, 315, 140, "Set quota", blank_screen);
  user_profile_menu->add_button(5, 145, 315, 185, "Change password", blank_screen);
  user_profile_menu->buttonslist->append(new BackButton(170, 190, 315, 230, "Back..."));

  // Main options menu screen
  options_menu = new Screen("Options menu");
  options_menu->add_button(5, 5, 315, 50, "Admin Options", admin_menu);
  options_menu->add_button(5, 55, 315, 95, "Beverage Options", beverage_options_menu);
  options_menu->add_button(5, 100, 315, 140, "User Profile", user_profile_menu);
  options_menu->add_button(5, 145, 315, 185, "Enable Party Mode", blank_screen);
  options_menu->buttonslist->append(new BackButton(170, 190, 315, 230, "Back..."));

  // Dispensing beverage screen
  dispensing = new Screen("Dispensing beverage screen");
  String beverage1_text = "Dispense Beer";
  String dispenseMessage = "Now dispensing";
  String dispenseMessage2 = "your beverage!";
  dispensing->add_text( (((315 - 5) / 2 ) - ((beverage1_text.length() * (FONT_SIZE / 2)))) , 10, beverage1_text);
  dispensing->add_text( (((315 - 5) / 2 ) - ((dispenseMessage.length() * (FONT_SIZE / 2)))) , 75, dispenseMessage);
  dispensing->add_text( (((315 - 5) / 2 ) - ((dispenseMessage2.length() * (FONT_SIZE / 2)))) , 91, dispenseMessage2);
  dispensing->buttonslist->append(new DisableButton(70, 140, (320 - 70), 180, "Stop"));

  // Dispensing beverage type 1 screen
  dispense_one = new Screen("Dispense Beer beverage");
  dispense_one->add_text( (((315 - 5) / 2 ) - ((beverage1_text.length() * (FONT_SIZE / 2)))) , 10, beverage1_text);
  dispense_one->buttonslist->append(new DispenseButton(5, 55, 315, 95, "8 0z", dispensing));
  dispense_one->buttonslist->append(new DispenseButton(5, 100, 315, 140, "16 Oz", dispensing));
  dispense_one->buttonslist->append(new FreePourButton(5, 145, 315, 185, "Free pour"));
  dispense_one->buttonslist->append(new BackButton(70, 190, (320 - 70), 230, "Back..."));

  // Dispensing beverage type 1 screen
  dispense_two = new Screen("Dispense Alchohol beverage");
  String beverage2_text = "Dispense Alcohol";
  dispense_two->add_text( (((315 - 5) / 2 ) - ((beverage2_text.length() * (FONT_SIZE / 2)))) , 10, beverage2_text);
  dispense_two->buttonslist->append(new DispenseButton(5, 55, 315, 95, "Single Shot", dispensing));
  dispense_two->buttonslist->append(new DispenseButton(5, 100, 315, 140, "Double Shot", dispensing));
  dispense_two->buttonslist->append(new FreePourButton(5, 145, 315, 185, "Free pour"));
  dispense_two->buttonslist->append(new BackButton(70, 190, (320 - 70), 230, "Back..."));

  add_beverage = new Screen("Add beverage screen");

  
  // Setup the main menu screen and objects
  main_menu = new MainMenu("Main menu");
  main_menu->createButtons();

  login = new Screen("login");
  login->add_text( 5, 5, "Username:");
  login->add_text( 5, 81, "Password:");
  login->formlist->append(new Form(5, 26, 315, 71, "", 4, &input_username, VISIBLE));
  login->formlist->append(new Form(5, 102, 315, 147, "", 4, &input_password, HIDDEN));
  login->buttonslist->append(new LoginButton(5, 192, 315, 237, "Login", main_menu));

  //Define the controlling pins for each servo
  servo1.attach(9);
  servo2.attach(10);
  servo1.write(position1);

  //Define the controlling pin for the pump.
  pinMode(8, INPUT);
  digitalWrite(8, LOW);

  screen_stack->push(login);  // Set start screen.
}

/********************************
 * Clear all form data from the previous screen when exiting a screen
 * that is not the keyboard screen.
 ********************************/
void clear_forms() {
  Node<Screen*>* screen_node_pointer = screen_stack->getHead();
  Node<Form*>* form_pointer;
  
  if (screen_node_pointer->getData() != keyboard_screen) {
    
    if (screen_node_pointer = screen_node_pointer->getNext()){
      
      form_pointer = screen_node_pointer->getData()->formlist->getHead();

      for (; form_pointer; form_pointer = form_pointer->getNext()) {

        form_pointer->getData()->set_text("");
        // Workaround:
        // For now, explicitly empty the text field, otherwise any hidden fields
        // will fill their text field with the hidden field string.
        form_pointer->getData()->text = "";
        
      }
    }
  }
}

void loop() {
  // put your main code here, to run repeatedly
  myGLCD.clrScr();;
  myGLCD.setBackColor(0, 0, 255);
  currScreen = screen_stack->getHead()->getData();
  clear_forms();
  
  if (bev_mod == true) {
    main_menu->createButtons();
    remove_beverage->createButtons();
    bev_mod = false;
  }
  
  currScreen->display_screen();

  myGLCD.setBackColor (0, 0, 0);


  
  while (true)
  {
    if (currScreen == dispensing) {
      delay(3000);
      pinMode(8, INPUT);
      screen_stack->pop();
      break;
    }
    if (myTouch.dataAvailable()) {
      myTouch.read();
      x = myTouch.getX();
      y = myTouch.getY();

      if (currScreen->check(x, y)) {
        break;
      }
    }
  }

}

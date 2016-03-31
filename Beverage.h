#pragma once

enum Slot { SLOT_1, SLOT_2, SLOT_3, SLOT_4, SLOT_5 };
enum BeverageType { BEER, LIQUOR };

class Beverage {

	String name;
	BeverageType type;
	float price;
	int amount;
	Slot slot;
	
public:

    Beverage(String n, BeverageType t, float p, int a, Slot s){
        this->name = n;
        this->type = t;
        this->price = p;
        this->amount = a;
        this->slot = s;
    }
    
	void setName (String n) { name = n; }
	String getName () { return name; }
	
	void setType (BeverageType t) { type = t; }
	BeverageType getType () { return type; }
	
	void setPrice (float p) { price = p; }
	float getPrice () { return price; }
	
	void setAmount (int a) { amount = a; }
	int getAmount () { return amount ; }

	void setSlot (Slot s) { slot = s; }
	Slot getSlot () { return slot; }
	
    /*
	bool operator == (Beverage& b) {
		if (this->name == b.name && 
			this->type == b.type && 
			this->amount == b.amount) {
			return true;
		} else {
			return false;
		}
	}
	*/
};
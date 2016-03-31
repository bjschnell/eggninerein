#pragma once

enum Priority { USER, ADMIN, OWNER };

class User {

	String id;
	String password;
	int quota;
	Priority priority;
	
public:

	User (String id, String password, int quota, Priority priority) {
		this->id = id;
		this->password = password;
		this->quota = quota;
		this->priority = priority;
	}

	void setID (String n) { id = n; }
	String getID () { return id; }
	
	void setPassword (String p) { password = p; }
	
	void setQuota (int q) { quota = q; }
	int getQuota () { return quota; }
	
	void setPriority (Priority p) { priority = p; }
	Priority getPriority () { return priority; }

};
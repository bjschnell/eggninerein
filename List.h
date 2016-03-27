#pragma once

	template <typename S>
	class Node {

		S data;
		Node<S>* next;
	
	public:

		Node() { next = NULL; }

		Node(S d) {
			data = d; 
			next = NULL;
		}
	
		void setData (S d) { data = d; };
		S getData () { return data; };
	
		void setNext (Node<S>* n) { next = n; }
		Node<S>* getNext () { return next; }
	
	};

template <typename T>
class List {

	Node<T>* head;
	Node<T>* tail;
	int size;
	
public:

	List () { 
		head = NULL; 
		tail = NULL; 
		size = 0;
	}
	
	int getSize () { return size; }
	
	void append (T item);
	void remove (T item);
	void push (T item);
	void pop();
	
	void incrementSize () { size++; }
	void decrementSize () { size--; }
	
	void setHead (Node<T>* h) { head = h; }
	Node<T>* getHead () { return head; }
	
	void setTail (Node<T>* t) { tail = t; }
	Node<T>* getTail () { return tail; }

};

template <typename T>
void List<T>::append (T item) {

	Node<T>* n = new Node<T>(item);
	
	if (head == NULL) {
		head = n;
		tail = n;
	} else {
		tail->setNext(n);
		tail = n;
	}
	size++;
	
}

template <typename T>
void List<T>::push (T item) {

	Node<T>* n = new Node<T>(item);

	if (head == NULL) {
		tail = n;
	} else {
		n->setNext(head);
	}
	head = n;
	size++;

}

template <typename T>
void List<T>::pop () {

	Node<T>* temp = head;
	head = head->getNext();
	delete temp;
	size--;

}

template <typename T>
void List<T>::remove (T item) {

	if (head == NULL) return;

	Node<T>* curr = head;
	Node<T>* prev = NULL;
	
	for (; curr; prev = curr, curr = curr->getNext()) {
	
		if (curr->getData() == item) {
		
			if (curr == head) {
				
				head = head->getNext();
				
			} else {
			
				prev->setNext(curr->getNext());
			
			}
			
			delete curr;
			size--;
			
		}
	}
}
/*

Implementation of a doubly-linked list of Type

Assumes Type has a copy constructor
	ie.  Type::Type (Type&)

*/

#ifndef __DLISTH__

#define __DLISTH__

template <class Type>
class DoubleList
{
	public:
		typedef struct tagDLnode {
			struct tagDLnode *next, *prev;
			Type *item;
		} DLnode;

	private:
		DLnode *head;
		DLnode *current;
		int size;

	public:
		int InsertBefore (const Type &item);
		int InsertAfter (const Type &item);
		int Append (const Type &item);
		int Prepend (const Type &item);

		Type *Current (void) const;
		Type *Next (void) const;
		Type *Previous (void) const;
		Type *Head (void) const;
		Type *Tail (void) const;

		Type *RemoveCurrent (void);
		Type *RemoveHead (void);
		Type *RemoveTail (void);

		DoubleList& operator ++ (int);
		DoubleList& operator ++ ();
		DoubleList& operator -- (int);
		DoubleList& operator -- ();

		int Size (void) const {return size;};

		DoubleList ();
		~DoubleList ();
};

template <class Type>
DoubleList<Type>::DoubleList ()
{
	head = new DLnode;
   current = NULL;
	size=0;
}
template <class Type>
DoubleList<Type>::~DoubleList ()
{
	while (size) {
		current = head->next;
		head->next = head->next->next;
		delete current->item;
		delete current;
		size--;
	}
   delete head;
}

// Functions to insert into the list
// Entry: Type item to insert
// Return: int number of items in list
template <class Type>
int DoubleList<Type>::InsertBefore (const Type &item)
{
	if (!size) return Prepend(item);
	DLnode *temp = new DLnode;
	if (!temp) return size;
	if (current == head->next) return Prepend(item);

	temp->item = new Type (item);
	temp->next = current;
	temp->prev = current->prev;
	current->prev->next = temp;
	current->prev = temp;
	size++;
	return size;
}
template <class Type>
int DoubleList<Type>::InsertAfter (const Type &item)
{
	if (!size) return Prepend(item);
	DLnode *temp = new DLnode;
	if (!temp) return size;
	if (current == head->prev) return Append(item);

	temp->item = new Type (item);
	temp->next = current->next;
	temp->prev = current;
	current->next->prev = temp;
	current->next = temp;
	size++;
	return size;
}
template <class Type>
int DoubleList<Type>::Append(const Type &item)
{
	if (!size) return Prepend(item);
	DLnode *temp = new DLnode;
	if (!temp) return size;

	temp->item = new Type (item);
	temp->next = head->next;
	temp->prev = head->prev;
	head->prev->next = temp;
	head->next->prev = temp;
	head->prev = temp;
	size++;
	return size;
}
template <class Type>
int DoubleList<Type>::Prepend(const Type &item)
{
	DLnode *temp = new DLnode;
	if (!temp) return size;

	temp->item = new Type (item);
	if (!size) {
		temp->next = temp;
		temp->prev = temp;
		head->prev = temp;
		current = temp;
	}
	else {
		temp->next = head->next;
		temp->prev = head->prev;
		head->prev->next = temp;
		head->next->prev = temp;
	}
	head->next = temp;
	size++;
	return size;
}

// Peek functions
// Entry: none
// Return: Type* of specified item
//			(caller is resp for deleting it)
template <class Type>
Type* DoubleList<Type>::Current (void) const
{
	if (!size) return NULL;
	return new Type (*current->item);
}
template <class Type>
Type* DoubleList<Type>::Next (void) const
{
	if (!size) return NULL;
	return new Type (*current->next->item);
}
template <class Type>
Type* DoubleList<Type>::Previous (void) const
{
	if (!size) return NULL;
	return new Type (*current->prev->item);
}
template <class Type>
Type* DoubleList<Type>::Head (void) const
{
	if (!size) return NULL;
	return new Type (*head->next->item);
}
template <class Type>
Type* DoubleList<Type>::Tail (void) const
{
	if (!size) return NULL;
	return new Type (*head->prev->item);
}

// Item removal functions
// Entry: none
// Return: Type* of specified item
// 		(caller is resp for deleting it)

template <class Type>
Type* DoubleList<Type>::RemoveCurrent (void)
{
	if (!size) return NULL;
	Type *titem = current->item;
	size--;
	if (!size) {
		delete current;
		return titem;
	}

	if (head->next == current) head->next = current->next;
	if (head->prev == current) head->prev = current->prev;

	DLnode *temp = current;
	current->next->prev = current->prev;
	current->prev->next = current->next;
	current = current->next;
	delete temp;

	return titem;
}
template <class Type>
Type* DoubleList<Type>::RemoveHead (void)
{
	if (!size) return NULL;
	Type *titem = head->next->item;
	size--;
	if (!size) {
		delete head->next;
		return titem;
	}

	DLnode *temp = head->next;
	head->next->next->prev = head->prev;
	head->prev->next = head->next->next;
	head->next = head->next->next;
	delete temp;

	return titem;
}
template <class Type>
Type* DoubleList<Type>::RemoveTail (void)
{
	if (!size) return NULL;
	Type *titem = head->prev->item;
	size--;
	if (!size) {
		delete head->prev;
		return titem;
	}

	DLnode *temp = head->prev;
	head->prev->prev->next= head->next;
	head->next->prev = head->prev->prev;
	head->prev = head->prev->prev;
	delete temp;

	return titem;
}


// Current pointer movement functions
// Entry: none
// Return: DoubleList&
template <class Type>
DoubleList<Type>& DoubleList<Type>::operator ++ (int)
{
	if (size) current = current->next;
	return *this;
}
template <class Type>
DoubleList<Type>& DoubleList<Type>::operator ++ ()
{
	return (*this)++;
}
template <class Type>
DoubleList<Type>& DoubleList<Type>::operator -- (int)
{
	if (size) current = current->prev;
	return *this;
}
template <class Type>
DoubleList<Type>& DoubleList<Type>::operator -- ()
{
	return (*this)--;
}

#endif

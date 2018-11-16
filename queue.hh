#include <iostream> //for queue.display

struct node //node for Queue
{
  std::string* value;
  int size;
  node *next;
};

class Queue //Queue for evictor
{
public:
  node *head;
  Queue()
    {head = NULL;}
  
  void enqueue(std::string* val, uint32_t sz)
  {
    node *temp = new node;
    temp->value = val;
    temp->size = sz;
    temp->next = NULL;
    if(head == NULL)
      {head = temp;}
    else
    {
      node *traverse;
      traverse = head;
      while(traverse->next != NULL)
        {traverse = traverse->next;}
      traverse->next = temp;
    }
  }

  int rem(std::string val)
  {
    node *curr;
    node *prev;
    curr = head;
    int out;
    while((curr->next != NULL) && (*curr->value != val))
    {
      prev = curr;
      curr = curr->next;
    }
    if(*curr->value != val) {return 0;}
    else if (curr == head)
    {
      out = curr->size;
      head = curr->next;
      delete curr->value;
      delete curr;
      return out;
    }
    else
    {
      out = curr->size;
      prev->next = curr->next;
      delete curr->value;
      delete curr;
      return out;
    }
  }

  int pushback(std::string val) //this might work. haven't though about case where head->value == val very much
  {
    node *curr;
    node *prev;
    curr = head;
    while((curr->next != NULL) && (*curr->value != val))
    {
      prev = curr;
      curr = curr->next;
    }
    if(*curr->value != val) {return 0;}
    else if (curr->next == NULL) {return curr->size;}
    else
    {
      node *newcurr;
      newcurr = curr;
      while((newcurr->next) != NULL) {newcurr = newcurr->next;}
      prev->next = curr->next;
      curr->next = NULL;
      newcurr->next = curr;
      return curr->size;
    }
  }

  int ret_size(std::string val)
  {
    node *curr;
    curr = head;
    while((curr->next != NULL) && (*curr->value != val))
    {
      curr = curr->next;
    }
    if(*curr->value != val) {return 0;}
    return curr->size;
  }

  void dequeue()
  {
    node *save;
    save = head;
    head = save->next;
    delete save;
  }

  void display()
  {
    node *traverse;
    traverse = head;
    while(traverse != NULL)
    {
      std::cout << traverse->size << "\t" << traverse->value << "\t" << *traverse->value << std::endl;
      traverse = traverse->next;
    }
    std::cout << std::endl;
  }
};
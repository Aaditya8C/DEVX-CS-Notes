struct Node
{
    int data;
    Node *next;
    Node(int val) : data(val), next(nullptr) {} // constructor
};

Node *head = nullptr;     // declare head
head = new Node(x);       // create new node
head->next = new Node(y); // link next node
head = head->next;        // move to next
delete head;              // delete node

struct DNode
{
    int data;
    DNode *prev;
    DNode *next;
    DNode(int val) : data(val), prev(nullptr), next(nullptr) {} // constructor
};

DNode *head = new DNode(x); // create node
head->next = new DNode(y);  // link forward
head->next->prev = head;    // link backward
head = head->next;          // move forward
head = head->prev;          // move backward
delete head;                // delete node

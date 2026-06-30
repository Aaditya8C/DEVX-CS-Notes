stack<int> st;           // declare
st.push(x);              // push
st.pop();                // pop
int top = st.top();      // get top
bool empty = st.empty(); // check empty
int size = st.size();    // get size

queue<int> q;           // declare
q.push(x);              // enqueue
q.pop();                // dequeue
int f = q.front();      // front element
bool empty = q.empty(); // check empty
int size = q.size();    // get size

priority_queue<int> pq;                               // declare max-heap
pq.push(x);                                           // push
pq.pop();                                             // pop max
int top = pq.top();                                   // get max
bool empty = pq.empty();                              // check empty
int size = pq.size();                                 // get size
priority_queue<int, vector<int>, greater<int>> minpq; // min-heap

deque<int> dq;           // declare
dq.push_back(x);         // insert at back
dq.push_front(x);        // insert at front
dq.pop_back();           // remove from back
dq.pop_front();          // remove from front
int front = dq.front();  // get front element
int back = dq.back();    // get back element
bool empty = dq.empty(); // check empty
int size = dq.size();    // get size

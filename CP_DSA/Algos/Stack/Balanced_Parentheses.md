# Balanced Parentheses

## Purpose

> Determines whether a string of brackets is valid (every opening bracket has a matching closing bracket in the correct order).

---

## When to Use

- Validate bracket sequences: `()`, `[]`, `{}`
- Evaluate or parse expressions
- Remove invalid parentheses
- Minimum additions to make string valid

---

## Time Complexity

| Case    | Complexity |
|---------|------------|
| Best    | O(n)       |
| Average | O(n)       |
| Worst   | O(n)       |

**Space Complexity:** O(n) — stack holds at most n/2 opening brackets.

---

## Core Idea

- Traverse the string character by character.
- Push every opening bracket onto the stack.
- On encountering a closing bracket, check if the stack top is the matching opener.
  - If yes → pop (matched pair found).
  - If no → invalid.
- After traversal, the string is valid if and only if the stack is empty.

---

## Critical Code Walkthrough

### Matching Check

```cpp
if (ch == ')' && !st.empty() && st.top() == '(') st.pop();
else if (ch == ']' && !st.empty() && st.top() == '[') st.pop();
else if (ch == '}' && !st.empty() && st.top() == '{') st.pop();
else st.push(ch);
```

Combining the three closing-bracket cases into a helper avoids repetition:

```cpp
auto isMatch = [](char open, char close) {
    return (open == '(' && close == ')') ||
           (open == '[' && close == ']') ||
           (open == '{' && close == '}');
};
```

### Empty Stack Guard

Always check `!st.empty()` before `st.top()` — popping from an empty stack is undefined behavior.

---

## Code

```cpp
#include <bits/stdc++.h>
using namespace std;

bool isValid(string s) {
    stack<char> st;

    auto isMatch = [](char open, char close) {
        return (open == '(' && close == ')') ||
               (open == '[' && close == ']') ||
               (open == '{' && close == '}');
    };

    for (char ch : s) {
        if (ch == '(' || ch == '[' || ch == '{') {
            st.push(ch);
        } else {
            if (st.empty() || !isMatch(st.top(), ch))
                return false;
            st.pop();
        }
    }

    return st.empty();
}

// Minimum additions to make valid
int minAddToMakeValid(string s) {
    int open = 0, close = 0;
    for (char ch : s) {
        if (ch == '(') {
            open++;
        } else {
            if (open > 0) open--;  // matched
            else close++;          // unmatched ')'
        }
    }
    return open + close;  // unmatched '(' + unmatched ')'
}

// Count valid pairs using counter (no stack)
bool isValidCounter(string s) {
    int count = 0;
    for (char ch : s) {
        if (ch == '(') count++;
        else {
            if (count == 0) return false;
            count--;
        }
    }
    return count == 0;
}
```

---

## Notes

- `isValidCounter` only works for single bracket type `()`.
- For multiple bracket types always use a stack.
- Return `false` early as soon as a mismatch or empty-stack pop is detected.
- After traversal, an empty stack means all openers were matched.
- Minimum additions = unmatched openers + unmatched closers.

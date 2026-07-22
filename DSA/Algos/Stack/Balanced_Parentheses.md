# Balanced Parentheses

## Introduction

Given a string of brackets, determine whether it is valid — every opening bracket must have a corresponding closing bracket of the same type, in the correct order. The stack is the natural data structure because it mirrors the nesting structure of brackets.

---

## Intuition

Brackets are inherently nested — the most recently opened bracket must be the first one closed. This "last opened, first closed" property is exactly LIFO, which means a stack can track which bracket needs to be closed next. Push opening brackets; on a closing bracket, check if the top of the stack is its matching opener. If it ever isn't, the string is invalid. If the stack is empty at the end, everything matched.

---

## When to Use

- Validating bracket sequences `()`, `[]`, `{}`
- Expression parsing and evaluation
- Checking nested structure validity (HTML tags, code blocks)
- Finding minimum insertions/removals to make a sequence valid
- Longest valid parentheses substring

---

## Recognition Pattern

```
"Valid parentheses"
"Balanced brackets"
"Matching pairs"
"Nested structure"
"Every open has a close"
```
→ Think Stack + bracket matching.

---

## Complexity Analysis

| Case    | Time | Space |
|---------|------|-------|
| Best    | O(n) | O(1)* |
| Average | O(n) | O(n)  |
| Worst   | O(n) | O(n)  |

*O(1) space only for single bracket type using a counter.

---

## Core Idea

Traverse the string left to right. Push every opening bracket `(`, `[`, `{` onto the stack. When a closing bracket is encountered, the stack top must be its matching opener — pop and continue. If the stack is empty (no opener waiting) or the top doesn't match, return `false`. After full traversal, the string is valid if and only if the stack is empty (no unmatched openers remain).

---

## Visualization

```
s = "({[]})"

'(' → push         stack: ['(']
'{' → push         stack: ['(', '{']
'[' → push         stack: ['(', '{', '[']
']' → top='[' ✓    stack: ['(', '{']       pop
'}' → top='{' ✓    stack: ['(']            pop
')' → top='(' ✓    stack: []               pop

Stack empty → VALID ✓

─────────────────────────────────────────
s = "([)]"

'(' → push         stack: ['(']
'[' → push         stack: ['(', '[']
')' → top='[' ✗                            INVALID → return false
```

---

## Critical Code Explanation

### The Empty Stack Guard

```cpp
if (st.empty() || !isMatch(st.top(), ch))
    return false;
```

Both conditions must be checked before popping. `st.empty()` must come first — calling `st.top()` on an empty stack is undefined behavior. A closing bracket with nothing on the stack means there's an unmatched closer.

### Why Push Openers, Not Closers

```cpp
for (char ch : s) {
    if (ch == '(' || ch == '[' || ch == '{') {
        st.push(ch);              // remember what needs to be closed
    } else {
        if (st.empty() || !isMatch(st.top(), ch)) return false;
        st.pop();                 // matched — done with this pair
    }
}
```

You push openers because they represent "pending work" — they haven't found their match yet. When a closer arrives, it either resolves the most recent pending opener (pop) or reveals a mismatch (invalid). The stack at any point holds exactly the unresolved openers in the order they were opened.

### Counter Shortcut — Single Bracket Type Only

```cpp
int count = 0;
for (char ch : s) {
    if (ch == '(') count++;
    else if (--count < 0) return false;  // more ')' than '(' seen so far
}
return count == 0;
```

This works only when there is exactly one bracket type. For mixed brackets, a counter cannot tell whether `)` closes a `(` or a `[`.

---

## Code

```cpp
#include <bits/stdc++.h>
using namespace std;

// Multi-bracket validation
bool isValid(string s) {
    stack<char> st;

    auto matches = [](char open, char close) {
        return (open == '(' && close == ')') ||
               (open == '[' && close == ']') ||
               (open == '{' && close == '}');
    };

    for (char ch : s) {
        if (ch == '(' || ch == '[' || ch == '{') {
            st.push(ch);
        } else {
            if (st.empty() || !matches(st.top(), ch))
                return false;
            st.pop();
        }
    }
    return st.empty();
}

// Minimum insertions to make valid (single bracket type)
int minInsertions(string s) {
    int open = 0, close = 0;
    for (char ch : s) {
        if (ch == '(') {
            open++;
        } else {
            if (open > 0) open--;   // this ')' closes an existing '('
            else close++;           // unmatched ')' — needs an inserted '('
        }
    }
    return open + close;   // open: unmatched '(' need ')'; close: unmatched ')' need '('
}

// Longest valid parentheses substring
int longestValid(string s) {
    stack<int> st;
    st.push(-1);   // base index sentinel
    int maxLen = 0;

    for (int i = 0; i < (int)s.size(); i++) {
        if (s[i] == '(') {
            st.push(i);
        } else {
            st.pop();
            if (st.empty()) st.push(i);          // new base
            else maxLen = max(maxLen, i - st.top());
        }
    }
    return maxLen;
}
```

---

## Why It Works

A string of brackets is valid if and only if its bracket structure forms a set of properly nested pairs. A stack naturally represents nesting depth — each push increases nesting, each pop decreases it. The invariant maintained throughout traversal is: *the stack contains exactly the openers that have been seen but not yet matched, in the order they were opened*. Checking the top on each closer verifies that the innermost unmatched opener matches the current closer, which is the exact condition for proper nesting.

---

## Important Notes

- Always check `st.empty()` **before** `st.top()` — the order matters and the reverse causes undefined behavior.
- The counter approach (`count++`/`count--`) only works for a single bracket type. Using it for mixed brackets is incorrect.
- The sentinel `-1` trick in the longest valid substring avoids an edge case where the entire string is valid — the sentinel ensures `i - st.top()` always gives the correct length from the last invalid position.
- When the problem asks for the **minimum number of removals** to make valid, the answer is the same as `minInsertions` — the count of unmatched openers plus unmatched closers.
- For problems involving nested data (like deeply nested JSON or HTML), the same stack approach generalizes naturally.

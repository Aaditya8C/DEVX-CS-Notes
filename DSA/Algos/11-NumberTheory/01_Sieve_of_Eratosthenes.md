# Sieve of Eratosthenes

## Introduction

Finds all prime numbers up to n in O(n log log n) by iteratively marking multiples of each prime as composite. The most efficient algorithm for bulk prime generation.

---

## Intuition

Every composite number has at least one prime factor ≤ its square root. So for each prime p, start marking composites from p² (all smaller multiples were already marked by smaller primes). Only iterate up to √n — any number ≤ n with no prime factor ≤ √n is itself prime.

---

## When to Use

- Finding all primes up to n (n ≤ 10⁷ comfortably, 10⁸ with care).
- Pre-computing prime factorizations (smallest prime factor sieve).
- Euler's totient function computation.
- Any problem needing bulk prime queries.

---

## Recognition Pattern

```
"Find all primes up to N"
"Count primes"
"Prime factorization for many numbers"
"Is X prime?" for many queries
```
→ Sieve first, then O(1) queries.

---

## Complexity Analysis

| Case    | Time          | Space |
|---------|---------------|-------|
| Best    | O(n log log n)| O(n)  |
| Average | O(n log log n)| O(n)  |
| Worst   | O(n log log n)| O(n)  |

The time is effectively O(n) in practice. Space: O(n) for the boolean array.

---

## Visualization

```
n=20, isPrime initially all true, set 0,1 = false

p=2: mark 4,6,8,10,12,14,16,18,20
p=3: mark 9,15 (6,12,18 already marked; start from p²=9)
p=5: p²=25 > 20 → stop (all remaining unmarked are prime)

Primes: 2, 3, 5, 7, 11, 13, 17, 19
```

---

## Critical Code Explanation

### Start from p² Not 2p

```cpp
for (int j = i * i; j <= n; j += i)
    isPrime[j] = false;
```

All multiples of `p` smaller than `p²` have already been marked by smaller primes. Starting from `p²` avoids redundant work and is the key optimization. Note: `i * i` can overflow for large `i` — use `(long long)i * i <= n` to be safe.

### Outer Loop to √n

```cpp
for (int i = 2; (long long)i * i <= n; i++)
```

Any composite number ≤ n has a factor ≤ √n. So all composites ≤ n are marked by the time `i > √n`. The outer loop can stop there.

---

## Code

```cpp
#include <bits/stdc++.h>
using namespace std;

// Basic Sieve — O(n log log n)
vector<bool> sieve(int n) {
    vector<bool> isPrime(n + 1, true);
    isPrime[0] = isPrime[1] = false;
    for (int i = 2; (long long)i * i <= n; i++)
        if (isPrime[i])
            for (int j = i * i; j <= n; j += i)
                isPrime[j] = false;
    return isPrime;
}

// Smallest Prime Factor sieve — for fast factorization
vector<int> spfSieve(int n) {
    vector<int> spf(n + 1);
    iota(spf.begin(), spf.end(), 0);   // spf[i] = i initially
    for (int i = 2; (long long)i * i <= n; i++)
        if (spf[i] == i)               // i is prime
            for (int j = i * i; j <= n; j += i)
                if (spf[j] == j) spf[j] = i;
    return spf;
}

// Fast factorization using SPF
vector<int> factorize(int x, vector<int>& spf) {
    vector<int> factors;
    while (x > 1) {
        factors.push_back(spf[x]);
        x /= spf[x];
    }
    return factors;
}

// GCD and LCM
int gcd(int a, int b) { return b == 0 ? a : gcd(b, a % b); }
long long lcm(long long a, long long b) { return a / gcd(a, b) * b; }
```

---

## Why It Works

Every composite number `c` has at least one prime factor `p ≤ √c`. When the sieve reaches `p`, it marks `c` as composite (among other multiples of `p`). Every composite is marked by its smallest prime factor. Every prime is never marked (no prime can be a multiple of a smaller prime). After the sieve, all unmarked numbers ≥ 2 are prime.

---

## Important Notes

- **Overflow:** `i * i` overflows int for i ≥ ~46341. Use `(long long)i * i <= n`.
- For n ≥ 10⁸, use a segmented sieve to reduce memory from O(n) to O(√n) per segment.
- The **SPF (Smallest Prime Factor) sieve** enables O(log n) factorization for any number ≤ n — precompute once, factorize many numbers.
- `gcd(a, b)` via Euclidean algorithm is O(log(min(a,b))). C++17 provides `std::gcd` and `std::lcm`.
- **LCM overflow:** always compute `a / gcd(a,b) * b` (not `a * b / gcd(a,b)`) to avoid intermediate overflow.

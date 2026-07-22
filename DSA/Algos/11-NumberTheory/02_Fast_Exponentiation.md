# Fast Exponentiation (Binary Exponentiation)

## Introduction

Computes `a^n mod m` in O(log n) by repeatedly squaring — instead of multiplying `a` by itself `n` times.

---

## Intuition

Write `n` in binary. `a^13 = a^(1101₂) = a^8 · a^4 · a^1`. Each power of 2 is obtained by squaring the previous: `a → a² → a⁴ → a⁸`. Only the bits set in `n` contribute to the product. The algorithm iterates through bits of `n`, squaring the base each time and multiplying the result when the current bit is 1.

---

## When to Use

- `a^n mod m` for large `n` (combinatorics, modular inverse, Fermat's little theorem).
- Matrix exponentiation (Fibonacci in O(log n)).
- Any repeated exponentiation with mod.

---

## Complexity Analysis

| Case    | Time    | Space |
|---------|---------|-------|
| Best    | O(log n)| O(1)  |
| Average | O(log n)| O(1)  |
| Worst   | O(log n)| O(1)  |

---

## Visualization

```
pow(2, 13):  13 = 1101₂

bit=1 (2^0): result *= 2   → result=2,   base=4
bit=0 (2^1): skip,          result=2,   base=16
bit=1 (2^2): result *= 16  → result=32,  base=256
bit=1 (2^3): result *= 256 → result=8192 base=65536

2^13 = 8192 ✓
```

---

## Code

```cpp
#include <bits/stdc++.h>
using namespace std;

// a^n mod m
long long powMod(long long a, long long n, long long m) {
    long long result = 1;
    a %= m;
    while (n > 0) {
        if (n & 1) result = result * a % m;  // current bit is set
        a = a * a % m;                        // square the base
        n >>= 1;                              // next bit
    }
    return result;
}

// Modular inverse — a^(m-2) mod m (m must be prime, by Fermat's little theorem)
long long modInverse(long long a, long long m) {
    return powMod(a, m - 2, m);
}

// nCr mod p — using precomputed factorials and modular inverses
const int MOD = 1e9 + 7;
vector<long long> fact, inv_fact;

void precompute(int n) {
    fact.resize(n + 1);
    inv_fact.resize(n + 1);
    fact[0] = 1;
    for (int i = 1; i <= n; i++) fact[i] = fact[i-1] * i % MOD;
    inv_fact[n] = modInverse(fact[n], MOD);
    for (int i = n - 1; i >= 0; i--) inv_fact[i] = inv_fact[i+1] * (i+1) % MOD;
}

long long nCr(int n, int r) {
    if (r < 0 || r > n) return 0;
    return fact[n] * inv_fact[r] % MOD * inv_fact[n-r] % MOD;
}
```

---

## Important Notes

- Always `a %= m` before the loop to prevent overflow in the first multiplication.
- In C++, `(a * b) % m` can overflow if `a, b ≈ 10⁹` and m = 10⁹+7 — use `__int128` or ensure `a < m` always (which the `a %= m` and `a = a*a%m` lines guarantee).
- **Modular inverse** requires `m` to be prime. For non-prime moduli, use the extended Euclidean algorithm.
- For `nCr % m` with large n: precompute factorials and inverse factorials, then `nCr = fact[n] * inv_fact[r] * inv_fact[n-r] % m`.
- `1e9 + 7` is the standard competitive programming modulus — it's prime, and two numbers < 10⁹+7 multiplied fit in `long long` (max ≈ 10¹⁸, `long long` max ≈ 9.2 × 10¹⁸).

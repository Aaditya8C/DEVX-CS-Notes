# RAG — Retrieval-Augmented Generation

> **One-liner:** RAG = Give your LLM access to external knowledge *at query time*, so it answers with facts, not guesses.

---

## Why RAG?

| Problem with plain LLMs | How RAG solves it |
|---|---|
| **Hallucination** — model confidently makes things up | Grounds answers in retrieved, verifiable documents |
| **Knowledge cutoff** — training data has a fixed date | Fetches up-to-date documents from your own store |
| **Expensive retraining** — updating model knowledge costs a lot | Just update the document store, no retraining needed |
| **Data privacy** — sensitive data can't go into a public model | Documents stay in your private vector DB; never in the model |

---

## Core Idea

```
User Query → Retrieve relevant docs → Feed docs + query to LLM → Grounded Answer
```

The LLM doesn't "know" your data — it *reads* the most relevant pieces at the moment you ask.

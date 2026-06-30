# RAG — End-to-End Example

**Scenario:** A company builds an internal HR chatbot that answers questions about their leave policy PDF.

---

## Step-by-Step Walkthrough

### Phase 1 — Ingestion (One-time setup)

```
leave_policy.pdf
        │
        ▼
  [Extract text]
  "Employees are entitled to 20 days of paid leave per year..."
        │
        ▼
  [Chunk into paragraphs]
  Chunk 1: "Employees are entitled to 20 days..."
  Chunk 2: "Sick leave can be taken without prior approval..."
  Chunk 3: "Leave encashment is allowed up to 10 days..."
        │
        ▼
  [Embed each chunk]  ← using OpenAI / Gemini Embedding API
  Chunk 1 → [0.23, -0.87, 0.44, ...]
  Chunk 2 → [0.11,  0.92, -0.33, ...]
        │
        ▼
  [Store in Vector DB]  ← e.g., Chroma DB
  (vector + original text + metadata saved)
```

---

### Phase 2 — Retrieval (Every time a user asks)

```
User: "How many paid leave days do I get?"
        │
        ▼
  [Embed the query]
  Query → [0.21, -0.85, 0.41, ...]
        │
        ▼
  [Search Vector DB]
  Top match: Chunk 1 — "Employees are entitled to 20 days of paid leave..."
        │
        ▼
  [Build Prompt]
  ┌─────────────────────────────────────────────────┐
  │ Context: "Employees are entitled to 20 days..." │
  │ Question: "How many paid leave days do I get?"  │
  └─────────────────────────────────────────────────┘
        │
        ▼
  [LLM generates answer]
  "According to the policy, you are entitled to 20 days of paid leave per year."
```

---

## Why this works

- The LLM never had to be trained on this PDF
- Answer is grounded in the actual document → no hallucination
- Update the PDF → re-ingest → bot is instantly up to date

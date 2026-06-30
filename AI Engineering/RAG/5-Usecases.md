# RAG Use Cases

Real-world scenarios where RAG adds clear value.

---

## By Domain

| Domain | Use Case | Why RAG? |
|---|---|---|
| **Enterprise** | Internal knowledge base / HR bot | Private docs stay in your DB, not in the model |
| **Legal** | Contract analysis, case research | Needs exact citations from specific documents |
| **Healthcare** | Clinical guidelines assistant | Knowledge must stay current & accurate |
| **Finance** | Earnings report Q&A, compliance | Facts must be traceable to source |
| **Customer Support** | Product FAQ chatbot | Answers grounded in your own docs/manuals |
| **Education** | Study assistant over a textbook | Personalized Q&A over specific material |
| **Code** | Codebase Q&A, PR review assistant | Retrieves relevant files/functions at query time |

---

## Key Signal: When to Choose RAG

> Use RAG when the answer depends on **specific, private, or frequently changing data** that the base model doesn't have.

```
❌ Don't need RAG  →  "What is photosynthesis?"  (general knowledge)
✅ Need RAG        →  "What does our refund policy say?"  (your own doc)
✅ Need RAG        →  "What were our Q3 2025 sales numbers?"  (recent private data)
```

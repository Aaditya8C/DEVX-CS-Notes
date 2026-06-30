# RAG Architectures

Overview of the most popular RAG patterns, from simple to advanced.

---

## Architecture Comparison

```
Standard RAG
  Query → Embed → Vector Search → LLM
  └── Simplest. Good starting point.

Hybrid RAG
  Query → [Vector Search + Keyword Search] → Re-rank → LLM
  └── Better recall. Combines semantic + exact match (BM25).

RAG with Memory
  Query → [Vector Search + Conversation History] → LLM
  └── Remembers past turns. Good for chatbots.

Graph RAG
  Query → Graph DB (entities & relationships) → LLM
  └── Best for highly connected data (e.g., knowledge graphs, org charts).

Agentic RAG
  Query → Agent decides [search / summarize / compute / call tools] → LLM
  └── Most flexible. Agent picks the right tool per step.

Multimodal RAG
  Query (text/image) → Retrieve text + images + tables → LLM
  └── Handles PDFs with diagrams, slides, mixed media.

Self-RAG
  Query → LLM decides IF retrieval is needed → retrieve if yes → LLM
  └── Avoids unnecessary retrieval. Model critiques its own output.
```

---

## When to Use What?

| Architecture | Use when… |
|---|---|
| **Standard** | Simple Q&A over a document set |
| **Hybrid** | You need both keyword precision + semantic recall |
| **RAG + Memory** | Building a conversational assistant |
| **Graph RAG** | Data has rich relationships (not just flat docs) |
| **Agentic RAG** | Queries need multi-step reasoning or external tools |
| **Multimodal RAG** | Your knowledge base has images, tables, or slides |
| **Self-RAG** | You want the model to self-verify and reduce noise |

---

## Complexity Spectrum

```
Simple ──────────────────────────────────────────► Complex
  Standard → Hybrid → Memory → Multimodal → Graph → Agentic → Self-RAG
```

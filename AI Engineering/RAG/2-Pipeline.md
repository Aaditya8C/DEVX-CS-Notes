# RAG Pipeline

The pipeline has **two phases**: Ingestion (done once / on updates) and Retrieval (done per query).

---

## 1. Ingestion Pipeline

Runs **offline** to prepare your knowledge base.

```mermaid
flowchart TD
    A[/📄 Raw Documents\nPDFs, Web pages, Databases/]
    --> B[🔍 Extract\nParse & clean text]
    B --> C[✂️ Chunk\nSplit into paragraphs / sentences]
    C --> D[🔢 Embed\nEmbedding Model API]
    D --> E[(🗄️ Vector DB\nStore vectors + metadata)]

    style A fill:#4A90D9,color:#fff,stroke:#2c6fad
    style B fill:#7B68EE,color:#fff,stroke:#5a4fcf
    style C fill:#7B68EE,color:#fff,stroke:#5a4fcf
    style D fill:#7B68EE,color:#fff,stroke:#5a4fcf
    style E fill:#2ECC71,color:#fff,stroke:#27ae60
```

---

## 2. Retrieval Pipeline

Runs **online** for every user query.

```mermaid
flowchart TD
    Q[/💬 User Query/]
    --> EQ[🔢 Embed Query\nSame Embedding Model]
    EQ --> VDB[(🗄️ Vector DB\nSimilarity Search)]
    VDB --> RC[📋 Ranked Chunks\nTop-K relevant results]
    RC --> BP[🧩 Build Prompt\nQuery + Context]
    Q --> BP
    BP --> LLM[🤖 LLM\nGPT-4 / Gemini / Claude]
    LLM --> ANS[/✅ Grounded Answer/]

    style Q fill:#4A90D9,color:#fff,stroke:#2c6fad
    style EQ fill:#7B68EE,color:#fff,stroke:#5a4fcf
    style VDB fill:#2ECC71,color:#fff,stroke:#27ae60
    style RC fill:#7B68EE,color:#fff,stroke:#5a4fcf
    style BP fill:#E67E22,color:#fff,stroke:#ca6f1e
    style LLM fill:#E74C3C,color:#fff,stroke:#c0392b
    style ANS fill:#1ABC9C,color:#fff,stroke:#17a589
```

---

## Key Components & Tool Choices

| Component | What it does | Popular Tools |
|---|---|---|
| **Chunking** | Splits documents into digestible pieces | LangChain, LlamaIndex |
| **Embedding Model** | Converts text → numerical vectors | OpenAI `text-embedding-3`, Gemini Embeddings |
| **Vector DB** | Stores & searches vectors by similarity | Chroma DB, Pinecone, Weaviate, FAISS |
| **LLM** | Generates the final answer | GPT-4, Gemini, Claude |

---

## Chunking Strategies (Quick Reference)

| Strategy | Best for |
|---|---|
| Fixed-size (tokens) | Simple, fast baseline |
| Sentence / Paragraph | Preserves natural context |
| Semantic chunking | Groups by meaning, not just size |
| Recursive character | LangChain default, works well generally |

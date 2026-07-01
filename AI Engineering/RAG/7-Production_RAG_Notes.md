# Production RAG Notes

## Overview

A production Retrieval-Augmented Generation (RAG) system is more than just:

```
Documents → Embeddings → Vector DB → LLM
```

A real-world pipeline looks like:

```text
Raw Documents
      │
Cleaning / Parsing / OCR
      │
Chunking
      │
Embeddings
      │
Vector Database
      │
Hybrid Retrieval (Dense + Sparse)
      │
Reranking
      │
Context Compression (optional)
      │
LLM
      │
Evaluation & Monitoring
```

---

# 1. Chunking Strategies

## Fixed Chunking

Splits documents by a fixed number of tokens (e.g. 500).

### Example

```text
Chunk 1 : Tokens 1–500
Chunk 2 : Tokens 501–1000
Chunk 3 : Tokens 1001–1500
```

### Pros

- Very simple
- Fast indexing
- Easy to implement

### Cons

- Can split a topic in half
- Lower retrieval quality

### Use Cases

- Small applications
- Generic PDF chatbots

---

## Semantic Chunking

Splits whenever the meaning/topic changes.

### Example

```
Vacation Policy
---------------
...

Medical Insurance
-----------------
...

Remote Work
-----------
...
```

Each section becomes one chunk.

### Pros

- Better retrieval accuracy
- Natural context boundaries

### Cons

- Slower
- Requires embeddings during chunk creation

### Use Cases

- Documentation
- Research papers
- Knowledge bases

---

## Hierarchical Chunking

Creates multiple retrieval levels.

```
Book
 ├── Chapter
 │     ├── Section
 │     │      └── Paragraph
```

Retrieval searches:

```
Chapter
 ↓
Section
 ↓
Paragraph
```

### Pros

- Excellent recall
- Scales well for enterprise datasets

### Cons

- Complex implementation
- More metadata management

### Use Cases

- Enterprise search
- Large documentation portals

---

## Production Recommendation

| Scenario | Strategy |
|----------|----------|
| Small chatbot | Fixed |
| Documentation | Semantic |
| Enterprise | Hierarchical |

Recommended:

- Chunk Size: **300–800 tokens**
- Overlap: **10–20%**

---

# 2. Embedding Models

Embeddings convert text into vectors.

## OpenAI text-embedding-3-small

**Pros**

- Excellent retrieval
- Multilingual
- Fast
- Low cost API

**Cons**

- Cloud only

**Best For**

- SaaS products

---

## Nomic Embed

**Pros**

- Open source
- Local inference
- Good long-context support

**Best For**

- Offline deployments
- Private documents

---

## BGE (BAAI)

Examples

```
BAAI/bge-small-en
BAAI/bge-base-en
BAAI/bge-large-en
```

**Pros**

- State-of-the-art retrieval
- Local deployment
- Enterprise friendly

---

## Comparison

| Feature | OpenAI | Nomic | BGE |
|---------|---------|---------|---------|
| Local | ❌ | ✅ | ✅ |
| API | ✅ | ❌ | ❌ |
| Cost | Paid | Free | Free |
| Retrieval | Excellent | Very Good | Excellent |

---

# 3. Vector Databases

Stores embeddings for similarity search.

## Qdrant

Features

- Fast ANN search
- Metadata filtering
- Hybrid search
- Quantization
- Docker support

Example

```python
from qdrant_client import QdrantClient

client = QdrantClient("localhost", port=6333)

results = client.search(
    collection_name="docs",
    query_vector=query_vector,
    limit=5
)
```

---

## Weaviate

Features

- Hybrid Search
- Graph relationships
- REST API
- Cluster support

Best suited for enterprise deployments.

---

# 4. Retrieval

## Dense Search

Uses embeddings.

Example

```
Vacation Leave
↓

Paid Time Off
```

Semantic meaning is preserved.

---

## Sparse Search (BM25)

Uses keyword matching.

Example

```
ERR_CONNECTION_RESET
```

Exact keywords are matched.

---

## Hybrid Search

```
Dense Score
      +
Sparse Score
      ↓
Final Score
```

Industry standard because it combines semantic understanding with exact keyword matching.

---

## Reranking

Pipeline

```
Retrieve Top-20

↓

Reranker

↓

Top-5

↓

LLM
```

Popular rerankers

- Cohere Rerank
- FlashRank

FlashRank is local, lightweight and CPU-friendly.

---

# 5. Evaluation (RAGAS)

## Faithfulness

Measures whether the answer is supported by retrieved context.

Hallucination ⇒ Low Faithfulness

---

## Answer Relevancy

Measures whether the answer actually answers the user's question.

Correct fact ≠ Relevant answer.

---

## Context Recall

Measures whether retrieval fetched enough supporting information.

Poor retrieval ⇒ Low recall.

---

# Production Best Practices

- Use semantic or hierarchical chunking.
- Keep chunks around **300–800 tokens**.
- Store metadata (document_id, page, section, source, version).
- Use Hybrid Retrieval (Dense + BM25).
- Retrieve Top-20 → Rerank → Send Top-5 to the LLM.
- Version your embeddings.
- Cache embeddings and frequent queries.
- Use metadata filters for access control.
- Continuously evaluate with RAGAS.

---

# LangChain

## What is LangChain?

LangChain is an orchestration framework for building LLM applications.

It provides reusable building blocks for:

- Document Loading
- Chunking
- Embeddings
- Vector Databases
- Retrieval
- Prompt Templates
- LLM Calls
- Chains
- Agents
- Memory

Instead of writing each component manually, LangChain provides standardized abstractions.

---

## Example 1 — Document Loader

```python
from langchain_community.document_loaders import PyPDFLoader

loader = PyPDFLoader("company_policy.pdf")
docs = loader.load()
```

---

## Example 2 — Text Splitter

```python
from langchain_text_splitters import RecursiveCharacterTextSplitter

splitter = RecursiveCharacterTextSplitter(
    chunk_size=500,
    chunk_overlap=100
)

chunks = splitter.split_documents(docs)
```

---

## Example 3 — Embeddings

```python
from langchain_openai import OpenAIEmbeddings

embeddings = OpenAIEmbeddings(
    model="text-embedding-3-small"
)
```

---

## Example 4 — Vector Store (Qdrant)

```python
from langchain_qdrant import QdrantVectorStore

vectorstore = QdrantVectorStore.from_documents(
    chunks,
    embeddings,
    url="http://localhost:6333",
    collection_name="docs"
)
```

---

## Example 5 — Retriever

```python
retriever = vectorstore.as_retriever(
    search_kwargs={"k":5}
)

documents = retriever.invoke(
    "What is the leave policy?"
)
```

---

## Example 6 — Prompt Template

```python
from langchain_core.prompts import ChatPromptTemplate

prompt = ChatPromptTemplate.from_template(
    "Answer using only the given context.\n\n{context}\n\nQuestion:{question}"
)
```

---

## Example 7 — LLM

```python
from langchain_openai import ChatOpenAI

llm = ChatOpenAI(
    model="gpt-4.1-mini"
)
```

---

## Example 8 — Retrieval Chain

```python
from langchain.chains import RetrievalQA

qa = RetrievalQA.from_chain_type(
    llm=llm,
    retriever=retriever
)

response = qa.invoke({
    "query":"Explain leave policy"
})
```

---

## Why LangChain?

Without LangChain:

```
Load PDF
↓

Split

↓

Embed

↓

Store

↓

Retrieve

↓

Prompt

↓

LLM

↓

Parse Output
```

Everything is manual.

With LangChain:

- Standard interfaces
- Easily swap LLMs
- Easily change vector DBs
- Faster prototyping
- Large ecosystem

---

## When Should You Use It?

Use LangChain when:

- Building RAG systems
- Connecting multiple AI components
- Developing quickly

Avoid it when:

- Maximum performance is required
- Full control over every component is needed
- Your pipeline is extremely simple

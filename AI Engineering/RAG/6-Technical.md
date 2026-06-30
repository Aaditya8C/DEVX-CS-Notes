# Hands-on RAG Implementation Notes

Quick revision-friendly summary of the implemented RAG pipeline.

---

## 1. Ingestion Phase

Processes raw data files, splits them into manageable chunks, generates vector embeddings, and stores them in a local vector database.

### A. Document Loading
Loads multiple document formats using specific LangChain loaders (`langchain_community.document_loaders`):
*   **PDF:** `PyPDFLoader`
*   **TXT:** `TextLoader`
*   **CSV:** `CSVLoader`
*   **Word (.docx):** `Docx2txtLoader`
*   **Excel (.xlsx):** `UnstructuredExcelLoader`
*   **JSON:** `JSONLoader`

### B. Text Chunking
Splits long documents into smaller chunks to fit LLM context windows and retain semantic retrieval precision:
*   **Tool:** `RecursiveCharacterTextSplitter` (`langchain_text_splitters`)
*   **Parameters:** `chunk_size` (e.g., 1000 characters), `chunk_overlap` (e.g., 200 characters to preserve context transitions).
*   **Strategy:** Splitting recursively using hierarchical separators: `["\n\n", "\n", " ", ""]`.

### C. Embedding Generation
Converts textual chunks into dense numerical vectors representing semantic meaning:
*   **Model:** `all-MiniLM-L6-v2` (via `SentenceTransformer`).
*   **Input:** Text chunks (`chunk.page_content`).
*   **Output:** Vector array of shape `(num_chunks, 384)` (384 dimensions).

### D. Vector Storage & Local Persistence
Stores embeddings for fast similarity search and saves them locally:
*   **Vector Database:** `faiss` (`faiss.IndexFlatL2` - uses L2/Euclidean distance).
*   **Persistence Structure (inside `faiss_store/`):**
    1.  `faiss.index`: Binary file storing the raw vector embeddings.
    2.  `metadata.pkl`: A serialized Python pickle file storing the corresponding text chunk mapping (`{"text": chunk.page_content}`) to reconstruct context later.

---

## 2. Retrieval & Generation Phase

Retrieves relevant context based on a user query and uses an LLM to generate the final response.

### A. Query Embedding
*   The incoming user search query is converted into a vector using the **same** `SentenceTransformer` model (`all-MiniLM-L6-v2`).

### B. Vector Database Search
*   **Action:** Query vector is searched against the local `faiss.IndexFlatL2`.
*   **Result:** Retrieves the top $k$ closest vector indices and their distance scores.

### C. Context Reconstruction
*   **Action:** Uses retrieved indices to lookup the corresponding raw text chunks from the loaded `metadata.pkl` file.
*   **Output:** Combines the text snippets into a single cohesive string representing the **context**.

### D. Prompt Engineering & LLM Execution
*   **LLM Model:** `gemini-2.5-flash` via LangChain's `ChatGoogleGenerativeAI`.
*   **Prompt Structure:**
    ```text
    Summarize the following context for the query: '{query}'

    Context:
    {context}

    Summary:
    ```
*   **Execution:** The formatted prompt is sent to Gemini, which generates and returns the final answer.
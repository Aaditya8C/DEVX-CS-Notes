from src.vectorstore import FaissVectorStore
from src.data_loader import load_all_documents
from src.embedding import EmbeddingPipeline

from src.search import RAGSearch

if __name__ == "__main__":
    docs = load_all_documents("data")
    # chunks = EmbeddingPipeline().chunk_documents(docs)

    # chunkVectors = EmbeddingPipeline().embed_chunks(chunks)
    store = FaissVectorStore("faiss_store")
    store.build_from_documents(docs)
    store.load()
    
    print(store.query("What is depression?", top_k=3))


    rag_search = RAGSearch()
    query = "What is depression?"
    summary = rag_search.search_and_summarize(query, top_k=3)
    print("Summary: ", summary)
    
    # print(chunkVectors)
# Introduction to n8n

n8n is an open-source, node-based workflow automation tool that allows you to connect different applications, APIs, databases, and AI models to automate tasks and build workflows.

---

## 1. What is n8n?
n8n is a **next-generation workflow automation tool** (often described as a self-hostable, developer-friendly alternative to Zapier). It operates on a **node-based visual workflow editor** where each node represents a specific application trigger, API request, logic branch, or AI component. 

Unlike traditional tools:
*   **JSON-Native:** Data passing between nodes is structured as standard JSON objects or arrays, making it intuitive for developers.
*   **Code-Friendly:** You can run raw JavaScript or Python directly inside a workflow using Code nodes to transform data.
*   **AI Integration:** Features native LangChain integrations, allowing you to easily build AI Agents with memory, custom tools, and vector stores without writing complex boilerplate code.

---

## 2. Where is it Used? (Core Use Cases)
n8n is used in various fields including software engineering, marketing, sales operations, and product management:
*   **API & Tool Integration:** Syncing data between CRM systems (like Salesforce, HubSpot), databases (PostgreSQL, MongoDB), and communication platforms (Slack, Discord, Email).
*   **Data Enrichment & Scraping:** Scraping websites, parsing HTML, enriching lead data with AI, and saving results to Google Sheets or CRMs.
*   **AI-Powered Chatbots & Assistants:** Creating custom AI assistants (e.g., WhatsApp ordering bots, support agents) connected to live databases and tools.
*   **Custom Notifications & Alerting:** Monitoring server health, error tracking databases (like Sentry), and routing critical alerts to on-call developers.

---

## 3. Why Use n8n? (Key Advantages & Alternatives)

### Why Choose n8n?
1.  **Self-Hostable & Free:** You can run it on your local system, Docker, or server for free. There are no limits on the number of executions, unlike cloud platforms that charge per step.
2.  **No Data Leakage:** Because you can host it yourself on-premise, your sensitive business data stays completely private.
3.  **Powerful Logic Control:** Advanced branching (`If`, `Switch`, `Merge`, `Filter`), looping, and native JavaScript/Python runtime.
4.  **First-Class AI Support:** The `@n8n/n8n-nodes-langchain` suite lets you visually construct complex RAG (Retrieval-Augmented Generation) chains and autonomous AI agents.

### Alternatives
*   **Zapier:** The market leader. Very easy to use with thousands of integrations, but becomes extremely expensive at scale and offers limited custom coding features.
*   **Make (formerly Integromat):** Powerful visual builder with advanced mapping, but can get complex, isn't self-hostable, and charges by execution operations.
*   **Windmill / Retool:** Developer-focused platforms suited for scripts and custom internal UI tools, but less focused on quick drag-and-drop third-party API integration.
*   **Apache Airflow:** Designed for heavy ETL/data engineering pipelines; requires writing pure python and is overkill for simple app automation.

---

## 4. Key N8N Developer Concepts

> [!TIP]
> **Data Pinning Feature:**
> While developing and testing workflows, **pinning the response values** of a node makes that specific test data static and available to all subsequent nodes in the workflow. This avoids triggering live APIs (like sending actual emails or calling paid LLMs) repeatedly during the development phase.
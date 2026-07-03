# Agentic AI — Multi-Agent Patterns

> **Resource:** [YouTube — Agentic AI Deep Dive](https://www.youtube.com/watch?v=EsTrWCV0Ph4&t=2s)

---

## 1. Multi-Agent MCP Orchestration

### What is MCP?

**MCP (Model Context Protocol)** is a standardized protocol that allows AI agents to **connect to and control external tools, data sources, and other agents** — think of it as USB-C for AI agents.

**Multi-Agent MCP Orchestration** means multiple specialized agents are connected via MCP and coordinated by an orchestrator to complete complex tasks.

### Architecture

```
                    [Orchestrator Agent]
                          |
        ┌─────────────────┼─────────────────┐
        ▼                 ▼                 ▼
  [Research Agent]  [Coder Agent]   [Writer Agent]
     (web search)    (runs code)    (formats docs)
        │                 │                 │
     MCP Server       MCP Server        MCP Server
   (Brave Search)   (Python REPL)    (Google Docs)
```

### Router Decision Hub

The **Router** is a specialized agent (or logic layer) that decides **which agent or tool should handle a given request**.

```
User: "Summarize this PDF and send it via email"

Router Decision:
  1. Task = summarize PDF    → route to Document Agent
  2. Task = send email       → route to Email Agent
  3. Combine outputs         → Orchestrator merges results
```

**Why it matters:** Without a router, every agent sees every task and tries to handle it — wasteful and error-prone. The router enforces **specialization**.

### Practical Example

A customer support pipeline:
- **Router** classifies incoming queries: billing, technical, or general
- **Billing Agent** handles payment questions (connected to CRM via MCP)
- **Tech Agent** handles bug reports (connected to GitHub/Jira via MCP)
- **General Agent** handles FAQs (connected to docs via MCP)

---

## 2. Stochastic Multi-Agent Consensus

### What is it?

**Stochastic** = involving randomness/probability.

When you send the **same query to the same agent (or multiple agents) repeatedly**, you get **slightly different outputs each time** — because LLMs are probabilistic (temperature > 0). **Consensus** means aggregating these varied outputs to reach a more reliable answer.

### Why it works

A single LLM response can be confidently wrong. Multiple independent responses expose the **distribution of likely answers** — the consensus is more trustworthy.

```
Query: "What's the best database for this use case?"

Run 1: PostgreSQL
Run 2: PostgreSQL
Run 3: MongoDB
Run 4: PostgreSQL
Run 5: PostgreSQL

Consensus: PostgreSQL (4/5 votes) ✓
```

### Techniques

| Technique | Description |
|---|---|
| **Majority Vote** | Pick the most common answer |
| **Self-Consistency** | Run the same CoT prompt N times, pick the most consistent reasoning path |
| **LLM-as-Judge** | A separate "judge" agent reviews all outputs and picks the best |
| **Ensemble** | Multiple different models answer; outputs are merged |

### Practical Example

For a **high-stakes code review**:
1. Send the code to 5 agent instances simultaneously
2. Each flags different potential bugs
3. A judge agent merges findings, removes duplicates
4. Final report is far more comprehensive than any single pass

---

## 3. Agent Chat Rooms

### What is it?

An **Agent Chat Room** is a shared communication space where **multiple agents can post messages, read each other's outputs, and build on each other's work** — like a Slack channel, but for AI agents.

### Architecture

```
[Chat Room / Shared Message Bus]
        ↑              ↑              ↑
  Agent A          Agent B        Agent C
(Researcher)     (Analyst)      (Writer)

Agent A posts: "Found 3 relevant papers"
Agent B reads it and posts: "Here's the statistical analysis of those papers"
Agent C reads both and posts: "Draft report written based on A & B's work"
```

### Why it's powerful

- Agents **don't need to be explicitly chained** — they self-coordinate by reading the room
- Enables **emergent collaboration** — agents naturally divide labor
- New agents can be **dropped into the room** without restructuring the whole system

### Practical Example

> **Task:** Write a competitor analysis report
>
> - `ResearchAgent` searches the web and posts raw data to the chat room
> - `DataAgent` reads the raw data, computes metrics, posts structured tables
> - `WriterAgent` reads all posts and composes the final report
> - `ReviewerAgent` reads the draft and posts feedback
> - `WriterAgent` revises based on the feedback

All of this happens asynchronously in the shared chat room — no rigid sequential pipeline needed.

---

## 4. Debate & Collaboration Among Agents

### Two modes of multi-agent interaction

#### 🤝 Collaboration
Agents **work together** toward a shared goal, each contributing their specialized output.

```
Goal: Build a web app

Agent A (Frontend): Designs the UI
Agent B (Backend):  Builds the API
Agent C (DevOps):   Sets up deployment

Orchestrator: Integrates all outputs
```

#### ⚔️ Debate
Agents **argue opposing positions** to pressure-test ideas and arrive at a more robust answer.

```
Question: "Should we use microservices or a monolith?"

Agent A (Pro-Microservices): Argues for scalability, isolation
Agent B (Pro-Monolith):      Argues for simplicity, speed of development

Judge Agent: Evaluates arguments, gives a nuanced recommendation
```

### Why Debate is valuable

- Forces agents to **justify their reasoning**
- Surfaces **hidden assumptions and edge cases**
- Leads to **higher quality decisions** than any single agent could produce alone
- Mimics how human expert panels work

### Debate Flow Example

```
Round 1: Agent A states position → Agent B challenges it
Round 2: Agent A defends → Agent B refines its counter-argument
Round 3: Judge Agent evaluates both sides
Output:  Balanced decision with pros/cons clearly stated
```

### When to use Debate vs Collaboration

| Scenario | Pattern |
|---|---|
| Building something together | Collaboration |
| Evaluating a risky decision | Debate |
| Checking for bugs / flaws | Debate (adversarial review) |
| Producing a multi-part deliverable | Collaboration |
| Verifying factual claims | Consensus (stochastic) |

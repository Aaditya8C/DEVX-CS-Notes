---
agent: agent
description: Convert raw AI Engineering learning notes into well-structured repository Markdown notes.
---

Use the user's input as raw learning material for an AI Engineering topic.

Before writing, inspect the relevant directory and nearby notes to understand
the existing structure, numbering, style, terminology, and learning progression.

Analyze the user's material first and determine its natural conceptual groups.
Do not assume that one user prompt corresponds to one Markdown file.

Group **2–4 closely related concepts into one coherent file** where they belong
to the same learning unit. Split the material into multiple files when it
contains distinct stages, architectural layers, mechanisms, or implementation
concerns.

For example, a broad topic such as MCP may naturally become:

- `1-Intro.md` — motivation, what existed before MCP, what MCP is, why it exists
- `2-Architecture.md` — components, communication model, architecture and flows
- `3-Transports-and-Implementation.md` — STDIO/remote transports, implementation,
  configuration and practical setup

This is only an example. Infer the best grouping from the actual material rather
than forcing this structure onto every topic.

Avoid both extremes: do not put an entire broad topic into one oversized file,
and do not create tiny files for individual concepts that are better understood
together.

Each file should represent a **meaningful learning unit** that can be read and
revised independently while fitting naturally into the sequence of surrounding
notes.

Use the user's points as the primary content requirements. Correct inaccuracies,
organize the material, and add missing practical context according to the AI
Engineering instructions.

Analyze any referenced images, source code, configurations, examples, or
directories in the workspace before writing about them. When an image is
relevant to a section, embed it in the appropriate Markdown file using the
correct relative path. When code or configuration is referenced, inspect the
actual files and explain the implementation from the code rather than assumptions.

Create or update the required Markdown files **directly in the workspace**.
Choose descriptive filenames and preserve the repository's existing numbering
convention. Do not merely return generated notes in chat.

Before creating new files, check whether the concepts are already documented.
Extend existing notes when appropriate and avoid duplicate explanations.

Preserve the learning progression of the topic:

**Motivation / Fundamentals → Architecture / Mechanism → Implementation /
Advanced Concepts → Production Considerations**

Use this progression as guidance, not as a mandatory file structure.

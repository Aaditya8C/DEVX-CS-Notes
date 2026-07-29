# Transports, Implementation, and Cursor Setup

> MCP is not a normal Express-style server that listens on a port. In the common local setup, it behaves more like a process that exchanges messages over standard input and output.

---

## STDIO vs SSE

| Transport             | Best fit                            | How it works                                                            | Trade-off                                                    |
| --------------------- | ----------------------------------- | ----------------------------------------------------------------------- | ------------------------------------------------------------ |
| STDIO                 | Local desktop integrations          | Client launches the server process and sends messages over stdin/stdout | Simple and fast, but tied to the local machine               |
| SSE / Streamable HTTP | Remote or multi-machine deployments | Messages travel over HTTP streaming                                     | More deployment complexity, but better for networked systems |

### Why STDIO is common

STDIO is ideal for local tools because the host and server can be launched together as a child process.

The protocol is message-based, usually using JSON-RPC.

Examples of common request types include:

- `tools/list`: list available tools
- `tools/call`: invoke a tool
- `resources/list`: list accessible resources
- `prompts/list`: list prompt templates

A simple request might look like this:

```json
{
  "jsonrpc": "2.0",
  "id": 1,
  "method": "tools/list"
}
```

The server responds with a structured result over the same transport channel.

## Demo implementation in this workspace

The demo server in [demo-mcp/index.js](./demo-mcp/index.js) shows the basic pattern.

### What the code does

1. It creates an MCP server with a name and version.
2. It registers a tool called `getWeatherDataByCityName`.
3. It exposes that tool through a stdio transport.

The implementation uses:

- `@modelcontextprotocol/server` for the MCP runtime
- `zod` for validating tool input
- `StdioServerTransport` for the local transport

A shortened version of the flow looks like this:

```js
import { McpServer } from "@modelcontextprotocol/server";
import { StdioServerTransport } from "@modelcontextprotocol/server/stdio";

const server = new McpServer({
  name: "Weather Data Fetcher",
  version: "1.0.0",
});

server.registerTool(
  "getWeatherDataByCityName",
  { city: z.string() },
  async ({ city }) => {
    // return tool result
  },
);

const transport = new StdioServerTransport();
await server.connect(transport);
```

## How to run the demo locally

1. Open the demo folder:

   ```bash
   cd "AI Engineering/MCP/demo-mcp"
   ```

2. Install the dependencies:

   ```bash
   npm install
   ```

3. Run the server:
   ```bash
   node index.js
   ```

The process will stay alive and wait for MCP requests over stdio.

## Cursor setup using mcp.json

To connect this server to Cursor, create a workspace-level configuration file at the repository root:

```json
{
  "mcpServers": {
    "weather-data-fetcher": {
      "command": "node",
      "args": [
        "/home/aaditya/Aaditya/System Design/DEVX-CS-Notes/AI Engineering/MCP/demo-mcp/index.js"
      ]
    }
  }
}
```

### Precise steps

1. Create a folder named `.cursor` at the repo root if it does not already exist.
2. Save the JSON above as `.cursor/mcp.json`.
3. Reload or reopen Cursor so it picks up the new configuration.
4. Open the MCP settings or server list in Cursor and confirm that `weather-data-fetcher` is detected.
5. Start chatting with the agent and ask it to use the weather tool.

## When to use which transport

- Use STDIO when the server is local and you want the simplest setup.
- Use SSE or streamable HTTP when the server must be reachable from another machine or deployed remotely.

## Key takeaway

MCP is a protocol for capability exchange, not just a web server pattern. STDIO is the easiest local path; SSE becomes important when the integration must run remotely.

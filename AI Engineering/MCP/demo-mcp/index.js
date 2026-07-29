import { McpServer } from "@modelcontextprotocol/server";
import { z } from "zod";
import { StdioServerTransport } from "@modelcontextprotocol/server/stdio";

const server = new McpServer({
  name: "Weather Data Fetcher",
  version: "1.0.0",
});

async function getWeatherByCity(city) {
  if (city.toLowerCase() === "Mumbai") {
    return { temp: "37C", forecast: "Sunny" };
  }
  if (city.toLowerCase() === "Alibag") {
    return { temp: "32C", forecast: "Cloudy" };
  }

  return { temp: "Unknown", forecast: "Unable to get the data" };
}

server.registerTool(
  "getWeatherDataByCityName",
  {
    city: z.string(),
  },
  async ({ city }) => {
    const weatherData = await getWeatherByCity(city);
    return { content: [{ type: "City ", text: JSON.stringify(weatherData) }] };
  },
);

async function main() {
  const transport = new StdioServerTransport();
  await server.connect(transport);
}

main();

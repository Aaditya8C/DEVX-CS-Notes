# Docker

---

## What is Docker

**Definition:** Docker is a platform that packages an application along with all its dependencies into a standardized unit called a **container**, so it runs identically on any machine.

**Problem it solves:**

```
Developer Machine (Linux)          Teammate Machine (macOS)
--------------------------         ------------------------
OS: Linux                          OS: macOS
Node: v20                          Node: v18  ← mismatch
MongoDB: 4.2                       MongoDB: 6.0 ← mismatch
App: Works fine                    App: Breaks
```

With Docker, the entire environment is bundled and shipped as a container — both machines run the exact same thing regardless of the host OS.

---

## Docker Container

**Definition:** A running instance of a Docker image. It is an isolated process that contains the application + all its dependencies.

```
┌──────────────────────────────────┐
│           Docker Container       │
│                                  │
│  ┌──────────┐  ┌──────────────┐  │
│  │   App    │  │ Dependencies │  │
│  │(Node v20)│  │(Mongo 4.2,   │  │
│  │          │  │ npm pkgs...) │  │
│  └──────────┘  └──────────────┘  │
│                                  │
└──────────────────────────────────┘
```

**Characteristics:**
- **Portable** — runs the same on Linux, macOS, Windows, or any cloud.
- **Lightweight** — shares the host OS kernel; no full OS overhead per container.

**Use case:** Multiple containers can run on a single machine in isolation.

```
Host Machine
├── Container A  (Node v20 + Mongo 4.2)
├── Container B  (Python 3.11 + PostgreSQL)
└── Container C  (Java 17 + Redis)
```

---

## Docker Image

**Definition:** A read-only blueprint/template for creating containers. Like a class — you instantiate it to get containers (objects).

```
Docker Image
    │
    ├── docker run → Container 1
    ├── docker run → Container 2
    └── docker run → Container 3
```

### Image Commands

| Command | Description |
|---|---|
| `docker pull <image>` | Download image from Docker Hub |
| `docker images` | List all local images |
| `docker run <image>` | Create and start a container from image |
| `docker run -it <image>` | Run container in interactive terminal mode |
| `docker stop <container_name/id>` | Gracefully stop a running container |
| `docker start <container_name/id>` | Start a stopped container |
| `docker ps` | List running containers |
| `docker ps -a` | List all containers (including stopped) |
| `docker rm <container_name/id>` | Remove a container |
| `docker rmi <image_name/id>` | Remove an image |

---

## Docker Image Layers

Images are built in **read-only layers**. Each instruction in a Dockerfile adds a layer. Only the final container layer is writable.

```
┌─────────────────────────┐  ← Container Layer (Read/Write)
├─────────────────────────┤
│       Layer N           │  ← e.g., COPY app files
├─────────────────────────┤
│       Layer 2           │  ← e.g., RUN npm install
├─────────────────────────┤
│       Layer 1           │  ← e.g., RUN apt-get install
├─────────────────────────┤
│       Base Layer        │  ← e.g., FROM node:20
└─────────────────────────┘       (All Read-Only)
```

**Why layers matter:**
- Layers are **cached** — rebuilding only re-runs changed layers.
- Layers are **shared** — two images using the same base layer don't duplicate it on disk.

---

## Port Binding

**Problem:** Containers run in their own isolated network. To access a container from the host machine, you must bind a host port to a container port.

```
Host Machine
├── Port 8080 ──→ Container A (Mongo 4.2) : Port 3306
└── Port 5000 ──→ Container B (Mongo 6.0) : Port 3306
```

Two containers can both use port 3306 internally — they are isolated. The host exposes them on different ports.

### Port Commands

| Command | Description |
|---|---|
| `docker run -p 8080:3306 <image>` | Bind host port 8080 to container port 3306 |
| `docker run -p 5000:3306 <image>` | Bind host port 5000 to container port 3306 |

Format: `-p <host_port>:<container_port>`

---

## Docker Troubleshoot (Quick Reference)

| Issue | Command |
|---|---|
| View container logs | `docker logs <container_name/id>` |
| Exec into running container | `docker exec -it <container_name/id> /bin/bash` |
| Inspect container details | `docker inspect <container_name/id>` |
| Check resource usage | `docker stats` |
| View all containers + status | `docker ps -a` |

---

## Docker vs Virtual Machine

| Feature | Docker (Container) | Virtual Machine |
|---|---|---|
| Virtualizes | Application layer only | Application layer + OS kernel |
| OS | Shares host OS kernel | Full guest OS per VM |
| Boot time | Seconds | Minutes |
| Size | MBs | GBs |
| Isolation | Process-level | Full hardware-level |
| Overhead | Very low | High |

```
Docker                         Virtual Machine
──────────────────────         ──────────────────────
App A  |  App B                App A  |  App B
────────────────────           ───────────────────────
Docker Engine                  Guest OS | Guest OS
────────────────────           ───────────────────────
Host OS (kernel shared)        Hypervisor
────────────────────           ───────────────────────
Hardware                       Host OS
                               ───────────────────────
                               Hardware
```

---

## Docker Network

**Definition:** Docker networks allow containers to communicate with each other. By default, containers are isolated.

### Network Commands

| Command | Description |
|---|---|
| `docker network ls` | List all networks |
| `docker network create <name>` | Create a custom network |
| `docker run --network <name> <image>` | Run container in a specific network |
| `docker network inspect <name>` | Inspect a network |

**Default networks:** `bridge` (default), `host`, `none`.

**Use case:** Put your app container and DB container on the same network so they can communicate by container name.

---

## Docker Compose

**Definition:** A tool to define and run **multi-container** applications using a single `.yaml` file. Replaces running multiple `docker run` commands manually.

**Without Compose:**
```
docker run -p 27017:27017 mongo:4.2
docker run -p 3000:3000 --network my-net my-app
```

**With Compose (`docker-compose.yaml`):**
```yaml
version: '3'
services:
  mongo:
    image: mongo:4.2
    ports:
      - "27017:27017"

  app:
    image: my-app
    ports:
      - "3000:3000"
    depends_on:
      - mongo
```

### Compose Commands

| Command | Description |
|---|---|
| `docker compose -f <filename> up -d` | Start all services in detached mode |
| `docker compose -f <filename> down` | Stop and remove all containers |
| `docker compose -f <filename> logs` | View logs for all services |

---

## Dockerizing an App

**Flow:**

```
App Source Code → Dockerfile → docker build → Docker Image → docker run → Container
```

### Dockerfile Instructions

| Instruction | Description |
|---|---|
| `FROM <image>` | Base image to build on (e.g., `FROM node:20`) |
| `WORKDIR <path>` | Set working directory inside container |
| `COPY <src> <dest>` | Copy files from host into container |
| `RUN <command>` | Execute a command during image build (e.g., `RUN npm install`) |
| `EXPOSE <port>` | Document which port the container listens on |
| `ENV KEY=VALUE` | Set environment variables |
| `CMD ["cmd", "arg"]` | Default command to run when container starts |

**Example Dockerfile (Node.js app):**
```dockerfile
FROM node:20

WORKDIR /app

COPY package*.json ./
RUN npm install

COPY . .

EXPOSE 3000

CMD ["node", "server.js"]
```

### Build & Run Commands

| Command | Description |
|---|---|
| `docker build -t <image-name> .` | Build image from Dockerfile in current directory |
| `docker build -t my-app:1.0 .` | Build with a specific tag/version |
| `docker run -p 3000:3000 my-app` | Run the built image as a container |

---

## Docker Volumes

**Problem:** Containers are ephemeral — data written inside a container is lost when the container is removed.

**Definition:** Volumes are a mechanism to **persist data** outside the container's filesystem, on the host machine.

```
Host Machine
├── /var/lib/docker/volumes/my-vol/   ← Actual data stored here
│
Container
└── /data/db  ←─────────────────────── Mounted to host volume
```

**Example:** MongoDB data persisted across container restarts:

```
docker run -v my-vol:/data/db mongo:4.2
```

If the container is removed and recreated, data in `my-vol` is still there.

### Volume Commands

| Command | Description |
|---|---|
| `docker run -v <vol-name>:<container-path> <image>` | Mount a volume when running a container |
| `docker volume ls` | List all volumes |
| `docker volume create <name>` | Create a named volume |
| `docker volume rm <name>` | Remove a volume |
| `docker volume inspect <name>` | Inspect volume details (location on host, etc.) |

**Types of mounts:**

| Type | Syntax | Use Case |
|---|---|---|
| Named Volume | `-v vol-name:/container/path` | Persistent DB data |
| Bind Mount | `-v /host/path:/container/path` | Dev: sync source code |
| tmpfs Mount | `--tmpfs /container/path` | Temp in-memory storage |

# 🔹 What is a Data Lake?

A **data lake** is a centralized storage repository that allows you to store **structured, semi-structured, and unstructured data** at any scale.

### ✅ Key Points:

- **Raw storage**: stores data _as-is_ (without enforcing schema at write-time)
- **All data types**: CSV, JSON, images, logs, video, and more

---

# 🔹 What is Data Warehousing?

A **data warehouse** is a system optimized for **querying and reporting** large volumes of **structured data**.

---

# 🖼️ Databricks

## 🔹 What is a Data Lakehouse?

A **data lakehouse** is a modern data architecture that combines the best parts of a **data lake** and a **data warehouse**.

**From a data lake, it takes:**

- ✅ Ability to store all kinds of data (raw, structured, unstructured)
- ✅ Low cost and scalable storage (like AWS S3)

**From a data warehouse, it takes:**

- ✅ Fast performance for queries
- ✅ Data management features like transactions and schema control

### 📷 Diagrams

<img src="/images/d1.png" alt="Diagram 1" width="100%" />
<img src="/images/d4.png" alt="Diagram 2" width="100%" />
<img src="/images/d3.png" alt="Diagram 3" width="100%" />
<img src="/images/d6.png" alt="Diagram 4" width="100%" />
<img src="/images/d5.png" alt="Diagram 5" width="100%" />

---

# 🔷 Databricks Architecture

<img src="/images/d7.png" alt="Diagram 6" width="100%" />

---

---

# 🔷 Databricks Unity Catalogue

Unity Catalogue:-
unified governance, open source.
it's defined once and secure everything.

<img src="/images/d8.png" alt="Diagram 7" width="100%" />

---

# 📌 Summary Table

| Aspect             | Data Lake                                 | Data Warehouse                      |
| ------------------ | ----------------------------------------- | ----------------------------------- |
| **Data Type**      | Structured, semi-structured, unstructured | Structured only                     |
| **Schema**         | Schema-on-read                            | Schema-on-write                     |
| **Use Cases**      | Data science, ML, big data exploration    | BI reporting, dashboards, analytics |
| **Cost Structure** | Low-cost object storage                   | More expensive, optimized storage   |

---

# 🔷 Databricks Data Intelligence Platform

The **Databricks Data Intelligence Platform** is used to build, run, and manage **data + AI workflows** on cloud at scale. It’s widely used in companies for things like **ETL pipelines**, **data analysis**, **ML model training**, and **reporting** — all in one place.

---

## 🔹 What You Can Do on Databricks:

| Task                  | What Databricks Provides                               |
| --------------------- | ------------------------------------------------------ |
| 🚀 Run Data Pipelines | Schedule jobs that clean and transform data            |
| 📊 Run SQL Queries    | Use Databricks SQL to query large datasets like a pro  |
| 🧠 Train ML Models    | Use notebooks + MLflow for full ML lifecycle           |
| 🧼 Manage Raw Data    | Store and clean data using Delta Lake (with schema!)   |
| 📈 Create Dashboards  | Build live dashboards or connect to Tableau / Power BI |

---

## 🔹 Tech Behind the Scenes:

- **Delta Lake** → Gives data lake reliability (ACID transactions, schema control)
- **Apache Spark** → For distributed data processing
- **Notebooks** → Write Python, SQL, Scala, R in one interactive place
- **MLflow** → Manage ML models and experiments
- **Cloud** → Runs on AWS / Azure / GCP with object storage (like S3)

---

## 🔹 Example Practical Workflow:

```plaintext
1. Read raw data from S3 (CSV, JSON, logs)
2. Clean + transform using Spark code in a notebook
3. Write output to Delta Lake tables
4. Run SQL queries on Delta tables to generate insights
5. Build a live dashboard or trigger ML model training
```

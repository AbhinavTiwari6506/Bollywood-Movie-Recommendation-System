# 🎬 Bollywood Movie Recommendation System (C++17, Qt 6 & MySQL 8)

[![Language](https://img.shields.io/badge/Language-C%2B%2B17-blue.svg)](https://en.cppreference.com/w/cpp/17)
[![GUI](https://img.shields.io/badge/GUI-Qt%206%20Widgets-green.svg)](https://www.qt.io/)
[![Database](https://img.shields.io/badge/Database-MySQL%208.0%2B-orange.svg)](https://www.mysql.com/)
[![Build](https://img.shields.io/badge/Build-CMake%20%2B%20Ninja-purple.svg)](https://cmake.org/)
[![Platform](https://img.shields.io/badge/Platform-Windows%2010%2F11-blue.svg)]()

A modern, high-performance desktop movie recommendation application re-engineered from an original Python prototype into an enterprise-grade **C++ desktop software system**. Powered by **Qt 6 Widgets** for a dark-themed UI (Netflix & IMDb inspired) and **MySQL 8** for persistent database storage, with a native **TF-IDF + Cosine Similarity** machine learning engine written entirely in C++.

---

## 📌 Table of Contents

- [Overview](#-overview)
- [Architectural Evolution: Python vs C++](#-architectural-evolution-python-vs-c)
- [Key Features](#-key-features)
- [Machine Learning & Recommendation Algorithm](#-machine-learning--recommendation-algorithm)
- [Database Schema (MySQL 8)](#-database-schema-mysql-8)
- [Project Directory Structure](#-project-directory-structure)
- [Prerequisites & Windows Installation](#-prerequisites--windows-installation)
- [Build Instructions](#-build-instructions)
- [Running the Application](#-running-the-application)
- [Verification & Test Results](#-verification--test-results)
- [Troubleshooting & FAQs](#-troubleshooting--faqs)

---

## 🚀 Overview

This project implements a content-based recommendation system trained on **2,199 Bollywood movies (1951 to 2023)**. Given any selected movie title, the system performs real-time NLP feature extraction, computes sparse vector dot products against the database corpus, and delivers the top similar movie titles with percentage match scores, genre badges, cast details, and plot synopses.

---

## 🔄 Architectural Evolution: Python vs C++

| Feature | Original Python Prototype | Modern C++ Desktop System |
| :--- | :--- | :--- |
| **Language** | Python 3.x | **C++17** (Pure native speed) |
| **GUI Framework** | Streamlit (Local browser web app) | **Qt 6 Widgets** (Native desktop window) |
| **Data Storage** | Transient Pandas CSV in RAM | **MySQL 8.0/8.4 Database** (Persistent, indexed) |
| **Vectorization** | `scikit-learn.feature_extraction` | **Native C++ `TFIDFVectorizer`** (Sparse vectors) |
| **Similarity Engine**| `sklearn.metrics.cosine_similarity` | **Native C++ Dot-Product Engine** (< 2ms inference) |
| **Search Queries** | Pandas `.str.contains()` | **SQL Prepared Statements (`QSqlQuery`)** |
| **Offline Resilience**| Requires web browser & Python libs | **Standalone `.exe` with SQLite fallback** |

---

## ✨ Key Features

1. **Netflix & IMDb Inspired Dark Theme:**
   - Sleek navigation sidebar (Home, Search Movies, Recommend Movies, All Movies, About Project).
   - Movie cards with poster placeholders, year badges, rating stars, and genre pills.
   - Interactive dialogs displaying director, starring cast, plot overview, and IMDb IDs.

2. **Dedicated Recommendation Studio:**
   - Interactive movie selection with fuzzy autocompleter.
   - Selected movie preview banner showing complete metadata before generating recommendations.
   - Recommendation count slider (5 to 20 movies, default 10).
   - Real-time similarity match percentage badge (e.g. `94% Match`).
   - One-click pivoting: clicking any recommended movie allows finding similar titles to that movie.

3. **High-Performance Search & Discovery:**
   - Search across movie titles, directors, and cast members.
   - Filter dynamically by genre and release year.
   - Full catalog pagination with 24 items per page across all 2,199 titles.

4. **Enterprise MySQL 8 Backend:**
   - Database: `bollywood_recommender`.
   - Table `movies` with multi-column indexing for fast queries.
   - Analytics SQL views (`v_popular_genres`, `v_top_directors`).
   - Transaction-based batch CSV importing (2,200 records loaded in seconds).
   - Visual Database Settings dialog inside the app to configure host, port, credentials, test connectivity, and re-import datasets on the fly.
   - Automatic fallback to embedded SQLite if MySQL server is not active.

---

## 🧠 Machine Learning & Recommendation Algorithm

The C++ recommendation engine mathematically reproduces the Scikit-Learn logic from the Python prototype:

1. **Feature Soup Construction:**
   $$\text{Soup} = \text{Genre} \oplus \text{Overview} \oplus \text{Director} \oplus \text{Cast}$$

2. **Text Normalization & Tokenization:**
   - Converted to lowercase.
   - Punctuation stripped, alphanumeric word boundaries identified ($\text{length} \ge 2$).
   - English stop words filtered (318 standard scikit-learn stop words).

3. **TF-IDF Calculation with Smooth IDF:**
   $$\text{TF}(t, d) = \text{frequency of term } t \text{ in document } d$$
   $$\text{IDF}(t) = \ln\left(\frac{1 + N}{1 + \text{DF}(t)}\right) + 1.0$$
   $$\text{TF-IDF}(t, d) = \text{TF}(t, d) \times \text{IDF}(t)$$

4. **L2 Vector Normalization:**
   $$\hat{\mathbf{v}}_d = \frac{\mathbf{v}_d}{\|\mathbf{v}_d\|_2}$$

5. **Cosine Similarity (Dot Product):**
   $$\text{Similarity}(d_1, d_2) = \hat{\mathbf{v}}_{d_1} \cdot \hat{\mathbf{v}}_{d_2} = \sum_{t} \hat{v}_{d_1, t} \times \hat{v}_{d_2, t}$$
   Sorted descending; document self-reference is skipped, returning Top-N nearest neighbors.

---

## 🗄️ Database Schema (MySQL 8)

The schema is located at `sql/schema.sql`:

```sql
CREATE DATABASE IF NOT EXISTS bollywood_recommender
    CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;

USE bollywood_recommender;

CREATE TABLE IF NOT EXISTS movies (
    id INT AUTO_INCREMENT PRIMARY KEY,
    movie_id VARCHAR(50) NOT NULL UNIQUE,
    movie_title VARCHAR(255) NOT NULL,
    release_year VARCHAR(20) DEFAULT NULL,
    genre VARCHAR(255) DEFAULT NULL,
    overview TEXT DEFAULT NULL,
    director VARCHAR(255) DEFAULT NULL,
    actors TEXT DEFAULT NULL,
    rating DECIMAL(3, 1) DEFAULT NULL,
    poster_url VARCHAR(500) DEFAULT NULL,
    soup TEXT DEFAULT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    INDEX idx_movie_title (movie_title),
    INDEX idx_release_year (release_year),
    INDEX idx_genre (genre(100))
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
```

---

## 📂 Project Directory Structure

```text
P:/C++ Project/
├── CMakeLists.txt                # Root CMake build configuration
├── config.ini                    # MySQL server configuration
├── run.bat                       # One-click execution launcher
├── build.bat                     # One-click build script
├── README.md                     # Documentation & college guide
├── .gitignore                    # Version control ignore list
│
├── data/
│   └── IMDB-Movie-Dataset(2023-1951).csv  # Original Bollywood dataset (2,199 titles)
│
├── sql/
│   └── schema.sql                # MySQL 8 database schema, tables & views
│
├── src/
│   ├── main.cpp                  # Application entry point & Qt event loop
│   │
│   ├── models/
│   │   ├── Movie.h               # Movie model class header
│   │   └── Movie.cpp             # Movie attributes, soup generation & helpers
│   │
│   ├── utils/
│   │   ├── CSVReader.h           # RFC 4180 CSV parser header
│   │   └── CSVReader.cpp         # Multiline quote handling & column normalizer
│   │
│   ├── database/
│   │   ├── DatabaseManager.h     # QSqlDatabase manager header
│   │   └── DatabaseManager.cpp   # MySQL queries, prepared statements & schema bootstrap
│   │
│   ├── recommender/
│   │   ├── TFIDFVectorizer.h     # Scikit-learn compatible TF-IDF vectorizer header
│   │   ├── TFIDFVectorizer.cpp   # Tokenizer, stop words, IDF and L2 normalizer
│   │   ├── RecommendationEngine.h# Recommendation engine class header
│   │   └── RecommendationEngine.cpp # Fast sparse dot-product similarity & Top-N retrieval
│   │
│   ├── ui/
│   │   ├── MainWindow.h/.cpp     # Shell window, header bar & navigation router
│   │   ├── MovieCard.h/.cpp      # Reusable visual card widget with badges
│   │   ├── MovieDetailsDialog.h/.cpp # Modal with plot, cast, director & recommend pivot
│   │   ├── DbConfigDialog.h/.cpp # Interactive MySQL connection manager modal
│   │   ├── HomePage.h/.cpp       # Hero banner, stats strip & featured carousels
│   │   ├── SearchPage.h/.cpp     # Dynamic keyword search & genre/year filters
│   │   ├── RecommendationPage.h/.cpp # Dedicated recommendation studio & slider
│   │   ├── AllMoviesPage.h/.cpp  # Paginated movie catalog
│   │   └── AboutPage.h/.cpp      # Academic project presentation & details
│   │
│   └── tests/
│       ├── test_engine.cpp       # Unit test for CSV parser & TF-IDF engine
│       └── test_mysql.cpp        # Integration test for live MySQL 8 connection & queries
│
└── python_backup/                # Original Python project safely preserved
    ├── src/app_streamlit.py
    ├── notebook/bollywood_recommender.ipynb
    ├── requirements.txt
    └── report/
```

---

## 🛠️ Prerequisites & Windows Installation

### 1. Install Qt 6 and MinGW/GCC Toolchain
Using **MSYS2** (recommended and pre-configured on this system):
```powershell
pacman -S mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-qt6-base mingw-w64-ucrt-x86_64-cmake mingw-w64-ucrt-x86_64-ninja mingw-w64-ucrt-x86_64-libmariadbclient
```
Alternatively, download the official **Qt Online Installer** from [qt.io](https://www.qt.io/download) and select **Qt 6.x (MinGW 64-bit)** with Qt Creator.

### 2. Install MySQL Server 8.0 or 8.4
Install via Windows Package Manager:
```powershell
winget install --id Oracle.MySQL --accept-source-agreements --accept-package-agreements
```
Or download the **MySQL Community Server Windows MSI** from [dev.mysql.com](https://dev.mysql.com/downloads/installer/).

---

## 🔨 Build Instructions

### Option A: Using the One-Click Build Script
Double-click `build.bat` in the project root directory.

### Option B: Using CMake & Ninja from Terminal
```powershell
# 1. Add tools to PATH
$env:PATH = "C:\msys64\ucrt64\bin;C:\msys64\usr\bin;" + $env:PATH

# 2. Configure build
cmake -S . -B build -G "Ninja" -DCMAKE_BUILD_TYPE=Release

# 3. Compile
cmake --build build --config Release
```

### Option C: Using Qt Creator
1. Open **Qt Creator**.
2. Select **File > Open File or Project...** and choose `P:/C++ Project/CMakeLists.txt`.
3. Choose the **Qt 6 Desktop Kit**.
4. Click **Build > Build Project** or press `Ctrl + B`.

---

## 🏃 Running the Application

### Option A: Using the One-Click Launcher
Double-click `run.bat` in the project root.

### Option B: From Command Line
```powershell
$env:PATH = "C:\msys64\ucrt64\bin;C:\msys64\usr\bin;" + $env:PATH
cd build
.\BollywoodMovieRecommender.exe
```

---

## 🧪 Verification & Test Results

The application includes two automated test suites verifying all core subsystems:

### 1. Engine Verification Test (`TestEngine.exe`)
Verifies the CSV parser, in-memory model, TF-IDF feature space, and cosine similarity ranking:
```powershell
.\build\TestEngine.exe
```
**Output Highlights:**
```text
[TEST 1] Testing CSV Reader...
-> Successfully read 2199 movies from CSV.
-> Sample Movie: Jawan (2023) | Director: Atlee

[TEST 4] Testing Recommendation Engine...
-> Engine trained successfully on 2199 movies.

[TEST 5] Generating Recommendations for 'Sholay'...
Top recommendations for 'Sholay':
   1. Parvarish (1977) | Similarity: 23.59% | Genre: Action, Crime, Drama
   2. Shaan (1980)     | Similarity: 21.56% | Genre: Action, Adventure, Comedy
   3. Zamaana Deewana (1995) | Similarity: 20.78% | Genre: Action, Comedy, Drama
   4. Mela (2000)      | Similarity: 20.03% | Genre: Action, Adventure, Comedy

[TEST 6] Generating Recommendations for '3 Idiots'...
Top recommendations for '3 Idiots':
   1. Untitled Aamir khan/Rajkumar Hirani movie | Similarity: 26.03%
   2. Rang De Basanti (2006) | Similarity: 13.20%
   3. Munna Bhai M.B.B.S. (2003) | Similarity: 13.05%
   4. Dunki (2023) | Similarity: 12.33%
```

### 2. Live MySQL 8 Integration Test (`TestMySQL.exe`)
Verifies direct connection to MySQL 8 on port 3306, table verification, prepared statements, and SQL queries:
```powershell
.\build\TestMySQL.exe
```
**Output Highlights:**
```text
Available SQL drivers:
 - QIBASE, QSQLITE, QMARIADB, QMYSQL, QODBC, QPSQL

[TEST 1] Connecting to MySQL 8 (QMYSQL driver)...
-> Successfully connected to MySQL 8 database: bollywood_recommender

[TEST 2] Importing CSV Dataset into MySQL...
-> Successfully imported 2199 movies into MySQL 'movies' table!

[TEST 3] Testing Prepared Statement Search in MySQL...
-> Search for 'Jawan' in MySQL returned 3 record(s):
   - ID: 1 | Title: Jawan | Year: 2023 | Director: Atlee | Genre: Action, Thriller

[TEST 5] Generating Recommendations for 'Jawan' from MySQL data...
   1. Happy New Year (2014) | Match: 16.12% | Director: Farah Khan
   2. Pathaan (2023)        | Match: 14.79% | Director: Siddharth Anand
   3. Om Shanti Om (2007)   | Match: 13.76% | Director: Farah Khan
   4. Finding Fanny (2014)  | Match: 13.45% | Director: Homi Adajania
```

---

## ❓ Troubleshooting & FAQs

### Q1: What if MySQL Server is not running on my PC?
**Answer:** The application is built to be resilient. If the MySQL daemon is not running, the application automatically switches to **SQLite Local Database mode (`bollywood_recommender.db`)**, ensuring smooth operation without crashing. You can connect to MySQL at any time using the in-app **⚙️ Database Settings** button.

### Q2: How do I start the MySQL Server on Windows?
**Answer:** Open PowerShell or Command Prompt and run:
```powershell
& "C:\Program Files\MySQL\MySQL Server 8.4\bin\mysqld.exe" --datadir="P:\C++ Project\mysql_data" --port=3306 --console
```
Or start the Windows service if installed:
```powershell
Start-Service MySQL84
```

### Q3: Where are database credentials configured?
**Answer:** In `config.ini` located right next to the executable:
```ini
[Database]
host=127.0.0.1
port=3306
database=bollywood_recommender
username=root
password=
driver=QMYSQL
dataset_path=data/IMDB-Movie-Dataset(2023-1951).csv
```
Credentials can also be edited directly inside the application GUI via **⚙️ Database Settings**.

---

## 👨‍💻 College Demonstration Checklist

For your college presentation or viva:
1. **Show the Desktop GUI:** Highlight the modern dark theme, the hero banner, and the navigation sidebar.
2. **Demonstrate Search & Filters:** Show real-time filtering by genre (Action, Comedy, Drama) and release year.
3. **Execute Recommendation Studio:** Select iconic movies like `3 Idiots` or `Sholay`, adjust the Top-N slider to 10, and click "Generate Recommendations".
4. **Show MySQL Backend:** Open MySQL CLI or Workbench and run `SELECT COUNT(*) FROM movies;` to show all 2,199 records actively stored in MySQL.
5. **Explain Machine Learning:** Walk through the TF-IDF feature soup formula and Cosine Similarity dot product in `src/recommender/TFIDFVectorizer.cpp` and `RecommendationEngine.cpp`.

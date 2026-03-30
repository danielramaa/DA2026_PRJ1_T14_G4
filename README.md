# DA2026 - Scientific Conference Assignment Tool

A tool to assign paper submissions to reviewers for scientific conferences, modeled as a Maximum Flow problem using the Edmonds-Karp algorithm.

## Features

- Parses CSV input files with submissions, reviewers and parameters
- Assigns reviewers to submissions based on topic expertise (mode 1: primary topics only, mode 2: primary + secondary submission topics)
- Risk Analysis (K=1): identifies reviewers whose absence would make a complete assignment impossible
- Interactive CLI menu and batch mode

## Usage

**Interactive mode:**
```bash
./DA2026_PRJ1_T14_G3
```

**Batch mode:**
```bash
./DA2026_PRJ1_T14_G3 -b input.csv output.csv
```

## Build
```bash
mkdir cmake-build-debug
cd cmake-build-debug
cmake ..
make
```

## Project Structure
```
├── include/       # Header files
├── src/           # Source files
├── docs/          # Doxygen documentation
└── inputs/        # Input datasets
```

## Authors

Daniel Ramaa — FEUP, L.EIC, 2025/26

Inês Veiga — FEUP, L.EIC, 2025/26

Pedro Lopes — FEUP, L.EIC, 2025/26

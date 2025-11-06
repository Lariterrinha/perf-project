# Embedded Image Processing Optimization (SABRE i.MX6)

**Authors:** Ana Beatriz Santos de Oliveira, Ana Luiza Diniz Santos, Larissa Anielle Terrinha de Oliveira

This project optimizes a C++ Median and Sobel filter pipeline for the ARM Cortex-A9 processor
We applied manual (algorithmic, data access) and compiler (`-O3`) optimizations.

---

## 📂 File Structure

  
```
│   myproject.cpp
│   my_functions.hpp
│   projet_24_v1.cpp
│   README.md
│
└───Versions
        algorithm_changes
        computational_reogarnization
        loop_unrolling_and_loop_optimization
        optimal_data_access_where_possible
        removal_of_redundant_computations
```
* `projet_24_v1.cpp`: Main file for benchmarking.
* `my_functions.cpp`: Our optimized filter implementations.

---
## Quick Links to Key Files:
### projet_24_v1.cpp:
The main application file, which handles video I/O, benchmarking, and calls the filter functions.
### my_functions.cpp
The core implementation file containing the final optimized versions of my_sobel() and my_median().
### /Versions/
This directory contains the source code snapshots from each progressive optimization step (e.g., algorithm_changes, loop_unrolling, optimal_data_access) for analysis, as described in the methodology

## 🛠️ Build and Run (Optimized)

### Open on file explorer  
Ctrl + L
```bash
sftp://linaro@192.168.1.2

```

### Run

1.Conection
```bash
ssh -X linaro@192.168.1.2
```
```bash
export DISPLAY=localhost:10.0
```

2. Test
```bash
xeyes
```
   
3.  **Transfer Files:** 
```bash
scp my_functions.hpp linaro@192.168.1.2:~/
```

```bash
scp my_functions.cpp projet_24_v1.cpp linaro@192.168.1.2:~/
```

4. See cameras to change on the code
```bash
sudo rm /dev/video*
```
```bash
ls /dev/video*
```
   
   
3.  **Compile (on board):**
```bash

g++ -O3 `pkg-config --cflags opencv` projet_24_v1.cpp my_functions.cpp `pkg-config --libs opencv` -o meu_codigo -lm

```
    
4.  **Run (on board):**
```bash
./meu_codigo 3
```
    
---










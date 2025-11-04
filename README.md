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

## 🛠️ Build and Run (Optimized)

1.  **Transfer Files:**
    ```bash
    scp projet_24_v1.cpp linaro@192.168.1.2:~/
    scp my_functions.cpp my_functions.hpp linaro@192.168.1.2:~/
    ```
    
2.  **Compile (on board):**
    ```bash
    ssh linaro@192.168.1.2 'cd ~ && g++ -O3 projet_24_v1.cpp my_functions.cpp -o meu_app_O3 -I/usr/include/opencv4 -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_imgcodecs -lm && echo "=== O3 OK ==="'
    ```
    
3.  **Run (on board):**
    ```bash
    ssh linaro@192.168.1.2 'cd ~ && for k in 3 7 ; do echo "=== O3 - KERNEL $k ===" && ./meu_app_O3 $k 2>&1 | tail -10; done'
    ```
    
---


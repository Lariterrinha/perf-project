#include "my_functions.hpp"
#include <math.h>
#include <algorithm>

void my_sobel(Mat im_in, Mat im_out) {
    int rows = im_in.rows;
    int cols = im_in.cols;
    
    // Pre-calculate pixel positions for better cache usage
    // Computational reorganization: use sliding registers for left/center/right
    for (int j = 1; j < rows-1; j++) {
        uchar* in_row = im_in.ptr<uchar>(j);
        uchar* in_row_prev = im_in.ptr<uchar>(j-1);
        uchar* in_row_next = im_in.ptr<uchar>(j+1);
        uchar* out_row = im_out.ptr<uchar>(j);

        // if image is too narrow, skip
        if (cols < 3) continue;

        // initialize registers for i = 1
        int left = in_row[0];
        int center = in_row[1];
        int right = in_row[2];

        int prev_left = in_row_prev[0];
        int prev_center = in_row_prev[1];
        int prev_right = in_row_prev[2];

        int next_left = in_row_next[0];
        int next_center = in_row_next[1];
        int next_right = in_row_next[2];

        // loop without branch inside: process bulk where i+3 is valid in unrolled pairs
        int last_safe = cols - 3;         // i+2 valid
        int last_safe_unroll = cols - 4;  // i+3 valid for unrolling by 2
        int i = 1;

        // unrolled loop: process two columns per iteration when possible
        for (; i <= last_safe_unroll; i += 2) {
            // --- first iteration (i)
            int gx1 = 2 * (right - left) + (prev_right - prev_left) + (next_right - next_left);
            int gy1 = 2 * (prev_center - next_center) + (prev_right - next_right) + (prev_left - next_left);
            int g1 = (abs(gx1) + abs(gy1)) >> 1;
            out_row[i] = (uchar)std::min(255, g1);

            // advance once (read i+2)
            int new_right = in_row[i + 2];
            int new_prev_right = in_row_prev[i + 2];
            int new_next_right = in_row_next[i + 2];
            left = center; center = right; right = new_right;
            prev_left = prev_center; prev_center = prev_right; prev_right = new_prev_right;
            next_left = next_center; next_center = next_right; next_right = new_next_right;

            // --- second iteration (i+1)
            int gx2 = 2 * (right - left) + (prev_right - prev_left) + (next_right - next_left);
            int gy2 = 2 * (prev_center - next_center) + (prev_right - next_right) + (prev_left - next_left);
            int g2 = (abs(gx2) + abs(gy2)) >> 1;
            out_row[i + 1] = (uchar)std::min(255, g2);

            // advance second time (read i+3)
            int new_right2 = in_row[i + 3];
            int new_prev_right2 = in_row_prev[i + 3];
            int new_next_right2 = in_row_next[i + 3];
            left = center; center = right; right = new_right2;
            prev_left = prev_center; prev_center = prev_right; prev_right = new_prev_right2;
            next_left = next_center; next_center = next_right; next_right = new_next_right2;
        }

        // process remaining safe single iterations where i+2 is valid
        for (; i <= last_safe; ++i) {
            int gx = 2 * (right - left) + (prev_right - prev_left) + (next_right - next_left);
            int gy = 2 * (prev_center - next_center) + (prev_right - next_right) + (prev_left - next_left);
            int g = (abs(gx) + abs(gy)) >> 1;
            out_row[i] = (uchar)std::min(255, g);

            // advance safely
            int new_right = in_row[i + 2];
            int new_prev_right = in_row_prev[i + 2];
            int new_next_right = in_row_next[i + 2];
            left = center; center = right; right = new_right;
            prev_left = prev_center; prev_center = prev_right; prev_right = new_prev_right;
            next_left = next_center; next_center = next_right; next_right = new_next_right;
        }

        // handle remaining columns (tail) using safe indexed loads
        for (; i < cols - 1; ++i) {
            int gx = 2 * (in_row[i + 1] - in_row[i - 1]) + (in_row_prev[i + 1] - in_row_prev[i - 1]) + (in_row_next[i + 1] - in_row_next[i - 1]);
            int gy = 2 * (in_row_prev[i] - in_row_next[i]) + (in_row_prev[i + 1] - in_row_next[i + 1]) + (in_row_prev[i - 1] - in_row_next[i - 1]);
            int g = (abs(gx) + abs(gy)) >> 1;
            out_row[i] = (uchar)std::min(255, g);
        }
    }
}

// Quick select algorithm for finding median
inline uchar quick_select(uchar* arr, int left, int right, int k) {
    while (left < right) {
        // Use middle element as pivot to improve performance on partially sorted data
        int mid = (left + right) / 2;
        uchar pivot = arr[mid];
        
        // Move pivot to end
        std::swap(arr[mid], arr[right]);
        
        // Partition with minimized memory access
        int i = left;
        int store = left;
        uchar tmp;
        
        for (; i < right; i++) {
            if (arr[i] <= pivot) {
                // Use direct assignment instead of swap when possible
                if (i != store) {
                    tmp = arr[store];
                    arr[store] = arr[i];
                    arr[i] = tmp;
                }
                store++;
            }
        }
        
        // Move pivot to final position
        std::swap(arr[store], arr[right]);
        
        // Quick return on exact match
        if (store == k) return arr[store];
        
        // Adjust search range
        if (store < k) left = store + 1;
        else right = store - 1;
    }
    return arr[left];
}

void my_median(Mat im_in, Mat im_out, int n) {
    int rows = im_in.rows;
    int cols = im_in.cols;
    int windowSize = n * n;
    int medianPos = windowSize / 2;
    int radius = n / 2;
    
    // For small windows (n ≤ 5), use histogram approach
    if (n <= 5) {
        // Use sliding histogram across columns to avoid rebuilding window every pixel
        vector<int> hist(256, 0);
        
        for (int r = 0; r < rows; r++) {
            // initialize histogram for column 0
            std::fill(hist.begin(), hist.end(), 0);
            int count = 0;

            // compute valid row range and cache pointers to avoid repeated ptr calls
            int rr_start = std::max(0, r - radius);
            int rr_end = std::min(rows - 1, r + radius);
            vector<uchar*> row_ptrs;
            row_ptrs.reserve(rr_end - rr_start + 1);
            for (int rr = rr_start; rr <= rr_end; ++rr) row_ptrs.push_back(im_in.ptr<uchar>(rr));

            // build initial histogram for c = 0 (columns 0..radius)
            int cc_start = std::max(0, 0 - radius);
            int cc_end = std::min(cols - 1, 0 + radius);
            for (int rr = 0; rr < (int)row_ptrs.size(); ++rr) {
                uchar* row_ptr = row_ptrs[rr];
                for (int cc = cc_start; cc <= cc_end; ++cc) {
                    hist[row_ptr[cc]]++;
                    ++count;
                }
            }

            for (int c = 0; c < cols; c++) {
                    if (c == 0) {
                        // first column already initialized
                    } else {
                        // slide window: remove column (c - radius - 1), add column (c + radius)
                        int col_remove = c - radius - 1;
                        int col_add = c + radius;

                        bool remove_valid = (col_remove >= 0 && col_remove < cols);
                        bool add_valid = (col_add >= 0 && col_add < cols);

                        for (int rr = rr_start; rr <= rr_end; ++rr) {
                            uchar* row_ptr = row_ptrs[rr - rr_start];
                            if (remove_valid) { hist[row_ptr[col_remove]]--; count--; }
                            if (add_valid) { hist[row_ptr[col_add]]++; count++; }
                        }
                    }

                    // Find median from histogram
                    int medianCount = (count - 1) / 2;
                    int sum = 0;
                    int medianVal = 0;
                    for (int v = 0; v < 256; v++) {
                        sum += hist[v];
                        if (sum > medianCount) { medianVal = v; break; }
                    }
                    im_out.at<uchar>(r, c) = (uchar)medianVal;
                }
            }
    }
    // For larger windows, use quick select with optimized data access
    else {
        vector<uchar> window(windowSize);
        vector<uchar*> row_ptrs(rows);
        
        // Pre-cache row pointers for better memory access
        for (int r = 0; r < rows; r++) {
            row_ptrs[r] = im_in.ptr<uchar>(r);
        }
        uchar* out_ptr = im_out.ptr<uchar>(0);
        
        for (int r = 0; r < rows; r++) {
            // Update output row pointer only when row changes
            if (r > 0) out_ptr = im_out.ptr<uchar>(r);
            
            // Pre-calculate row bounds for the window
            int rr_start = std::max(0, r - radius);
            int rr_end = std::min(rows - 1, r + radius);
            
            for (int c = 0; c < cols; c++) {
                // Pre-calculate column bounds for the window
                int cc_start = std::max(0, c - radius);
                int cc_end = std::min(cols - 1, c + radius);
                
                // Gather window values using direct pointer access
                int idx = 0;
                for (int rr = rr_start; rr <= rr_end; rr++) {
                    uchar* row = row_ptrs[rr];
                    for (int cc = cc_start; cc <= cc_end; cc++) {
                        window[idx++] = row[cc];
                    }
                }
                
                // Find median using quick select and write directly to output
                out_ptr[c] = quick_select(window.data(), 0, idx - 1, idx / 2);
            }
        }
    }
}
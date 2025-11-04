#include "my_functions.hpp"
#include <math.h>
#include <algorithm>

void my_sobel(Mat im_in, Mat im_out) {
    int rows = im_in.rows;
    int cols = im_in.cols;
    
    // Pre-calculate pixel positions for better cache usage
    for (int j = 1; j < rows-1; j++) {
        uchar* in_row = im_in.ptr<uchar>(j);
        uchar* in_row_prev = im_in.ptr<uchar>(j-1);
        uchar* in_row_next = im_in.ptr<uchar>(j+1);
        uchar* out_row = im_out.ptr<uchar>(j);
        
        for (int i = 1; i < cols-1; i++) {
            // Using L1 norm approximation instead of L2
            int gx = 2*(in_row[i+1] - in_row[i-1]) + 
                      (in_row_prev[i+1] - in_row_prev[i-1]) +
                      (in_row_next[i+1] - in_row_next[i-1]);
                      
            int gy = 2*(in_row_prev[i] - in_row_next[i]) +
                      (in_row_prev[i+1] - in_row_next[i+1]) +
                      (in_row_prev[i-1] - in_row_next[i-1]);
            
            // L1 norm (Manhattan distance) - faster than sqrt
            int g = (abs(gx) + abs(gy)) >> 1;  // divide by 2 to keep in range
            
            out_row[i] = (uchar)std::min(255, g);
        }
    }
}

// Quick select algorithm for finding median
inline uchar quick_select(uchar* arr, int left, int right, int k) {
    while (left < right) {
        uchar pivot = arr[right];
        int i = left;
        
        for (int j = left; j < right; j++) {
            if (arr[j] <= pivot) {
                std::swap(arr[i], arr[j]);
                i++;
            }
        }
        std::swap(arr[i], arr[right]);
        
        if (i == k) return arr[i];
        else if (i < k) left = i + 1;
        else right = i - 1;
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
        vector<int> hist(256, 0);
        vector<uchar> window(windowSize);
        
        for (int r = 0; r < rows; r++) {
            for (int c = 0; c < cols; c++) {
                // Reset histogram
                std::fill(hist.begin(), hist.end(), 0);
                
                // Fill histogram
                int count = 0;
                for (int rr = r - radius; rr <= r + radius; rr++) {
                    for (int cc = c - radius; cc <= c + radius; cc++) {
                        if (rr >= 0 && rr < rows && cc >= 0 && cc < cols) {
                            hist[im_in.at<uchar>(rr, cc)]++;
                            count++;
                        }
                    }
                }
                
                // Find median from histogram
                int medianCount = (count - 1) / 2;
                int sum = 0;
                for (int i = 0; i < 256; i++) {
                    sum += hist[i];
                    if (sum > medianCount) {
                        im_out.at<uchar>(r, c) = i;
                        break;
                    }
                }
            }
        }
    }
    // For larger windows, use quick select
    else {
        vector<uchar> window(windowSize);
        
        for (int r = 0; r < rows; r++) {
            for (int c = 0; c < cols; c++) {
                // Gather window values
                int idx = 0;
                for (int rr = r - radius; rr <= r + radius; rr++) {
                    for (int cc = c - radius; cc <= c + radius; cc++) {
                        if (rr >= 0 && rr < rows && cc >= 0 && cc < cols) {
                            window[idx++] = im_in.at<uchar>(rr, cc);
                        }
                    }
                }
                
                // Find median using quick select
                im_out.at<uchar>(r, c) = quick_select(window.data(), 0, idx - 1, idx / 2);
            }
        }
    }
}
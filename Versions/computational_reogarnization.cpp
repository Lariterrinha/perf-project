#include "my_functions.hpp"
#include <cmath>
#include <algorithm>

// Computational reorganization: Sobel with sliding registers (left/center/right)
void my_sobel(const Mat& im_in, Mat& im_out) {
    int rows = im_in.rows;
    int cols = im_in.cols;

    if (im_out.empty() || im_out.rows != rows || im_out.cols != cols)
        im_out.create(rows, cols, im_in.type());

    for (int j = 1; j < rows - 1; ++j) {
        uchar* in_row = im_in.ptr<uchar>(j);
        uchar* in_row_prev = im_in.ptr<uchar>(j - 1);
        uchar* in_row_next = im_in.ptr<uchar>(j + 1);
        uchar* out_row = im_out.ptr<uchar>(j);

        if (cols < 3) continue;

        // initialize sliding registers for column 1
        int left = in_row[0];
        int center = in_row[1];
        int right = in_row[2];

        int prev_left = in_row_prev[0];
        int prev_center = in_row_prev[1];
        int prev_right = in_row_prev[2];

        int next_left = in_row_next[0];
        int next_center = in_row_next[1];
        int next_right = in_row_next[2];

        int last_safe = cols - 3; // i+2 valid
        int i = 1;

        // process bulk where i+2 is valid without per-iteration branching
        for (; i <= last_safe; ++i) {
            int gx = 2 * (right - left) + (prev_right - prev_left) + (next_right - next_left);
            int gy = 2 * (prev_center - next_center) + (prev_right - next_right) + (prev_left - next_left);
            int g = (std::abs(gx) + std::abs(gy)) >> 1;
            out_row[i] = static_cast<uchar>(std::min(255, g));

            // advance registers reading i+2 which is safe in this loop
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
            int g = (std::abs(gx) + std::abs(gy)) >> 1;
            out_row[i] = static_cast<uchar>(std::min(255, g));
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

void my_median(const Mat& im_in, Mat& im_out, int n) {
    int rows = im_in.rows;
    int cols = im_in.cols;
    int windowSize = n * n;
    int radius = n / 2;

    if (im_out.empty() || im_out.rows != rows || im_out.cols != cols)
        im_out.create(rows, cols, im_in.type());

    // For small windows (n ≤ 5), use sliding histogram across columns
    if (n <= 5) {
        vector<int> hist(256, 0);

        for (int r = 0; r < rows; ++r) {
            // prepare row pointers and valid row range
            int rr_start = std::max(0, r - radius);
            int rr_end = std::min(rows - 1, r + radius);
            vector<uchar*> row_ptrs;
            row_ptrs.reserve(rr_end - rr_start + 1);
            for (int rr = rr_start; rr <= rr_end; ++rr) row_ptrs.push_back(im_in.ptr<uchar>(rr));

            // initialize histogram for first column (c = 0)
            std::fill(hist.begin(), hist.end(), 0);
            int count = 0;
            int cc_start = std::max(0, 0 - radius);
            int cc_end = std::min(cols - 1, 0 + radius);
            for (uchar* row_ptr : row_ptrs) {
                for (int cc = cc_start; cc <= cc_end; ++cc) {
                    hist[row_ptr[cc]]++;
                    ++count;
                }
            }

            for (int c = 0; c < cols; ++c) {
                if (c > 0) {
                    int col_remove = c - radius - 1;
                    int col_add = c + radius;

                    bool remove_valid = (col_remove >= 0 && col_remove < cols);
                    bool add_valid = (col_add >= 0 && col_add < cols);

                    for (int rr = rr_start; rr <= rr_end; ++rr) {
                        uchar* row_ptr = row_ptrs[rr - rr_start];
                        if (remove_valid) { hist[row_ptr[col_remove]]--; count--; }
                        if (add_valid)   { hist[row_ptr[col_add]]++; count++; }
                    }
                }

                // find median from histogram
                int medianCount = (count - 1) / 2;
                int sum = 0;
                int medianVal = 0;
                for (int v = 0; v < 256; ++v) {
                    sum += hist[v];
                    if (sum > medianCount) { medianVal = v; break; }
                }
                im_out.at<uchar>(r, c) = static_cast<uchar>(medianVal);
            }
        }
    }
    // For larger windows, keep quick-select approach (unchanged)
    else {
        vector<uchar> window(windowSize);
        vector<uchar*> row_ptrs(rows);
        for (int r = 0; r < rows; ++r) row_ptrs[r] = im_in.ptr<uchar>(r);

        for (int r = 0; r < rows; ++r) {
            int rr_start = std::max(0, r - radius);
            int rr_end = std::min(rows - 1, r + radius);
            for (int c = 0; c < cols; ++c) {
                int cc_start = std::max(0, c - radius);
                int cc_end = std::min(cols - 1, c + radius);

                int idx = 0;
                for (int rr = rr_start; rr <= rr_end; ++rr) {
                    uchar* row = row_ptrs[rr];
                    for (int cc = cc_start; cc <= cc_end; ++cc) {
                        window[idx++] = row[cc];
                    }
                }

                im_out.at<uchar>(r, c) = quick_select(window.data(), 0, idx - 1, idx / 2);
            }
        }
    }
}
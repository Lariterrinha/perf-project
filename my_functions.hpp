#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>

/*INCLUDES pour version 4.1*/
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"

using namespace std;
using namespace cv;

// Use references to avoid unnecessary copies
void my_sobel(const Mat& im_in, Mat& im_out);
void my_median(const Mat& im_in, Mat& im_out, int n);

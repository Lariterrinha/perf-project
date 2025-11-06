#include <stdio.h>
#include <stdlib.h>
#include <iostream>

/*INCLUDES pour version 4.1*/
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"

using namespace std;
using namespace cv;

void my_sobel (Mat , Mat );
void my_median (Mat, Mat, int );

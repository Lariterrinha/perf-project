#include "my_functions.hpp"
#include <cmath>


void my_sobel (const Mat& im_in, Mat& im_out)
{
	int rows = im_in.rows; // height
	int cols = im_in.cols; // width

	// Ensure output has the same size and type
	if (im_out.empty() || im_out.rows != rows || im_out.cols != cols)
		im_out.create(rows, cols, im_in.type());

	// iterate rows (y) then cols (x)
	for (int j = 1; j < rows-1; ++j) {
		for (int i = 1; i < cols-1; ++i) {
			int n = im_in.at<uchar>(j-1,i);
			int s = im_in.at<uchar>(j+1,i);
			int e = im_in.at<uchar>(j,i+1);
			int w = im_in.at<uchar>(j,i-1);
			int ne = im_in.at<uchar>(j-1,i+1);
			int nw = im_in.at<uchar>(j-1,i-1);
			int se = im_in.at<uchar>(j+1,i+1);
			int sw = im_in.at<uchar>(j+1,i-1);
			int c = im_in.at<uchar>(j,i);

			int gx = 2*e + ne + se - 2*w - sw - nw;
			int gy = 2*n + ne + nw - 2*s - sw - se;

			float g = std::sqrt((float)(gx*gx) + (float)(gy*gy));
			if (g > 255.0f) g = 255.0f;

			im_out.at<uchar>(j,i) = static_cast<uchar>(g);
		}
	}
}

void my_median (const Mat& im_in, Mat& im_out, int n)
{
	if (n <= 1) {
		// copy input to output
		im_in.copyTo(im_out);
		return;
	}

	int rows = im_in.rows; // height
	int cols = im_in.cols; // width
	int k = n * n;
	vector<uchar> v;
	v.reserve(k);

	if (im_out.empty() || im_out.rows != rows || im_out.cols != cols)
		im_out.create(rows, cols, im_in.type());

	int half = k / 2; // median index for k elements

	for (int r = 0; r < rows; ++r) {
		for (int c = 0; c < cols; ++c) {
			v.clear();
			for (int rr = r - n/2; rr <= r - n/2 + n - 1; ++rr) {
				for (int cc = c - n/2; cc <= c - n/2 + n - 1; ++cc) {
					if ((rr >= 0) && (rr < rows) && (cc >= 0) && (cc < cols)) {
						v.push_back(im_in.at<uchar>(rr, cc));
					}
				}
			}

			if (!v.empty()) {
				sort(v.begin(), v.end());
				im_out.at<uchar>(r, c) = v[v.size() / 2];
			} else {
				im_out.at<uchar>(r, c) = im_in.at<uchar>(r, c);
			}
		}
	}
}
#include "my_functions.hpp"
#include <math.h>


void my_sobel (Mat im_in, Mat im_out)
{

 int rows = im_in.rows; // height
 int cols = im_in.cols; // width
 
 
    for (int i = 1; i< cols-1; i++)
 	for (int j = 1; j< rows-1; j++){
 		int n = im_in.at<uchar>(j-1,i);
 		int s = im_in.at<uchar>(j+1,i);
 		int e = im_in.at<uchar>(j,i+1);
 		int w = im_in.at<uchar>(j,i-1);
 		int ne = im_in.at<uchar>(j-1,i+1);
 		int nw = im_in.at<uchar>(j-1,i-1);
 		int se = im_in.at<uchar>(j+1,i+1);
 		int sw = im_in.at<uchar>(j+1,i-1);
 		int c = im_in.at<uchar>(j,i);
 	
 	 	int gx = 2*e + ne + se - 2*w - sw - nw + 0*n + 0*c + 0*s;
 	 	int gy = 2*n + ne + nw - 2*s - sw - se + 0*w + 0*c + 0*e;
 	 	
 	 	float g = sqrt ((float)(gx*gx) + (float)(gy*gy));
 	 	
 	 	if (g>255)
 	 		g=255;
 	 	
 	 	im_out.at<uchar>(j,i) = (uchar) (g);
 	 	
 	 	}

};

void my_median (Mat im_in, Mat im_out, int n)
{

 int k = n*n;
 int rows = im_in.rows; // height
 int cols = im_in.cols; // width
 int r, c, rr, cc, p;
 vector<uchar> v (k);
 
    for(r=0;r<rows;r++){
      for(c=0;c<cols;c++){
         p = 0;
         for(rr=(r-(n/2));rr<(r-(n/2)+n);rr++){
            for(cc=(c-(n/2));cc<(c-(n/2)+n);cc++){
               if((rr>=0)&&(rr<rows)&&(cc>=0)&&(cc<cols)){
                  v[p] = im_in.at<uchar>(rr,cc);
                  p++;
	       }
            }
         }
        
         sort (v.begin(), v.end());
         im_out.at<uchar>(r,c) = v[k/2+1];
      }

}

};
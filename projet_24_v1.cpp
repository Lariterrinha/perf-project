/*
 * Fichier source pour le projet d'unitÃƒÂ©
 *  INF-4101C
 *---------------------------------------------------------------------------------------------------
 * Pour compiler, suivez l'exemple : g++ `pkg-config --cflags opencv` projet_base.cpp `pkg-config --libs opencv` -o projet -lm
 ou 
 g++ `pkg-config --cflags opencv4` projet_base.cpp `pkg-config --libs opencv4` -o projet -lm

 *---------------------------------------------------------------------------------------------------
 * auteur : Eva Dokladalova 09/2015
 * modification : Eva Dokladalova 10/2021
 * modification : Eva Dokladalova 10/2023
 */


/* 
 * Libraries standardes
 *
 */ 
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <time.h>
#include <sys/time.h>
/* 
 * Libraries OpenCV "obligatoires" 
 *
 */ 
/* INCLUDES pour version 2.4
#include "highgui.h"
#include "cv.h"
#include "opencv2/opencv.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"*/

/*INCLUDES pour version 4.1*/
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"

#include "my_functions.hpp"
  
/* -------------------------------------------------------------------
 * Deinition des "namespace" pour raccourcir cv::, std::, ou autre
 *
 ---------------------------------------------------------------------*/  
using namespace		std;
using namespace		cv;

/*----------------------------------------------
 *
 *--------------- MAIN FUNCTION ---------------
 *
 ----------------------------------------------*/
 
 
int main (int argc, char *argv[]) {
//----------------------------------------------
// Video acquisition - opening
// on ouvre ici la lecture du flux vidÃƒÂ©o
//----------------------------------------------
  VideoCapture		cap("/dev/video1"); // le numÃƒÂ©ro 0 indique le point d'accÃƒÂ¨s Ãƒ  la camÃƒÂ©ra 0 = > /dev/video0
  if(!cap.isOpened()){
    cout << "Errore"; return -1;
  
  }

// On peut choisir la dÃƒÂ©finition de l'image traitÃ©e - mais il faut respecter la resolution compatible avec la camera.
// HD resolution  1 920 Ãƒâ€” 1 080,
  int			rows								  = 480;	
  int			cols								  = 640;
  cap.set(CAP_PROP_FRAME_WIDTH, cols);	 
  cap.set(CAP_PROP_FRAME_HEIGHT, rows);

//----------------------------------------------
// DÃ©claration des variables - ATTENTION, declaration n'implique pas automatiquement l'allocation de l'espace memoire pour les images !!!
//
// Mat - structure contenant l'image 2D niveau de gris
// Mat3b - structure contenant l'image 2D en couleur (trois cannaux)
// ATTENTION - une dÃƒÂ©claration ne signifie pas une allocation !!
// 
//
  Mat3b	im_in;			// couleur (3cannaux)
  Mat	im_in_gray;		// niveau de gris (1 cannal)
  Mat	im_blurred;		// niveau de gris (1 cannal)

  Mat 	grad_x;
  Mat	grad_y;
  Mat	abs_grad_y;
  Mat	abs_grad_x;
  Mat	grad;

// variable contenant les paramÃƒÂ¨tres des images ou d'ÃƒÂ©xÃƒÂ©cution  
  int	ddepth	  = CV_16S;
  int	scale	  = 1;
  int	delta	  = 0;	
  unsigned char	key = '0';

//----------------------------------------------------
// CrÃƒÂ©ation des fenÃƒÂªtres pour affichage des rÃƒÂ©sultats
// vous pouvez ne pas les utiliser ou ajouter selon ces exemple
// code commentÃƒÂ© pour la vesrion 2.4

 /* cvNamedWindow("Video input", WINDOW_AUTOSIZE);
  cvNamedWindow("Video gray levels", WINDOW_AUTOSIZE);
  cvNamedWindow("Video Mediane", WINDOW_AUTOSIZE);
  cvNamedWindow("Video Edge detection", WINDOW_AUTOSIZE);
// placement arbitraire des  fenÃƒÂªtre sur ÃƒÂ©cran 
// sinon les fenÃƒÂªtres sont superposÃƒÂ©e l'une sur l'autre
  cvMoveWindow("Video input", 10, 30);
  cvMoveWindow("Video gray levels", 800, 30);
  cvMoveWindow("Video Mediane", 10, 500);
  cvMoveWindow("Video Edge detection", 800, 500);*/
  
 namedWindow("Video input", WINDOW_AUTOSIZE);
 namedWindow("Video gray levels", WINDOW_AUTOSIZE);
 namedWindow("Video Mediane", WINDOW_AUTOSIZE);
 namedWindow("Video Edge detection", WINDOW_AUTOSIZE);
// placement arbitraire des  fenÃƒÂªtre sur ÃƒÂ©cran 
// sinon les fenÃƒÂªtres sont superposÃƒÂ©e l'une sur l'autre
 moveWindow("Video input", 10, 30);
 moveWindow("Video gray levels", 800, 30);
 moveWindow("Video Mediane", 10, 500);
 moveWindow("Video Edge detection", 800, 500);
  
  
// --------------------------------------------------
// boucle de 100 itÃ©rations
//
 int	n;
 if (argc == 2)
   n = atoi(argv[1]);	//taille de filtre mÃ©dian - doit Ãªtre variables !
 
  double iter  = 20; 
  
  
  struct timeval	t0;
  struct timeval	t1;
  double		t_ref_median = 0.0;
  double		t_ref_grad  = 0.0;
  double		t_mycode_median  = 0.0;
  double		t_mycode_grad    = 0.0;
  double		temp  = 0.0;
  double		no_measures = iter;
  
  
  
  while(iter-- !=0 ){
 	  	
// acquisition d'une trame video - librairie OpenCV
    cap.read(im_in);

//conversion en niveau de gris - librairie OpenCV
//on travaille en niveau de gris pour ce projet (simplifie la problÃ©matique)
    cvtColor(im_in, im_in_gray, COLOR_BGR2GRAY);
    
// **************************************************************************
// CODE DE REFERENCE - avec OPENCV, aujourd'hui un standard en traitement d'image
// --------------------------------------------------------------------------
// mesure de temps d'exÃ©cution de cette partie :
    gettimeofday(&t0,NULL);
  // **********************************************************************
  // calcul de la mediane - librairie OpenCV
     medianBlur(im_in_gray, im_blurred, n);
     gettimeofday(&t1,NULL);
     temp  = (double)((t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec)/1000.0;
     t_ref_median  = t_ref_median + temp;
     printf("-------------------------------------\n");
     printf("Median Reference \t %g \n", temp);
     
    // mesure de temps d'exÃ©cution de cette partie :
    gettimeofday(&t0,NULL);
    // **********************************************************************   
     // calcul du gradient- librairie OpenCV
    /// Gradient Y
    Sobel( im_blurred, grad_x, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT );
    /// absolute value
    convertScaleAbs( grad_x, abs_grad_x );
    /// Gradient Y
    Sobel( im_blurred, grad_y, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT );
    /// absolute value
    convertScaleAbs( grad_y, abs_grad_y );
    /// Total Gradient (approximate)
    addWeighted( abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad ); 
    // **********************************************************************
    // mesure de temps d'exÃ©cution de cette partie :
    gettimeofday(&t1,NULL);
    temp  = (double)((t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec)/1000.0;
    t_ref_grad = t_ref_grad + temp;
    
    printf("Sobel Reference \t %g \n", temp);
    // **********************************************************************



    // **************************************************************************
    // CODE A OPTIMISER 
    // --------------------------------------------------------------------------
    // mesure de temps d'exÃ©cution de cette partie :
	gettimeofday(&t0,NULL);
    // **********************************************************************
    my_median (im_in_gray, im_blurred,n);
    gettimeofday(&t1,NULL);
    temp  = (double)((t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec)/1000.0;
    t_mycode_median = t_mycode_median + temp;
          printf("-------------------------------------\n");
     printf("Median MyCode \t %g \n", temp);
    // mesure de temps d'exÃ©cution de cette partie :
    gettimeofday(&t0,NULL);
    // ********************************************************************** 
     my_sobel (im_blurred, grad);
  
    // **********************************************************************
    // mesure de temps d'exÃ©cution de cette partie :
    gettimeofday(&t1,NULL);
    temp  = (double)((t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec)/1000.0;
    t_mycode_grad  = t_mycode_grad + temp;
      printf("Sobel MyCode \t %g \n", temp);
    // ********************************************************************** 
    imshow("Video input",im_in);
    imshow("Video gray levels",im_in_gray);
    imshow("Video Mediane",im_blurred);    
    imshow("Video Edge detection",grad);  
  
    key	= waitKey(1);
  }
  printf("========================================\n");
  // Emprimer le temps d'exÃ©uction moyen par version
   t_ref_median   = t_ref_median / no_measures;
   t_ref_grad = t_ref_grad / no_measures;
   t_mycode_median  = t_mycode_median / no_measures;
   t_mycode_grad  = t_mycode_grad / no_measures;  
   
   printf(" Mean meadian time referebce  %f ms\n",t_ref_median);
   printf(" Mean sobel time referebce  %f ms\n",t_ref_grad);
   printf(" Mean median time mycode %f ms\n",t_mycode_median);
   printf(" Mean sobel time mycode %f ms\n\n",t_mycode_grad);
   
      printf(" Speedup median %f ms\n",t_ref_median/t_mycode_median);
      printf(" Speedup sobel %f ms\n",t_ref_grad/t_mycode_grad);
   
    printf("------------------\n");
   
}

    
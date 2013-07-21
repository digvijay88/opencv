#include "precomp.hpp"
#include <zxing/DecodeHints.h>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <cmath>
#include <climits>

using namespace std;

namespace cv
{
namespace barcode
{

//constructor
KatonaNyu::KatonaNyu(Size _gausskernelSize, double _sigma) :
		gausskernelSize(_gausskernelSize), sigma(_sigma)
{
}


KatonaNyu::~KatonaNyu()
{
}

//apply bottom hat filter to the image
static void applyBottomhatFilter(Mat& image)
{
//Define the SE here
  Mat se1,se2,dst1,dst2;
  se1 = getStructuringElement(MORPH_RECT, Size(1,25)/*To be changed*/);
  se2 = getStructuringElement(MORPH_RECT, Size(25,1)/*To be changed*/);

  morphologyEx(image,dst1,MORPH_BLACKHAT,se1);
  morphologyEx(image,dst2,MORPH_BLACKHAT,se2);
  
  imwrite("/home/diggy/image1.jpg",dst1);
  imwrite("/home/diggy/image2.jpg",dst2);

  // how to choose which one to use? based on number of non-zero pixels
  int cnt1 = 0;
  int cnt2 = 0;
  for(int i=0;i<image.rows;i++)
  {
    for(int j=0;j<image.cols;j++)
    {   
      if(dst1.at<int>(i,j) != 0)
        cnt1++;
      if(dst2.at<int>(i,j) != 0)
        cnt2++;
    }   
  }
  
  if(cnt1 > cnt2)
    image = dst1;
  else
    image = dst2;

}

// compute MaxFreq
static void computeMaxFreqandThreshold(Mat& image);
{
  Mat freq_dom_image;
  Mat planes[] = {Mat_<float>(image), Mat::zeros(image.size(), CV_8UC1)};
  merge(planes,2,freq_dom_image);

  dft(freq_dom_image,freq_dom_image);

  split(freq_dom_image,planes);

  int m = planes[0].rows;
  int n = planes[1].cols;
  
  float max_mag = sqrt((planes[0].at<float>(0,0)*planes[0].at<float>(0,0)) + (planes[1].at<float>(0,0)*planes[1].at<float>(0,0)));
  int mag_x = 0;
  int mag_y = 0;
  for(int i=0;i<m;i++)
  {
    for(int j=0;j<n;j++)
    {
      float temp_mag = sqrt((planes[0].at<float>(i,j)*planes[0].at<float>(i,j)) + (planes[1].at<float>(i,j)*planes[1].at<float>(i,j)));
      if(temp_mag > max_mag)
      {
	max_mag = temp_mag;
	mag_x = i;
	mag_y = j;
      }
    }
  }
  
  // Need to check what is more and what is less according to the paper.......
  //Right now taking 80%
  
  float thresh = 0.8*max_mag;
  //TODO: do this later i.e. finding the threshold and do whatever .

}

//compute area threshold to remove FP and apply
static void computeAreaThresholdandApply(Mat& thresh_image)
{
  Mat edges;
  float areaThreshold;
//  blur(thresh_image,edges,Size(5,5));

  Canny(edges,edges,100,100*3,5);
  vector<vector<Point> > contours;
  vector<vec4i> hierarchy;

  findContours(edges,contours,hierarchy,CV_RETR_LIST,CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
  int max_size = 0;
  for(int i=0;i<contours.size();i++)
    if(contours[i].size > max_size)
      max_size = contours[i].size;

  if(thresh_image.rows*thresh_image.cols > 800*800)
    areaThreshold = 0.5*max_size;
  else
    areaThreshold = 0.25*max_size;

  Mat out_draw = zeros( filtrd.size(), CV_8UC1 );

  for(int i=0;i<contours.size();i++)
  {
    if(contours[i].size > areaThreshold)
    {
      Scalar color( rand()&255);
      drawContours(out_draw,contours,i,color,CV_FILLED,8,hierarchy);
    }
  }

  thresh_image = out_draw;
}

//compute distance threshold value and remove far objects
static void removeFarObjects(Mat &image,Mat &distanceMap);
{
  //calculate distance threshold
  float dist_thresh = INT_MAX;
  for(int i=0;i<distanceMap.rows;i++)
  {
    float sum = 0;
    for(int j=0;j<distanceMap.cols;j++)
      sum += distanceMap.at<float>(i,j);
    
    sum /= distanceMap.cols;
    if(sum < dist_thresh)
      dist_thresh = sum;
  }

  // next how do we check which regions are far by?
}
  
//removing unwanted text and other regions using morphology : dilation and then erosion. SE to be defined there
static void removeRegionsUsingMorphology(Mat& bin_image)
{
}

//remove the left FP regions based on size and proportions
static void removeUnwantedRegions(Mat& bin_image)
{
}

// fill the rectangle and points vector
static void convertToRectandPoints(Mat& bin_image,vector<RotatedRect>& barcode_rect,vector<Point>& barcode_cpoints)
{
}

void KatonaNyu::operator()(InputArray _image, vector<RotatedRect>& _barcode_rect,
			vector<Point>& _barcode_cpoints,string& decode_output) const
{
  Mat thresh_image;
  
  //preprocess the image to get a binary image as output
  preprocessImage(_image,thresh_image);

  //from the binary image, save obtain the barcode detection regions
  findBarcodeRegions(thresh_image, _barcode_rect, _barcode_cpoints);
}

void KatonaNyu::preprocessImage(InputArray _image, OutputArray thresh_image) const
{
  Mat image = _image.getMat();
  Mat clone_image = image.clone();

  //convert to grayscale
  if(image.type() != CV_8UC1)
    cvtColor(image,clone_image,COLOR_BGR2GRAY);

  //smoothen the image to reduce image noise
  Mat image_smooth;
  GaussianBlur(image,image_smooth,gausskernelSize,sigma);

  //applying bottomhat filtering to the image and the SE being linear is defined there.
  applyBottomhatFilter(image_smooth);

  //frequency of the most frequently occuring element
  //compute the threshold next using MaxFreq and the image size and threshold the image and save it as thresh_image
  computeMaxFreqandThreshold(image_smooth);

  thresh_image = image_smooth;
  
}

void KatonaNyu::findBarcodeRegions(InputArray thresh_image, vector<RotatedRect>& barcode_rect,
			vector<Point>& barcode_cpoints) const
{
  //compute area threshold to remove FP and apply
  Mat image = thresh_image.getMat();
  computeAreaThresholdandApply(image);

  //compute distance map for the image and then distance threshold from that.
  Mat distanceMap;
  Mat inv_image = image;
  
  for(int i=0;i<image.rows;i++)
  {
    for(int j=0;j<image.cols;j++)
    {
      if(image.at<int>(i,j) != 0)
        inv_image.at<int>(i,j) = 0;
      else
        inv_image.at<int>(i,j) = 1;
    }
  }
  distanceTransform(inv_image,distanceMap,CV_DIST_L2, CV_DIST_MASK_PRECISE);
  
  //compute distance threshold and remove Far regions using distance map and threshold
  removeFarObjects(image,distanceMap);

  //removing unwanted text and other regions using morphology : dilation and then erosion. SE to be defined there
  removeRegionsUsingMorphology(image);

  //remove the left FP regions based on size and proportions
  removeUnwantedRegions(image);

  // fill the rectangle and points vector
  convertToRectandPoints(image,barcode_rect, barcode_cpoints);
}

}
}

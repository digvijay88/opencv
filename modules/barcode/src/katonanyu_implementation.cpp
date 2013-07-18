#include "precomp.hpp"
#include <zxing/DecodeHints.h>
#include <cstdio>
#include <iostream>
#include <fstream>

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

//apply bottom hat filter to the image
static void applyBottomhatFilter(Mat& image)
{
//Define the SE here
  Mat se1,se2,dst1,dst2;
  se1 = getStructuringElement(MORPH_RECT, Size(1,10)/*To be changed*/);
  se2 = getStructuringElement(MORPH_RECT, Size(10,1)/*To be changed*/);

  morphologyEx(image,dst1,MORPH_BLACKHAT,se1);
  morphologyEx(image,dst2,MORPH_BLACKHAT,se2);

  // how to choose which one to use?
}

// compute MaxFreq
static float computeMaxFreq(Mat &image)
{
// How to do this?
}

//compute area threshold to remove FP and apply
static void computeAreaThresholdandApply(Mat& bin_image)
{
}

//compute distance map for the binary image
static void computeDistanceMap(Mat& bin_image,Mat& distanceMap)
{
}

//compute distance threshold value
static float computeDistanceThresh(Mat& distanceMap)
{
}
  
//removing Far regions using distance map and threshold
static void removeFarByRegions(Mat& bin_image,Mat& distanceMap,float distanceThresh)
{
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
static void convertToRectandPoints(Mat& bin_image,vector<RotatedRect>& barcode_rect,vector<vector<Point> >& barcode_cpoints)
{
}

void KatonaNyu::operator()(InputArray _image, vector<RotatedRect>& _barcode_rect,
			vector<vector<Point> >& _barcode_cpoints) const
{
  Mat bin_image;
  
  //preprocess the image to get a binary image as output
  preprocessImage(_image,bin_image);

  //from the binary image, save obtain the barcode detection regions
  findBarcodeRegions(bin_image, _barcode_rect, _barcode_cpoints);
}

void KatonaNyu::preprocessImage(InputArray _image, Mat& bin_image) const
{
  Mat image = _image.getMat();

  //convert to grayscale
  if(image.type() != CV_8UC1)
    cvtColor(image,_image,COLOR_BGR2GRAY);

  //smoothen the image to reduce image noise
  Mat image_smooth;
  GaussianBlur(image,image_smooth,gausskernelSize,sigma);

  //applying bottomhat filtering to the image and the SE being linear is defined there.
  applyBottomhatFilter(image_smooth);

  //frequency of the most frequently occuring element
  float MaxFreq = computeMaxFreq(image_smooth);

  //compute the threshold next using MaxFreq and the image size and binarise the image and save it as bin_image
  
}

void KatonaNyu::findBarcodeRegions(Mat& bin_image, vector<RotatedRect>& barcode_rect,
			vector<vector<Point> >& barcode_cpoints) const
{
  //compute area threshold to remove FP and apply
  computeAreaThresholdandApply(bin_image);

  //compute distance map for the image and then distance threshold from that.
  Mat distanceMap;
  computeDistanceMap(bin_image,distanceMap);
  float distanceThresh = computeDistanceThresh(distanceMap);
  
  //removing Far regions using distance map and threshold
  removeFarByRegions(bin_image,distanceMap,distanceThresh);

  //removing unwanted text and other regions using morphology : dilation and then erosion. SE to be defined there
  removeRegionsUsingMorphology(bin_image);

  //remove the left FP regions based on size and proportions
  removeUnwantedRegions(bin_image);

  // fill the rectangle and points vector
  convertToRectandPoints(bin_image,barcode_rect, barcode_cpoints);
}

}
}

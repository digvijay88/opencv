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


KatonaNyu::~KatonaNyu()
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
  
  imwrite("/home/diggy/image1.jpg",dst1);
  imwrite("/home/diggy/image2.jpg",dst2);
  // how to choose which one to use?
}

// compute MaxFreq
static float computeMaxFreq(Mat &image)
{
  return 0;
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
  return 0;
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
static void convertToRectandPoints(Mat& bin_image,vector<RotatedRect>& barcode_rect,vector<Point>& barcode_cpoints)
{
}

void KatonaNyu::operator()(InputArray _image, vector<RotatedRect>& _barcode_rect,
			vector<Point>& _barcode_cpoints,string& decode_output) const
{
  Mat bin_image;
  
  //preprocess the image to get a binary image as output
  preprocessImage(_image,bin_image);

  //from the binary image, save obtain the barcode detection regions
  findBarcodeRegions(bin_image, _barcode_rect, _barcode_cpoints);
}

void KatonaNyu::preprocessImage(InputArray _image, OutputArray bin_image) const
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
  float MaxFreq = computeMaxFreq(image_smooth);

  //compute the threshold next using MaxFreq and the image size and binarise the image and save it as bin_image
  
}

void KatonaNyu::findBarcodeRegions(InputArray bin_image, vector<RotatedRect>& barcode_rect,
			vector<Point>& barcode_cpoints) const
{
  //compute area threshold to remove FP and apply
  Mat image = bin_image.getMat();
  computeAreaThresholdandApply(image);

  //compute distance map for the image and then distance threshold from that.
  Mat distanceMap;
  computeDistanceMap(image,distanceMap);
  float distanceThresh = computeDistanceThresh(distanceMap);
  
  //removing Far regions using distance map and threshold
  removeFarByRegions(image,distanceMap,distanceThresh);

  //removing unwanted text and other regions using morphology : dilation and then erosion. SE to be defined there
  removeRegionsUsingMorphology(image);

  //remove the left FP regions based on size and proportions
  removeUnwantedRegions(image);

  // fill the rectangle and points vector
  convertToRectandPoints(image,barcode_rect, barcode_cpoints);
}

}
}

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
static void applyBottomhatFilter(Mat& image,int fctr)
{
//Define the SE here
  Mat se1,se2,dst1,dst2;
  int lenth_se = 25;
  if(fctr >= 2)
    lenth_se = ((25*image.cols*image.rows)/(720*480));

  cout << "Length for SE used is " << lenth_se << endl;
  
  se1 = getStructuringElement(MORPH_RECT, Size(1,lenth_se)/*To be changed*/);
  se2 = getStructuringElement(MORPH_RECT, Size(lenth_se,1)/*To be changed*/);

  morphologyEx(image,dst1,MORPH_BLACKHAT,se1);
  morphologyEx(image,dst2,MORPH_BLACKHAT,se2);
  
  imwrite("/home/diggy/git/out_image/image1.jpg",dst1);
  imwrite("/home/diggy/git/out_image/image2.jpg",dst2);

  // how to choose which one to use? based on number of non-zero pixels
  int cnt1 = countNonZero(dst1);
  int cnt2 = countNonZero(dst2);
  
  if(cnt1 > cnt2)
  {
    image = dst1;
    cout << "Took FIRST yo" << endl;
  }
  else
  {
    image = dst2;
    cout << "Took SECOND yo" << endl;
  }

}

// compute MaxFreq
static void computeMaxFreqandThreshold(Mat& image)
{
  float range[] = {0,256};
  const float* histRange = { range };
  Mat gray_hist;
  int histSize = 256;
  imshow("image",image);
  waitKey(0);
  calcHist(&image,1,0,Mat(),gray_hist,1, &histSize, &histRange,true,false);
  
  int MaxFreq = 0;
//  int ind = 10;
  int totalfreq = 0;
  int max_value = -1;
  for(int i=0;i<histSize;i++)
  {
    totalfreq += gray_hist.at<float>(i);
    cout << "Indx " << i << " -> " << gray_hist.at<float>(i) << endl;
    if(gray_hist.at<float>(i) > MaxFreq)
    {
      MaxFreq = gray_hist.at<float>(i);
//      ind = i;
    }
    if(i == 255 && max_value == -1)
      max_value = 255;
    else if(gray_hist.at<float>(i) == 0 && max_value == -1)
        max_value = i-1;
  }
  cout << "MaxFreq is " << MaxFreq << " out of total " << totalfreq << endl;
  cout << "maximum value is " << max_value << endl;
//  cout << "Indx is " << ind << endl;
  // Need to check what is more and what is less according to the paper.......
  //Right now taking 80%  
  //TODO: do this later i.e. finding the threshold and do whatever .
  float thresh = 0.8*max_value;

  for(int i=0;i<histSize;i++)
    if(gray_hist.at<float>(i) < thresh)
    {
//      cout << i << "->" << gray_hist.at<float>(i) << "| ";
      gray_hist.at<float>(i) = 0;
    }

// imwrite("/home/diggy/git/out_image/before.jpg",image);
 for(int i=0;i<image.rows;i++)
 {
   for(int j=0;j<image.cols;j++)
   {
     int pxl_val = (int)image.at<uchar>(i,j);

     /// Temp solution
     if(pxl_val < 50)
       image.at<uchar>(i,j) = 0;
     else
       image.at<uchar>(i,j) = 255;

/*     if(pxl_val < (int)thresh)
       image.at<uchar>(i,j) = 0;
     else
     {
 //      if(gray_hist.at<float>((int)pxl_val) == 0)
 //        image.at<uchar>(i,j) = 0;
 //      else
       image.at<uchar>(i,j) = 255;
     }*/
   }
 }
   
 Mat bin_image;
 threshold(image,bin_image,120,255,THRESH_BINARY);
 image = bin_image;
 imwrite("/home/diggy/git/out_image/preprocessed.jpg",image);

}

//compute area threshold to remove FP and apply
static void computeAreaThresholdandApply(Mat& bin_image)
{
  Mat edges;
  float areaThreshold;
  blur(bin_image,edges,Size(3,3));

  cout << "1" << endl;
  Canny(edges,edges,100,100*3,5);
  vector<vector<Point> > contours;
  vector<Vec4i> hierarchy;

  cout << "1" << endl;
  findContours(edges,contours,hierarchy,RETR_LIST,CHAIN_APPROX_SIMPLE, Point(0, 0));
  cout << "1" << endl;
  int max_size = 0;
  for(int i=0;i<contours.size();i++)
    if(contours[i].size() > max_size)
      max_size = contours[i].size();

  cout << "1" << endl;
  if(bin_image.rows*bin_image.cols > 800*800)
    areaThreshold = 0.5*max_size;
  else
    areaThreshold = 0.25*max_size;

  cout << "1" << endl;
  Mat out_draw = Mat::zeros( bin_image.size(), CV_8UC1 );
  Mat out_draw1 = Mat::zeros( bin_image.size(), CV_8UC1 );

  for(int i=0;i<contours.size();i++)
  {
    if(contours[i].size() > areaThreshold)
    {
      Scalar color(255);
      drawContours(out_draw,contours,i,color,FILLED,8,hierarchy);
    }
    if(contours[i].size() == max_size)
    {
      Scalar color(255);
      drawContours(out_draw1,contours,i,color,FILLED,8,hierarchy);
    }
  }
  cout << "1" << endl;

  bin_image = out_draw;
  imwrite("/home/diggy/git/out_image/areathreshold1.jpg",bin_image);
  imwrite("/home/diggy/git/out_image/areathreshold2.jpg",out_draw1);
}

//compute distance threshold value and remove far objects
static void removeFarObjects(Mat &image,Mat &distanceMap)
{
  //calculate distance threshold
  float dist_thresh = INT_MAX;
  for(int i=0;i<distanceMap.rows;i++)
  {
    float sum = 0;
    for(int j=0;j<distanceMap.cols;j++)
      sum += (float)distanceMap.at<uchar>(i,j);
    
    sum /= distanceMap.cols;
    if(sum < dist_thresh)
      dist_thresh = sum;
  }
  cout << "distance threshold is " << dist_thresh << endl;

  // next how do we check which regions are far by?
  for(int i=0;i<distanceMap.rows;i++)
    for(int j=0;j<distanceMap.cols;j++)
      if((float)distanceMap.at<uchar>(i,j) > dist_thresh)
        image.at<uchar>(i,j) = 0;

  imshow("farobects removed",image);
  waitKey(0);
}
  
//removing unwanted text and other regions using morphology : dilation and then erosion. SE to be defined there
static void removeRegionsUsingMorphology(Mat& bin_image)
{
  //Need to change the size to max(40,width_widest_bar*3);
  int ln_se = 40;
//  int ln_se = max(40/*,width_widest_bar*3*/);
  Mat se = getStructuringElement(MORPH_RECT,Size(ln_se,ln_se));
  Mat morphed_image;
  dilate(bin_image,morphed_image,se);

  se = getStructuringElement(MORPH_RECT,Size(ln_se/3,1));
  erode(morphed_image,bin_image,se);

}

//remove the left FP regions based on size and proportions
static void removeUnwantedRegions(Mat& bin_image)
{
  Mat edges;
  float areaThreshold = 0;

  blur(bin_image,edges,Size(5,5));
  Canny(edges,edges,100,100*3,5);
  vector<vector<Point> > contours;
  vector<Vec4i> hierarchy;

  findContours(edges,contours,hierarchy,RETR_LIST,CHAIN_APPROX_SIMPLE, Point(0, 0));

  for(int i=0;i<contours.size();i++)
    if(contours[i].size() > areaThreshold)
      areaThreshold = contours[i].size();

  if(bin_image.cols*bin_image.rows > 800*800)
    areaThreshold = areaThreshold/2;
  else
    areaThreshold = areaThreshold/4;

  Mat out_draw = Mat::zeros(bin_image.size(),bin_image.type());
  
  for(int i=0;i<contours.size();i++)
  {
    if(contours[i].size() > areaThreshold)
    {
      Scalar color( 1);
      drawContours(out_draw,contours,i,color,FILLED,8,hierarchy);
    }
  }


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

  cout << "convert to grayscale" << endl;
  if(image.type() != CV_8UC1)
    cvtColor(image,clone_image,COLOR_BGR2GRAY);
  
  cout << "size of the image is " << endl;
  int wd =  image.cols;
  int ht = image.rows;
  cout << wd << " " << ht << endl;
  int fctr = (wd*ht)/(720*480);
  cout << fctr << " is the factor " << endl;

  cout << "smoothen the image to reduce image noise using sigma value " << sigma << endl;
  Mat image_smooth;
  if(fctr >= 2)
  {
    Mat tmp = image;
    Mat dst = tmp;
    pyrDown(tmp,dst,Size(tmp.cols/fctr,tmp.rows/fctr));
    GaussianBlur(dst,tmp,Size(0,0),sigma);
    pyrUp(tmp,dst,Size(tmp.cols*fctr,tmp.rows*fctr));
    image_smooth = dst;
  }
  else
    GaussianBlur(image,image_smooth,Size(0,0),sigma);

  
  //applying bottomhat filtering to the image and the SE being linear is defined there.
  applyBottomhatFilter(image_smooth,fctr);

  //frequency of the most frequently occuring element
  //compute the threshold next using MaxFreq and the image size and threshold the image and save it as bin_image
  computeMaxFreqandThreshold(image_smooth);
//  imwrite("/home/diggy/git/out_image/image3.jpg",image_smooth);
  image_smooth.copyTo(bin_image);
  
}

void KatonaNyu::findBarcodeRegions(InputArray bin_image, vector<RotatedRect>& barcode_rect,
			vector<Point>& barcode_cpoints) const
{
  //compute area threshold to remove FP and apply
  Mat image = bin_image.getMat();
//  computeAreaThresholdandApply(image);

  //compute distance map for the image and then distance threshold from that.
  Mat distanceMap;
  Mat inv_image = image;
  imshow("inv_image",inv_image);
  waitKey(0);
  
  for(int i=0;i<image.rows;i++)
  {
    for(int j=0;j<image.cols;j++)
    {
      if((int)image.at<uchar>(i,j) != 0)
        inv_image.at<uchar>(i,j) = 0;
      else
        inv_image.at<uchar>(i,j) = 255;
//      cout << (int)inv_image.at<uchar>(i,j) << " ";
    }
  }
  cout << endl;
  imshow("inv_image",inv_image);
  waitKey(0);
  distanceTransform(inv_image,distanceMap,DIST_L2, DIST_MASK_PRECISE);
  float max_inv = -1;
  for(int i=0;i<distanceMap.rows;i++)
    for(int j=0;j<distanceMap.cols;j++)
      if(max_inv < distanceMap.at<float>(i,j))
        max_inv = distanceMap.at<float>(i,j);
  cout << "maximum value for image inverse is " << max_inv << endl;

  for(int i=0;i<distanceMap.rows;i++)
    for(int j=0;j<distanceMap.cols;j++)
      distanceMap.at<float>(i,j) = (int)((distanceMap.at<float>(i,j)/max_inv)*255);

  cout << "Type of distance transform is " << distanceMap.type() << endl;
  imshow("distance transform",distanceMap);
  waitKey(0);
  
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

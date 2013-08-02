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
  Mat se1,se2;
  Mat dst1,dst2;
  int lenth_se = 25;
//  if(fctr >= 1)
//    lenth_se = ((25*image.cols*image.rows)/(720*480));
//  cout << "Length for SE used is " << lenth_se << endl;
  
  se1 = getStructuringElement(MORPH_RECT, Size(1,lenth_se)/*To be changed*/);
  se2 = getStructuringElement(MORPH_RECT, Size(lenth_se,1)/*To be changed*/);
 
  morphologyEx(image,dst1,MORPH_BLACKHAT,se1);
  morphologyEx(image,dst2,MORPH_BLACKHAT,se2);
  
  if(countNonZero(dst1) > countNonZero(dst2))
    image = dst1;
  else
    image = dst2;

//  imshow("bottom hat",image);
//  waitKey(0);
}

// compute MaxFreq
static void computeMaxFreqandThreshold(Mat& image)
{
  float range[] = {0,256};
  const float* histRange = { range };
  Mat gray_hist;
  int histSize = 256;
  calcHist(&image,1,0,Mat(),gray_hist,1, &histSize, &histRange,true,false);
  
  int MaxFreq = 0;
  int totalfreq = 0;
  int max_value = -1;
  for(int i=histSize-1;i>=0;i--)
  {
    totalfreq += gray_hist.at<float>(i);
    if(gray_hist.at<float>(i) > MaxFreq)
      MaxFreq = gray_hist.at<float>(i);
    
    if((int)gray_hist.at<float>(i) != 0 && max_value == -1)
    {
      if(i == 0)
      {
        cout << "THERE IS SOMETHING SERIOUSLY WRONG !!" << endl;
	max_value = 0;
      }
      else
        max_value = i;
    }
  }
  cout << "MaxFreq is " << MaxFreq << " out of total " << totalfreq << endl;
  cout << "======== Relative maxfreq is " << (100*MaxFreq)/totalfreq << endl;
  cout << "maximum value is " << max_value << endl;
//  cout << "Indx is " << ind << endl;
  // Need to check what is more and what is less according to the paper.......
  //Right now taking 80%  
  //TODO: do this later i.e. finding the threshold and do whatever .
  float thresh = 0.8*max_value;
  thresh = 50; /////////////////////////////////////////////////////////////////////////////////harcoded value

  for(int i=0;i<histSize;i++)
    if(gray_hist.at<float>(i) < thresh)
      gray_hist.at<float>(i) = 0;

// imwrite("/tmp/before.jpg",image);
  for(int i=0;i<image.rows;i++)
  {
    for(int j=0;j<image.cols;j++)
    {
      int pxl_val = (int)image.at<uchar>(i,j);

      if(pxl_val < (int)thresh)
        image.at<uchar>(i,j) = 0;
      else
        image.at<uchar>(i,j) = 255;
    }
  }
  Mat bin_image;
  threshold(image,bin_image,120,255,THRESH_BINARY);
  image = bin_image;
}

//compute area threshold to remove FP and apply
static void computeAreaThresholdandApply(Mat& bin_image)
{
  Mat edges;
  float areaThreshold;
//  blur(bin_image,edges,Size(3,3));

  cout << "1" << endl;
//  Canny(edges,edges,100,100*3,5);
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
  imwrite("/tmp/areathreshold1.jpg",bin_image);
  imwrite("/tmp/areathreshold2.jpg",out_draw1);
}

//compute distance threshold value and remove far objects
static void removeFarObjects(Mat &image,Mat &distanceMap)
{
  //calculate distance threshold
  cout << image.cols << " " << image.rows << endl;
  float dist_thresh = INT_MAX;
  int ind = 0;
  for(int i=0;i<distanceMap.rows;i++)
  {
    float sum = 0;
    for(int j=0;j<distanceMap.cols;j++)
      sum += (float)distanceMap.at<float>(i,j);
    
    sum /= distanceMap.cols;
    if(sum < dist_thresh)
    {
      dist_thresh = sum;
      ind = i;
    }
  }
  cout << "distance threshold is " << dist_thresh << " at index " << ind << endl;

  // next how do we check which regions are far by?
  for(int i=0;i<distanceMap.rows;i++)
    for(int j=0;j<distanceMap.cols;j++)
      if((float)distanceMap.at<float>(i,j) > dist_thresh)
        image.at<uchar>(i,j) = 0;

//  imshow("dmap",image);
//  waitKey(0);

}
  
//removing unwanted text and other regions using morphology : dilation and then erosion. SE to be defined there
static void removeRegionsUsingMorphology(Mat& bin_image)
{
  //Need to change the size to max(40,width_widest_bar*3);
  int ln_se = 20;
//  int ln_se = max(40/*,width_widest_bar*3*/);
  Mat se = getStructuringElement(MORPH_RECT,Size(ln_se,ln_se));
  Mat morphed_image;
  dilate(bin_image,morphed_image,se);

  se = getStructuringElement(MORPH_RECT,Size(ln_se/2,1));
  erode(morphed_image,bin_image,se);
}

// a basic function to combine very close objects for thin barcodes 
static void useMorphology(Mat& bin_image)
{
  //Need to change the size to max(40,width_widest_bar*3);
  int ln_se = 5;
//  int ln_se = max(40/*,width_widest_bar*3*/);
  Mat se = getStructuringElement(MORPH_RECT,Size(ln_se,ln_se));
  Mat morphed_image;
  dilate(bin_image,morphed_image,se);
  bin_image = morphed_image;
}


//remove the left FP regions based on size and proportions
static void removeUnwantedRegions(Mat& bin_image)
{
  Mat edges;
  bin_image.copyTo(edges);
  float areaThreshold = 0;

//  blur(bin_image,edges,Size(5,5));
//  Canny(edges,edges,100,100*3,5);
  vector<vector<Point> > contours;
  vector<Vec4i> hierarchy;

  findContours(edges,contours,hierarchy,RETR_LIST,CHAIN_APPROX_NONE);
  
  cout << "*****" << contours.size() << "******" << endl;

  for(int i=0;i<contours.size();i++)
  {
    float temp_area = contourArea(contours[i]);
//    cout << "CONTOUR SIZE " << temp_area << endl;
    if(temp_area > areaThreshold)
      areaThreshold = contourArea(contours[i]);
  }

  if(bin_image.cols*bin_image.rows > 800*800)
    areaThreshold = areaThreshold/2;
  else
    areaThreshold = areaThreshold/4;
  cout << "area threshold is " << areaThreshold << endl;
  Mat out_draw = Mat::zeros(bin_image.size(),bin_image.type());
//  imshow("before",bin_image); 
//  waitKey(0);
  for(int i=0;i<contours.size();i++)
  {
    if(contourArea(contours[i]) > areaThreshold)
    {
      Scalar color( 255);
      drawContours(out_draw,contours,i,color,FILLED,8,hierarchy);
    }
  }

  threshold(out_draw,bin_image,120,255,THRESH_BINARY);
//  imshow("after",bin_image); 
//  waitKey(0);

}

static void invert_image(Mat& image,Mat& out_image)
{
  out_image = Mat::zeros(image.size(),image.type());
  for(int i=0;i<image.rows;i++)
  {
    for(int j=0;j<image.cols;j++)
    {
      if((int)image.at<uchar>(i,j) != 0)
        out_image.at<uchar>(i,j) = 0;
      else
        out_image.at<uchar>(i,j) = 255;
    }
  }
  Mat temp_image;
  threshold(out_image,temp_image,120,255,THRESH_BINARY);
  out_image = temp_image;
}

// fill the rectangle and points vector
static void convertToRectandPoints(Mat& bin_image,vector<RotatedRect>& barcode_rect,vector<Point>& barcode_cpoints)
{
}

void KatonaNyu::operator()(InputArray _image, vector<RotatedRect>& _barcode_rect,
			vector<Point>& _barcode_cpoints,string& decode_output) const
{
//  OutputArray bin_image;
   
  //preprocess the image to get a binary image as output
  preprocessImage(_image,_barcode_rect,_barcode_cpoints);

  //from the binary image, save obtain the barcode detection regions
//  findBarcodeRegions(bin_image, _barcode_rect, _barcode_cpoints);
}
  


void KatonaNyu::preprocessImage(InputArray _image,vector<RotatedRect>& barcode_rect,
			vector<Point>& barcode_cpoints) const
{
  Mat image = _image.getMat();

  cout << "convert to grayscale" << endl;
  if(image.type() != CV_8UC1)
    cvtColor(image,image,COLOR_BGR2GRAY);
  
  cout << "size of the image is " << endl;
  int wd =  image.cols;
  int ht = image.rows;
  cout << wd << " " << ht << endl;
/*  int fctr = (wd*ht)/(720*480);
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
  else*/
  GaussianBlur(image,image,Size(0,0),sigma);
//  imshow("smooth image",image_smooth);
//  waitKey(0);
//  image = image_smooth;

  //Use resize() as pyramid. Create three pyramids.
  Mat pyr1,pyr2,pyr3;
  //pyramid1
  pyr1 = image;

  //pyramid2
  resize(image,pyr2,Size(image.cols/2,image.rows/2),INTER_NEAREST);

  //pyramid3
  resize(image,pyr3,Size(image.cols/4,image.rows/4),INTER_NEAREST);

  //applying bottomhat filtering to the image and the SE being linear is defined there.
  applyBottomhatFilter(pyr1);
  imwrite("/tmp/bottom1.jpg",pyr1);
  applyBottomhatFilter(pyr2);
  imwrite("/tmp/bottom2.jpg",pyr2);
  applyBottomhatFilter(pyr3);
  imwrite("/tmp/bottom3.jpg",pyr3);

  //frequency of the most frequently occuring element
  //compute the threshold next using MaxFreq and the image size and threshold the image and save it as bin_image
  Mat bin1,bin2,bin3;
  computeMaxFreqandThreshold(pyr1);
  imwrite("/tmp/bin1.jpg",pyr1);
  computeMaxFreqandThreshold(pyr2);
  imwrite("/tmp/bin2.jpg",pyr2);
  computeMaxFreqandThreshold(pyr3);
  imwrite("/tmp/bin3.jpg",pyr3);
  
  //Little morphology to combine 
  // a basic function to combine very close objects for thin barcodes 
  useMorphology(pyr1);
  imwrite("/tmp/morph_pre1.jpg",pyr1);
  useMorphology(pyr2);
  imwrite("/tmp/morph_pre2.jpg",pyr2);
  useMorphology(pyr3);
  imwrite("/tmp/morph_pre3.jpg",pyr3);

  //remove the left FP regions based on size and proportions
  removeUnwantedRegions(pyr1);
  imwrite("/tmp/nounwanted1.jpg",pyr1);
  removeUnwantedRegions(pyr2);
  imwrite("/tmp/nounwanted2.jpg",pyr2);
  removeUnwantedRegions(pyr3);
  imwrite("/tmp/nounwanted3.jpg",pyr3);

  //compute distance map for the image and then distance threshold from that.
  Mat dmap1,dmap2,dmap3;
  Mat inv1,inv2,inv3;
  invert_image(pyr1,inv1);
  distanceTransform(inv1,dmap1,DIST_L2, DIST_MASK_PRECISE);
  invert_image(pyr2,inv2);
  distanceTransform(inv2,dmap2,DIST_L2, DIST_MASK_PRECISE);
  invert_image(pyr3,inv3);
  distanceTransform(inv3,dmap3,DIST_L2, DIST_MASK_PRECISE);

  //compute distance threshold and remove Far regions using distance map and threshold
  removeFarObjects(pyr1,dmap1);
  imwrite("/tmp/dmap1.jpg",pyr1);
  removeFarObjects(pyr2,dmap2);
  imwrite("/tmp/dmap2.jpg",pyr2);
  removeFarObjects(pyr3,dmap3);
  imwrite("/tmp/dmap3.jpg",pyr3);


  //removing unwanted text and other regions using morphology : dilation and then erosion. SE to be defined there
  removeRegionsUsingMorphology(pyr1);
  imwrite("/tmp/morph1.jpg",pyr1);
  removeRegionsUsingMorphology(pyr2);
  imwrite("/tmp/morph2.jpg",pyr2);
  removeRegionsUsingMorphology(pyr3);
  imwrite("/tmp/morph3.jpg",pyr3);


  //remove the left FP regions based on size and proportions
  removeUnwantedRegions(pyr1);
  imwrite("/tmp/final1.jpg",pyr1);
  removeUnwantedRegions(pyr2);
  imwrite("/tmp/final2.jpg",pyr2);
  removeUnwantedRegions(pyr3);
  imwrite("/tmp/final3.jpg",pyr3);

  // fill the rectangle and points vector
//  convertToRectandPoints(image,barcode_rect, barcode_cpoints);
}

}
}

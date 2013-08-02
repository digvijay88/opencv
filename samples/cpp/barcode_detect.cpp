#include <opencv2/core/utility.hpp>
#include "opencv2/barcode1D.hpp"
#include "opencv2/highgui.hpp"


#include<iostream>
#include<string>

using namespace cv;
using namespace barcode;
using namespace std;

string img_name;

static int parseCmdArgs(int argc, char** argv)
{
  if(argc == 1)
  {
    cout << "Enter the image name" << endl;
    return -1;
  }
  for(int i=1;i<argc;i++)
  {
    if(string(argv[i]) == "-img")
    {
      img_name = string(argv[i+1]);
      i++;
    }
  }
  return 1;
}

int main(int argc,char** argv)
{
  int retval = parseCmdArgs(argc, argv);
//  if(retval)
//    return -1;
  cout << img_name << endl;  
  Mat image = imread(img_name.c_str(),IMREAD_GRAYSCALE);
  if(image.empty())
  {
    cout << "Unable to read the input image!!" << endl;
    return 0;
  }
  cout << "went in" << endl; 
  vector<RotatedRect> barcode_rect;
  vector<Point> barcode_cpoints;
  Mat output;
  KatonaNyu detectr;
  detectr.preprocessImage(image,barcode_rect,barcode_cpoints);
  
//  detectr.findBarcodeRegions(output,barcode_rect,barcode_cpoints);

  return 0;
}

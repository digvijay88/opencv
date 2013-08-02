#include "test_precomp.hpp"
#include <fstream>
#include<cmath>
#include<algorithm>
#include<vector>
#include<string>
#include<boost/filesystem.hpp>
#include "opencv2/highgui.hpp"
#include "opencv2/core.hpp"
#include<iostream>


using namespace std;
using namespace cv;
using namespace boost::filesystem;
using namespace barcode;

const string BARCODE1D_DIR = "barcode";

class CV_BarcodeDetect1DTest : public cvtest::BaseTest, public KatonaNyu
{
public:
//  CV_BarcodeDetect1DTest(const Ptr<Detector1D> &_detector) : detector(_detector)
  CV_BarcodeDetect1DTest(const Ptr<KatonaNyu> &_detector) : detector(_detector)
  {
  }

protected:
  virtual void run(int)
  {
    initModule_barcode1D();
//    CV_Assert(!detector.empty());

    //TODO: get all the images
    string imgName = ts->get_data_path() + "barcode/zxing/zxing-2.2/core/test/data/blackbox/upca-1/12.png" ;
    cout << "this is the string " << imgName << endl;
    ts->printf(cvtest::TS::LOG,"LOADING problem: Unable to load image %s\n... check check",imgName.c_str());
    Mat image = imread(imgName);
    if(image.empty())
    {
      ts->printf(cvtest::TS::LOG,"LOADING problem: Unable to load image %s\n .. blah .. blah",imgName.c_str());
      ts->set_failed_test_info(cvtest::TS::FAIL_INVALID_TEST_DATA);
//      continue; 	//because everything will be inside another for loop
    }

    vector<RotatedRect> barcode_rect;
    vector<Point> barcode_cpoints;
    string decode_output;
    Mat image_out;
    detector->preprocessImage(image,barcode_rect,barcode_cpoints);
//    detector->detect(image, barcode_rect, barcode_cpoints, decode_output);

/*    if(barcode_rect.empty() && barcode_cpoints.empty())
    {
      ts->printf(cvtest::TS::LOG,"DETECTION error: Unable to detect barcode in the image %s\n",imgName.c_str());
      ts->set_failed_test_info(cvtest::TS::FAIL_INVALID_OUTPUT);
//      continue;
    }
    
    Rect r(0,0,image.cols,image.rows);

    if(!barcode_cpoints.empty())
    {
      for(size_t i=0;i<barcode_cpoints.size();i++)
      {
        const Point& pt_temp = barcode_cpoints[i];

	if(!r.contains(pt_temp))
	{
          ts->printf(cvtest::TS::LOG,"DETECTION error: Detected region out of bound in the image %s\n",imgName.c_str());
          ts->set_failed_test_info(cvtest::TS::FAIL_INVALID_OUTPUT);
	}
      }
    }*/
    ts->set_failed_test_info(cvtest::TS::OK);
  }

  Ptr<KatonaNyu> detector;

};


//Different tests
TEST(Barcode1D_Detector_KatonaNyu, validation)
{
  CV_BarcodeDetect1DTest test(Algorithm::create<Detector1D>("Barcode1D.KatonaNyu"));
  test.safe_run();
}




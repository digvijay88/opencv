#include "perf_precomp.hpp"
#include<boost/filesystem.hpp>
#include<vector>

using namespace std;
using namespace cv;
using namespace perf;
using namespace boost::filesystem;
using std::tr1::make_tuple;
using std::tr1::get;

typedef perf::TestBaseWithParam<std::string> zxing_wrap;
typedef vector<path> vec;

//void loadImagePaths(vector<string> &image_paths, const string& dir_name);

PARAM_TEST_CASE(barcode_zxing_wrap)
{
  vector<string> image_paths;
  
  virtual void SetUp()
  {	
    string dir_name = ts->get_data_path() + "barcode/zxing/zxing-2.2/core/test/data/blackbox/upca-1";
    path p(dir_name);
  
    vec v;
    copy(directory_iterator(p), directory_iterator(), back_inserter(v));
  
    vec::const_iterator it;
    for(it=v.begin();it != v.end(); it++)
    {
      string temp_str = it->string();
      if(temp_str.compare(temp_str.size()-4,4,".png")==0 || temp_str.compare(temp_str.size()-4,4,".jpg")==0 || temp_str.compare(temp_str.size()-4,4,".JPG")==0)
        image_paths.push_back(temp_str);
    }
  }
};

// Regression test for detection
PERF_TEST_P(barcode_zxing_wrap,detect)
{
  string img_name = getDataPath(GetParam());
  Mat image = imread(img_name,IMREAD_GRAYSCALE);

  if(image.empty())
    FAIL() << "Unable to load image file: " << img_name;

  vector<RotatedRect> barcode_rect;
  vector<Point> barcode_cpoints;
  string decode_output;

  ZXING_WRAP detector;

  TEST_CYCLE() detector(image,barcode_rect,barcode_cpoints,decode_output);

  SANITY_CHECK(barcode_cpoints);
}


// Regression test for decoding
PERF_TEST_P(barcode_zxing_wrap,decode)
{
  string img_name = getDataPath(GetParam());
  Mat image = imread(img_name,IMREAD_GRAYSCALE);

  if(image.empty())
    FAIL() << "Unable to load image file: " << img_name;

  vector<RotatedRect> barcode_rect;
  vector<Point> barcode_cpoints;
  string decode_output;

  ZXING_WRAP decoder;

  TEST_CYCLE() decoder(image,barcode_rect,barcode_cpoints,decode_output);

  SANITY_CHECK(decode_output);
}

INSTANTIATE_TEST_CASE_P(barcode_zxing_wrap,
	::testing::Values(image_paths));


//void loadImagePaths(vector<string> &image_paths, const string& dir_name)
//{
//}

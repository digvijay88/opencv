#include "perf_precomp.hpp"
#include<boost/filesystem.hpp>

using namespace std;
using namespace cv;
using namespace perf;
using namespace boost::filesystem;
using std::tr1::make_tuple;
using std::tr1::get;

typedef perf::TestBaseWithParam<std::string> zxing_wrap;
typedef vector<path> vec;

vector<string> image_paths;

void loadImagePaths(vector<string> &image_paths, const string& dir_name);

PERF_TEST_P(barcode_zxing_wrap,detect,testing::Values(image_paths))
{
  string img_name = getDataPath(GetParam());
  Mat image = imread(img_name,IMREAD_GRAYSCALE);

  if(image.empty())
    FAIL() << "Unable to load image file: " << img_name;

  Ptr<Detector1D> dt = Algorithm::create<Detector1D>("Barcode1D.ZXING_WRAP");

  ASSERT_FALSE(dt.empty());

  vector<RotatedRect> barcode_rect;
  vector<Point> barcode_cpoints;
  string decode_output;

//  TEST_CYCLE() dt->Detect
}


void loadImagePaths(vector<string> &image_paths, const string& dir_name)
{
  string dir_name = ts->get_data_path() + dir_name;
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

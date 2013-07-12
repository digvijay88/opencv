#include "perf_precomp.hpp"
#include<boost/filesystem.hpp>

using namespace std;
using namespace cv;
using namespace perf;
using namespace boost::filesystem;
using std::tr1::make_tuple;
using std::tr1::get;

typedef perf::TestBaseWithParam<std::string> zxing_wrap;

vector<string> image_paths;

void loadImagePaths(&image_paths);

void loadImagePaths(vector<string> &image_paths)
{
  ts->get_data_path();
}

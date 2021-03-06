#include "test_precomp.hpp"
#include <fstream>
#include "opencv2/highgui.hpp"
#include "opencv2/core.hpp"
#include<boost/filesystem.hpp>
#include "zxing_decode.hpp"
#include<vector>
#include<string>


using namespace std;
using namespace cv;
using namespace boost::filesystem;

#include "zxing/MultiFormatReader.h"

using namespace zxing;

typedef vector<path> vec_P;

class CV_BARCODE_DECODETest : public cvtest::BaseTest
{
	public:
		CV_BARCODE_DECODETest();
		~CV_BARCODE_DECODETest();
	protected:
		void run(int);
};


CV_BARCODE_DECODETest::CV_BARCODE_DECODETest() {}
CV_BARCODE_DECODETest::~CV_BARCODE_DECODETest() {}

void CV_BARCODE_DECODETest::run(int)
{
	vector<string> dataset_paths;
	string dir_name = "zxing/zxing-2.2/core/test/data/blackbox/upca-1";
	string dir_path = ts->get_data_path();
	dataset_paths.push_back(dir_path + dir_name);
//	dataset_paths.push_back("/home/diggy/git/opencv_extra/testdata/cv/barcode1D_dataset/zxing/zxing-2.2/core/test/data/blackbox/upca-1");

	for (int i = 0; i < dataset_paths.size (); i++)
	{
		path p = dataset_paths[i].c_str ();
		if (exists (p))
		{
			path GT = dataset_paths[i] + "/gt/";

			vec_P images_path;
			vec_P gt_path;
			copy (directory_iterator(p),directory_iterator(),back_inserter(images_path));
			copy (directory_iterator(GT), directory_iterator(),back_inserter(gt_path));

			vec_P::const_iterator it_gt = gt_path.begin();
			for (vec_P::const_iterator it = images_path.begin();it != images_path.end(); it++)
			{
				string temp_st = it->string();
				if (temp_st.compare (temp_st.size () - 4, 4, ".png") == 0 || temp_st.compare (temp_st.size () - 4, 4, ".jpg") == 0 || temp_st.compare (temp_st.size () - 4, 4, ".JPG") == 0 || temp_st.compare (temp_st.size () - 4, 4, ".PNG"))
				{
					//decode the image and store the string output
					MultiFormatReader mf;
					Mat img = imread(temp_st,IMREAD_GRAYSCALE);
					string decode_out;
					vector<Point> zx_pts;
					decode_image(&mf,img ,decode_out,zx_pts);
				
					//read the GT						
					string temp_gt = it_gt->string();
					FileStorage fs(temp_gt,FileStorage::READ);
					string gt_out;
					fs["decode_out"] >> gt_out;
					fs.release();
					
					EXPECT_STREQ(gt_out.c_str(),decode_out.c_str());		
					
				}
				it_gt++;
			}


		}
	}

	cout << "DONE!" << endl;
}


TEST(DECODE_1DBARCODE, accuracy) { CV_BARCODE_DECODETest test; test.safe_run(); }

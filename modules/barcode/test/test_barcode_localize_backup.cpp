#include "test_precomp.hpp"
#include <fstream>
#include<cmath>
#include<algorithm>
#include<vector>
#include<string>
#include<boost/filesystem.hpp>
#include "opencv2/highgui.hpp"
#include "opencv2/core.hpp"
#include "zxing_decode.hpp"


using namespace std;
using namespace cv;
using namespace boost::filesystem;

#include "zxing/MultiFormatReader.h"

using namespace zxing;

typedef vector<path> vec_P;

struct MatchPathSeparator
{
	bool operator () (char ch) const
	{
		return
			ch == '/';
	}
};

string basename (string const &pathname)
{
	//      cout << find_if(pathname.rbegin(), pathname.rend(),MatchPathSeparator()).base() << endl;
	string temp = string (find_if(pathname.rbegin (), pathname.rend (), MatchPathSeparator ()).base (), pathname.end ());
	return temp;
}

float min_dist(float x,float y,vector<Point> points,int ind)
{
	float x1,y1,x2,y2;
	x1 = points[ind].x;
	y1 = points[ind].y;
	x2 = points[ind+2].x;
	y2 = points[ind+2].y;

	float a,b,c;
	a = x2 - x1;
	b = y2 - y1;

	return ((a*(y1-y) - b*(x1-x))/sqrt(a*a + b*b));
}

class CV_BARCODE_LOCALIZETest:public cvtest::BaseTest
{
	public:
		CV_BARCODE_LOCALIZETest ();
		~CV_BARCODE_LOCALIZETest ();
	protected:
		void run (int);
};


CV_BARCODE_LOCALIZETest::CV_BARCODE_LOCALIZETest ()
{
}

CV_BARCODE_LOCALIZETest::~CV_BARCODE_LOCALIZETest ()
{
}

void CV_BARCODE_LOCALIZETest::run (int)
{

	vector<string> dataset_paths;
	string dir_name = "zxing/zxing-2.2/core/test/data/blackbox/upca-1";
	string dir_path = ts->get_data_path();
	dataset_paths.push_back(dir_path + dir_name);
	//	dataset_paths.push_back("/home/diggy/git/opencv_extra/testdata/cv/barcode1D_dataset/zxing/zxing-2.2/core/test/data/blackbox/upca-1");

	for (int i = 0; i < dataset_paths.size (); i++)
	{
		cout << dataset_paths[i] << endl;
		path p (dataset_paths[i].c_str());
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
				if (temp_st.compare(temp_st.size()-4,4,".png") == 0 || temp_st.compare(temp_st.size()-4,4,".jpg") == 0 || temp_st.compare (temp_st.size()-4,4,".JPG") == 0 || temp_st.compare (temp_st.size()-4,4,".PNG"))
				{
					//got the image, now decode it using the function decode_image
					MultiFormatReader mf;
					Mat img = imread(temp_st,IMREAD_GRAYSCALE);
					string decode_out;
					vector<Point> zx_pts;
					decode_image(&mf,img ,decode_out,zx_pts);
					//TO BE ADDED: if decoding fails.


					//read the GT for the same image
					string temp_gt = it_gt->string();
					FileStorage fs(temp_gt,FileStorage::READ);

					vector<vector<Point> > gt_points;
					fs["corner_points"] >> gt_points;
					fs.release();
					cout << gt_points.size() << endl;

					if(gt_points.size() != 4)
					{
						cout << "There is some error in the ground truth of FILE: " << temp_gt << endl;
						exit(0);
					}

					//got the two points from zxing and four corner points from GT , now testing for PASS/FAIL.
					// 2 conditions to be satisfied
					// Condition #1: Divide the line joining the two zxing points into 9 segments
					// which will give us 10 points on the line. If 8/10 lies inside the quadrilateral
					// connecting the 4 gt points, then PASS.

					float x1 = zx_pts[0].x;
					float y1 = zx_pts[0].y;
					float x2 = zx_pts[1].x;
					float y2 = zx_pts[1].y;

					float left_dist = min_dist(x1,y1,gt_points[0],0);	//0 means the edge 0->2
					float right_dist = min_dist(x1,y1,gt_points[0],1); //1 means the edge 1->3
					if(right_dist < left_dist)
					{
						left_dist = right_dist;
						right_dist = min_dist(x2,y2,gt_points[0],0);
					}
					else
						right_dist = min_dist(x2,y2,gt_points[0],1);

					//Now we got the distance between left point and left edge and right point and right edge.
					//Next step is to find the threshold. To find that we calculate the width of the barcode
					//and the number of bars that are present in the barcode and we keep the threshold to be
					//equal to the width of the average bar.
					float barcode_width = sqrt(((gt_points[0][0].x-gt_points[0][1].x)*(gt_points[0][0].x-gt_points[0][1].x))+
							((gt_points[0][0].y-gt_points[0][1].y)*(gt_points[0][0].y-gt_points[0][1].y)));
					barcode_width /= 2; //assuming half of the widht is white.
					float barcode_thresh = barcode_width/decode_out.size();

					EXPECT_LT(left_dist,barcode_thresh);
					EXPECT_LT(right_dist,barcode_thresh);
				}
				it_gt++;
			}
		}
		else
			cout << "PATH: " << dataset_paths[i] << " does not exist" << endl;
	}

	cout << "Done " << endl;
}


TEST (LOCALIZE_1DBARCODE, accuracy) {CV_BARCODE_LOCALIZETest test;test.safe_run ();}

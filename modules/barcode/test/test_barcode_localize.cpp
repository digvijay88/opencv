#include "test_precomp.hpp"
#include <fstream>
#include "opencv2/opencv.hpp"
#include<cmath>
#include<algorithm>

using namespace std;
using namespace cv;
using namespace boost::filesystem;

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
	dataset_paths.push_back("/home/diggy/git/opencv_extra/testdata/cv/barcode1D_dataset/zxing/zxing-2.2/core/test/data/blackbox/upca-1");


	for (int i = 0; i < dataset_paths.size (); i++)
	{
		path p = dataset_paths (i).c_str ();
		if (exists (p))
		{
			path GT = dataset_paths (i) + "/gt/";

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
					//got the image, now decode it using the function decode_image
					MultiFormatReader mf;
					Mat img = imread(temp_st,CV_LOAD_IMAGE_GRAYSCALE);
					string decode_out;
					vector<Point> zx_pts;
					decode_image(&mf,img ,decode_out,zx_pts);

					//read the GT for the same image
					string temp_gt = it_gt->string();
					FileStorage fs(temp_gt,FILESTORAGE::READ);
					
					vector<vector<Point> > gt_points;
					fs["corner_points"] >> gt_points;
					fs.release();
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
					float lenth = sqrt((x2-x1)*(x2-x1) + ((y2-y1)*(y2-y1)));
					float norm_x = (x2-x1)/lenth;
					float norm_y = (y2-y1)/lenth;
					int pnts_count = 0;		// if this is greater than or equal to 8, then PASS
					for(int j=0:j < 10;j++)
					{
						int new_x = (int)(x1 + (norm_x*lenth*j)/9.0);
						int new_y = (int)(y1 + (norm_y*lenth*j)/9.0);
						
						//check if the new coordinate is inside the quadrilateral or not.
						// the check will go like, 1->2, 2->3, 3->4 , 4->1

						//need to do this swap, because of the way points are stored in the ground truth
						swap(gt_points[2].x,gt_points[3].x);
						swap(gt_points[2].y,gt_points[3].y);

						//checking the cross product 
						vector<int> cross_p;
						for(int k=0;k<4;k++)
							cross_p.push_back((int)(((new_x-gt_points[k].x)*(gt_points[(k+1)%4].y-gt_points[k].y)) - ((gt_points[(k+1)%4].x-gt_points[k].x)*(new_y-gt_points[k].y))));

						//checking if all the cross products are of the same sign or not.
						int flag = 0;
						for(int k=0;k<4;k++)
							if(cross_p[k]*cross_p[(k+1)%4] < 0)
								flag = 1;

						if(flag == 0)
							pnts_count++;
					}
					
					EXPECT_GE(pnts_count,8);	
					
					if(pnts_count < 8)
						continue;

					//Condition #2: The two points given by the zxing result will draw a scan line across the barcode.
					//The points will lie closes to either the edge 1->4 or 2->3. If it is true, then it will PASS
					// this additional test.
					// Is this really necessary? Probably some other test.	
				}
				it_gt++;
			}
		}
		else
			cout << "PATH: " << dataset_paths (i) << " does not exist" << endl;
	}

	cout << "Done " << endl;
}


TEST (LOCALIZE_1DBARCODE, accuracy) {CV_BARCODE_LOCALIZETest test;test.safe_run ();}

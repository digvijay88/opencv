#include<iostream>
#include<cstdio>
#include<cv.h>
#include<highgui.h>
#include<boost/filesystem.hpp>
#include<vector>
#include<string>
#include<cstring>
#include<fstream>
#include "opencv2/opencv.hpp"
//#include "zxing_decode.h"


using namespace std;
using namespace cv;
using namespace boost::filesystem;

typedef vector<path> vec;
ofstream file;
FileStorage fs;
vector<vector<CvPoint> > corner_points;
vector<CvPoint> temp_points;
int point_count;

struct MatchPathSeparator
{
	bool operator()( char ch ) const
	{
		return ch == '/';
	}
};

struct RemoveExtension
{
	bool operator()(char ch) const
	{
		return ch == '.';
	}
};

string convertInt(int number)
{
	stringstream ss;//create a stringstream
	ss << number;//add number to the stream
	return ss.str();//return a string with the contents of the stream
}

void mousehandler(int event,int X, int Y,int flags,void *param)
{
	if(event == CV_EVENT_LBUTTONDOWN)
	{
		cout << X << " " << Y << " "<< endl;
		CvPoint pt;
		pt.x = X;
		pt.y = Y;
		temp_points.push_back(pt);
		point_count++;
		if(point_count%4 == 0)
		{
			corner_points.push_back(temp_points);
			temp_points.clear();
		}
		//		string st = convertInt(x) + ' ' + convertInt(y) + '\n';
		//		file << st;
	}
}

string basename( string const& pathname )
{
	//	cout << find_if(pathname.rbegin(), pathname.rend(),MatchPathSeparator()).base() << endl;
	string temp = string ( find_if( pathname.rbegin(), pathname.rend(), MatchPathSeparator() ).base(), pathname.end() );
	return string(temp.begin(),find_if(temp.rbegin(),temp.rend(),RemoveExtension()).base()-1);
}

int decide_directory(path p)
{
	int flag = 0;
	string temp = p.string();
	if(string::npos != temp.find("QVGA"))
		return 1;
	else if(string::npos != temp.find("VGA"))
		return 2;
	else if(string::npos != temp.find("UCSC_UPC_Dataset"))
		return 3;
	else if(string::npos != temp.find("zxing"))
		return 4;
	else
		return 0;
}


int main(int argc,char *argv[])
{
	path p (argv[1]);   // p reads clearer than argv[1] in the following code
	//	file.open("coordinates.txt");

	if (exists(p))    // does p actually exist?
	{
		if (is_directory(p))      // is p a directory?
		{
			cout << p << " is a directory. Thank You!!\n";
			vec v;
			copy(directory_iterator(p), directory_iterator(), back_inserter(v));
			//			cout << "hell yeah" << endl;
			vector<string> images_list;
			cvNamedWindow("AnnotateGT",CV_WINDOW_NORMAL);
			cvSetMouseCallback("AnnotateGT", mousehandler);
			//			cout << images_list.size() << endl;
			/*		int cnt = 0;*/

			int dir_flag = decide_directory(p);
			string format_temp;
			if(dir_flag == 4)
			{
				cout << "Enter the barcode format: ";
				cin >> format_temp;
			}
			cout << dir_flag << endl;
			int flgg = 0;
			for (vec::const_iterator it (v.begin()); it != v.end(); ++it)
			{
				string temp = it->string();
				//				cout << temp << endl;
				//				cout << basename(temp) << endl;
				if(temp.compare(temp.size()-4,4,".png") == 0 || temp.compare(temp.size()-4,4,".jpg") == 0)
				{
					cout << temp << endl;
					string f_name;
					bool barcode_present;
					vector<string> barcode_format, decode_out;
					corner_points.clear();
					temp_points.clear();
					point_count = 0;
					if(dir_flag == 1)		//blade/QVGA
					{
						f_name = basename(temp);
						decode_out.push_back(f_name.substr(0,f_name.size()-2));
						f_name = "./gt/" + f_name + ".yml";
						cout << "Filename is " << f_name << endl;
						cout << "The decode output is " << decode_out[0] << endl;
						barcode_present = true;
						barcode_format.push_back("UPC_A");
					}
					else if(dir_flag == 2)		//blade/VGA
					{
						f_name = basename(temp);	
						decode_out.push_back(f_name.substr(0,f_name.size()-1));
						f_name = "./gt/" + f_name + ".yml";
						cout << "Filename is " << f_name << endl;
						cout << "The decode output is " << decode_out[0] << endl;
						barcode_present = true;
						barcode_format.push_back("UPC_A");
					}
					else if(dir_flag == 3)		//manduchi
					{
						f_name = basename(temp);	
						decode_out.push_back(f_name.substr(0,f_name.find("_")));
						f_name = "./gt/" + f_name + ".yml";
						cout << "Filename is " << f_name << endl;
						cout << "The decode output is " << decode_out[0] << endl;
						barcode_present = true;
						barcode_format.push_back("UPC_A");
					}
					else if(dir_flag == 4)		//zxing
					{
						f_name = "./gt/" + basename(temp) + ".yml";
						cout << "Filename is " << f_name << endl;
						//						if(flgg == 0)
						//						{
						//							if(basename(temp) == "5")
						//								flgg = 1;
						//							continue;
						//						}

						string inStr = temp.substr(0,temp.size()-3) + "txt";
						cout << inStr << endl;
						ifstream inFile;
						inFile.open(inStr.c_str());
						string one_line;
						getline(inFile,one_line);
						cout << one_line << endl;
						inFile.close();
						decode_out.push_back(one_line);
						barcode_format.push_back(format_temp);
						barcode_present = true;
					}
					else
					{
						cout << "NOT A DATASET DIRECTORY" << endl;
						return 0;
					}

					Mat img = imread(temp);
					imshow("AnnotateGT",img);
					cvWaitKey(0);

					fs.open(f_name,FileStorage::WRITE);

					fs << "barcode_present" << barcode_present;
					fs << "barcode_format" << "[";
					for(int i=0;i < barcode_format.size();i++)
						fs << "[:" << barcode_format[i] << "]";
					fs << "]";

					fs << "decode_out" << "[:";
					for(int i=0;i < decode_out.size();i++)
						fs << "[:" << decode_out[i] << "]";
					fs << "]";

					fs << "corner_points" << "[";
					for(int i=0;i < corner_points.size();i++)
					{
						fs << "[:";
						for(int j=0;j < corner_points[i].size();j++)
						{
							//							cout << corner_points[i][j];
							fs << "[:" << corner_points[i][j].x << corner_points[i][j].y << "]";
						}
						fs << "]";
					}
					fs << "]";

					fs.release();
					//					if(!parse_with_zxing(img))
					//					{
					//						cout << "Zxing Failed in decoding" << endl;
					//					}
				}
			}
			cvDestroyWindow("AnnotateGT");
		}
		else
			cout << p << " is not a directory. Please pass a directory path as argument\n";
	}
	else
		cout << p << " does not exist\n";
	//	file.close();
	return 0;
}

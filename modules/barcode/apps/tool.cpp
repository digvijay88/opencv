#include<iostream>
#include<cstdio>
#include<cv.h>
#include<highgui.h>
#include<boost/filesystem.hpp>
#include<vector>
#include<string>
#include<cstring>
#include<fstream>
//#include "zxing_decode.h"

using namespace std;
using namespace cv;
using namespace boost::filesystem;

typedef vector<path> vec;

ofstream file;

string convertInt(int number)
{
	stringstream ss;//create a stringstream
	ss << number;//add number to the stream
	return ss.str();//return a string with the contents of the stream
}

void mousehandler(int event,int x, int y,int flags,void *param)
{
	if(event == CV_EVENT_LBUTTONDOWN)
	{
		cout << x << " " << y << " "<< endl;
	       	string st = convertInt(x) + ' ' + convertInt(y) + '\n';
		file << st;
	}
}

int main(int argc,char *argv[])
{
	path p (argv[1]);   // p reads clearer than argv[1] in the following code
	file.open("coordinates.txt");

	if (exists(p))    // does p actually exist?
	{
		if (is_directory(p))      // is p a directory?
		{
			cout << p << " is a directory. Thank You!!\n";
			vec v;
			copy(directory_iterator(p), directory_iterator(), back_inserter(v));
			vector<string> images_list;
			cvNamedWindow("AnnotateGT",CV_WINDOW_NORMAL);
			cvSetMouseCallback("AnnotateGT", mousehandler);
			int cnt = 0;
			for (vec::const_iterator it (v.begin()); it != v.end(); ++it)
			{
				//				cout << "   " << *it << '\n';
				string temp = it->string();
				if(temp.compare(temp.size()-4,4,".png") == 0 || temp.compare(temp.size()-4,4,".jpg") == 0)
				{
					cout << temp << endl;
					Mat img = imread(temp);
					temp = temp + '\n';
					file << temp;
					imshow("AnnotateGT",img);
					cvWaitKey(0);
					if(cnt == 2)
						break;
					cnt++;
					//					img.deallocate();	
				}
			}
			cvDestroyWindow("AnnotateGT");
		}
		else
			cout << p << " is not a directory. Please pass a directory path as argument\n";
	}
	else
		cout << p << " does not exist\n";
	file.close();
	return 0;
}

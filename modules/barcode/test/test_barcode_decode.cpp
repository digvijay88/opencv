#include "test_precomp.hpp"
#include <fstream>

using namespace std;
using namespace cv;

class CV_BARCODE_DECODETest : public cvtest::BaseTest
{
public:
	CV_BARCODE_DECODETest();
	~CV_BARCODE_DECODETest();
protected:
	void run();
};


CV_BARCODE_DECODETest::CV_BARCODE_DECODETest() {}
CV_BARCODE_DECODETest::~CV_BARCODE_DECODETest() {}

void CV_BARCODE_DECODETest::run()
{
	ifstream ifs1,ifs2;
	
	ifs1.open("in1.txt",ifstream::in);	
	ifs2.open("in2.txt",ifstream::in);	
	
	string s1,s2;
	while(getline(ifs1,s1))
	{
		getline(ifs2,s2);

		EXPECT_STREQ(s1.c_str(),s2.c_str());
	}	

	ifs1.close();
	ifs2.close();
}


TEST(Decode_1DBARCODE, accuracy) { CV_BARCODE_DECODETest test; test.safe_run(); }

//This testing code opens two text files and matches the corresponding lines.

#include <stdio.h>
#include<iostream>
#include<fstream>

using namespace std;

#include "gtest/gtest.h"

using ::testing::EmptyTestEventListener;
using ::testing::InitGoogleTest;
using ::testing::Test;
using ::testing::TestCase;
using ::testing::TestEventListeners;
using ::testing::TestInfo;
using ::testing::TestPartResult;
using ::testing::UnitTest;

ifstream ifs1, ifs2;
int indx1, indx2;

namespace {

	TEST(StringTest,equality)
	{
		string s1,s2;
		while(getline(ifs1,s1))
		{
			getline(ifs2,s2);

			EXPECT_STREQ(s1.c_str(),s2.c_str());					
		}
	}


}  // namespace

int main(int argc, char **argv) 
{

	InitGoogleTest(&argc, argv);

	ifs1.open(argv[1],ifstream::in);
	ifs2.open(argv[2],ifstream::in);


	int ret_val = RUN_ALL_TESTS();

	ifs1.close();
	ifs2.close();

	// This is an example of using the UnitTest reflection API to inspect test
	// results. Here we discount failures from the tests we expected to fail.
	UnitTest& unit_test = *UnitTest::GetInstance();
	int unexpectedly_failed_tests = 0;

	//	cout << "Total unit tests " << unit_test.total_test_case_count() << endl;

	for (int i = 0; i < unit_test.total_test_case_count(); ++i) 
	{
		const TestCase& test_case = *unit_test.GetTestCase(i);
		//		cout << "Total unit test cases " << test_case.total_test_count() << endl;
		for (int j = 0; j < test_case.total_test_count(); ++j) 
		{
			const TestInfo& test_info = *test_case.GetTestInfo(j);
			// Counts failed tests that were not meant to fail (those without
			// 'Fails' in the name).
			if (test_info.result()->Failed())
				unexpectedly_failed_tests++;
		}
	}

	cout << "Number of failed tests are " << unexpectedly_failed_tests << endl;

	// Test that were meant to fail should not affect the test program outcome.
	if (unexpectedly_failed_tests == 0)
		ret_val = 0;

	return ret_val;
}

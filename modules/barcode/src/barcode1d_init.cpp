#include "precomp.hpp"

using namespace cv;
using namespace zxing;

//Detector
Ptr<Barcode1D> Barcode1D::create(const std::string& locator_type)
{
  return Algorithm::create<Barcode1D> ("Barcode1D." + locator_type);
}

///Giving AlgorithmInfo for various barcode detectors
//zxing AlgorithmInfo

CV_INIT_ALGORITHM(ZXING,"Barcode1D.zxing",
		  obj.info()->addParam(obj,"readerType",obj.readerType)
		  obj.info()->addParam(obj,"DecodeHints",obj.DecodeHints));


//Decoder

//////////////////////////////initModule function////////////////////

bool cv::initModule_barcode1d(void)
{
	bool all = true;
	all &= !ZXING_info_auto.name().empty();

	return all;
}

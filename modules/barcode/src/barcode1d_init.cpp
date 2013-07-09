#include "precomp.hpp"

using namespace cv;
using namespace zxing;

//Detector
Ptr<Detector1D> Detector1D::create(const std::string& locator_type)
{
  return Algorithm::create<Detector1D> ("Detector1D." + locator_type);
}

///Giving AlgorithmInfo for various barcode detectors
//zxing AlgorithmInfo


CV_INIT_ALGORITHM(ZXINGdetect,"Detector1D.zxing",
		  obj.info()->addParam(obj,"readerType",obj.readerType));


//Decoder

//////////////////////////////initModule function////////////////////

bool cv::initModule_barcode1d(void)
{
	bool all = true;
	all &= !ZXINGdetect_info_auto.name().empty();

	return all;
}

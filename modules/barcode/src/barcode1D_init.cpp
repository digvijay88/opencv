#include "precomp.hpp"

using namespace cv;
using namespace barcode;

//Detector
Ptr<Barcode1D> Barcode1D::create(const String& locator_type)
{
  return Algorithm::create<Barcode1D> ("Barcode1D." + locator_type);
}

///Giving AlgorithmInfo for various barcode detectors
//zxing AlgorithmInfo

CV_INIT_ALGORITHM(ZXING_WRAP, "Barcode1D.ZXING_WRAP",
//		  obj.info()->addParam(obj,"readerType",obj.readerType);
		  obj.info()->addParam(obj,"decode_hints",obj.decode_hints));


CV_INIT_ALGORITHM(KatonaNyu, "Barcode1D.KatonaNyu",
		  obj.info()->addParam(obj,"gausskernelSize",obj.gausskernelSize);
		  obj.info()->addParam(obj,"sigma",obj.sigma));

//////////////////////////////initModule function////////////////////

bool cv::barcode::initModule_barcode1D(void)
{
	bool all = true;
	all &= !ZXING_WRAP_info_auto.name().empty();
	all &= !KatonaNyu_info_auto.name().empty();

	return all;
}

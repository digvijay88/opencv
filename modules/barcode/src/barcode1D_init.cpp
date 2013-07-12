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
		  obj.info()->addParam(obj,"DecodeHints",obj.DecodeHints));


//Decoder

//////////////////////////////initModule function////////////////////

bool initModule_barcode1D(void)
{
	bool all = true;
	all &= !ZXING_WRAP_info_auto.name().empty();

	return all;
}

#ifndef ZXING_DECODE_H
#define ZXING_DECODE_H

#include<string>
#include<fstream>
#include<iostream>
#include<vector>
#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include<boost/filesystem.hpp>

using namespace cv;
using namespace std;
using namespace boost::filesystem;

//////////////ZXING BARCODE READER////////////////////////////////////////////////////
#include "zxing/LuminanceSource.h"
#include "zxing/MultiFormatReader.h"
#include "zxing/oned/OneDReader.h"
#include "zxing/oned/EAN8Reader.h"
#include "zxing/oned/EAN13Reader.h"
#include "zxing/oned/Code128Reader.h"
#include "zxing/oned/UPCAReader.h"
#include "zxing/datamatrix/DataMatrixReader.h"
#include "zxing/qrcode/QRCodeReader.h"
#include "zxing/aztec/AztecReader.h"
#include "zxing/common/GlobalHistogramBinarizer.h"
#include "zxing/Exception.h"
#include "zxing/DecodeHints.h"

using namespace zxing;
using namespace oned;
using namespace datamatrix;
using namespace qrcode;
using namespace aztec;


/**
 * Just a utility return the ascii string corresponding to a BarCodeType
 * @param bctype    Enumerated value of the barcode we want an ascii stream for .
 * @return           string of BarCodeType
 */
/*std::string Barcode1D::getBarcodeName(BarCodeType bctype)
{
	enum BarCodeType m = BarCodeType_MAX;
	if((bctype < 0)||(bctype > m+1)) bctype = ALL;
	string s(barcodeNames[bctype]);
	return s;
}*/

class OpenCVBitmapSource : public LuminanceSource
{
	private:
		cv::Mat m_pImage;

	public:
		OpenCVBitmapSource(cv::Mat &image);

		~OpenCVBitmapSource();

		int getWidth() const;
		int getHeight() const; 

		ArrayRef<char> getRow(int y, ArrayRef<char> row) const;

		ArrayRef<char> getMatrix() const;
		/*
		// The following methods are not supported by this demo (the DataMatrix Reader doesn't call these methods)
		bool isCropSupported() const { return false; }
		Ref<LuminanceSource> crop(int left, int top, int width, int height) {}
		bool isRotateSupported() const { return false; }
		Ref<LuminanceSource> rotateCounterClockwise() {}
		 */
};


//This is your barcode reader call to Zxing C++ version
CV_EXPORTS void decode_image(Reader *reader, cv::Mat &image, std::string &bar_read, vector<Point> &pts);

#endif

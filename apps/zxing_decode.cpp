/**
 * Gary Bradski, Reading 1D barcodes
 *   License BSD, (c) 2013
 *
 *   Working example of how to call zxing using OpenCV 2.4+ cv::Mat
 *
 * Calling example, this one for 128 barcodes:
 *
 *   Code128Reader cr; //Instantiate a zxing barcode reader, int this case for 128 barcodes, but you can use any of their 1D or multi readers here
 *   ... by magic, I find, rectify and islotate a barcode into cv::Mat barcodeImage ...
 *   decode_image(&cr, barcodeImage);  //Decode the isolated rectified barcode or fail
 *
 */

#include<string>
#include<fstream>
#include<iostream>
#include<vector>
#include<cv.h>
#include<highgui.h>

using namespace cv;
using namespace std;

//////////////ZXING BARCODE READER////////////////////////////////////////////////////
#include <zxing/LuminanceSource.h>
#include <zxing/MultiFormatReader.h>
#include <zxing/oned/OneDReader.h>
#include <zxing/oned/EAN8Reader.h>
#include <zxing/oned/EAN13Reader.h>
#include <zxing/oned/Code128Reader.h>
#include<zxing/oned/UPCAReader.h>
#include <zxing/datamatrix/DataMatrixReader.h>
#include <zxing/qrcode/QRCodeReader.h>
#include <zxing/aztec/AztecReader.h>
#include <zxing/common/GlobalHistogramBinarizer.h>
#include <zxing/Exception.h>
#include <zxing/DecodeHints.h>

using namespace zxing;
using namespace oned;
using namespace datamatrix;
using namespace qrcode;
using namespace aztec;

namespace{  //Don't pollute the namespace, hide names that are only needed locally
	const char* barcodeNames[] = {
		"ALL",
		"AZTEC",
		"CODABAR",
		"CODE_39",
		"CODE_93",
		"CODE_128",
		"DATA_MATRIX",
		"EAN_8",
		"EAN_13",
		"ITF",
		"MAXICODE",
		"PDF_417",
		"QR_CODE",
		"RSS_14",
		"RSS_EXPANDED",
		"UPC_A",
		"UPC_E",
		"UPC_EAN_EXTENSION"
	};
}

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
		OpenCVBitmapSource(cv::Mat &image)
			: LuminanceSource(image.cols, image.rows)
		{
			m_pImage = image.clone();
		}

		~OpenCVBitmapSource()
		{
		}

		int getWidth() const { return m_pImage.cols; }
		int getHeight() const { return m_pImage.rows; }

		ArrayRef<char> getRow(int y, ArrayRef<char> row) const
		{
			int width_ = getWidth();
			if (!row)
				row = ArrayRef<char>(width_);

			const char *p = m_pImage.ptr<char>(y);
			for(int x = 0; x<width_; ++x, ++p)
				row[x] = *p;
			return row;
		}

		ArrayRef<char> getMatrix() const
		{
			int width_ = getWidth();
			int height_ =  getHeight();
			ArrayRef<char> matrix = ArrayRef<char>(width_*height_);
			for (int y = 0; y < height_; ++y)
			{
				const char *p = m_pImage.ptr<char>(y);
				for(int x = 0; x < width_; ++x, ++p)
				{
					matrix[y*width_ + x] = *p;
				}
			}
			return matrix;
		}
		/*
		// The following methods are not supported by this demo (the DataMatrix Reader doesn't call these methods)
		bool isCropSupported() const { return false; }
		Ref<LuminanceSource> crop(int left, int top, int width, int height) {}
		bool isRotateSupported() const { return false; }
		Ref<LuminanceSource> rotateCounterClockwise() {}
		 */
};

static int bccnt = 0, bcread = 0, bcnoread = 0;  //Get rid of these, I just use them to keep statistics during testing

//This is your barcode reader call to Zxing C++ version
void decode_image(Reader *reader, cv::Mat &image, std::string &bar_read)
{
	int bread = 1;
	try
	{
		reader = new MultiFormatReader;
		Ref<OpenCVBitmapSource> source(new OpenCVBitmapSource(image));
		Ref<Binarizer> binarizer(new GlobalHistogramBinarizer(source));
		Ref<BinaryBitmap> bitmap(new BinaryBitmap(binarizer));
		cout << "here" << endl;
		Ref<Result> result(reader->decode(bitmap, DecodeHints(DecodeHints::TRYHARDER_HINT)));//+DecodeHints::DEFAULT_HINT)));
		bar_read = result->getText()->getText();  //Note that double getText, yes, if you look into zxing code, 2 indirections are needed
		bcread += 1;
	}
	catch (zxing::Exception& e)
	{
		bar_read = "FAIL";  //The way I indicate that we failed to read a barcode
		bread = 0;
		bcnoread += 1;
//		cerr << "Error: " << e.what() << endl;
	}

	bccnt += 1;
//	printf("bc%d: G:%d, B:%d %f\n",bccnt,bcread,bcnoread,(float)bcread/(float)bccnt); //Just for debug statistics, get rid of this
//	return bread;
}

enum BarCodeType
{
	ALL,                                //!< ALL
	AZTEC, //2D
	CODABAR,                           //!< CODABAR
	CODE_39,                           //!< CODE_39
	CODE_93,                           //!< CODE_93
	CODE_128,                          //!< CODE_128
	DATA_MATRIX,                       //!< DATA_MATRIX
	EAN_8,                             //!< EAN_8
	EAN_13,                            //!< EAN_13
	ITF,                               //!< ITF
	MAXICODE,                          //!< MAXICODE
	PDF_417,                           //!< PDF_417
	QR_CODE,                           //!< QR_CODE
	RSS_14,                            //!< RSS_14
	RSS_EXPANDED,                      //!< RSS_EXPANDED
	UPC_A,                             //!< UPC_A
	UPC_E,                             //!< UPC_E
	UPC_EAN_EXTENSION,                 //!< UPC_EAN_EXTENSION
	BarCodeType_MAX = UPC_EAN_EXTENSION//!< BarCodeType_MAX
};

int main(int argc, char** argv)
{
	string infile = argv[1];
	cout << infile << endl;
	Mat img = imread(infile,CV_LOAD_IMAGE_GRAYSCALE);
	if(!img.data)
	{
		cout << "cannot load the file. Wrong path." << endl;
		return 0;
	}

	MultiFormatReader *mf;
	string out;
	cout << "Decoding the image" << endl;
	decode_image(mf,img,out);
	cout << "Output " << out << endl;
	return 0;
}

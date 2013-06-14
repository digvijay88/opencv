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

#include "zxing_decode.h"

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

OpenCVBitmapSource::OpenCVBitmapSource(cv::Mat &image): LuminanceSource(image.cols, image.rows)
{
	m_pImage = image.clone();
}

OpenCVBitmapSource::~OpenCVBitmapSource()
{
}

int OpenCVBitmapSource:: getWidth() const
{
	return m_pImage.cols;
}

int OpenCVBitmapSource:: getHeight() const
{
	return m_pImage.rows;
}


ArrayRef<char> OpenCVBitmapSource::getRow(int y, ArrayRef<char> row) const
{
	int width_ = getWidth();
	if (!row)
		row = ArrayRef<char>(width_);

	const char *p = m_pImage.ptr<char>(y);
	for(int x = 0; x<width_; ++x, ++p)
		row[x] = *p;
	return row;
}

ArrayRef<char> OpenCVBitmapSource::getMatrix() const
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


//This is your barcode reader call to Zxing C++ version
void decode_image(Reader *reader, cv::Mat &image, std::string &bar_read)
{
	int bread = 1;
	try
	{
		Ref<OpenCVBitmapSource> source(new OpenCVBitmapSource(image));
		Ref<Binarizer> binarizer(new GlobalHistogramBinarizer(source));
		Ref<BinaryBitmap> bitmap(new BinaryBitmap(binarizer));
		cout << "here" << endl;
		Ref<Result> result(reader->decode(bitmap, DecodeHints(DecodeHints::TRYHARDER_HINT)));//+DecodeHints::DEFAULT_HINT)));
		bar_read = result->getText()->getText();  //Note that double getText, yes, if you look into zxing code, 2 indirections are needed
	}
	catch (zxing::Exception& e)
	{
		bar_read = "FAIL";  //The way I indicate that we failed to read a barcode
		//		cerr << "Error: " << e.what() << endl;
	}

}

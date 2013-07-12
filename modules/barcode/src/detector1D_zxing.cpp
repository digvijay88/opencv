/*#**********************************************************************************************
** IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
**
** By downloading, copying, installing or using the software you agree to this license.
** If you do not agree to this license, do not download, install,
** copy or use the software.
**
**
**                          License Agreement
**               For Open Source Computer Vision Library
**
** Copyright (C) 2000-2008, Intel Corporation, all rights reserved.
** Copyright (C) 2008-2011, Willow Garage Inc., all rights reserved.
** Third party copyrights are property of their respective owners.
** 
** Redistribution and use in source and binary forms, with or without modification,
** are permitted provided that the following conditions are met:
** 
**   * Redistributions of source code must retain the above copyright notice,
**     this list of conditions and the following disclaimer.
** 
**   * Redistributions in binary form must reproduce the above copyright notice,
**     this list of conditions and the following disclaimer in the documentation
**     and/or other materials provided with the distribution.
** 
**   * The name of the copyright holders may not be used to endorse or promote products
**     derived from this software without specific prior written permission.
** 
** This software is provided by the copyright holders and contributors "as is" and
** any express or implied warranties, including, but not limited to, the implied
** warranties of merchantability and fitness for a particular purpose are disclaimed.
** In no event shall the Intel Corporation or contributors be liable for any direct,
** indirect, incidental, special, exemplary, or consequential damages
** (including, but not limited to, procurement of substitute goods or services;
** loss of use, data, or profits; or business interruption) however caused
** and on any theory of liability, whether in contract, strict liability,
** or tort (including negligence or otherwise) arising in any way out of
** the use of this software, even if advised of the possibility of such damage.
**  
************************************************************************************************/
/*#**********************************************************************************************
**                Creation - enhancement process 2012-2013                                      *
**                                                                                              *
** Authors: Claudia Rapuano (c.rapuano@gmail.com), University La Sapienza di Roma, Rome, Italy  *
** 	        Stefano Fabri (s.fabri@email.it), Rome, Italy                                       *
************************************************************************************************/

#include "precomp.hpp"

class OpenCVBitmapSource : public LuminanceSource
{
private:
  cv::Mat m_pImage;

public:
  OpenCVBitmapSource(cv::Mat &image) : LuminanceSource(image.cols, image.rows)
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


namespace cv
{

namespace barcode {

Detector1D::~Detector1D() {
  // TODO Auto-generated destructor stub
}

Ptr<Detector1D> Detector1D::create( const std::string& locator_type )
{
  if(locator_type.find("zxing") == 0)
  {
    return Algorithm::create<Detector1D> ("Detector1D." + locator_type);
  }
  //  (void) locator_type;
  return NULL;
}

void Detector1D::locate(const Mat& image, std::vector<RotatedRect>& barcodes)
{
  MultiFormatReader reader;
  Mat image_gray;
  cvtColor(image,image_gray,CV_BGR2GRAY);
  Ref<OpenCVBitmapSource> source(new OpenCVBitmapSource(image_gray));
  Ref<Binarizer> binarizer(new GlobalHistogramBinarizer(source));
  Ref<BinaryBitmap> bitmap(new BinaryBitmap(binarizer));

  Ref<Result> result(reader->decode(bitmap, DecodeHints(DecodeHints::TRYHARDER_HINT)));
  ArrayRef< Ref<ResultPoint> >& points (result->getResultPoints());

}

}
}

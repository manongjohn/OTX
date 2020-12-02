#include "iwa_bloomfx.h"

#include "tparamuiconcept.h"

#include <QVector>
#include <QPair>

namespace {
// convert sRGB color space to power space
template <typename T = double>
inline T to_linear_color_space(T nonlinear_color, T exposure, T gamma) {
  return std::pow(nonlinear_color, gamma) / exposure;
}
// convert power space to sRGB color space
template <typename T = double>
inline T to_nonlinear_color_space(T linear_color, T exposure, T gamma) {
  return std::pow(linear_color * exposure, T(1) / gamma);
}
template <class T = double>
const T &clamp(const T &v, const T &lo, const T &hi) {
  assert(!(hi < lo));
  return (v < lo) ? lo : (hi < v) ? hi : v;
}

void blurByRotate(cv::Mat &mat) {
  double angle = 45.0;

  int size   = std::ceil(std::sqrt(mat.cols * mat.cols + mat.rows * mat.rows));
  int width  = ((size - mat.cols) % 2 == 0) ? size : size + 1;
  int height = ((size - mat.rows) % 2 == 0) ? size : size + 1;

  cv::Point2f center((mat.cols - 1) / 2.0, (mat.rows - 1) / 2.0);

  cv::Mat rot = cv::getRotationMatrix2D(center, angle, 1.0);
  rot.at<double>(0, 2) += (width - mat.cols) / 2.0;
  rot.at<double>(1, 2) += (height - mat.rows) / 2.0;

  cv::Mat tmp;
  cv::warpAffine(mat, tmp, rot, cv::Size(width, height));

  center = cv::Point2f((width - 1) / 2.0, (height - 1) / 2.0);
  rot    = cv::getRotationMatrix2D(center, -angle, 1.0);
  rot.at<double>(0, 2) += (mat.cols - width) / 2.0;
  rot.at<double>(1, 2) += (mat.rows - height) / 2.0;

  cv::warpAffine(tmp, mat, rot, mat.size());
}

}  // namespace

//--------------------------------------------
// Iwa_BloomFx
//--------------------------------------------
Iwa_BloomFx::Iwa_BloomFx()
    : m_gamma(2.2), m_gain(2.0), m_size(100.0), m_alpha_rendering(false) {
  addInputPort("Source", m_source);
  bindParam(this, "gamma", m_gamma);
  bindParam(this, "gain", m_gain);
  bindParam(this, "size", m_size);
  bindParam(this, "alpha_rendering", m_alpha_rendering);

  m_gamma->setValueRange(0.1, 5.0);
  m_gain->setValueRange(0.1, 10.0);
  m_size->setValueRange(0.1, 1024.0);

  m_size->setMeasureName("fxLength");
}
//------------------------------------------------

double Iwa_BloomFx::getSizePixelAmount(const double val, const TAffine affine) {
  /*--- Convert to vector --- */
  TPointD vect;
  vect.x = val;
  vect.y = 0.0;
  /*--- Apply geometrical transformation ---*/
  // For the following lines I referred to lines 586-592 of
  // sources/stdfx/motionblurfx.cpp
  TAffine aff(affine);
  aff.a13 = aff.a23 = 0; /* ignore translation */
  vect              = aff * vect;
  /*--- return the length of the vector ---*/
  return sqrt(vect.x * vect.x + vect.y * vect.y);
}
//------------------------------------------------

template <typename RASTER, typename PIXEL>
void Iwa_BloomFx::setSourceTileToMat(const RASTER ras, cv::Mat &imgMat,
                                     const double gamma) {
  double maxi = static_cast<double>(PIXEL::maxChannelValue);  // 255or65535
  for (int j = 0; j < ras->getLy(); j++) {
    const PIXEL *pix = ras->pixels(j);
    cv::Vec3f *mat_p = imgMat.ptr<cv::Vec3f>(j);
    for (int i = 0; i < ras->getLx(); i++, pix++, mat_p++) {
      double pix_a = static_cast<double>(pix->m) / maxi;
      if (pix_a <= 0.0) {
        *mat_p = cv::Vec3f(0, 0, 0);
        continue;
      }
      double bgra[3];
      bgra[0] = static_cast<double>(pix->b) / maxi;
      bgra[1] = static_cast<double>(pix->g) / maxi;
      bgra[2] = static_cast<double>(pix->r) / maxi;
      for (int c = 0; c < 3; c++) {
        // assuming that the source image is premultiplied
        bgra[c] = to_linear_color_space(bgra[c] / pix_a, 1.0, gamma) * pix_a;
      }
      *mat_p = cv::Vec3f(bgra[0], bgra[1], bgra[2]);
    }
  }
}
//------------------------------------------------
template <typename RASTER, typename PIXEL>
void Iwa_BloomFx::setMatToOutput(const RASTER ras, const RASTER srcRas,
                                 cv::Mat &ingMat, const double gamma,
                                 const double gain, const bool withAlpha,
                                 const int margin) {
  double maxi = static_cast<double>(PIXEL::maxChannelValue);  // 255or65535
  for (int j = 0; j < ras->getLy(); j++) {
    cv::Vec3f const *mat_p = ingMat.ptr<cv::Vec3f>(j);
    PIXEL *pix             = ras->pixels(j);
    PIXEL *srcPix          = srcRas->pixels(j + margin) + margin;

    for (int i = 0; i < ras->getLx(); i++, pix++, srcPix++, mat_p++) {
      double nonlinear_b =
          to_nonlinear_color_space((double)(*mat_p)[0] * gain, 1.0, gamma);
      double nonlinear_g =
          to_nonlinear_color_space((double)(*mat_p)[1] * gain, 1.0, gamma);
      double nonlinear_r =
          to_nonlinear_color_space((double)(*mat_p)[2] * gain, 1.0, gamma);

      nonlinear_b = clamp(nonlinear_b, 0.0, 1.0);
      nonlinear_g = clamp(nonlinear_g, 0.0, 1.0);
      nonlinear_r = clamp(nonlinear_r, 0.0, 1.0);

      pix->r = (typename PIXEL::Channel)(nonlinear_r * (maxi + 0.999999));
      pix->g = (typename PIXEL::Channel)(nonlinear_g * (maxi + 0.999999));
      pix->b = (typename PIXEL::Channel)(nonlinear_b * (maxi + 0.999999));
      if (withAlpha) {
        double chan_a =
            std::max(std::max(nonlinear_b, nonlinear_g), nonlinear_r);
        pix->m = std::max((typename PIXEL::Channel)(chan_a * (maxi + 0.999999)),
                          srcPix->m);
      } else
        pix->m = (typename PIXEL::Channel)(PIXEL::maxChannelValue);
    }
  }
}

//------------------------------------------------

void Iwa_BloomFx::doCompute(TTile &tile, double frame,
                            const TRenderSettings &settings) {
  // If the source is not connected, then do nothing
  if (!m_source.isConnected()) {
    tile.getRaster()->clear();
    return;
  }
  // obtain parameters
  double gamma = m_gamma->getValue(frame);
  double gain  = m_gain->getValue(frame);
  double size  = getSizePixelAmount(m_size->getValue(frame), settings.m_affine);
  bool withAlpha = m_alpha_rendering->getValue();

  int margin = static_cast<int>(std::ceil(size));
  TRectD _rect(tile.m_pos, TDimensionD(tile.getRaster()->getLx(),
                                       tile.getRaster()->getLy()));
  _rect = _rect.enlarge(static_cast<double>(margin));
  TDimensionI dimSrc(static_cast<int>(_rect.getLx() + 0.5),
                     static_cast<int>(_rect.getLy() + 0.5));

  // obtain the source tile
  TTile sourceTile;
  m_source->allocateAndCompute(sourceTile, _rect.getP00(), dimSrc,
                               tile.getRaster(), frame, settings);

  // set the source image to cvMat, converting to linear color space
  cv::Mat imgMat(cv::Size(dimSrc.lx, dimSrc.ly), CV_32FC3);
  TRaster32P ras32 = tile.getRaster();
  TRaster64P ras64 = tile.getRaster();
  if (ras32)
    setSourceTileToMat<TRaster32P, TPixel32>(sourceTile.getRaster(), imgMat,
                                             gamma);
  else if (ras64)
    setSourceTileToMat<TRaster64P, TPixel64>(sourceTile.getRaster(), imgMat,
                                             gamma);

  // compute size and intensity ratios of resampled layers
  // resample size is reduced from the specified size, taking into account
  // that the gaussian blur (x 2) and the blur by rotation resampling (x sqrt2)
  double no_blur_size = size / (2 * 1.5);
  // find the mimimum "power of 2" value which is the same as or larger than the
  // filter size
  int level         = 1;
  double power_of_2 = 1.0;
  while (1) {
    if (power_of_2 >= no_blur_size) break;
    level++;
    power_of_2 *= 2.0;
  }
  // store the size of resampled layers
  QVector<cv::Size> sizes;
  double tmp_filterSize = no_blur_size;
  double width          = static_cast<double>(imgMat.size().width);
  double height         = static_cast<double>(imgMat.size().height);
  for (int lvl = 0; lvl < level - 1; lvl++) {
    int tmp_w = static_cast<int>(std::ceil(width / tmp_filterSize));
    int tmp_h = static_cast<int>(std::ceil(height / tmp_filterSize));
    sizes.push_front(cv::Size(tmp_w, tmp_h));
    tmp_filterSize *= 0.5;
  }
  sizes.push_front(imgMat.size());

  // the filter is based on the nearest power-of-2 sized one with an adjustment
  // reducing the sizes and increasing the intensity with this ratio
  double ratio = power_of_2 / no_blur_size;
  // base filter sizes will be 1, 2, 4, ... 2^(level-1)
  // intensity of the filter with sizes > 2
  double intensity_all = power_of_2 / (power_of_2 * 2.0 - 1.0);
  // intensity of the filter with size 1, so that the amount of the filter at
  // the center point is always 1.0
  double intensity_front = 1.0 - (1.0 - intensity_all) * ratio;

  std::vector<cv::Mat> dst(level);
  cv::Size const ksize(3, 3);

  cv::Mat tmp;
  int i;
  // for each level of filter (from larger to smaller)
  for (i = 0; i < level;) {
    // scaling down the size
    if (i) {
      cv::resize(imgMat, tmp, sizes[i], 0.0, 0.0, cv::INTER_AREA);
      imgMat = tmp;
    }
    // gaussian blur
    cv::GaussianBlur(imgMat, dst[i], ksize, 0.0);
    ++i;
  }
  // for each level of filter (from smaller to larger)
  for (--i; i > 0; --i) {
    // scaling up the size
    cv::resize(dst[i], tmp, dst[i - 1].size());
    // blur by rotational resampling in order to reduce box-shaped artifact
    blurByRotate(tmp);
    // add to the upper resampled image
    if (i > 1)
      dst[i - 1] += tmp;
    else
      imgMat = dst[0] * intensity_front + tmp * intensity_all;
  }

  // get the subimage without margin
  cv::Rect roi(cv::Point(margin, margin),
               cv::Size(tile.getRaster()->getLx(), tile.getRaster()->getLy()));
  imgMat = imgMat(roi);

  // set the result to the tile, converting to rgb channel values
  if (ras32)
    setMatToOutput<TRaster32P, TPixel32>(tile.getRaster(),
                                         sourceTile.getRaster(), imgMat, gamma,
                                         gain, withAlpha, margin);
  else if (ras64)
    setMatToOutput<TRaster64P, TPixel64>(tile.getRaster(),
                                         sourceTile.getRaster(), imgMat, gamma,
                                         gain, withAlpha, margin);
}
//------------------------------------------------

bool Iwa_BloomFx::doGetBBox(double frame, TRectD &bBox,
                            const TRenderSettings &info) {
  if (!m_source.isConnected()) {
    bBox = TRectD();
    return false;
  }
  bool ret   = m_source->doGetBBox(frame, bBox, info);
  int margin = static_cast<int>(
      std::ceil(getSizePixelAmount(m_size->getValue(frame), info.m_affine)));
  if (margin > 0) {
    bBox = bBox.enlarge(static_cast<double>(margin));
  }
  return ret;
}
//------------------------------------------------

bool Iwa_BloomFx::canHandle(const TRenderSettings &info, double frame) {
  return false;
}
//------------------------------------------------

void Iwa_BloomFx::getParamUIs(TParamUIConcept *&concepts, int &length) {
  concepts = new TParamUIConcept[length = 1];

  concepts[0].m_type  = TParamUIConcept::RADIUS;
  concepts[0].m_label = "Size";
  concepts[0].m_params.push_back(m_size);
}
//------------------------------------------------

FX_PLUGIN_IDENTIFIER(Iwa_BloomFx, "iwa_BloomFx")
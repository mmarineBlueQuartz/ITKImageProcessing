/* ============================================================================
 * Copyright (c) 2019 BlueQuartz Software, LLC
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice, this
 * list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * Neither the names of any of the BlueQuartz Software contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS �AS IS�
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#pragma once

#include "itkAmoebaOptimizer.h"
#include "itkFFTConvolutionImageFilter.h"

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/SIMPLib.h"

#include "ITKImageProcessing/ITKImageProcessingConstants.h"
#include "ITKImageProcessing/ITKImageProcessingDLLExport.h"
#include "ITKImageProcessing/ITKImageProcessingVersion.h"
#include "SIMPLib/Geometry/ImageGeom.h"

using Grayscale_T = uint8_t;
using PixelValue_T = double;

class GridMontage;
using GridMontageShPtr = std::shared_ptr<GridMontage>;

struct RegionBounds;

/**
 * @class FFTConvolutionCostFunction FFTConvolutionCostFunction.h ITKImageProcessingFilters/util/FFTConvolutionCostFunction.h
 * @brief This class was used as a testing class to observe the behavior of the Amoeba optimizer
 * It can be removed if not working on understanding how the Amoeba optimizer works.
 * It is derived from itk::SingleValuedCostFunction.
 */
class ITKImageProcessing_EXPORT MultiParamCostFunction : public itk::SingleValuedCostFunction
{
public:
  itkNewMacro(MultiParamCostFunction);

  /**
   * @brief Initializes the cost function based on a given set of values.
   * @param mins
   */
  void Initialize(std::vector<double> mins);

  /**
   * @brief Override for itk::SingleValuedCostFunction::GetDerivative that throws an exception.
   * @param unused
   * @param unused
   */
  void GetDerivative(const ParametersType&, DerivativeType&) const override;

  /**
   * @brief Returns the number of parameters.
   * @return
   */
  uint32_t GetNumberOfParameters() const override;

  /**
   * @brief Gets the MeasureType value for the given parameters
   * @param parameters
   * @return
   */
  MeasureType GetValue(const ParametersType& parameters) const override;

private:
  std::vector<double> m_Mins;
};

/**
 * @class FFTConvolutionCostFunction FFTConvolutionCostFunction.h ITKImageProcessingFilters/util/FFTConvolutionCostFunction.h
 * @brief The FFTConvolutionCostFunction class is used as the cost function for the Blend filter.
 * It is derived from itk::SingleValuedCostFunction.
 */
class ITKImageProcessing_EXPORT FFTConvolutionCostFunction : public itk::SingleValuedCostFunction
{
public:
  static const uint8_t IMAGE_DIMENSIONS = 2;
  using Cell_T = size_t;
  using PixelCoord = itk::Index<IMAGE_DIMENSIONS>;
  using InputImage = itk::Image<PixelValue_T, IMAGE_DIMENSIONS>;
  using OutputImage = itk::Image<PixelValue_T, IMAGE_DIMENSIONS>;
  using KernelImages = std::pair<InputImage::Pointer, InputImage::Pointer>;
  using GridKey = std::pair<Cell_T, Cell_T>;
  using GridPair = std::pair<GridKey, GridKey>;
  using RegionPair = std::pair<InputImage::RegionType, InputImage::RegionType>;
  using OverlapPair = std::pair<GridPair, RegionPair>;
  using OverlapPairs = std::vector<OverlapPair>;
  using ImageGrid = std::map<GridKey, InputImage::Pointer>;
  using Filter = itk::FFTConvolutionImageFilter<InputImage, InputImage, OutputImage>;
  using PixelTypei = std::array<int64_t, 2>;

  
  using CropMap = std::map<GridKey, RegionBounds>;

  // The m_overlaps is a vector of pairs, with the first index being the
  // grid location of an overlap region (i.e. 'Row 0, Column: 1; Row: 1, Column: 1')
  // and the second index being the ITK RegionTypes that define the overlap regions

  // If using Dave's algorithm, the u_v array just uses:
  // u_v = [ u v u^2 v^2 uv u^2*v u*v^2 ]
  // Note that there is no translation coefficient here (a coefficient of 1)
  // i.e. It doesn't need to be programmatically generated using the degree of
  // a polynomial, it can just be hard coded
  // Here, u and v are just x and y of the image pre-warp coordinates (or post-warp, depending on convention used)
  // In contrast, a generic polynomial would use an array like:
  // b = sum(from i = 0, j = 0) to degree) of a_ij * u^i * v^j
  // u_v = [ 1 u v uv u^2 v^2 u^2*v u*v^2 ... ]
  // For a degree of 1, an m_IJ matrix would look like:
  // m_IJ = [(0, 0), (1, 0), (0, 1), (1, 1)]
  // Of each pair, the first is the i (the value to raise the u coordinate to)
  // and the second is the j (the value to raise the v coordinate to)

  itkNewMacro(FFTConvolutionCostFunction);

  /**
   * @brief Initializes the cost function based on a given set of values.
   * @param chosenDataContainers
   * @param rowChar
   * @param colChar
   * @param degree
   * @param overlapPercentage
   * @param dca
   * @param amName
   * @param daName
   */
  void Initialize(const GridMontageShPtr& montage, int degree, float overlapPercentage, const DataContainerArrayShPtr& dca,
                  const QString& amName, const QString& daName);

  /**
   * @brief This method is called by Initialize as a parallel task algorithm operating on each DataContainer.
   * @param montage
   * @param amName
   * @param daName
   */
  void InitializeDataContainer(const GridMontageShPtr& montage, size_t row, size_t column, const QString& amName, const QString& daName);

  /**
   * @brief This method is called by Initialize as a parallel task algorithm operating on each image in the ImageGrid to calculate the overlap amounts.
   * @param image
   * @param overlapPercentage
   */
  void InitializePercentageOverlaps(const ImageGrid::value_type& image, float overlapPercentage);

  void InitializePositionalOverlaps(const GridMontageShPtr& montage, size_t row, size_t column);

  /**
   * @brief Override for itk::SingleValuedCostFunction::GetDerivative that throws an exception.
   * @param unused
   * @param unused
   */
  void GetDerivative(const ParametersType&, DerivativeType&) const override;

  /**
   * @brief Returns the target number of parameters.  This is calculated based on the degree value.
   * @return
   */
  uint32_t GetNumberOfParameters() const override;

  /**
   * @brief Returns the MeasureType value calculated from the given parameters.
   * @param parameters
   * @return
   */
  MeasureType GetValue(const ParametersType& parameters) const override;

  /**
   * @brief This method is called from GetValue as a parallel task to apply the transform for images in the ImageGrid.
   * @param parameters
   * @param eachImage
   * @param distortedGrid
   * @param cropMap
   */
  void applyTransformation(const ParametersType& parameters, const ImageGrid::value_type& eachImage, ImageGrid& distortedGrid, CropMap& cropMap) const;

  /**
   * @brief This method is called by applyTransformation as a parallel task to apply the transform to each pixel.
   * @param tolerance
   * @param parameters
   * @param inputImage
   * @param distortedImage
   * @param bufferedRegion
   * @param iter
   */
  void applyTransformationPixel(double tolerance, const ParametersType& parameters, const InputImage::Pointer& inputImage, const InputImage::Pointer& distortedImage,
                                const InputImage::RegionType& bufferedRegion, itk::ImageRegionIterator<InputImage> iter, const GridKey& gridKey, CropMap& cropMap) const;

  /**
   * @brief This method is called by applyTransformationPixel as a parallel task calculating pixel coordinates.
   * @param parameters
   * @param pixel
   * @param idx
   * @param x_trans
   * @param y_trans
   * @param x_ref
   * @param y_ref
   */
  void calculatePixelCoordinates(const ParametersType& parameters, const InputImage::Pointer& inputImage, const InputImage::Pointer& distortedImage, const GridKey& gridKey, CropMap& cropMap, const PixelCoord& pixel, double x_trans, double y_trans, double tolerance, double lastXIndex, double lastYIndex) const;

  void adjustCropMap(const PixelCoord& pixel, const InputImage::Pointer& inputImage, const GridKey& gridKey, CropMap& cropMap) const;

  void updateCropMapBounds(const InputImage::Pointer& inputImage, CropMap& cropMap) const;

  void cropOverlap(OverlapPair& overlap, ImageGrid& distortedGrid, CropMap& cropMap) const;

  void cropDistortedGrid(const GridKey& gridKey, ImageGrid& distortedGrid, CropMap& cropMap) const;

  /**
  * @brief Crops the OverlapPair using the provided ImageGrid and CropMap
  */
  void cropOverlapHorizontal(OverlapPair& overlap, const ImageGrid& distortedGrid, const CropMap& cropMap) const;

  void cropOverlapVertical(OverlapPair& overlap, const ImageGrid& distortedGrid, const CropMap& cropMap) const;

  /**
   * @brief This method is called by GetValue to find the FFT Convolution and accumulate the maximum value from each overlap.
   * @param overlap
   * @param distortedGrid
   * @param residual
   */
  void findFFTConvolutionAndMaxValue(const OverlapPair& overlap, ImageGrid& distortedGrid, MeasureType& residual) const;

  /**
   * @brief Returns the ImageGrid value.
   * @return
   */
  ImageGrid getImageGrid() const;

  /**
   * @brief Returns the target tile width.
   * @return
   */
  double getImageDimX() const;

  /**
   * @brief Returns the target tile height.
   * @return
   */
  double getImageDimY() const;

private:
  /**
   * @brief Calculates the ImageDim_x and ImageDim_y values for a montage.
   * @param montage
   */
  void calculateImageDim(const GridMontageShPtr& montage);
  
  KernelImages calculateKernelImages(const OverlapPair& overlap, ImageGrid& distortedGrid) const;

  /**
   * @brief Returns the pixel index for the given row, column, parameters, and translation amount.
   * @param row
   * @param col
   * @param parameters
   * @param x_trans
   * @param y_trans
   * @return
   */
  PixelTypei calculateNew2OldPixel(int64_t row, int64_t col, const ParametersType& parameters, double x_trans, double y_trans) const;

  /**
   * @brief Checks if the regions of the given CropMap are valid.
   * @param cropMap
   * @return
   */
  bool isCropMapValid(const CropMap& cropMap) const;

  /**
   * @brief Checks if the given RegionBounds are valid.
   * @param bounds
   * @return
   */
  bool isRegionValid(const RegionBounds& bounds) const;

  // --------------------------------------------------------------------------
  // Variables
  GridMontageShPtr m_Montage = nullptr;
  int m_Degree = 2;
  size_t m_OverlapXAmt;
  size_t m_OverlapYAmt;
  std::vector<std::pair<size_t, size_t>> m_IJ;
  OverlapPairs m_Overlaps;
  ImageGrid m_ImageGrid;
  double m_ImageDim_x;
  double m_ImageDim_y;
};

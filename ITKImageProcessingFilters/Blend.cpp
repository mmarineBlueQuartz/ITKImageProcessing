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
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS “AS IS”
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

#include "Blend.h"

#include "SIMPLib/Common/Constants.h"

#include "SIMPLib/FilterParameters/ChoiceFilterParameter.h"
#include "SIMPLib/FilterParameters/FilterParameter.h"
#include "SIMPLib/FilterParameters/FloatFilterParameter.h"
#include "SIMPLib/FilterParameters/IntFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedChoicesFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
//#include "SIMPLib/FilterParameters/MultiDataContainerSelectionFilterParameter.h"

#include "ITKImageProcessing/ITKImageProcessingConstants.h"
#include "ITKImageProcessing/ITKImageProcessingVersion.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "itkAmoebaOptimizer.h"
#include <itkFFTConvolutionImageFilter.h>

// TODO Set Image Type with filter parameter
using GrayScaleColor = uint8_t;
using RGBColor = std::tuple<uint8_t, uint8_t, uint8_t>;

// A simple cost function that whose minimum should be at (-B/2, C)
class SimpleCostFunction : public itk::SingleValuedCostFunction
{
  static const int8_t m_NumParams = 1;
  static const int8_t m_B = -2;
  static const int8_t m_C = 1;
  static constexpr double m_initialX = 7.0;

public:
  itkNewMacro(SimpleCostFunction)

  void Initialize() {}

  void GetDerivative(const ParametersType&, DerivativeType&) const override
  {
    throw std::exception("Derivatives are not implemented for the optimization type");
  }

  uint32_t GetNumberOfParameters() const override { return m_NumParams; }

  MeasureType GetValue(const ParametersType& parameters) const override
  {
    return parameters[0] * parameters[0] + parameters[0] * m_B + m_C;
  }

  ParametersType GetInitialValues()
  {
    ParametersType initParams(GetNumberOfParameters());
    initParams[0] = m_initialX;
    return initParams;
  }
};

// Cost function => f(x, y) = (a - x^2) + b(y - x^2)^2
class MultiParamCostFunction : public itk::SingleValuedCostFunction
{
  static const int8_t m_NumParams = 2;
  static const int8_t m_A = 1;
  static const int8_t m_B = 100;
  static constexpr double m_initialX = -1.0;
  static constexpr double m_initialY = -1.0;

public:
  itkNewMacro(MultiParamCostFunction)

  void Initialize() {}

  void GetDerivative(const ParametersType&, DerivativeType&) const override
  {
    throw std::exception("Derivatives are not implemented for the optimization type");
  }

  uint32_t GetNumberOfParameters() const override { return m_NumParams; }

  MeasureType GetValue(const ParametersType& parameters) const override
  {
    return pow((m_A - parameters[0]), 2) + m_B * pow(( parameters[1] - parameters[0] * parameters[0] ), 2);
  }

  ParametersType GetInitialValues()
  {
    ParametersType initParams(GetNumberOfParameters());
    initParams[0] = m_initialX;
    initParams[1] = m_initialY;
    return initParams;
  }
};

class FFTConvolutionCostFunction : public itk::SingleValuedCostFunction
{
  static const uint8_t IMAGE_DIMENSIONS = 2;
  using Cell_T = size_t;
  using Image_T = GrayScaleColor;
  using PixelCoord = itk::Index<IMAGE_DIMENSIONS>;
  using ITKImage = itk::Image<Image_T, IMAGE_DIMENSIONS>;
  using GridKey = std::pair<Cell_T, Cell_T>;
  using GridPair = std::pair<GridKey, GridKey>;
  using RegionPair = std::pair<ITKImage::RegionType, ITKImage::RegionType>;
  using OverlapPair = std::pair<GridPair, RegionPair>;
  using ImageGrid = std::map<std::pair<Cell_T, Cell_T>, ITKImage::Pointer>;

  int m_degree = 2;
  std::vector<std::pair<size_t, size_t>> m_IJ;
  std::vector<OverlapPair> m_overlaps;
  ImageGrid m_imageGrid;
  itk::FFTConvolutionImageFilter<ITKImage>::Pointer m_filter;

public:
  itkNewMacro(FFTConvolutionCostFunction)

  void Initialize(int degree, float overlapPercentage,
                  DataContainerArrayShPtr dca,
                  const QString& amName, const QString& dataAAName,
                  const QString& xAAName, const QString& yAAName)
  {
    m_filter = itk::FFTConvolutionImageFilter<ITKImage>::New();
    m_degree = degree;
    for(size_t listOneIndex = 0; listOneIndex < m_degree; listOneIndex++)
    {
      for(size_t listTwoIndex = 0; listTwoIndex < m_degree; listTwoIndex++)
      {
        m_IJ.push_back(std::make_pair(listTwoIndex, listOneIndex));
      }
    }

    // Cache a bunch of stuff
    GridKey imageKey;
    std::tuple<size_t, size_t, size_t> dims;
    size_t width, height, overlapDim;
    int cLength;
    QString name;
    PixelCoord idx;

    ITKImage::Pointer eachImage;
    PixelCoord imageOrigin;
    ITKImage::SizeType imageSize;
    PixelCoord kernelOrigin;
    ITKImage::SizeType kernelSize;

    AttributeMatrixShPtr am;
    m_imageGrid.clear();
    // Populate and assign eachImage to m_imageGrid
    for(const auto& eachDC : dca->getDataContainers())
    {
      am = eachDC->getAttributeMatrix(amName);
      dims = eachDC->getGeometryAs<ImageGeom>()->getDimensions();
      width = std::get<0>(dims);
      height = std::get<1>(dims);

      imageOrigin[0] = 0;
      imageOrigin[1] = 0;
      imageSize[0] = width;
      imageSize[1] = height;
      eachImage = ITKImage::New();
      eachImage->SetRegions(ITKImage::RegionType(imageOrigin, imageSize));
      eachImage->Allocate();
      for(size_t pxlIdx = 0; pxlIdx < (width * height); ++pxlIdx)
      {
        idx[0] = am->getAttributeArrayAs<Int64ArrayType>(xAAName)->getValue(pxlIdx);
        idx[1] = am->getAttributeArrayAs<Int64ArrayType>(yAAName)->getValue(pxlIdx);
        eachImage->SetPixel(idx, am->getAttributeArrayAs<UInt8ArrayType>(dataAAName)->getValue(pxlIdx));
      }

      // TODO Parameterize 'R' and 'C'
      name = eachDC->getName();
      cLength = name.size() - name.indexOf('C') - 1;
      imageKey = std::make_pair(
        static_cast<Cell_T>(name.midRef(name.indexOf('R') + 1, name.size() - cLength - 2).toULong()),
        static_cast<Cell_T>(name.rightRef(cLength).toULong())
      );
      m_imageGrid.insert_or_assign(imageKey, eachImage);
    }

    // Populate m_overlaps
    m_overlaps.clear();
    for (const auto& eachImage : m_imageGrid) {
      auto rightImage{m_imageGrid.find(std::make_pair(eachImage.first.first, eachImage.first.second + 1))};
      auto bottomImage{m_imageGrid.find(std::make_pair(eachImage.first.first + 1, eachImage.first.second))};
      if(rightImage != m_imageGrid.end())
      {
        overlapDim = static_cast<size_t>(roundf(width * overlapPercentage));
        imageOrigin[0] = static_cast<itk::IndexValueType>(width - overlapDim);
        imageOrigin[1] = 0;
        imageSize[0] = overlapDim;
        imageSize[1] = height;

        kernelOrigin[0] = 0;
        kernelOrigin[1] = 0;
        kernelSize[0] = overlapDim;
        kernelSize[1] = height;

        m_overlaps.push_back(std::make_pair(
          std::make_pair(eachImage.first, rightImage->first),
          std::make_pair(
            ITKImage::RegionType(imageOrigin, imageSize),
            ITKImage::RegionType(kernelOrigin, kernelSize)
          )
        ));
      }
      if(bottomImage != m_imageGrid.end())
      {
        overlapDim = static_cast<size_t>(roundf(height * overlapPercentage));
        imageOrigin[0] = 0;
        imageOrigin[1] = 0;
        imageSize[0] = width;
        imageSize[1] = overlapDim;

        kernelOrigin[0] = static_cast<itk::IndexValueType>(height - overlapDim);
        kernelOrigin[1] = 0;
        kernelSize[0] = width;
        kernelSize[1] = overlapDim;

        m_overlaps.push_back(std::make_pair(
          std::make_pair(eachImage.first, bottomImage->first),
          std::make_pair(
            ITKImage::RegionType(imageOrigin, imageSize),
            ITKImage::RegionType(kernelOrigin, kernelSize)
          )
        ));
      }
    }
  }

  void GetDerivative(const ParametersType&, DerivativeType&) const override
  {
    throw std::exception("Derivatives are not implemented for the optimization type");
  }

  uint32_t GetNumberOfParameters() const override
  {
    return static_cast<uint32_t>(2 * (m_degree * m_degree + 2 * m_degree + 1));
  }

  ParametersType GetInitialValues() const
  {
    const uint32_t numCoeffs = GetNumberOfParameters();
    const uint32_t half = numCoeffs / 2;
    ParametersType params{numCoeffs};
    double value = 0.0;
    double increment = 0.1;
    for(size_t x = 0; x < numCoeffs; ++x)
    {
      value = 0.0;
      if(x == 1 || x == half + 2)
      {
        value = increment;
      }
      if(x == 2)
      {
        value = increment;
      }
      if(x == half + 1)
      {
        value = -increment;
      }
      params[x] = value;
    }
    return params;
  }

  MeasureType GetValue(const ParametersType& parameters) const override
  {
    const double tolerance = 0.05;
    ImageGrid distortedGrid;

    // Cache a bunch of stuff
    ITKImage::RegionType bufferedRegion;
    ITKImage::SizeType dims;
    size_t width;
    size_t height;
    double lastXIndex;
    double lastYIndex;

    double x_trans;
    double y_trans;
    double x = 0;
    double y = 0;
    double u_v = 0;
    double term = 0;

    std::pair<int, int> eachIJ{};
    PixelCoord eachPixel;
    ITKImage::Pointer distortedImage;

    // Apply the Transform to each image in the image grid
    for(const auto& eachImage : m_imageGrid) // TODO Parallelize this
    {
      bufferedRegion = eachImage.second->GetBufferedRegion();
      dims = bufferedRegion.GetSize();
      width = dims[0];
      height = dims[1];
      lastXIndex = width - 1 + tolerance;
      lastYIndex = height - 1 + tolerance;

      x_trans = (width - 1) / 2.0;
      y_trans = (height - 1) / 2.0;

      distortedImage = ITKImage::New();
      distortedImage->SetRegions(bufferedRegion);
      distortedImage->Allocate(); // NOTE Does SetBufferedRegion already allocate?

      // Iterate through the pixels in eachImage and apply the transform
      // TODO Parallelize this
      itk::ImageRegionIterator<ITKImage> it(eachImage.second, bufferedRegion);
      for(it.GoToBegin(); !it.IsAtEnd(); ++it)
      {
        PixelCoord pixel = it.GetIndex();
        x = x_trans;
        y = y_trans;
        // TODO Parallelize this
        for(size_t idx = 0; idx < parameters.size(); ++idx)
        {
          eachIJ = m_IJ[idx - (idx >= m_IJ.size() ? m_IJ.size() : 0)];

          u_v = pow((pixel[0] - x_trans), eachIJ.first) * pow((pixel[1] - y_trans), eachIJ.second);

          term = u_v * parameters[idx];
          idx < ( m_IJ.size() / 2 ) ? x += term : y += term;
        }

        // This check effectively "clips" data
        if(x >= -tolerance && x <= lastXIndex && y >= -tolerance && y <= lastYIndex)
        {
          eachPixel[0] = round(x);
          eachPixel[1] = round(y);
          distortedImage->SetPixel(eachPixel, eachImage.second->GetPixel(eachPixel));
        }
      }
      distortedGrid.insert_or_assign(eachImage.first, distortedImage);
    }

    // Find the FFT Convolution and accumulate the maximum value from each overlap
    ITKImage::Pointer image;
    ITKImage::Pointer kernel;
    ITKImage::Pointer fftConvolve;
    MeasureType residual = 0.0; // NOTE Should this be initialized to a very large number?
    // NOTE Can't be easily parallelized if multiple threads are changing cached
    // memory like 'residual', not to mention the pointers...
    for(const auto& eachOverlap : m_overlaps) // TODO Parallelize this?
    {
      image = distortedGrid.at(eachOverlap.first.first);
      image->SetRequestedRegion(eachOverlap.second.first);
      image->GetRequestedRegion().IsInside(eachOverlap.second.first);
      m_filter->SetInput(image);

      kernel = distortedGrid.at(eachOverlap.first.second);
      kernel->SetRequestedRegion(eachOverlap.second.second);
      kernel->GetRequestedRegion().IsInside(eachOverlap.second.second);
      m_filter->SetKernelImage(kernel);

      m_filter->Update();
      fftConvolve = m_filter->GetOutput();
      residual += *std::max_element(
        fftConvolve->GetBufferPointer(),
        fftConvolve->GetBufferPointer() + fftConvolve->GetPixelContainer()->Size()
      );
    }
    return sqrt(residual);
  }
};

uint Blend::GetIterationsFromStopDescription(const QString& stopDescription) const
{
  if (GetConvergenceFromStopDescription(stopDescription))
  {
    const QString startStr = "have been met in ";
    const int startIdx = stopDescription.indexOf(startStr) + startStr.length();
    return stopDescription.midRef(startIdx, stopDescription.indexOf(" iterations") - startIdx).toUInt();
  }
  return 0;
}

bool Blend::GetConvergenceFromStopDescription(const QString& stopDescription) const
{
  return stopDescription.contains("have been met");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Blend::Blend()
: m_OverlapMethod(0)
{
  initialize();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Blend::~Blend() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Blend::initialize()
{
  setErrorCondition(0, "");
  setWarningCondition(0, "");
  setCancel(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Blend::setupFilterParameters()
{
  FilterParameterVectorType parameters;

  //  MultiDataContainerSelectionFilterParameter::Pointer dcs{
  //    MultiDataContainerSelectionFilterParameter::New(
  //      "Chosen Data Containers", "ChosenDataContainers",
  //      getChosenDataContainers(), FilterParameter::Category::RequiredArray,
  //      std::bind(&Blend::setChosenDataContainers, this, std::placeholders::_1),
  //      std::bind(&Blend::getChosenDataContainers, this), {}
  //  )};

  IntFilterParameter::Pointer maxIterations{IntFilterParameter::New("Max Iterations", "MaxIterations", 1000, FilterParameter::Category::Parameter,
                                                                    std::bind(&Blend::setMaxIterations, this, std::placeholders::_1), std::bind(&Blend::getMaxIterations, this))};

  IntFilterParameter::Pointer degree{
      IntFilterParameter::New("Degree", "Degree", 1, FilterParameter::Category::Parameter, std::bind(&Blend::setDegree, this, std::placeholders::_1), std::bind(&Blend::getDegree, this))};

  LinkedChoicesFilterParameter::Pointer overlapMethod{LinkedChoicesFilterParameter::New("Overlap Method", "OverlapMethod", 0, FilterParameter::Parameter,
                                                                                        std::bind(&Blend::setOverlapMethod, this, std::placeholders::_1), std::bind(&Blend::getOverlapMethod, this),
                                                                                        {"Percent", "Origins"}, {"OverlapPercentage"})};

  FloatFilterParameter::Pointer overlapPercentage{FloatFilterParameter::New("Overlap Percentage", "OverlapPercentage", 0.0f, FilterParameter::Category::Parameter,
                                                                            std::bind(&Blend::setOverlapPercentage, this, std::placeholders::_1), std::bind(&Blend::getOverlapPercentage, this), 0)};

  StringFilterParameter::Pointer amName{StringFilterParameter::New("Attribute Matrix Name", "AttributeMatrixName", {}, FilterParameter::Category::Parameter,
                                                                   std::bind(&Blend::setAttributeMatrixName, this, std::placeholders::_1), std::bind(&Blend::getAttributeMatrixName, this), {})};

  StringFilterParameter::Pointer xAAName{StringFilterParameter::New("Attribute Array Name", "AttributeArrayName", {}, FilterParameter::Category::Parameter,
                                                                    std::bind(&Blend::setXAttributeArrayName, this, std::placeholders::_1), std::bind(&Blend::getXAttributeArrayName, this), {})};

  StringFilterParameter::Pointer yAAName{StringFilterParameter::New("Attribute Array Name", "AttributeArrayName", {}, FilterParameter::Category::Parameter,
                                                                    std::bind(&Blend::setYAttributeArrayName, this, std::placeholders::_1), std::bind(&Blend::getYAttributeArrayName, this), {})};

  StringFilterParameter::Pointer dataAAName{StringFilterParameter::New("Attribute Array Name", "AttributeArrayName", {}, FilterParameter::Category::Parameter,
                                                                       std::bind(&Blend::setDataAttributeArrayName, this, std::placeholders::_1), std::bind(&Blend::getDataAttributeArrayName, this),
                                                                       {})};

  //  parameters.push_back(dcs);
  parameters.push_back(maxIterations);
  parameters.push_back(degree);
  parameters.push_back(overlapMethod);
  parameters.push_back(overlapPercentage);
  parameters.push_back(amName);
  parameters.push_back(xAAName);
  parameters.push_back(yAAName);
  parameters.push_back(dataAAName);

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Blend::dataCheck()
{
  setErrorCondition(0, "");
  setWarningCondition(0, "");
  //  OverlapMethod overlapMethod = static_cast<OverlapMethod>(m_OverlapMethod);
  // Check bounds of m_overlapPercent
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Blend::preflight()
{
  // These are the REQUIRED lines of CODE to make sure the filter behaves correctly
  setInPreflight(true);              // Set the fact that we are preflighting.
  emit preflightAboutToExecute();    // Emit this signal so that other widgets can do one file update
  emit updateFilterParameters(this); // Emit this signal to have the widgets push their values down to the filter
  dataCheck();                       // Run our DataCheck to make sure everthing is setup correctly
  emit preflightExecuted();          // We are done preflighting this filter
  setInPreflight(false);             // Inform the system this filter is NOT in preflight mode anymore.
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Blend::execute()
{
  initialize();
  dataCheck();
  if(getCancel())
  {
    return;
  }

  if(getWarningCode() < 0)
  {
    QString ss = QObject::tr("Some warning message");
    setWarningCondition(-66400, ss);
    notifyStatusMessage(ss);
  }

  if(getErrorCode() < 0)
  {
    QString ss = QObject::tr("Some error message");
    setErrorCondition(-66500, ss);
    notifyStatusMessage(ss);
    return;
  }

  // These should probably be parametized
  const double lowTolerance = 1E-2;
  const double highTolerance = 1E-2;

  itk::AmoebaOptimizer::Pointer m_optimizer = itk::AmoebaOptimizer::New();
  m_optimizer->SetMaximumNumberOfIterations(m_MaxIterations);
  m_optimizer->SetFunctionConvergenceTolerance(lowTolerance);
  m_optimizer->SetParametersConvergenceTolerance(highTolerance);

//  using CostFunctionType = SimpleCostFunction;
  using CostFunctionType = MultiParamCostFunction;
//  using CostFunctionType = FFTConvolutionCostFunction;
  CostFunctionType implementation;
  implementation.Initialize(
//    m_Degree, m_OverlapPercentage, getDataContainerArray(),
//    m_AttributeMatrixName, m_DataAttributeArrayName, m_XAttributeArrayName, m_YAttributeArrayName
  );
  m_optimizer->SetInitialPosition(implementation.GetInitialValues());
  m_optimizer->SetCostFunction(&implementation);

  m_optimizer->StartOptimization();

  // Can get rid of this after debugging is done for filter
  QString a;
  for (const auto& eachCoeff : m_optimizer->GetCurrentPosition())
  {
    a += QString::number(eachCoeff) + ", ";
  }
  qDebug() << "Position: [ " << a.leftRef(a.length() - 2) << " ]";
  qDebug() << "Value: " << m_optimizer->GetValue();

  // Determine whether the solution truly converged
  QString stopReason = QString::fromStdString(m_optimizer->GetStopConditionDescription());
  if (!GetConvergenceFromStopDescription(stopReason))
  {
    setErrorCondition(-66800, stopReason);
    notifyStatusMessage(stopReason);
    return;
  }

  // Can get rid of this after debugging is done for filter
  qDebug() << "Number of Iterations: " << GetIterationsFromStopDescription(stopReason);

  // TODO Set up new Attribute matrix with results of filter
  // Should contain the transformation matrix (a)
  // And a data array with a single element for the iterations run
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer Blend::newFilterInstance(bool copyFilterParameters) const
{
  Blend::Pointer filter = Blend::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return std::move(filter);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString Blend::getCompiledLibraryName() const
{
  return ITKImageProcessingConstants::ITKImageProcessingBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString Blend::getBrandingString() const
{
  return "ITKImageProcessing";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString Blend::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << ITKImageProcessing::Version::Major() << "." << ITKImageProcessing::Version::Minor() << "." << ITKImageProcessing::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString Blend::getGroupName() const
{
  return SIMPL::FilterGroups::ProcessingFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString Blend::getSubGroupName() const
{
  return "Processing";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString Blend::getHumanLabel() const
{
  return "Blend";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QUuid Blend::getUuid()
{
  return QUuid("{f2c81aa6-8edf-51d8-8c30-81ad983815e9}");
}

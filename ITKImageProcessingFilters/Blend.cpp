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

#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
#include <tbb/task_group.h>
#include <tbb/task_scheduler_init.h>
#endif

#include "SIMPLib/Common/Constants.h"

#include "SIMPLib/FilterParameters/ChoiceFilterParameter.h"
#include "SIMPLib/FilterParameters/DoubleFilterParameter.h"
#include "SIMPLib/FilterParameters/FilterParameter.h"
#include "SIMPLib/FilterParameters/FloatFilterParameter.h"
#include "SIMPLib/FilterParameters/IntFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedChoicesFilterParameter.h"
#include "SIMPLib/FilterParameters/MultiDataContainerSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/Utilities/SIMPLibParallelAlgorithm.hpp"

#include "ITKImageProcessing/ITKImageProcessingConstants.h"
#include "ITKImageProcessing/ITKImageProcessingVersion.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "itkAmoebaOptimizer.h"
#include <itkFFTConvolutionImageFilter.h>

using PixelValue_T = double;

// =============================================================================
class MultiParamCostFunction : public itk::SingleValuedCostFunction
{
  std::vector<double> m_mins{};

public:
  itkNewMacro(MultiParamCostFunction)

  // =============================================================================
  void Initialize(std::vector<double> mins)
  {
    m_mins = mins;
  }

  // =============================================================================
  void GetDerivative(const ParametersType&, DerivativeType&) const override
  {
    throw std::exception();
  }

  // =============================================================================
  uint32_t GetNumberOfParameters() const override
  {
    return static_cast<uint32_t>(m_mins.size());
  }

  // =============================================================================
  MeasureType GetValue(const ParametersType& parameters) const override
  {
    MeasureType residual = 0.0;
    size_t numParams = parameters.size();
    for(size_t idx = 0; idx < numParams; ++idx)
    {
      double minValue = m_mins[idx];
      double paramValue = parameters[idx];
      residual += (idx == numParams - 1) ? minValue : pow(paramValue - minValue, 2);
    }
    return residual;
  }
};

// =============================================================================
template <class Image_T>
class FFTConvolutionCostFunction : public itk::SingleValuedCostFunction
{
  static const uint8_t IMAGE_DIMENSIONS = 2;
  using Cell_T = size_t;
  using PixelCoord = itk::Index<IMAGE_DIMENSIONS>;
  using InputImage = itk::Image<Image_T, IMAGE_DIMENSIONS>;
  using OutputImage = itk::Image<uint8_t, IMAGE_DIMENSIONS>;
  using GridKey = std::pair<Cell_T, Cell_T>;
  using GridPair = std::pair<GridKey, GridKey>;
  using RegionPair = std::pair<typename InputImage::RegionType, typename InputImage::RegionType>;
  using OverlapPair = std::pair<GridPair, RegionPair>;
  using OverlapPairs = std::vector<OverlapPair>;
  using ImageGrid = std::map<std::pair<Cell_T, Cell_T>, typename InputImage::Pointer>;
  using FilterType = itk::FFTConvolutionImageFilter<InputImage, InputImage, OutputImage>;

  size_t m_degree = 2;
  std::vector<std::pair<size_t, size_t>> m_IJ;
  OverlapPairs m_overlaps;
  ImageGrid m_imageGrid;
  typename FilterType::Pointer m_filter;

public:
  itkNewMacro(FFTConvolutionCostFunction)

  void Initialize(QStringList chosenDataContainers, QChar rowChar, QChar colChar, size_t degree, float overlapPercentage, DataContainerArrayShPtr dca, const QString& amName, const QString& dataAAName,
                  const QString& xAAName, const QString& yAAName)
  {
    m_filter = FilterType::New();
    m_degree = degree;
    for(size_t listOneIndex = 0; listOneIndex < m_degree; listOneIndex++)
    {
      for(size_t listTwoIndex = 0; listTwoIndex < m_degree; listTwoIndex++)
      {
        m_IJ.push_back(std::make_pair(listTwoIndex, listOneIndex));
      }
    }

    // Cache loop variables
    GridKey imageKey;
    std::tuple<size_t, size_t, size_t> dims;
    size_t width, height, overlapDim;
    int cLength;
    QString name;
    PixelCoord idx;

    typename InputImage::Pointer eachImage;
    PixelCoord imageOrigin;
    imageOrigin[0] = 0;
    imageOrigin[1] = 0;
    typename InputImage::SizeType imageSize;
    PixelCoord kernelOrigin;
    typename InputImage::SizeType kernelSize;

    AttributeMatrixShPtr am;
    m_imageGrid.clear();

    // Populate and assign eachImage to m_imageGrid
    for(const auto& eachDC : dca->getDataContainers())
    {
      if(!chosenDataContainers.contains(eachDC->getName()))
      {
        continue;
      }
      am = eachDC->getAttributeMatrix(amName);
      dims = eachDC->getGeometryAs<ImageGeom>()->getDimensions();
      width = std::get<0>(dims);
      height = std::get<1>(dims);

      imageSize[0] = width;
      imageSize[1] = height;
      eachImage = InputImage::New();
      eachImage->SetRegions(InputImage::RegionType(imageOrigin, imageSize));
      eachImage->Allocate();
      for(size_t pxlIdx = 0; pxlIdx < (width * height); ++pxlIdx)
      {
        DataArray<PixelValue_T>::Pointer da = am->getAttributeArrayAs<DataArray<PixelValue_T>>(dataAAName);
        // TODO Validate this gets the appropriate data to set for that pixel
        // NOTE Uses a 1D array with multiple components juxtaposed
        // Will need to create a data structure that can be filled by iterating
        // over the first through the last components
        int numComponents = da->getNumberOfComponents();
        size_t sz = da->getComponentDimensions()[0];
        std::vector<PixelValue_T> valueArray(sz);
        for(size_t eachCompIdx = 0; eachCompIdx < sz; ++eachCompIdx)
        {
          // TODO
        }

        idx[0] = am->getAttributeArrayAs<Int64ArrayType>(xAAName)->getValue(pxlIdx);
        idx[1] = am->getAttributeArrayAs<Int64ArrayType>(yAAName)->getValue(pxlIdx);
        eachImage->SetPixel(idx, valueArray);
      }

      name = eachDC->getName();
      cLength = name.size() - name.indexOf(colChar) - 1;
      imageKey = std::make_pair(static_cast<Cell_T>(name.midRef(name.indexOf(rowChar) + 1, name.size() - cLength - 2).toULong()), static_cast<Cell_T>(name.rightRef(cLength).toULong()));
      m_imageGrid.insert_or_assign(imageKey, eachImage);
    }

    // Populate m_overlaps
    m_overlaps.clear();
    for(const auto& eachImage : m_imageGrid)
    {
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

        m_overlaps.push_back(
            std::make_pair(std::make_pair(eachImage.first, rightImage->first), std::make_pair(InputImage::RegionType(imageOrigin, imageSize), InputImage::RegionType(kernelOrigin, kernelSize))));
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

        m_overlaps.push_back(
            std::make_pair(std::make_pair(eachImage.first, bottomImage->first), std::make_pair(InputImage::RegionType(imageOrigin, imageSize), InputImage::RegionType(kernelOrigin, kernelSize))));
      }
    }
  }

  // =============================================================================
  void GetDerivative(const ParametersType&, DerivativeType&) const override
  {
    throw std::exception();
  }

  // =============================================================================
  uint32_t GetNumberOfParameters() const override
  {
    return static_cast<uint32_t>(2 * (m_degree * m_degree + 2 * m_degree + 1));
  }

  // =============================================================================
  MeasureType GetValue(const ParametersType& parameters) const override
  {
    const double tolerance = 0.05;
    ImageGrid distortedGrid;

    // Cache a bunch of stuff
    typename InputImage::RegionType bufferedRegion;
    typename InputImage::SizeType dims;
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
    typename InputImage::Pointer distortedImage;

    SIMPL_INIT_PARALLEL_ALGORITHMS()

    // Apply the Transform to each image in the image grid
    for(const auto& eachImage : m_imageGrid) // Parallelize this
    {
      SIMPL_RUN_PARALLEL_ALGORITHM(applyTransformation(eachImage))
    }

    SIMPL_END_PARALLEL_ALGORITHMS()

    // Find the FFT Convolution and accumulate the maximum value from each overlap
    std::atomic<MeasureType> residual{0.0};
    for(const auto& eachOverlap : m_overlaps) // Parallelize this
    {
      SIMPL_RUN_PARALLEL_ALGORITHM(findFFTConvolution(eachOverlap))
    }

    SIMPL_END_PARALLEL_ALGORITHMS()

    return sqrt(residual);
  }

  // =============================================================================
  void applyTransformation(const ImageGrid& image)
  {
    bufferedRegion = eachImage.second->GetBufferedRegion();
    dims = bufferedRegion.GetSize();
    width = dims[0];
    height = dims[1];
    lastXIndex = width - 1 + tolerance;
    lastYIndex = height - 1 + tolerance;

    x_trans = (width - 1) / 2.0;
    y_trans = (height - 1) / 2.0;

    distortedImage = InputImage::New();
    distortedImage->SetRegions(bufferedRegion);
    distortedImage->Allocate();

    // Iterate through the pixels in eachImage and apply the transform
    itk::ImageRegionIterator<InputImage> it(eachImage.second, bufferedRegion);
    for(it.GoToBegin(); !it.IsAtEnd(); ++it) // TODO Parallelize this
    {
      PixelCoord pixel = it.GetIndex();
      x = x_trans;
      y = y_trans;
      for(size_t idx = 0; idx < parameters.size(); ++idx) // TODO Parallelize this
      {
        eachIJ = m_IJ[idx - (idx >= m_IJ.size() ? m_IJ.size() : 0)];

        u_v = pow((pixel[0] - x_trans), eachIJ.first) * pow((pixel[1] - y_trans), eachIJ.second);

        term = u_v * parameters[idx];
        idx < (m_IJ.size() / 2) ? x += term : y += term;
      }

      // This check effectively "clips" data
      if(x >= -tolerance && x <= lastXIndex && y >= -tolerance && y <= lastYIndex)
      {
        eachPixel[0] = static_cast<int64_t>(round(x));
        eachPixel[1] = static_cast<int64_t>(round(y));
        // The value obtained with eachImage.second->GetPixel(eachPixel) could have
        // its "contrast" adjusted based on its radial location from the center of
        // the image at this step to compensate for error encountered from radial effects
        distortedImage->SetPixel(eachPixel, eachImage.second->GetPixel(eachPixel));
      }
    }
    distortedGrid.insert_or_assign(eachImage.first, distortedImage);
  }

  // =============================================================================
  void findFFTConvolution(const OverlapPair& pair)
  {
    typename InputImage::Pointer image = distortedGrid.at(eachOverlap.first.first);
    image->SetRequestedRegion(eachOverlap.second.first);
    image->GetRequestedRegion().IsInside(eachOverlap.second.first);
    m_filter->SetInput(image);

    typename InputImage::Pointer kernel = distortedGrid.at(eachOverlap.first.second);
    kernel->SetRequestedRegion(eachOverlap.second.second);
    kernel->GetRequestedRegion().IsInside(eachOverlap.second.second);
    m_filter->SetKernelImage(kernel);

    m_filter->Update();
    typename OutputImage::Pointer fftConvolve = m_filter->GetOutput();
    residual = residual + *std::max_element(fftConvolve->GetBufferPointer(), fftConvolve->GetBufferPointer() + fftConvolve->GetPixelContainer()->Size());
  }
};

// =============================================================================
uint Blend::GetIterationsFromStopDescription(const QString& stopDescription) const
{
  if(GetConvergenceFromStopDescription(stopDescription))
  {
    const QString startStr = "have been met in ";
    const int startIdx = stopDescription.indexOf(startStr) + startStr.length();
    return stopDescription.midRef(startIdx, stopDescription.indexOf(" iterations") - startIdx).toUInt();
  }
  return m_MaxIterations;
}

// =============================================================================
bool Blend::GetConvergenceFromStopDescription(const QString& stopDescription) const
{
  return stopDescription.contains("have been met");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Blend::Blend()
: m_MaxIterations(1000)
, m_Degree(1)
, m_OverlapPercentage(0.0f)
, m_InitialSimplexGuess("0.1;0.1;0.1;0.1;0.1;0.1;0.1;0.1")
, m_LowTolerance(1E-2)
, m_RowCharacter("R")
, m_ColumnCharacter("C")
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
  clearErrorCode();
  clearWarningCode();
  setCancel(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Blend::setupFilterParameters()
{
  FilterParameterVectorType parameters;

  {
    MultiDataContainerSelectionFilterParameter::RequirementType req;
    parameters.push_back(SIMPL_NEW_MDC_SELECTION_FP("Chosen Data Containers", ChosenDataContainers, FilterParameter::Category::RequiredArray, Blend, req));
  }

  parameters.push_back(SIMPL_NEW_INTEGER_FP("Max Iterations", MaxIterations, FilterParameter::Category::Parameter, Blend));
  parameters.push_back(SIMPL_NEW_INTEGER_FP("Degree", Degree, FilterParameter::Category::Parameter, Blend));
  parameters.push_back(SIMPL_NEW_FLOAT_FP("Overlap Percentage", OverlapPercentage, FilterParameter::Category::Parameter, Blend));
  parameters.push_back(SIMPL_NEW_DOUBLE_FP("Low Tolerance", LowTolerance, FilterParameter::Category::Parameter, Blend));
  parameters.push_back(SIMPL_NEW_DOUBLE_FP("High Tolerance", HighTolerance, FilterParameter::Category::Parameter, Blend));
  parameters.push_back(SIMPL_NEW_STRING_FP("Initial Simplex Guess", InitialSimplexGuess, FilterParameter::Category::Parameter, Blend));
  parameters.push_back(SIMPL_NEW_STRING_FP("Attribute Matrix Name", AttributeMatrixName, FilterParameter::Category::Parameter, Blend));
  parameters.push_back(SIMPL_NEW_STRING_FP("X Attribute Array Name", XAttributeArrayName, FilterParameter::Category::Parameter, Blend));
  parameters.push_back(SIMPL_NEW_STRING_FP("Y Attribute Array Name", YAttributeArrayName, FilterParameter::Category::Parameter, Blend));
  parameters.push_back(SIMPL_NEW_STRING_FP("Data Attribute Array Name", DataAttributeArrayName, FilterParameter::Category::Parameter, Blend));
  parameters.push_back(SIMPL_NEW_STRING_FP("Row Character", RowCharacter, FilterParameter::Category::Parameter, Blend));
  parameters.push_back(SIMPL_NEW_STRING_FP("Column Character", ColumnCharacter, FilterParameter::Category::Parameter, Blend));

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Blend::dataCheck()
{
  clearErrorCode();
  clearWarningCode();

  m_initialGuess.clear();
  for(const auto& eachCoeff : m_InitialSimplexGuess.split(";"))
  {
    bool coerced = false;
    m_initialGuess.push_back(eachCoeff.toDouble(&coerced));
    if(!coerced)
    {
      setErrorCondition(-66500, "A coefficient could not be translated into a floating-point precision number");
    }
  }

  size_t len = static_cast<size_t>(2 * m_Degree * m_Degree + 4 * m_Degree + 2);
  if(len != m_initialGuess.size())
  {
    setErrorCondition(-66400, "Number of coefficients in initial guess is not compatible with degree number");
  }

  if(m_OverlapPercentage < 0.0f || m_OverlapPercentage >= 1.00f)
  {
    setErrorCondition(-66500, "Overlap Percentage should be a floating-point precision number between 0.0 and 1.0");
  }

  // Requirements to avoid crashing
  if(getDataContainerArray()->getDataContainers().size() <= 0)
  {
    setErrorCondition(-66700, "At least one DataContainer required");
    return;
  }
  DataContainer::Pointer dc = getDataContainerArray()->getDataContainers()[0];
  if(nullptr == dc->getAttributeMatrix(m_AttributeMatrixName))
  {
    setErrorCondition(-66800, QString("AttributeMatrix: %1 required").arg(m_AttributeMatrixName));
    return;
  }
  if(nullptr == getDataContainerArray()->getDataContainers()[0]->getAttributeMatrix(m_AttributeMatrixName)->getAttributeArray(m_DataAttributeArrayName))
  {
    setErrorCondition(-66900, QString("DataArray: %1 required").arg(m_DataAttributeArrayName));
    return;
  }

  QString typeName = getDataContainerArray()->getDataContainers()[0]->getAttributeMatrix(m_AttributeMatrixName)->getAttributeArray(m_DataAttributeArrayName)->getTypeAsString();
  for(const auto& eachDC : getDataContainerArray()->getDataContainers())
  {
    if(!m_ChosenDataContainers.contains(eachDC->getName()))
    {
      continue;
    }
    else if(eachDC->getAttributeMatrix(m_AttributeMatrixName)->getAttributeArray(m_DataAttributeArrayName)->getTypeAsString() != typeName)
    {
      setErrorCondition(-66600, "Not all data attribute arrays are the same type");
    }
    // TODO If any data array of the data to be blended has a component dimension > 1
    // There needs to be an error thrown
  }
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

  if(getErrorCode() < 0 || getCancel())
  {
    return;
  }

  if(getWarningCode() < 0)
  {
    QString ss = QObject::tr("An unknown warning occurred");
    setWarningCondition(-66450, ss);
    // notifyStatusMessage(ss);
  }

  DataContainerShPtr blendDC = getDataContainerArray()->createNonPrereqDataContainer(this, DataArrayPath(m_blendDCName, m_transformAMName, ""));
  AttributeMatrixShPtr blendAM = blendDC->createAndAddAttributeMatrix({1}, m_transformAMName, AttributeMatrix::Type::Generic);

  blendAM->createAndAddAttributeArray<UInt64ArrayType>(this, m_iterationsAAName, 0, {1});
  blendAM->createAndAddAttributeArray<DoubleArrayType>(this, m_transformAAName, 0, {m_initialGuess.size()});
  blendAM->createAndAddAttributeArray<DoubleArrayType>(this, m_valueAAName, 0, {1});
  getDataContainerArray()->addOrReplaceDataContainer(blendDC);

  itk::AmoebaOptimizer::ParametersType initialParams(m_initialGuess.size());
  for(size_t idx = 0; idx < m_initialGuess.size(); ++idx)
  {
    initialParams[idx] = m_initialGuess[idx];
  }

  itk::AmoebaOptimizer::Pointer m_optimizer = itk::AmoebaOptimizer::New();
  m_optimizer->SetMaximumNumberOfIterations(m_MaxIterations);
  m_optimizer->SetFunctionConvergenceTolerance(m_LowTolerance);
  m_optimizer->SetParametersConvergenceTolerance(m_HighTolerance);
  m_optimizer->SetInitialPosition(initialParams);

  using CostFunctionType = MultiParamCostFunction;
  // TODO Figure out the type/dimensions that need to be passed in as the template parameter
  // for the FFTConvolutionCostFunction
  // Probably something like std::vector<double>
  // using CostFunctionType = FFTConvolutionCostFunction<std::vector<double>>;
  CostFunctionType implementation;
  implementation.Initialize(std::vector<double>{1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0} //    m_ChosenDataContainers, m_RowCharacter, m_ColumnCharacter, m_Degree, m_OverlapPercentage,
                                                                                        //    getDataContainerArray(),
                                                                                        //    m_AttributeMatrixName, m_DataAttributeArrayName, m_XAttributeArrayName, m_YAttributeArrayName
  );
  m_optimizer->SetCostFunction(&implementation);
  m_optimizer->StartOptimization();

  QString stopReason = QString::fromStdString(m_optimizer->GetStopConditionDescription());
  // Can get rid of this after debugging is done for filter
  std::list<double> transform;
  for(const auto& eachCoeff : m_optimizer->GetCurrentPosition())
  {
    transform.push_back(eachCoeff);
  }
  qDebug() << "Initial Position: [ " << m_initialGuess << " ]";
  qDebug() << "Final Position: [ " << transform << " ]";
  qDebug() << "Number of Iterations: " << GetIterationsFromStopDescription(stopReason);
  qDebug() << "Value: " << m_optimizer->GetValue();

  if(!GetConvergenceFromStopDescription(stopReason))
  {
    setErrorCondition(-66800, stopReason);
    notifyStatusMessage(stopReason);
    return;
  }

  AttributeMatrixShPtr transformAM = getDataContainerArray()->getDataContainer(m_blendDCName)->getAttributeMatrix(m_transformAMName);
  transformAM->getAttributeArrayAs<UInt64ArrayType>(m_iterationsAAName)->push_back(GetIterationsFromStopDescription(stopReason));
  transformAM->getAttributeArrayAs<DoubleArrayType>(m_valueAAName)->push_back(m_optimizer->GetValue());
  transformAM->getAttributeArrayAs<DoubleArrayType>(m_transformAAName)->setArray(transform);
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

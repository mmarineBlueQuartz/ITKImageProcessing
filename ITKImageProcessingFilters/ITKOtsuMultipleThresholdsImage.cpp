// File automatically generated

/*
 * Your License or Copyright can go here
 */

#include "ITKImageProcessing/ITKImageProcessingFilters/ITKOtsuMultipleThresholdsImage.h"
#include "SIMPLib/ITK/SimpleITKEnums.h"

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"

#include "SIMPLib/Geometry/ImageGeom.h"

#include "SIMPLib/ITK/Dream3DTemplateAliasMacro.h"
#include "SIMPLib/ITK/itkDream3DImage.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ITKOtsuMultipleThresholdsImage::ITKOtsuMultipleThresholdsImage()
{
  m_NumberOfThresholds = StaticCastScalar<int, int, int>(1u);
  m_LabelOffset = StaticCastScalar<int, int, int>(0u);
  m_NumberOfHistogramBins = StaticCastScalar<double, double, double>(128u);
  m_ValleyEmphasis = StaticCastScalar<bool, bool, bool>(false);

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ITKOtsuMultipleThresholdsImage::~ITKOtsuMultipleThresholdsImage() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ITKOtsuMultipleThresholdsImage::setupFilterParameters()
{
  FilterParameterVectorType parameters;

  parameters.push_back(SIMPL_NEW_INTEGER_FP("NumberOfThresholds", NumberOfThresholds, FilterParameter::Parameter, ITKOtsuMultipleThresholdsImage));
  parameters.push_back(SIMPL_NEW_INTEGER_FP("LabelOffset", LabelOffset, FilterParameter::Parameter, ITKOtsuMultipleThresholdsImage));
  parameters.push_back(SIMPL_NEW_DOUBLE_FP("NumberOfHistogramBins", NumberOfHistogramBins, FilterParameter::Parameter, ITKOtsuMultipleThresholdsImage));
  parameters.push_back(SIMPL_NEW_BOOL_FP("ValleyEmphasis", ValleyEmphasis, FilterParameter::Parameter, ITKOtsuMultipleThresholdsImage));


  QStringList linkedProps;
  linkedProps << "NewCellArrayName";
  parameters.push_back(SIMPL_NEW_LINKED_BOOL_FP("Save as New Array", SaveAsNewArray, FilterParameter::Parameter, ITKOtsuMultipleThresholdsImage, linkedProps));
  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::Defaults::AnyPrimitive, SIMPL::Defaults::AnyComponentSize, AttributeMatrix::Type::Cell, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Attribute Array to filter", SelectedCellArrayPath, FilterParameter::RequiredArray, ITKOtsuMultipleThresholdsImage, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::CreatedArray));
  parameters.push_back(SIMPL_NEW_STRING_FP("Filtered Array", NewCellArrayName, FilterParameter::CreatedArray, ITKOtsuMultipleThresholdsImage));

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ITKOtsuMultipleThresholdsImage::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setSelectedCellArrayPath(reader->readDataArrayPath("SelectedCellArrayPath", getSelectedCellArrayPath()));
  setNewCellArrayName(reader->readString("NewCellArrayName", getNewCellArrayName()));
  setSaveAsNewArray(reader->readValue("SaveAsNewArray", getSaveAsNewArray()));
  setNumberOfThresholds(reader->readValue("NumberOfThresholds", getNumberOfThresholds()));
  setLabelOffset(reader->readValue("LabelOffset", getLabelOffset()));
  setNumberOfHistogramBins(reader->readValue("NumberOfHistogramBins", getNumberOfHistogramBins()));
  setValleyEmphasis(reader->readValue("ValleyEmphasis", getValleyEmphasis()));

  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <typename InputPixelType, typename OutputPixelType, unsigned int Dimension> void ITKOtsuMultipleThresholdsImage::dataCheck()
{
  clearErrorCode();
  clearWarningCode();

  // Check consistency of parameters
  this->CheckIntegerEntry<uint8_t, int>(m_NumberOfThresholds, "NumberOfThresholds", true);
  this->CheckIntegerEntry<uint8_t, int>(m_LabelOffset, "LabelOffset", true);
  this->CheckIntegerEntry<uint32_t, double>(m_NumberOfHistogramBins, "NumberOfHistogramBins", true);

  ITKImageProcessingBase::dataCheck<InputPixelType, OutputPixelType, Dimension>();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ITKOtsuMultipleThresholdsImage::dataCheckInternal()
{
  Dream3DArraySwitchMacroOutputType(this->dataCheck, getSelectedCellArrayPath(), -4,uint8_t, 0);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------

template <typename InputPixelType, typename OutputPixelType, unsigned int Dimension> void ITKOtsuMultipleThresholdsImage::filter()
{
  typedef itk::Dream3DImage<InputPixelType, Dimension> InputImageType;
  typedef itk::Dream3DImage<OutputPixelType, Dimension> OutputImageType;
  // define filter
  typedef itk::OtsuMultipleThresholdsImageFilter<InputImageType, OutputImageType> FilterType;
  typename FilterType::Pointer filter = FilterType::New();
  filter->SetNumberOfThresholds(static_cast<uint8_t>(m_NumberOfThresholds));
  filter->SetLabelOffset(static_cast<uint8_t>(m_LabelOffset));
  filter->SetNumberOfHistogramBins(static_cast<uint32_t>(m_NumberOfHistogramBins));
  filter->SetValleyEmphasis(static_cast<bool>(m_ValleyEmphasis));
  this->ITKImageProcessingBase::filter<InputPixelType, OutputPixelType, Dimension, FilterType>(filter);
//{
//  QString outputVal = "Thresholds :%1";
//  m_Thresholds = filter->GetThresholds();
//}

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ITKOtsuMultipleThresholdsImage::filterInternal()
{
  Dream3DArraySwitchMacroOutputType(this->filter, getSelectedCellArrayPath(), -4,uint8_t, 0);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer ITKOtsuMultipleThresholdsImage::newFilterInstance(bool copyFilterParameters) const
{
  ITKOtsuMultipleThresholdsImage::Pointer filter = ITKOtsuMultipleThresholdsImage::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ITKOtsuMultipleThresholdsImage::getHumanLabel() const
{
  return "ITK::Otsu Multiple Thresholds Image Filter";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QUuid ITKOtsuMultipleThresholdsImage::getUuid()
{
  return QUuid("{6e66563a-edcf-5e11-bc1d-ceed36d8493f}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ITKOtsuMultipleThresholdsImage::getSubGroupName() const
{
  return "ITK Thresholding";
}

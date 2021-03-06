// File automatically generated

/*
 * Your License or Copyright can go here
 */

#include "ITKImageProcessing/ITKImageProcessingFilters/ITKStandardDeviationProjectionImage.h"
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
ITKStandardDeviationProjectionImage::ITKStandardDeviationProjectionImage()
{
  m_ProjectionDimension = StaticCastScalar<double, double, double>(0u);

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ITKStandardDeviationProjectionImage::~ITKStandardDeviationProjectionImage() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ITKStandardDeviationProjectionImage::setupFilterParameters()
{
  FilterParameterVectorType parameters;

  parameters.push_back(SIMPL_NEW_DOUBLE_FP("ProjectionDimension", ProjectionDimension, FilterParameter::Parameter, ITKStandardDeviationProjectionImage));


  QStringList linkedProps;
  linkedProps << "NewCellArrayName";
  parameters.push_back(SIMPL_NEW_LINKED_BOOL_FP("Save as New Array", SaveAsNewArray, FilterParameter::Parameter, ITKStandardDeviationProjectionImage, linkedProps));
  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::Defaults::AnyPrimitive, SIMPL::Defaults::AnyComponentSize, AttributeMatrix::Type::Cell, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Attribute Array to filter", SelectedCellArrayPath, FilterParameter::RequiredArray, ITKStandardDeviationProjectionImage, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::CreatedArray));
  parameters.push_back(SIMPL_NEW_STRING_FP("Filtered Array", NewCellArrayName, FilterParameter::CreatedArray, ITKStandardDeviationProjectionImage));

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ITKStandardDeviationProjectionImage::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setSelectedCellArrayPath(reader->readDataArrayPath("SelectedCellArrayPath", getSelectedCellArrayPath()));
  setNewCellArrayName(reader->readString("NewCellArrayName", getNewCellArrayName()));
  setSaveAsNewArray(reader->readValue("SaveAsNewArray", getSaveAsNewArray()));
  setProjectionDimension(reader->readValue("ProjectionDimension", getProjectionDimension()));

  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <typename InputPixelType, typename OutputPixelType, unsigned int Dimension> void ITKStandardDeviationProjectionImage::dataCheck()
{
  clearErrorCode();
  clearWarningCode();

  // Check consistency of parameters
  this->CheckIntegerEntry<unsigned int, double>(m_ProjectionDimension, "ProjectionDimension", true);

  ITKImageProcessingBase::dataCheck<InputPixelType, OutputPixelType, Dimension>();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ITKStandardDeviationProjectionImage::dataCheckInternal()
{
  Dream3DArraySwitchMacroOutputType(this->dataCheck, getSelectedCellArrayPath(), -4, typename itk::NumericTraits<typename InputImageType::PixelType>::RealType, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------

template <typename InputPixelType, typename OutputPixelType, unsigned int Dimension> void ITKStandardDeviationProjectionImage::filter()
{
  typedef itk::Dream3DImage<InputPixelType, Dimension> InputImageType;
  typedef itk::Dream3DImage<OutputPixelType, Dimension> OutputImageType;
  // define filter
  typedef itk::StandardDeviationProjectionImageFilter<InputImageType, OutputImageType> FilterType;
  typename FilterType::Pointer filter = FilterType::New();
  filter->SetProjectionDimension(static_cast<unsigned int>(m_ProjectionDimension));
  this->ITKImageProcessingBase::filter<InputPixelType, OutputPixelType, Dimension, FilterType>(filter);

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ITKStandardDeviationProjectionImage::filterInternal()
{
  Dream3DArraySwitchMacroOutputType(this->filter, getSelectedCellArrayPath(), -4, typename itk::NumericTraits<typename InputImageType::PixelType>::RealType, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer ITKStandardDeviationProjectionImage::newFilterInstance(bool copyFilterParameters) const
{
  ITKStandardDeviationProjectionImage::Pointer filter = ITKStandardDeviationProjectionImage::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ITKStandardDeviationProjectionImage::getHumanLabel() const
{
  return "ITK::Standard Deviation Projection Image Filter";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QUuid ITKStandardDeviationProjectionImage::getUuid()
{
  return QUuid("{89b327a7-c6a0-5965-b8aa-9d8bfcedcc76}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ITKStandardDeviationProjectionImage::getSubGroupName() const
{
  return "ITK ImageStatistics";
}

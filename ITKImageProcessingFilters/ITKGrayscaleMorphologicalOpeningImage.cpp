// File automatically generated

/*
 * Your License or Copyright can go here
 */

#include "ITKImageProcessing/ITKImageProcessingFilters/ITKGrayscaleMorphologicalOpeningImage.h"
#include "SIMPLib/ITK/SimpleITKEnums.h"

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/ChoiceFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"

#include "SIMPLib/Geometry/ImageGeom.h"

#include "SIMPLib/ITK/Dream3DTemplateAliasMacro.h"
#include "SIMPLib/ITK/itkDream3DImage.h"
#include <itkFlatStructuringElement.h>


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ITKGrayscaleMorphologicalOpeningImage::ITKGrayscaleMorphologicalOpeningImage()
{
  m_SafeBorder = StaticCastScalar<bool, bool, bool>(true);
  m_KernelRadius = CastStdToVec3<std::vector<unsigned int>, FloatVec3Type, float>(std::vector<unsigned int>(3, 1));
  m_KernelType = StaticCastScalar<int, int, int>(itk::simple::sitkBall);

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ITKGrayscaleMorphologicalOpeningImage::~ITKGrayscaleMorphologicalOpeningImage() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ITKGrayscaleMorphologicalOpeningImage::setupFilterParameters()
{
  FilterParameterVectorType parameters;

  // Structuring element
  {
    ChoiceFilterParameter::Pointer parameter = ChoiceFilterParameter::New();
    parameter->setHumanLabel("Kernel Type");
    parameter->setPropertyName("KernelType");
    parameter->setSetterCallback(SIMPL_BIND_SETTER(ITKGrayscaleMorphologicalOpeningImage, this, KernelType));
    parameter->setGetterCallback(SIMPL_BIND_GETTER(ITKGrayscaleMorphologicalOpeningImage, this, KernelType));

    QVector<QString> choices;
    choices.push_back("Annulus");
    choices.push_back("Ball");
    choices.push_back("Box");
    choices.push_back("Cross");
    parameter->setChoices(choices);
    parameter->setCategory(FilterParameter::Parameter);
    parameters.push_back(parameter);
  }
  // Other parameters
  parameters.push_back(SIMPL_NEW_BOOL_FP("SafeBorder", SafeBorder, FilterParameter::Parameter, ITKGrayscaleMorphologicalOpeningImage));
  parameters.push_back(SIMPL_NEW_FLOAT_VEC3_FP("KernelRadius", KernelRadius, FilterParameter::Parameter, ITKGrayscaleMorphologicalOpeningImage));


  QStringList linkedProps;
  linkedProps << "NewCellArrayName";
  parameters.push_back(SIMPL_NEW_LINKED_BOOL_FP("Save as New Array", SaveAsNewArray, FilterParameter::Parameter, ITKGrayscaleMorphologicalOpeningImage, linkedProps));
  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::Defaults::AnyPrimitive, SIMPL::Defaults::AnyComponentSize, AttributeMatrix::Type::Cell, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Attribute Array to filter", SelectedCellArrayPath, FilterParameter::RequiredArray, ITKGrayscaleMorphologicalOpeningImage, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::CreatedArray));
  parameters.push_back(SIMPL_NEW_STRING_FP("Filtered Array", NewCellArrayName, FilterParameter::CreatedArray, ITKGrayscaleMorphologicalOpeningImage));

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ITKGrayscaleMorphologicalOpeningImage::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setSelectedCellArrayPath(reader->readDataArrayPath("SelectedCellArrayPath", getSelectedCellArrayPath()));
  setNewCellArrayName(reader->readString("NewCellArrayName", getNewCellArrayName()));
  setSaveAsNewArray(reader->readValue("SaveAsNewArray", getSaveAsNewArray()));
  setSafeBorder(reader->readValue("SafeBorder", getSafeBorder()));
  setKernelRadius(reader->readFloatVec3("KernelRadius", getKernelRadius()));
  setKernelType(reader->readValue("KernelType", getKernelType()));

  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <typename InputPixelType, typename OutputPixelType, unsigned int Dimension> void ITKGrayscaleMorphologicalOpeningImage::dataCheck()
{
  clearErrorCode();
  clearWarningCode();

  // Check consistency of parameters
  this->CheckVectorEntry<unsigned int, FloatVec3Type>(m_KernelRadius, "KernelRadius", true);

  ITKImageProcessingBase::dataCheck<InputPixelType, OutputPixelType, Dimension>();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ITKGrayscaleMorphologicalOpeningImage::dataCheckInternal()
{
  Dream3DArraySwitchMacro(this->dataCheck, getSelectedCellArrayPath(), -4);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------

template <typename InputPixelType, typename OutputPixelType, unsigned int Dimension> void ITKGrayscaleMorphologicalOpeningImage::filter()
{
  typedef itk::Dream3DImage<InputPixelType, Dimension> InputImageType;
  typedef itk::Dream3DImage<OutputPixelType, Dimension> OutputImageType;
  typedef itk::FlatStructuringElement<Dimension> StructuringElementType;
  typedef typename StructuringElementType::RadiusType RadiusType;
  RadiusType elementRadius = CastVec3ToITK<FloatVec3Type, RadiusType, typename RadiusType::SizeValueType>(m_KernelRadius, RadiusType::Dimension);
  StructuringElementType structuringElement;
  switch(getKernelType())
  {
  case 0:
    structuringElement = StructuringElementType::Annulus(elementRadius, false);
    break;
  case 1:
    structuringElement = StructuringElementType::Ball(elementRadius, false);
    break;
  case 2:
    structuringElement = StructuringElementType::Box(elementRadius);
    break;
  case 3:
    structuringElement = StructuringElementType::Cross(elementRadius);
    break;
  default:
    setErrorCondition(-20, "Unsupported structuring element");
    return;
  }
  // define filter
  typedef itk::GrayscaleMorphologicalOpeningImageFilter<InputImageType, OutputImageType, StructuringElementType> FilterType;
  typename FilterType::Pointer filter = FilterType::New();
  filter->SetSafeBorder(static_cast<bool>(m_SafeBorder));
  filter->SetKernel(structuringElement);
  this->ITKImageProcessingBase::filter<InputPixelType, OutputPixelType, Dimension, FilterType>(filter);

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ITKGrayscaleMorphologicalOpeningImage::filterInternal()
{
  Dream3DArraySwitchMacro(this->filter, getSelectedCellArrayPath(), -4);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer ITKGrayscaleMorphologicalOpeningImage::newFilterInstance(bool copyFilterParameters) const
{
  ITKGrayscaleMorphologicalOpeningImage::Pointer filter = ITKGrayscaleMorphologicalOpeningImage::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ITKGrayscaleMorphologicalOpeningImage::getHumanLabel() const
{
  return "ITK::Grayscale Morphological Opening Image Filter";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QUuid ITKGrayscaleMorphologicalOpeningImage::getUuid()
{
  return QUuid("{c88ac42b-9477-5088-9ec0-862af1e0bb56}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ITKGrayscaleMorphologicalOpeningImage::getSubGroupName() const
{
  return "ITK BiasCorrection";
}

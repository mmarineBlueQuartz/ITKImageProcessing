// File automatically generated

/*
 * Your License or Copyright can go here
 */

#pragma once

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif

#include "ITKImageProcessingBase.h"

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/SIMPLib.h"

// Auto includes
#include <SIMPLib/FilterParameters/DoubleFilterParameter.h>
#include <algorithm>
#include <itkCurvatureAnisotropicDiffusionImageFilter.h>

#include "ITKImageProcessing/ITKImageProcessingDLLExport.h"

/**
 * @brief The ITKCurvatureAnisotropicDiffusionImage class. See [Filter documentation](@ref ITKCurvatureAnisotropicDiffusionImage) for details.
 */
class ITKImageProcessing_EXPORT ITKCurvatureAnisotropicDiffusionImage : public ITKImageProcessingBase
{
  Q_OBJECT
  PYB11_CREATE_BINDINGS(ITKCurvatureAnisotropicDiffusionImage SUPERCLASS ITKImageProcessingBase)
  PYB11_PROPERTY(double TimeStep READ getTimeStep WRITE setTimeStep)
  PYB11_PROPERTY(double ConductanceParameter READ getConductanceParameter WRITE setConductanceParameter)
  PYB11_PROPERTY(double ConductanceScalingUpdateInterval READ getConductanceScalingUpdateInterval WRITE setConductanceScalingUpdateInterval)
  PYB11_PROPERTY(double NumberOfIterations READ getNumberOfIterations WRITE setNumberOfIterations)

public:
  SIMPL_SHARED_POINTERS(ITKCurvatureAnisotropicDiffusionImage)
  SIMPL_FILTER_NEW_MACRO(ITKCurvatureAnisotropicDiffusionImage)
  SIMPL_TYPE_MACRO_SUPER_OVERRIDE(ITKCurvatureAnisotropicDiffusionImage, AbstractFilter)

  ~ITKCurvatureAnisotropicDiffusionImage() override;

  SIMPL_FILTER_PARAMETER(double, TimeStep)
  Q_PROPERTY(double TimeStep READ getTimeStep WRITE setTimeStep)

  SIMPL_FILTER_PARAMETER(double, ConductanceParameter)
  Q_PROPERTY(double ConductanceParameter READ getConductanceParameter WRITE setConductanceParameter)

  SIMPL_FILTER_PARAMETER(double, ConductanceScalingUpdateInterval)
  Q_PROPERTY(double ConductanceScalingUpdateInterval READ getConductanceScalingUpdateInterval WRITE setConductanceScalingUpdateInterval)

  SIMPL_FILTER_PARAMETER(double, NumberOfIterations)
  Q_PROPERTY(double NumberOfIterations READ getNumberOfIterations WRITE setNumberOfIterations)


  /**
   * @brief newFilterInstance Reimplemented from @see AbstractFilter class
   */
  AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters) const override;

  /**
   * @brief getHumanLabel Reimplemented from @see AbstractFilter class
   */
  const QString getHumanLabel() const override;

  /**
   * @brief getSubGroupName Reimplemented from @see AbstractFilter class
   */
  const QString getSubGroupName() const override;

  /**
   * @brief getUuid Return the unique identifier for this filter.
   * @return A QUuid object.
   */
  const QUuid getUuid() override;

  /**
   * @brief setupFilterParameters Reimplemented from @see AbstractFilter class
   */
  void setupFilterParameters() override;

  /**
   * @brief readFilterParameters Reimplemented from @see AbstractFilter class
   */
  void readFilterParameters(AbstractFilterParametersReader* reader, int index) override;

protected:
  ITKCurvatureAnisotropicDiffusionImage();

  /**
   * @brief dataCheckInternal overloads dataCheckInternal in ITKImageBase and calls templated dataCheck
   */
  void dataCheckInternal() override;

  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  template <typename InputImageType, typename OutputImageType, unsigned int Dimension> void dataCheck();

  /**
  * @brief filterInternal overloads filterInternal in ITKImageBase and calls templated filter
  */
  void filterInternal() override;

  /**
  * @brief Applies the filter
  */
  template <typename InputImageType, typename OutputImageType, unsigned int Dimension> void filter();

public:
  ITKCurvatureAnisotropicDiffusionImage(const ITKCurvatureAnisotropicDiffusionImage&) = delete;    // Copy Constructor Not Implemented
  ITKCurvatureAnisotropicDiffusionImage(ITKCurvatureAnisotropicDiffusionImage&&) = delete;         // Move Constructor Not Implemented
  ITKCurvatureAnisotropicDiffusionImage& operator=(const ITKCurvatureAnisotropicDiffusionImage&) = delete; // Copy Assignment Not Implemented
  ITKCurvatureAnisotropicDiffusionImage& operator=(ITKCurvatureAnisotropicDiffusionImage&&) = delete;      // Move Assignment Not Implemented
};

#ifdef __clang__
#pragma clang diagnostic pop
#endif


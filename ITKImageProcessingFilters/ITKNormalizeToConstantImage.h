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
#include <itkNormalizeToConstantImageFilter.h>

#include "ITKImageProcessing/ITKImageProcessingDLLExport.h"

/**
 * @brief The ITKNormalizeToConstantImage class. See [Filter documentation](@ref ITKNormalizeToConstantImage) for details.
 */
class ITKImageProcessing_EXPORT ITKNormalizeToConstantImage : public ITKImageProcessingBase
{
  Q_OBJECT
  PYB11_CREATE_BINDINGS(ITKNormalizeToConstantImage SUPERCLASS ITKImageProcessingBase)
  PYB11_PROPERTY(double Constant READ getConstant WRITE setConstant)

public:
  SIMPL_SHARED_POINTERS(ITKNormalizeToConstantImage)
  SIMPL_FILTER_NEW_MACRO(ITKNormalizeToConstantImage)
  SIMPL_TYPE_MACRO_SUPER_OVERRIDE(ITKNormalizeToConstantImage, AbstractFilter)

  ~ITKNormalizeToConstantImage() override;

  SIMPL_FILTER_PARAMETER(double, Constant)
  Q_PROPERTY(double Constant READ getConstant WRITE setConstant)


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
  ITKNormalizeToConstantImage();

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
  ITKNormalizeToConstantImage(const ITKNormalizeToConstantImage&) = delete;    // Copy Constructor Not Implemented
  ITKNormalizeToConstantImage(ITKNormalizeToConstantImage&&) = delete;         // Move Constructor Not Implemented
  ITKNormalizeToConstantImage& operator=(const ITKNormalizeToConstantImage&) = delete; // Copy Assignment Not Implemented
  ITKNormalizeToConstantImage& operator=(ITKNormalizeToConstantImage&&) = delete;      // Move Assignment Not Implemented
};

#ifdef __clang__
#pragma clang diagnostic pop
#endif


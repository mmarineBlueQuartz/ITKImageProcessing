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
#include <SIMPLib/FilterParameters/IntFilterParameter.h>
#include <itkZeroCrossingImageFilter.h>

#include "ITKImageProcessing/ITKImageProcessingDLLExport.h"

/**
 * @brief The ITKZeroCrossingImage class. See [Filter documentation](@ref ITKZeroCrossingImage) for details.
 */
class ITKImageProcessing_EXPORT ITKZeroCrossingImage : public ITKImageProcessingBase
{
  Q_OBJECT
  PYB11_CREATE_BINDINGS(ITKZeroCrossingImage SUPERCLASS ITKImageProcessingBase)
  PYB11_PROPERTY(int ForegroundValue READ getForegroundValue WRITE setForegroundValue)
  PYB11_PROPERTY(int BackgroundValue READ getBackgroundValue WRITE setBackgroundValue)

public:
  SIMPL_SHARED_POINTERS(ITKZeroCrossingImage)
  SIMPL_FILTER_NEW_MACRO(ITKZeroCrossingImage)
  SIMPL_TYPE_MACRO_SUPER_OVERRIDE(ITKZeroCrossingImage, AbstractFilter)

  ~ITKZeroCrossingImage() override;

  SIMPL_FILTER_PARAMETER(int, ForegroundValue)
  Q_PROPERTY(int ForegroundValue READ getForegroundValue WRITE setForegroundValue)

  SIMPL_FILTER_PARAMETER(int, BackgroundValue)
  Q_PROPERTY(int BackgroundValue READ getBackgroundValue WRITE setBackgroundValue)


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
  ITKZeroCrossingImage();

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
  ITKZeroCrossingImage(const ITKZeroCrossingImage&) = delete;    // Copy Constructor Not Implemented
  ITKZeroCrossingImage(ITKZeroCrossingImage&&) = delete;         // Move Constructor Not Implemented
  ITKZeroCrossingImage& operator=(const ITKZeroCrossingImage&) = delete; // Copy Assignment Not Implemented
  ITKZeroCrossingImage& operator=(ITKZeroCrossingImage&&) = delete;      // Move Assignment Not Implemented
};

#ifdef __clang__
#pragma clang diagnostic pop
#endif


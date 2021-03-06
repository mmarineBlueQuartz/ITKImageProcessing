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
#include <itkApproximateSignedDistanceMapImageFilter.h>

#include "ITKImageProcessing/ITKImageProcessingDLLExport.h"

/**
 * @brief The ITKApproximateSignedDistanceMapImage class. See [Filter documentation](@ref ITKApproximateSignedDistanceMapImage) for details.
 */
class ITKImageProcessing_EXPORT ITKApproximateSignedDistanceMapImage : public ITKImageProcessingBase
{
  Q_OBJECT
  PYB11_CREATE_BINDINGS(ITKApproximateSignedDistanceMapImage SUPERCLASS ITKImageProcessingBase)
  PYB11_PROPERTY(double InsideValue READ getInsideValue WRITE setInsideValue)
  PYB11_PROPERTY(double OutsideValue READ getOutsideValue WRITE setOutsideValue)

public:
  SIMPL_SHARED_POINTERS(ITKApproximateSignedDistanceMapImage)
  SIMPL_FILTER_NEW_MACRO(ITKApproximateSignedDistanceMapImage)
  SIMPL_TYPE_MACRO_SUPER_OVERRIDE(ITKApproximateSignedDistanceMapImage, AbstractFilter)

  ~ITKApproximateSignedDistanceMapImage() override;

  SIMPL_FILTER_PARAMETER(double, InsideValue)
  Q_PROPERTY(double InsideValue READ getInsideValue WRITE setInsideValue)

  SIMPL_FILTER_PARAMETER(double, OutsideValue)
  Q_PROPERTY(double OutsideValue READ getOutsideValue WRITE setOutsideValue)


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
  ITKApproximateSignedDistanceMapImage();

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
  ITKApproximateSignedDistanceMapImage(const ITKApproximateSignedDistanceMapImage&) = delete;    // Copy Constructor Not Implemented
  ITKApproximateSignedDistanceMapImage(ITKApproximateSignedDistanceMapImage&&) = delete;         // Move Constructor Not Implemented
  ITKApproximateSignedDistanceMapImage& operator=(const ITKApproximateSignedDistanceMapImage&) = delete; // Copy Assignment Not Implemented
  ITKApproximateSignedDistanceMapImage& operator=(ITKApproximateSignedDistanceMapImage&&) = delete;      // Move Assignment Not Implemented
};

#ifdef __clang__
#pragma clang diagnostic pop
#endif


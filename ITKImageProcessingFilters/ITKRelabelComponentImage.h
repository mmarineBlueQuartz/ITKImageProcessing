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
#include <SIMPLib/FilterParameters/BooleanFilterParameter.h>
#include <SIMPLib/FilterParameters/DoubleFilterParameter.h>
#include <SIMPLib/FilterParameters/FloatVec3FilterParameter.h>
#include <itkRelabelComponentImageFilter.h>

#include "ITKImageProcessing/ITKImageProcessingDLLExport.h"

/**
 * @brief The ITKRelabelComponentImage class. See [Filter documentation](@ref ITKRelabelComponentImage) for details.
 */
class ITKImageProcessing_EXPORT ITKRelabelComponentImage : public ITKImageProcessingBase
{
  Q_OBJECT
  PYB11_CREATE_BINDINGS(ITKRelabelComponentImage SUPERCLASS ITKImageProcessingBase)
  PYB11_PROPERTY(double MinimumObjectSize READ getMinimumObjectSize WRITE setMinimumObjectSize)
  PYB11_PROPERTY(bool SortByObjectSize READ getSortByObjectSize WRITE setSortByObjectSize)
  PYB11_PROPERTY(double NumberOfObjects READ getNumberOfObjects WRITE setNumberOfObjects)
  PYB11_PROPERTY(double OriginalNumberOfObjects READ getOriginalNumberOfObjects WRITE setOriginalNumberOfObjects)
  PYB11_PROPERTY(FloatVec3Type SizeOfObjectsInPhysicalUnits READ getSizeOfObjectsInPhysicalUnits WRITE setSizeOfObjectsInPhysicalUnits)
  PYB11_PROPERTY(FloatVec3Type SizeOfObjectsInPixels READ getSizeOfObjectsInPixels WRITE setSizeOfObjectsInPixels)

public:
  SIMPL_SHARED_POINTERS(ITKRelabelComponentImage)
  SIMPL_FILTER_NEW_MACRO(ITKRelabelComponentImage)
  SIMPL_TYPE_MACRO_SUPER_OVERRIDE(ITKRelabelComponentImage, AbstractFilter)

  ~ITKRelabelComponentImage() override;

  SIMPL_FILTER_PARAMETER(double, MinimumObjectSize)
  Q_PROPERTY(double MinimumObjectSize READ getMinimumObjectSize WRITE setMinimumObjectSize)

  SIMPL_FILTER_PARAMETER(bool, SortByObjectSize)
  Q_PROPERTY(bool SortByObjectSize READ getSortByObjectSize WRITE setSortByObjectSize)

  SIMPL_FILTER_PARAMETER(double, NumberOfObjects)
  Q_PROPERTY(double NumberOfObjects READ getNumberOfObjects)

  SIMPL_FILTER_PARAMETER(double, OriginalNumberOfObjects)
  Q_PROPERTY(double OriginalNumberOfObjects READ getOriginalNumberOfObjects)

  SIMPL_FILTER_PARAMETER(FloatVec3Type, SizeOfObjectsInPhysicalUnits)
  Q_PROPERTY(FloatVec3Type SizeOfObjectsInPhysicalUnits READ getSizeOfObjectsInPhysicalUnits)

  SIMPL_FILTER_PARAMETER(FloatVec3Type, SizeOfObjectsInPixels)
  Q_PROPERTY(FloatVec3Type SizeOfObjectsInPixels READ getSizeOfObjectsInPixels)

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
  ITKRelabelComponentImage();

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
  ITKRelabelComponentImage(const ITKRelabelComponentImage&) = delete;    // Copy Constructor Not Implemented
  ITKRelabelComponentImage(ITKRelabelComponentImage&&) = delete;         // Move Constructor Not Implemented
  ITKRelabelComponentImage& operator=(const ITKRelabelComponentImage&) = delete; // Copy Assignment Not Implemented
  ITKRelabelComponentImage& operator=(ITKRelabelComponentImage&&) = delete;      // Move Assignment Not Implemented
};

#ifdef __clang__
#pragma clang diagnostic pop
#endif


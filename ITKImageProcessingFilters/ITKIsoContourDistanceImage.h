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
#include <itkIsoContourDistanceImageFilter.h>

#include "ITKImageProcessing/ITKImageProcessingDLLExport.h"

/**
 * @brief The ITKIsoContourDistanceImage class. See [Filter documentation](@ref ITKIsoContourDistanceImage) for details.
 */
class ITKImageProcessing_EXPORT ITKIsoContourDistanceImage : public ITKImageProcessingBase
{
  Q_OBJECT
  PYB11_CREATE_BINDINGS(ITKIsoContourDistanceImage SUPERCLASS ITKImageProcessingBase)
  PYB11_PROPERTY(double LevelSetValue READ getLevelSetValue WRITE setLevelSetValue)
  PYB11_PROPERTY(double FarValue READ getFarValue WRITE setFarValue)

public:
  SIMPL_SHARED_POINTERS(ITKIsoContourDistanceImage)
  SIMPL_FILTER_NEW_MACRO(ITKIsoContourDistanceImage)
  SIMPL_TYPE_MACRO_SUPER_OVERRIDE(ITKIsoContourDistanceImage, AbstractFilter)

  ~ITKIsoContourDistanceImage() override;

  SIMPL_FILTER_PARAMETER(double, LevelSetValue)
  Q_PROPERTY(double LevelSetValue READ getLevelSetValue WRITE setLevelSetValue)

  SIMPL_FILTER_PARAMETER(double, FarValue)
  Q_PROPERTY(double FarValue READ getFarValue WRITE setFarValue)


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
  ITKIsoContourDistanceImage();

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
  ITKIsoContourDistanceImage(const ITKIsoContourDistanceImage&) = delete;    // Copy Constructor Not Implemented
  ITKIsoContourDistanceImage(ITKIsoContourDistanceImage&&) = delete;         // Move Constructor Not Implemented
  ITKIsoContourDistanceImage& operator=(const ITKIsoContourDistanceImage&) = delete; // Copy Assignment Not Implemented
  ITKIsoContourDistanceImage& operator=(ITKIsoContourDistanceImage&&) = delete;      // Move Assignment Not Implemented
};

#ifdef __clang__
#pragma clang diagnostic pop
#endif


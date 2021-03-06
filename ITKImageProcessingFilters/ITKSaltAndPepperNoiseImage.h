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
#include <itkSaltAndPepperNoiseImageFilter.h>

#include "ITKImageProcessing/ITKImageProcessingDLLExport.h"

/**
 * @brief The ITKSaltAndPepperNoiseImage class. See [Filter documentation](@ref ITKSaltAndPepperNoiseImage) for details.
 */
class ITKImageProcessing_EXPORT ITKSaltAndPepperNoiseImage : public ITKImageProcessingBase
{
  Q_OBJECT
  PYB11_CREATE_BINDINGS(ITKSaltAndPepperNoiseImage SUPERCLASS ITKImageProcessingBase)
  PYB11_PROPERTY(double Probability READ getProbability WRITE setProbability)
  PYB11_PROPERTY(double Seed READ getSeed WRITE setSeed)

public:
  SIMPL_SHARED_POINTERS(ITKSaltAndPepperNoiseImage)
  SIMPL_FILTER_NEW_MACRO(ITKSaltAndPepperNoiseImage)
  SIMPL_TYPE_MACRO_SUPER_OVERRIDE(ITKSaltAndPepperNoiseImage, AbstractFilter)

  ~ITKSaltAndPepperNoiseImage() override;

  SIMPL_FILTER_PARAMETER(double, Probability)
  Q_PROPERTY(double Probability READ getProbability WRITE setProbability)

  SIMPL_FILTER_PARAMETER(double, Seed)
  Q_PROPERTY(double Seed READ getSeed WRITE setSeed)


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
  ITKSaltAndPepperNoiseImage();

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
  ITKSaltAndPepperNoiseImage(const ITKSaltAndPepperNoiseImage&) = delete;    // Copy Constructor Not Implemented
  ITKSaltAndPepperNoiseImage(ITKSaltAndPepperNoiseImage&&) = delete;         // Move Constructor Not Implemented
  ITKSaltAndPepperNoiseImage& operator=(const ITKSaltAndPepperNoiseImage&) = delete; // Copy Assignment Not Implemented
  ITKSaltAndPepperNoiseImage& operator=(ITKSaltAndPepperNoiseImage&&) = delete;      // Move Assignment Not Implemented
};

#ifdef __clang__
#pragma clang diagnostic pop
#endif


// File automatically generated

/*
 * Your License or Copyright can go here
 */
#prama once


#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif

#include "ITKImageProcessingBase.h"

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/SIMPLib.h"

// Auto includes
${IncludeName}

/**
 * @brief The ${FilterName} class. See [Filter documentation](@ref ${FilterName}) for details.
 */
class ${FilterName} : public ITKImageProcessingBase
{
  Q_OBJECT

public:
  SIMPL_SHARED_POINTERS(${FilterName})
  SIMPL_FILTER_NEW_MACRO(${FilterName})
  SIMPL_TYPE_MACRO_SUPER_OVERRIDE(${FilterName}, AbstractFilter)

  ~${FilterName}() override;

  ${Parameters} /**
                 * @brief newFilterInstance Reimplemented from @see AbstractFilter class
                 */
  AbstractFilter::Pointer
  newFilterInstance(bool copyFilterParameters) const override;

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
  void readFilterParameters(AbstractFilterParametersReader * reader, int index) override;

protected:
  ${FilterName}();

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

private:
  ${FilterName}(const ${FilterName}&) = delete;    // Copy Constructor Not Implemented
  void operator=(const ${FilterName}&) = delete;   // Move assignment Not Implemented
};

#ifdef __clang__
#pragma clang diagnostic pop
#endif

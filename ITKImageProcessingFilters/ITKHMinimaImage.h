// File automatically generated

/*
 * Your License or Copyright can go here
 */

#ifndef _ITKHMinimaImage_h_
#define _ITKHMinimaImage_h_

#include "ITKImageBase.h"

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"

//Auto includes
#include <SIMPLib/FilterParameters/BooleanFilterParameter.h>
#include <SIMPLib/FilterParameters/DoubleFilterParameter.h>
#include <itkHMinimaImageFilter.h>


/**
 * @brief The ITKHMinimaImage class. See [Filter documentation](@ref ITKHMinimaImage) for details.
 */
class ITKHMinimaImage : public ITKImageBase
{
  Q_OBJECT

  public:
    SIMPL_SHARED_POINTERS(ITKHMinimaImage)
    SIMPL_STATIC_NEW_MACRO(ITKHMinimaImage)
    SIMPL_TYPE_MACRO_SUPER_OVERRIDE(ITKHMinimaImage, AbstractFilter)

    virtual ~ITKHMinimaImage();

    SIMPL_FILTER_PARAMETER(double, Height)
    Q_PROPERTY(double Height READ getHeight WRITE setHeight)

    SIMPL_FILTER_PARAMETER(bool, FullyConnected)
    Q_PROPERTY(bool FullyConnected READ getFullyConnected WRITE setFullyConnected)



    /**
     * @brief newFilterInstance Reimplemented from @see AbstractFilter class
     */
    virtual AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters) override;

    /**
     * @brief getHumanLabel Reimplemented from @see AbstractFilter class
     */
    virtual const QString getHumanLabel() override;

    /**
     * @brief getSubGroupName Reimplemented from @see AbstractFilter class
     */
    virtual const QString getSubGroupName() override;
    
    /**
     * @brief setupFilterParameters Reimplemented from @see AbstractFilter class
     */
    virtual void setupFilterParameters() override;

    /**
     * @brief readFilterParameters Reimplemented from @see AbstractFilter class
     */
    virtual void readFilterParameters(AbstractFilterParametersReader* reader, int index) override;

  protected:
    ITKHMinimaImage();

    /**
     * @brief dataCheckInternal overloads dataCheckInternal in ITKImageBase and calls templated dataCheck
     */
    void virtual dataCheckInternal() override;

    /**
     * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
     */
    template<typename InputImageType, typename OutputImageType, unsigned int Dimension>
    void dataCheck();

    /**
    * @brief filterInternal overloads filterInternal in ITKImageBase and calls templated filter
    */
    void virtual filterInternal() override;

    /**
    * @brief Applies the filter
    */
    template<typename InputImageType, typename OutputImageType, unsigned int Dimension>
    void filter();

  private:

    ITKHMinimaImage(const ITKHMinimaImage&); // Copy Constructor Not Implemented
    void operator=(const ITKHMinimaImage&); // Operator '=' Not Implemented
};

#endif /* _ITKHMinimaImage_H_ */

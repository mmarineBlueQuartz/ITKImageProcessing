/* ============================================================================
* Copyright (c) 2009-2016 BlueQuartz Software, LLC
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*
* Redistributions of source code must retain the above copyright notice, this
* list of conditions and the following disclaimer.
*
* Redistributions in binary form must reproduce the above copyright notice, this
* list of conditions and the following disclaimer in the documentation and/or
* other materials provided with the distribution.
*
* Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
* contributors may be used to endorse or promote products derived from this software
* without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
* USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
* The code contained herein was partially funded by the followig contracts:
*    United States Air Force Prime Contract FA8650-07-D-5800
*    United States Air Force Prime Contract FA8650-10-D-5210
*    United States Prime Contract Navy N00173-07-C-2068
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#pragma once

#include "SIMPLib/ITK/itkFijiConfigurationFileReader.hpp"

#include "ITKImageProcessing/ITKImageProcessingFilters/ITKImportMontage.h"

// our PIMPL private class
class ITKImportFijiMontagePrivate;

/**
 * @brief The ITKImportFijiMontage class. See [Filter documentation](@ref ITKImportFijiMontage) for details.
 */
class ITKImageProcessing_EXPORT ITKImportFijiMontage : public ITKImportMontage
{
  Q_OBJECT
  PYB11_CREATE_BINDINGS(ITKImportFijiMontage SUPERCLASS ITKImportMontage)
  PYB11_PROPERTY(QString FijiConfigFilePath READ getFijiConfigFilePath WRITE setFijiConfigFilePath)
  Q_DECLARE_PRIVATE(ITKImportFijiMontage)
public:
  SIMPL_SHARED_POINTERS(ITKImportFijiMontage)
  SIMPL_FILTER_NEW_MACRO(ITKImportFijiMontage)
  SIMPL_TYPE_MACRO_SUPER_OVERRIDE(ITKImportFijiMontage, ITKImportMontage)

  ~ITKImportFijiMontage() override;

  SIMPL_FILTER_PARAMETER(QString, FijiConfigFilePath)
  Q_PROPERTY(QString FijiConfigFilePath READ getFijiConfigFilePath WRITE setFijiConfigFilePath)

  typedef std::vector<ITKImageReader::Pointer> ImageReaderVector;
  typedef std::vector<itk::FijiImageTileData> TileDataVector;

  SIMPL_PIMPL_PROPERTY_DECL(QString, FijiConfigFilePathCache)
  SIMPL_PIMPL_PROPERTY_DECL(QDateTime, LastRead)

  /**
   * @brief getCompiledLibraryName Reimplemented from @see AbstractFilter class
   */
  const QString getCompiledLibraryName() const override;

  /**
   * @brief getBrandingString Returns the branding string for the filter, which is a tag
   * used to denote the filter's association with specific plugins
   * @return Branding string
   */
  const QString getBrandingString() const override;

  /**
   * @brief getFilterVersion Returns a version string for this filter. Default
   * value is an empty string.
   * @return
   */
  const QString getFilterVersion() const override;

  /**
   * @brief newFilterInstance Reimplemented from @see AbstractFilter class
   */
  AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters) const override;

  /**
   * @brief getGroupName Reimplemented from @see AbstractFilter class
   */
  const QString getGroupName() const override;

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
   * @brief getHumanLabel Reimplemented from @see AbstractFilter class
   */
  const QString getHumanLabel() const override;

  /**
   * @brief setupFilterParameters Reimplemented from @see AbstractFilter class
   */
  void setupFilterParameters() override;

  /**
   * @brief execute Reimplemented from @see AbstractFilter class
   */
  void execute() override;

  /**
   * @brief preflight Reimplemented from @see AbstractFilter class
   */
  void preflight() override;

signals:
  /**
   * @brief updateFilterParameters Emitted when the Filter requests all the latest Filter parameters
   * be pushed from a user-facing control (such as a widget)
   * @param filter Filter instance pointer
   */
  void updateFilterParameters(AbstractFilter* filter);

  /**
   * @brief parametersChanged Emitted when any Filter parameter is changed internally
   */
  void parametersChanged();

  /**
   * @brief preflightAboutToExecute Emitted just before calling dataCheck()
   */
  void preflightAboutToExecute();

  /**
   * @brief preflightExecuted Emitted just after calling dataCheck()
   */
  void preflightExecuted();

protected:
  ITKImportFijiMontage();

  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck();

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

private:
  QScopedPointer<ITKImportFijiMontagePrivate> const d_ptr;

public :
  ITKImportFijiMontage(const ITKImportFijiMontage&) = delete; // Copy Constructor Not Implemented
  ITKImportFijiMontage(ITKImportFijiMontage&&) = delete;                   // Move Constructor Not Implemented
  ITKImportFijiMontage& operator=(const ITKImportFijiMontage&) = delete;   // Copy Assignment Not Implemented
  ITKImportFijiMontage& operator=(ITKImportFijiMontage&&) = delete;        // Move Assignment Not Implemented
};


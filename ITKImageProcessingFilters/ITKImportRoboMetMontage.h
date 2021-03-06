/*
 * Your License or Copyright Information can go here
 */

#pragma once

#include <QtCore/QFile>

#include "ITKImageProcessing/ITKImageProcessingFilters/ITKImportMontage.h"

 // our PIMPL private class
class ITKImportRoboMetMontagePrivate;

/**
 * @brief The ITKImportRoboMetMontage class. See [Filter documentation](@ref ITKImportRoboMetMontage) for details.
 */
class ITKImageProcessing_EXPORT ITKImportRoboMetMontage : public ITKImportMontage
{
  Q_OBJECT
  
  PYB11_CREATE_BINDINGS(ITKImportRoboMetMontage SUPERCLASS AbstractFilter)
  PYB11_PROPERTY(int SliceNumber READ getSliceNumber WRITE setSliceNumber)
  PYB11_PROPERTY(QString MetaDataAttributeMatrixName READ getMetaDataAttributeMatrixName WRITE setMetaDataAttributeMatrixName)
  PYB11_PROPERTY(QString RegistrationFile READ getRegistrationFile WRITE setRegistrationFile)
  PYB11_PROPERTY(QString ImageFilePrefix READ getImageFilePrefix WRITE setImageFilePrefix)
  PYB11_PROPERTY(QString ImageFileExtension READ getImageFileExtension WRITE setImageFileExtension)
   
  Q_DECLARE_PRIVATE(ITKImportRoboMetMontage)
public:
  SIMPL_SHARED_POINTERS(ITKImportRoboMetMontage)
  SIMPL_FILTER_NEW_MACRO(ITKImportRoboMetMontage)
  SIMPL_TYPE_MACRO_SUPER_OVERRIDE(ITKImportRoboMetMontage, AbstractFilter)

  ~ITKImportRoboMetMontage() override;
  /**
   * @brief These get filled out if there are errors. Negative values are error codes
   */
  SIMPL_INSTANCE_PROPERTY(int, ErrorCode)

  SIMPL_INSTANCE_STRING_PROPERTY(ErrorMessage)

  SIMPL_FILTER_PARAMETER(int, SliceNumber)
  Q_PROPERTY(int SliceNumber READ getSliceNumber WRITE setSliceNumber)

  SIMPL_FILTER_PARAMETER(QString, MetaDataAttributeMatrixName)
  Q_PROPERTY(QString MetaDataAttributeMatrixName READ getMetaDataAttributeMatrixName WRITE setMetaDataAttributeMatrixName)

  SIMPL_FILTER_PARAMETER(QString, RegistrationFile)
  Q_PROPERTY(QString RegistrationFile READ getRegistrationFile WRITE setRegistrationFile)

  SIMPL_FILTER_PARAMETER(QString, ImageFilePrefix)
  Q_PROPERTY(QString ImageFilePrefix READ getImageFilePrefix WRITE setImageFilePrefix)

  SIMPL_FILTER_PARAMETER(QString, ImageFileExtension)
  Q_PROPERTY(QString ImageFileExtension READ getImageFileExtension WRITE setImageFileExtension)

  using ReaderMap = QMap<QString, ITKImageReader::Pointer>;

  SIMPL_PIMPL_PROPERTY_DECL(QString, RoboMetConfigFilePathCache)
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
  ITKImportRoboMetMontage();

  /**
   * @brief parseFijiConfigFile Parses the Fiji file with the configuration coordinates
   * @return Integer error value
   */
  int32_t parseRoboMetConfigFile();

  /**
   * @brief parseRobometConfigFile Parses the Robomet file with the configuration coordinates
   * @param reader QFile to read
   * @return Integer error value
   */
  int32_t parseRobometConfigFile(QFile& reader);

  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck();

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

  /**
   * @brief Get the file path for an image file
   */
  QString getImageFilePath(const QString &filePath, int imageNumber, int row, int col);

private:
  QScopedPointer<ITKImportRoboMetMontagePrivate> const d_ptr;
	
  int32_t m_NumImages;
  std::vector<QString> m_RegisteredFilePaths;
  QMap<QString, QPointF> m_CoordsMap;
  std::vector<int> m_Rows;
  std::vector<int> m_Columns;

  /**
   * @brief clearParsingCache
   */
  void clearParsingCache();

public :
  ITKImportRoboMetMontage(const ITKImportRoboMetMontage&) = delete; // Copy Constructor Not Implemented
  ITKImportRoboMetMontage(ITKImportRoboMetMontage&&) = delete;                   // Move Constructor Not Implemented
  ITKImportRoboMetMontage& operator=(const ITKImportRoboMetMontage&) = delete;   // Copy Assignment Not Implemented
  ITKImportRoboMetMontage& operator=(ITKImportRoboMetMontage&&) = delete;        // Move Assignment Not Implemented
};


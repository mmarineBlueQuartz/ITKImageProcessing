/* ============================================================================
 * Copyright (c) 2009-2019 BlueQuartz Software, LLC
 * All rights reserved.
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
 * Neither the names of any of the BlueQuartz Software contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
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
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "ImportAxioVisionV4Montage.h"

#include <array>
#include <cstring>
#include <limits>
#include <set>

#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QSharedPointer>
#include <QtCore/QString>
#include <QtCore/QTextStream>
#include <QtCore/QVector>

#include "SIMPLib/CoreFilters/ConvertColorToGrayScale.h"
#include "SIMPLib/DataArrays/StringDataArray.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/BooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/DataContainerCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/FloatFilterParameter.h"
#include "SIMPLib/FilterParameters/FloatVec3FilterParameter.h"
#include "SIMPLib/FilterParameters/InputFileFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/PreflightUpdatedValueFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/Filtering/FilterManager.h"
#include "SIMPLib/Geometry/ImageGeom.h"

#include "ITKImageProcessing/ITKImageProcessingConstants.h"
#include "ITKImageProcessing/ITKImageProcessingVersion.h"
#include "ITKImageProcessing/ZeissXml/ZeissTagMapping.h"
#include "MetaXmlUtils.h"

#define ZIF_PRINT_DBG_MSGS 0

static const QString k_AttributeArrayNames("AttributeArrayNames");
static const QString k_DataContaineNameDefaultName("Tile");
static const QString k_TileAttributeMatrixDefaultName("Tile Data");
static const QString k_GrayScaleTempArrayName("gray_scale_temp");
static const QString k_AxioVisionMetaData("AxioVision MetaData");

enum createdPathID : RenameDataPath::DataID_t
{
  // DataContainerID = 1
};

/* ############## Start Private Implementation ############################### */
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class ImportAxioVisionV4MontagePrivate
{
  Q_DISABLE_COPY(ImportAxioVisionV4MontagePrivate)
  Q_DECLARE_PUBLIC(ImportAxioVisionV4Montage)
  ImportAxioVisionV4Montage* const q_ptr;
  ImportAxioVisionV4MontagePrivate(ImportAxioVisionV4Montage* ptr);

  QDomElement m_Root;
  ZeissTagsXmlSection::Pointer m_RootTagsSection;
  QString m_InputFile_Cache;
  QDateTime m_LastRead;
  QString m_MontageInformation;
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImportAxioVisionV4MontagePrivate::ImportAxioVisionV4MontagePrivate(ImportAxioVisionV4Montage* ptr)
: q_ptr(ptr)
, m_Root(QDomElement())
, m_InputFile_Cache("")
, m_LastRead(QDateTime())
{
}

/* ############## Start Public Implementation ############################### */

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImportAxioVisionV4Montage::ImportAxioVisionV4Montage()
: m_InputFile("")
, m_DataContainerName(k_DataContaineNameDefaultName)
, m_CellAttributeMatrixName(k_TileAttributeMatrixDefaultName)
, m_ImageDataArrayName("Image Data")
, m_MetaDataAttributeMatrixName(k_AxioVisionMetaData)
, m_ConvertToGrayScale(false)
, m_ImportAllMetaData(false)
, m_FileWasRead(false)
, m_ChangeOrigin(false)
, m_ChangeSpacing(false)
, d_ptr(new ImportAxioVisionV4MontagePrivate(this))
{
  m_ColorWeights[0] = 0.2125f;
  m_ColorWeights[1] = 0.7154f;
  m_ColorWeights[2] = 0.0721f;

  m_Origin = FloatVec3Type(0.0f, 0.0f, 0.0f);
  m_Spacing = FloatVec3Type(1.0f, 1.0f, 1.0f);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImportAxioVisionV4Montage::~ImportAxioVisionV4Montage() = default;

SIMPL_PIMPL_PROPERTY_DEF(ImportAxioVisionV4Montage, QString, InputFile_Cache)
SIMPL_PIMPL_PROPERTY_DEF(ImportAxioVisionV4Montage, QDateTime, LastRead)
SIMPL_PIMPL_PROPERTY_DEF(ImportAxioVisionV4Montage, QDomElement, Root)
SIMPL_PIMPL_PROPERTY_DEF(ImportAxioVisionV4Montage, ZeissTagsXmlSection::Pointer, RootTagsSection)

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportAxioVisionV4Montage::setupFilterParameters()
{
  FilterParameterVectorType parameters;
  parameters.push_back(SIMPL_NEW_INPUT_FILE_FP("AxioVision XML File (_meta.xml)", InputFile, FilterParameter::Parameter, ImportAxioVisionV4Montage, "*.xml"));

  PreflightUpdatedValueFilterParameter::Pointer param = SIMPL_NEW_PREFLIGHTUPDATEDVALUE_FP("Montage Information", MontageInformation, FilterParameter::Parameter, ImportAxioVisionV4Montage);
  param->setReadOnly(true);
  parameters.push_back(param);

  parameters.push_back(SIMPL_NEW_BOOL_FP("Import All MetaData", ImportAllMetaData, FilterParameter::Parameter, ImportAxioVisionV4Montage));

  QStringList linkedProps("Origin");
  linkedProps.push_back("UsePixelCoordinates");
  parameters.push_back(SIMPL_NEW_LINKED_BOOL_FP("Change Origin", ChangeOrigin, FilterParameter::Parameter, ImportAxioVisionV4Montage, linkedProps));
  parameters.push_back(SIMPL_NEW_FLOAT_VEC3_FP("Origin", Origin, FilterParameter::Parameter, ImportAxioVisionV4Montage));
  parameters.push_back(SIMPL_NEW_BOOL_FP("Pixel Coordinates", UsePixelCoordinates, FilterParameter::Parameter, ImportAxioVisionV4Montage));

  linkedProps.clear();
  linkedProps << "Spacing";
  parameters.push_back(SIMPL_NEW_LINKED_BOOL_FP("Change Spacing", ChangeSpacing, FilterParameter::Parameter, ImportAxioVisionV4Montage, linkedProps));
  parameters.push_back(SIMPL_NEW_FLOAT_VEC3_FP("Spacing", Spacing, FilterParameter::Parameter, ImportAxioVisionV4Montage));

  linkedProps.clear();
  linkedProps << "ColorWeights";
  parameters.push_back(SIMPL_NEW_LINKED_BOOL_FP("Convert To GrayScale", ConvertToGrayScale, FilterParameter::Parameter, ImportAxioVisionV4Montage, linkedProps));
  parameters.push_back(SIMPL_NEW_FLOAT_VEC3_FP("Color Weighting", ColorWeights, FilterParameter::Parameter, ImportAxioVisionV4Montage));

  parameters.push_back(SIMPL_NEW_DC_CREATION_FP("DataContainer Prefix", DataContainerName, FilterParameter::CreatedArray, ImportAxioVisionV4Montage));
  parameters.push_back(SIMPL_NEW_STRING_FP("Cell Attribute Matrix Name", CellAttributeMatrixName, FilterParameter::CreatedArray, ImportAxioVisionV4Montage));
  parameters.push_back(SIMPL_NEW_STRING_FP("Image DataArray Name", ImageDataArrayName, FilterParameter::CreatedArray, ImportAxioVisionV4Montage));
  parameters.push_back(SIMPL_NEW_STRING_FP("MetaData AttributeMatrix Name", MetaDataAttributeMatrixName, FilterParameter::CreatedArray, ImportAxioVisionV4Montage));

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportAxioVisionV4Montage::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportAxioVisionV4Montage::dataCheck()
{
  clearErrorCode();
  clearWarningCode();

  QString ss;
  QFileInfo fi(getInputFile());
  if(getInputFile().isEmpty())
  {
    ss = QObject::tr("%1 needs the Input File Set and it was not.").arg(ClassName());
    setErrorCondition(-387, ss);
  }
  else if(!fi.exists())
  {
    ss = QObject::tr("The input file '%1' does not exist.").arg(getInputFile());
    setErrorCondition(-388, ss);
  }

  if(fi.isDir())
  {
    ss = QObject::tr("The input path '%1' is a directory. Please select an XML file.").arg(getInputFile());
    setErrorCondition(-395, ss);
  }

  if(getDataContainerName().isEmpty())
  {
    ss = QObject::tr("The Data Container Name cannot be empty.");
    setErrorCondition(-392, ss);
  }
  if(getCellAttributeMatrixName().isEmpty())
  {
    ss = QObject::tr("The Attribute Matrix Name cannot be empty.");
    setErrorCondition(-393, ss);
  }
  if(getImageDataArrayName().isEmpty())
  {
    ss = QObject::tr("The Image Data Array Name cannot be empty.");
    setErrorCondition(-394, ss);
  }

  if(getErrorCode() < 0)
  {
    return;
  }

  QString filtName = ITKImageProcessingConstants::ImageProcessingFilters::k_ReadImageFilterClassName;
  FilterManager* fm = FilterManager::Instance();
  IFilterFactory::Pointer filterFactory = fm->getFactoryFromClassName(filtName);
  if(nullptr != filterFactory.get())
  {
    // If we get this far, the Factory is good so creating the filter should not fail unless something has
    // horribly gone wrong in which case the system is going to come down quickly after this.
    AbstractFilter::Pointer filter = filterFactory->create();
    if(nullptr == filter.get())
    {
      ss = QObject::tr("The '%1' filter is not Available, did the ITKImageProcessing Plugin Load.").arg(filtName);
      setErrorCondition(-391, ss);
    }
  }
  if(getErrorCode() < 0)
  {
    return;
  }

  DataContainerArray::Pointer dca = getDataContainerArray();
  if(nullptr == dca.get())
  {
    ss = QObject::tr("%1 needs a valid DataContainerArray").arg(ClassName());
    setErrorCondition(-390, ss);
    return;
  }

  // Parse the XML file to get all the meta-data information and create all the
  // data structure that is needed.
  QFile xmlFile(getInputFile());
  readMetaXml(&xmlFile);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportAxioVisionV4Montage::preflight()
{
  // These are the REQUIRED lines of CODE to make sure the filter behaves correctly
  setInPreflight(true);              // Set the fact that we are preflighting.
  emit preflightAboutToExecute();    // Emit this signal so that other widgets can do one file update
  emit updateFilterParameters(this); // Emit this signal to have the widgets push their values down to the filter
  dataCheck();                       // Run our DataCheck to make sure everthing is setup correctly
  emit preflightExecuted();          // We are done preflighting this filter
  setInPreflight(false);             // Inform the system this filter is NOT in preflight mode anymore.
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportAxioVisionV4Montage::execute()
{
  int err = 0;
  // typically run your dataCheck function to make sure you can get that far and all your variables are initialized
  dataCheck();
  // Check to make sure you made it through the data check. Errors would have been reported already so if something
  // happens to fail in the dataCheck() then we simply return
  if(getErrorCode() < 0)
  {
    return;
  }
  clearErrorCode();
  clearWarningCode();

  /* If some error occurs this code snippet can report the error up the call chain*/
  if(err < 0)
  {
    QString ss = QObject::tr("Error Importing a Zeiss AxioVision file set.");
    setErrorCondition(-90000, ss);
    return;
  }
}

// -----------------------------------------------------------------------------
QString ImportAxioVisionV4Montage::getMontageInformation()
{
  return d_ptr->m_MontageInformation;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportAxioVisionV4Montage::readMetaXml(QIODevice* device)
{
  QString errorStr;
  int errorLine;
  int errorColumn;

  QFileInfo fi(getInputFile());
  QDateTime lastModified(fi.lastModified());

  QDomDocument domDocument;
  QDomElement root;
  ZeissTagsXmlSection::Pointer rootTagsSection;

  if(getInputFile() == getInputFile_Cache() && getLastRead().isValid() && lastModified.msecsTo(getLastRead()) >= 0)
  {
    // We are reading from the cache, so set the FileWasRead flag to false
    m_FileWasRead = false;
    root = getRoot();
    rootTagsSection = getRootTagsSection();
  }
  else
  {
    // We are reading from the file, so set the FileWasRead flag to true
    m_FileWasRead = true;

    if(!domDocument.setContent(device, true, &errorStr, &errorLine, &errorColumn))
    {
      QString ss = QObject::tr("Parse error at line %1, column %2:\n%3").arg(errorLine).arg(errorColumn).arg(errorStr);
      setErrorCondition(-70000, ss);
      return;
    }

    root = domDocument.documentElement();

    QDomElement tags = root.firstChildElement(ITKImageProcessingConstants::Xml::Tags);
    if(tags.isNull())
    {
      QString ss = QObject::tr("Could not find the <ROOT><Tags> element. Aborting Parsing. Is the file a Zeiss _meta.xml file");
      setErrorCondition(-70001, ss);
      return;
    }

    // First parse the <ROOT><Tags> section to get the values of how many images we are going to have
    rootTagsSection = MetaXmlUtils::ParseTagsSection(this, tags);
    if(nullptr == rootTagsSection.get())
    {
      return;
    }

    // Set the data into the cache
    setRootTagsSection(rootTagsSection);

    QDomElement rootCopy = root.cloneNode().toElement();
    setRoot(rootCopy);

    // Set the file path and time stamp into the cache
    setLastRead(QDateTime::currentDateTime());
    setInputFile_Cache(getInputFile());
  }

  // Now parse each of the <pXXX> tags
  parseImages(root, rootTagsSection);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportAxioVisionV4Montage::parseImages(QDomElement& root, const ZeissTagsXmlSection::Pointer& rootTagsSection)
{

  int32_t imageCount = MetaXmlUtils::GetInt32Entry(this, rootTagsSection.get(), Zeiss::MetaXML::ImageCountRawId);
  if(getErrorCode() < 0)
  {
    return;
  }

  m_RowCount = MetaXmlUtils::GetInt32Entry(this, rootTagsSection.get(), Zeiss::MetaXML::ImageCountVId);
  if(getErrorCode() < 0)
  {
    return;
  }

  m_ColumnCount = MetaXmlUtils::GetInt32Entry(this, rootTagsSection.get(), Zeiss::MetaXML::ImageCountUId);
  if(getErrorCode() < 0)
  {
    return;
  }

  AbstractZeissMetaData::Pointer fileNamePtr = rootTagsSection->getEntry(Zeiss::MetaXML::FilenameId);
  StringZeissMetaEntry::Pointer imageNamePtr = std::dynamic_pointer_cast<StringZeissMetaEntry>(fileNamePtr);
  QString imageName = imageNamePtr->getValue();

  // Figure out the max padding digits for both the imageCount (we need that to generate the proper xml tag) and
  // the row/col values because we need to have a consistent numbering format for later filters.
  int imageCountPadding = MetaXmlUtils::CalculatePaddingDigits(imageCount);
  int32_t rowCountPadding = MetaXmlUtils::CalculatePaddingDigits(m_RowCount);
  int32_t colCountPadding = MetaXmlUtils::CalculatePaddingDigits(m_ColumnCount);
  int charPaddingCount = std::max(rowCountPadding, colCountPadding);
  FloatVec3Type minCoord = {std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max()};
  FloatVec3Type minSpacing = {std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max()};

  DataContainerArray::Pointer dca = getDataContainerArray();

  std::vector<ImageGeom::Pointer> geometries;

  // Loop over every image in the _meta[0]ml file
  for(int p = 0; p < imageCount; p++)
  {
    // Generate the xml tag that is for this image
    QString pTag;
    QTextStream out(&pTag);
    out << "p";
    out.setFieldWidth(imageCountPadding);
    out.setFieldAlignment(QTextStream::AlignRight);
    out.setPadChar('0');
    out << p;

    // Send a status update on the progress
    QString msg = QString("%1: Importing file %2 of %3").arg(getHumanLabel()).arg(p).arg(imageCount);
    notifyStatusMessage(msg);

    // Drill down into the XML document....
    QDomElement photoEle = root.firstChildElement(pTag);
    if(photoEle.isNull())
    {
      QString ss = QObject::tr("Could not find the <ROOT><%1> element. Aborting Parsing. Is the file a Zeiss _meta.xml file").arg(pTag);
      setErrorCondition(-70002, ss);
      return;
    }
    // Get the TAGS section
    QDomElement tags = photoEle.firstChildElement(ITKImageProcessingConstants::Xml::Tags);
    if(tags.isNull())
    {
      QString ss = QObject::tr("Could not find the <ROOT><%1><Tags> element. Aborting Parsing. Is the file a Zeiss _meta.xml file").arg(pTag);
      setErrorCondition(-70003, ss);
      return;
    }
    // Now Parse the TAGS section
    ZeissTagsXmlSection::Pointer photoTagsSection = MetaXmlUtils::ParseTagsSection(this, tags);
    if(nullptr == photoTagsSection.get())
    {
      QString ss = QObject::tr("Error Parsing the <ROOT><%1><Tags> element. Aborting Parsing. Is the file a Zeiss AxioVision _meta.xml file").arg(pTag);
      setErrorCondition(-70004, ss);
      return;
    }

    // Get the Row Index (Zero Based)
    int32_t rowIndex = MetaXmlUtils::GetInt32Entry(this, photoTagsSection.get(), Zeiss::MetaXML::ImageIndexVId);
    if(getErrorCode() < 0)
    {
      return;
    }
    // Get the Columnn Index (Zero Based)
    int32_t colIndex = MetaXmlUtils::GetInt32Entry(this, photoTagsSection.get(), Zeiss::MetaXML::ImageIndexUId);
    if(getErrorCode() < 0)
    {
      return;
    }

    // Create our DataContainer Name using a Prefix and a rXXcYY format.
    QString dcName = getDataContainerName().getDataContainerName();
    QTextStream dcNameStream(&dcName);
    dcNameStream << "_r";
    dcNameStream.setFieldWidth(charPaddingCount);
    dcNameStream.setFieldAlignment(QTextStream::AlignRight);
    dcNameStream.setPadChar('0');
    dcNameStream << rowIndex;
    dcNameStream.setFieldWidth(0);
    dcNameStream << "c";
    dcNameStream.setFieldWidth(charPaddingCount);
    dcNameStream << colIndex;

    // Create the DataContainer with a name based on the ROW & COLUMN indices
    DataContainer::Pointer dc = dca->createNonPrereqDataContainer<AbstractFilter>(this, dcName);

    // Create the Image Geometry
    ImageGeom::Pointer image = initializeImageGeom(root, photoTagsSection);
    dc->setGeometry(image);

    minSpacing = image->getSpacing();

    FloatVec3Type origin = image->getOrigin();
    minCoord[0] = std::min(origin[0], minCoord[0]);
    minCoord[1] = std::min(origin[1], minCoord[1]);
    minCoord[2] = 0.0f;

    geometries.emplace_back(image);

    if(getImportAllMetaData())
    {
      QVector<size_t> dims = {1};
      AttributeMatrix::Pointer metaAm = dc->createAndAddAttributeMatrix(dims, getMetaDataAttributeMatrixName(), AttributeMatrix::Type::Generic);
      ZeissTagsXmlSection::MetaDataType tagMap = photoTagsSection->getMetaDataMap();
      for(const auto& value : tagMap)
      {
        IDataArray::Pointer dataArray = value->createDataArray(!getInPreflight());
        metaAm->insertOrAssign(dataArray);
      }
    }
    // Read the image into a data array
    importImage(dc.get(), imageName, pTag, p);

    if(getConvertToGrayScale())
    {
      convertToGrayScale(dc.get(), imageName, pTag);
    }
  }

  QString montageInfo;
  QTextStream ss(&montageInfo);
  ss << "Columns=" << m_ColumnCount << "  Rows=" << m_RowCount << "  Num. Images=" << imageCount;

  FloatVec3Type overrideOrigin = minCoord;
  FloatVec3Type overrideSpacing = minSpacing;

  // Now adjust the origin/spacing if needed
  if(getChangeOrigin() || getChangeSpacing())
  {

    if(getChangeOrigin())
    {
      overrideOrigin = {m_Origin[0], m_Origin[1], m_Origin[2]};
    }
    if(getChangeSpacing())
    {
      overrideSpacing = {m_Spacing[0], m_Spacing[1], m_Spacing[2]};
    }

    for(const auto& image : geometries)
    {
      FloatVec3Type currentOrigin = image->getOrigin();
      FloatVec3Type currentSpacing = image->getSpacing();

      for(size_t i = 0; i < 3; i++)
      {
        float delta = currentOrigin[i] - minCoord[i];
        if (m_UsePixelCoordinates)
        {
          // Convert to Pixel Coords
          delta = delta / currentSpacing[i];
        }
        //      // Convert to the override origin
        //      delta = delta * overrideSpacing[i];
        currentOrigin[i] = overrideOrigin[i] + delta;
        if (m_UsePixelCoordinates)
        {
          // Convert back to physical coords
          currentOrigin[i] = currentOrigin[i] * currentSpacing[i];
        }
      }
      image->setOrigin(currentOrigin.data());
      image->setSpacing(overrideSpacing.data());
    }
  }
  ss << "\nOrigin: " << overrideOrigin[0] << ", " << overrideOrigin[1] << ", " << overrideOrigin[2];
  ss << "  Spacing: " << overrideSpacing[0] << ", " << overrideSpacing[1] << ", " << overrideSpacing[2];
  d_ptr->m_MontageInformation = montageInfo;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportAxioVisionV4Montage::addMetaData(const AttributeMatrix::Pointer& metaAm, const ZeissTagsXmlSection::Pointer& photoTagsSection, int index)
{
  ZeissTagMapping::Pointer tagMap = ZeissTagMapping::instance();

  ZeissTagsXmlSection::MetaDataType tags = photoTagsSection->getMetaDataMap();

  QMapIterator<int, AbstractZeissMetaData::Pointer> iter(tags);
  while(iter.hasNext())
  {
    iter.next();
    StringZeissMetaEntry::Pointer zStrVal = std::dynamic_pointer_cast<StringZeissMetaEntry>(iter.value());

    QString tagName = tagMap->nameForId(iter.key());

    IDataArray::Pointer iDataArray = metaAm->getAttributeArray(tagName);
    StringDataArray::Pointer strArray = std::dynamic_pointer_cast<StringDataArray>(iDataArray);
    strArray->setValue(index, zStrVal->getValue());
    // IDataArray::Pointer dataArray = iter.value()->createDataArray(!getInPreflight());
    // metaData->insertOrAssign(dataArray);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportAxioVisionV4Montage::addRootMetaData(const AttributeMatrix::Pointer& metaAm, const ZeissTagsXmlSection::Pointer& rootTagsSection, int index)
{
  ZeissTagMapping::Pointer tagMap = ZeissTagMapping::instance();
  ZeissTagsXmlSection::MetaDataType tags = rootTagsSection->getMetaDataMap();

  QMapIterator<int, AbstractZeissMetaData::Pointer> iter(tags);
  while(iter.hasNext())
  {
    iter.next();
    StringZeissMetaEntry::Pointer zStrVal = std::dynamic_pointer_cast<StringZeissMetaEntry>(iter.value());

    QString tagName = tagMap->nameForId(iter.key());
    if(tagName == "ScaleFactorForX" || tagName == "ScaleFactorForY")
    {
      IDataArray::Pointer iDataArray = metaAm->getAttributeArray(tagName);
      StringDataArray::Pointer strArray = std::dynamic_pointer_cast<StringDataArray>(iDataArray);
      strArray->setValue(index, zStrVal->getValue());
      // IDataArray::Pointer dataArray = iter.value()->createDataArray(!getInPreflight());
      // metaData->insertOrAssign(dataArray);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportAxioVisionV4Montage::importImage(DataContainer* dc, const QString& imageName, const QString& pTag, int imageIndex)
{

  QFileInfo fi(imageName);
  QString imagePath = fi.completeBaseName() + "_" + pTag + "." + fi.suffix();
  // QString dataArrayName = fi.completeBaseName() + "_" + pTag;
  QString imageDataArrayNamme = getImageDataArrayName();

  // Add this Array name to the DataArray that will ensure the correct ordering
  // attributeArrayNames->setValue(imageIndex, dataArrayName);

  fi = QFileInfo(getInputFile());
  imagePath = fi.absoluteDir().path() + "/" + imagePath;
  //   std::string sPath = imagePath.toStdString();

  QString filtName = ITKImageProcessingConstants::ImageProcessingFilters::k_ReadImageFilterClassName;
  FilterManager* fm = FilterManager::Instance();
  IFilterFactory::Pointer filterFactory = fm->getFactoryFromClassName(filtName);
  if(nullptr != filterFactory.get())
  {
    // If we get this far, the Factory is good so creating the filter should not fail unless something has
    // horribly gone wrong in which case the system is going to come down quickly after this.
    AbstractFilter::Pointer filter = filterFactory->create();

    // Connect up the Error/Warning/Progress object so the filter can report those things
    connect(filter.get(), SIGNAL(messageGenerated(const AbstractMessage::Pointer&)), this, SIGNAL(messageGenerated(const AbstractMessage::Pointer&)));
    DataContainerArray::Pointer dca = DataContainerArray::New();

    filter->setDataContainerArray(dca); // AbstractFilter implements this so no problem
	
	// Add to filename list
	m_FilenameList.push_back(imagePath);

    bool propWasSet = filter->setProperty("FileName", imagePath);
    if(!propWasSet)
    {
      QString ss = QObject::tr("Error Setting Property '%1' into filter '%2' which is a subfilter called by %3. The property was not set which could mean the property was not exposed with a "
                               "Q_PROPERTY macro. Please notify the developers.")
                       .arg("InputFileName", filtName, getHumanLabel());
      setErrorCondition(-70015, ss);
    }
    QVariant var;
    var.setValue(DataArrayPath(dc->getName(), "", ""));
    propWasSet = filter->setProperty("DataContainerName", var);
    if(!propWasSet)
    {
      QString ss = QObject::tr("Error Setting Property '%1' into filter '%2' which is a subfilter called by %3. The property was not set which could mean the property was not exposed with a "
                               "Q_PROPERTY macro. Please notify the developers.")
                       .arg("DataContainerName", filtName, getHumanLabel());
      setErrorCondition(-70016, ss);
    }

    propWasSet = filter->setProperty("CellAttributeMatrixName", getCellAttributeMatrixName());
    if(!propWasSet)
    {
      QString ss = QObject::tr("Error Setting Property '%1' into filter '%2' which is a subfilter called by %3. The property was not set which could mean the property was not exposed with a "
                               "Q_PROPERTY macro. Please notify the developers.")
                       .arg("CellAttributeMatrixName", filtName, getHumanLabel());
      setErrorCondition(-70017, ss);
    }

    propWasSet = filter->setProperty("ImageDataArrayName", imageDataArrayNamme);
    if(!propWasSet)
    {
      QString ss = QObject::tr("Error Setting Property '%1' into filter '%2' which is a subfilter called by %3. The property was not set which could mean the property was not exposed with a "
                               "Q_PROPERTY macro. Please notify the developers.")
                       .arg("ImageDataArrayName", filtName, getHumanLabel());
      setErrorCondition(-70018, ss);
    }

    if(getInPreflight())
    {
      filter->preflight();
    }
    else
    {
      filter->execute();
    }

    if(getErrorCode() >= 0 && filter->getErrorCode() >= 0)
    {
      QString targetName = dc->getName();
      DataContainer::Pointer fromDca = dca->getDataContainer(targetName);
      AttributeMatrix::Pointer cellAttrMat = fromDca->getAttributeMatrix(getCellAttributeMatrixName());
      dc->addOrReplaceAttributeMatrix(cellAttrMat);
    }
  }
  else
  {
    QString ss = QObject::tr("Error trying to instantiate the '%1' filter which is typically included in the 'ImageProcessing' plugin.")
                     .arg(ITKImageProcessingConstants::ImageProcessingFilters::k_ReadImageFilterClassName);
    setErrorCondition(-70019, ss);
    return;
  }
}

// -----------------------------------------------------------------------------
//
// -------------------------------------------------------------------------
void ImportAxioVisionV4Montage::convertToGrayScale(DataContainer* dc, const QString& imageName, const QString& pTag)
{

  DataArrayPath dap(dc->getName(), getCellAttributeMatrixName(), getImageDataArrayName());

  ConvertColorToGrayScale::Pointer filter = ConvertColorToGrayScale::New();

  // Connect up the Error/Warning/Progress object so the filter can report those things
  connect(filter.get(), SIGNAL(messageGenerated(const AbstractMessage::Pointer&)), this, SIGNAL(messageGenerated(const AbstractMessage::Pointer&)));

  filter->setDataContainerArray(getDataContainerArray()); // AbstractFilter implements this so no problem
  filter->setConversionAlgorithm(0);
  filter->setColorWeights(getColorWeights());
  QVector<DataArrayPath> inputDataArrayVector = {dap};
  filter->setInputDataArrayVector(inputDataArrayVector);
  filter->setCreateNewAttributeMatrix(false);
  filter->setOutputAttributeMatrixName(getCellAttributeMatrixName());
  filter->setOutputArrayPrefix(k_GrayScaleTempArrayName);

  if(getInPreflight())
  {
    filter->preflight();
  }
  else
  {
    filter->execute();
  }

  if(filter->getErrorCode() >= 0)
  {
    AttributeMatrix::Pointer am = dc->getAttributeMatrix(getCellAttributeMatrixName());
    IDataArray::Pointer rgb = am->removeAttributeArray(getImageDataArrayName());
    IDataArray::Pointer gray = am->removeAttributeArray(k_GrayScaleTempArrayName + getImageDataArrayName());
    gray->setName(rgb->getName());
    am->insertOrAssign(gray);
  }
  else
  {
    setErrorCondition(filter->getErrorCode(), "Grayscale conversion failed. The data must be RGB or RGBA to convert.");
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImageGeom::Pointer ImportAxioVisionV4Montage::initializeImageGeom(const QDomElement& root, const ZeissTagsXmlSection::Pointer& photoTagsSection)
{

  ImageGeom::Pointer image = ImageGeom::CreateGeometry(SIMPL::Geometry::ImageGeometry);

  //#######################################################################
  // Parse out the Pixel Dimensions of the image.
  SizeVec3Type dims;
  AbstractZeissMetaData::Pointer ptr = photoTagsSection->getEntry(Zeiss::MetaXML::ImageWidthPixelId);
  Int32ZeissMetaEntry::Pointer int32Entry = ZeissMetaEntry::convert<Int32ZeissMetaEntry>(ptr);
  Q_ASSERT_X(int32Entry.get() != nullptr, "Could not Cast to Int32ZeissMetaEntry", "");

  dims[0] = int32Entry->getValue();

  ptr = photoTagsSection->getEntry(Zeiss::MetaXML::ImageHeightPixelId);
  int32Entry = ZeissMetaEntry::convert<Int32ZeissMetaEntry>(ptr);
  Q_ASSERT_X(int32Entry.get() != nullptr, "Could not Cast to Int32ZeissMetaEntry", "");

  dims[1] = int32Entry->getValue();
  dims[2] = 1;
  image->setDimensions(dims);

  //#######################################################################
  // Initialize the Origin to the Stage Positions
  float stageXPos = MetaXmlUtils::GetFloatEntry(this, photoTagsSection.get(), Zeiss::MetaXML::StagePositionXId);
  float stageYPos = MetaXmlUtils::GetFloatEntry(this, photoTagsSection.get(), Zeiss::MetaXML::StagePositionYId);
  FloatVec3Type origin = {stageXPos, stageYPos, 0.0f};
  image->setOrigin(origin);

  //#######################################################################
  // Initialize the Spacing of the geometry
  bool ok = false;
  FloatVec3Type scaling = {1.0f, 1.0f, 1.0f};
  QDomElement scalingDom = root.firstChildElement(ITKImageProcessingConstants::Xml::Scaling).firstChildElement("Factor_0");
  scaling[0] = scalingDom.text().toFloat(&ok);
  scalingDom = root.firstChildElement(ITKImageProcessingConstants::Xml::Scaling).firstChildElement("Factor_1");
  scaling[1] = scalingDom.text().toFloat(&ok);
  image->setSpacing(scaling);

  //#######################################################################
  // Initialize the Length Units of the geometry
  QDomElement unitsDom = root.firstChildElement(ITKImageProcessingConstants::Xml::Scaling).firstChildElement("Type_0");
  int xUnits = unitsDom.text().toInt(&ok);
  // We are going to assume that the units in both the X and Y are the same. Why would they be different?
  IGeometry::LengthUnit lengthUnit = ZeissUnitMapping::Instance()->convertToIGeometryLengthUnit(xUnits);
  image->setUnits(lengthUnit);
  image->setName("AxioVision V4 Geometry");

  return image;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer ImportAxioVisionV4Montage::newFilterInstance(bool copyFilterParameters) const
{
  ImportAxioVisionV4Montage::Pointer filter = ImportAxioVisionV4Montage::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ImportAxioVisionV4Montage::getCompiledLibraryName() const
{
  return ITKImageProcessingConstants::ITKImageProcessingBaseName;;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ImportAxioVisionV4Montage::getBrandingString() const
{
  return ITKImageProcessingConstants::ITKImageProcessingPluginDisplayName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ImportAxioVisionV4Montage::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << ITKImageProcessing::Version::Major() << "." << ITKImageProcessing::Version::Minor() << "." << ITKImageProcessing::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ImportAxioVisionV4Montage::getGroupName() const
{
  return SIMPL::FilterGroups::IOFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ImportAxioVisionV4Montage::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::ImportFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ImportAxioVisionV4Montage::getHumanLabel() const
{
  return "Zeiss AxioVision Import (V4)";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QUuid ImportAxioVisionV4Montage::getUuid()
{
  return QUuid("{411b008c-006f-51b2-ba05-99e51a01af3c}");
}

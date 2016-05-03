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
*    United States Air Force Prime Contract FA8650-10-D-5210
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include <QtCore/QCoreApplication>
#include <QtCore/QFile>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/Common/FilterPipeline.h"
#include "SIMPLib/Common/FilterManager.h"
#include "SIMPLib/Common/FilterFactory.hpp"
#include "SIMPLib/Plugin/ISIMPLibPlugin.h"
#include "SIMPLib/Plugin/SIMPLibPluginLoader.h"
#include "SIMPLib/Utilities/UnitTestSupport.hpp"
#include "SIMPLib/Utilities/QMetaObjectUtilities.h"

#include <itkImage.h>

#include "ITKImageProcessing/ITKImageProcessingFilters/itkImageToDream3DDataFilter.h"
#include "ITKImageProcessing/ITKImageProcessingFilters/itkDream3DDataToImageFilter.h"

#include "ITKImageProcessingTestFileLocations.h"

class ITKImageProcessingFilterTest
{

  public:
    ITKImageProcessingFilterTest() {}
    virtual ~ITKImageProcessingFilterTest() {}


  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void RemoveTestFiles()
  {
  #if REMOVE_TEST_FILES
    QFile::remove(UnitTest::ITKImageProcessingFilterTest::TestFile1);
    QFile::remove(UnitTest::ITKImageProcessingFilterTest::TestFile2);
  #endif
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int TestFilterAvailability()
  {
    // Now instantiate the ITKImageProcessingFilter Filter from the FilterManager
    QString filtName = "ITKImageProcessingFilter";
    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer filterFactory = fm->getFactoryForFilter(filtName);
    if (NULL == filterFactory.get())
    {
      std::stringstream ss;
      ss << "The ITKImageProcessingFilterTest Requires the use of the " << filtName.toStdString() << " filter which is found in the ITKImageProcessing Plugin";
      DREAM3D_TEST_THROW_EXCEPTION(ss.str())
    }
    return 0;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int Test1()
  {
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   /* Please write ITKImageProcessingFilter test code here.
    *
    * Your IO test files are:
    * UnitTest::ITKImageProcessingFilterTest::TestFile1
    * UnitTest::ITKImageProcessingFilterTest::TestFile2
    *
    * SIMPLib provides some macros that will throw exceptions when a test fails
    * and thus report that during testing. These macros are located in the
    * SIMPLib/Utilities/UnitTestSupport.hpp file. Some examples are:
    *
    * SIMPLib_REQUIRE_EQUAL(foo, 0)
    * This means that if the variable foo is NOT equal to Zero then test will fail
    * and the current test will exit immediately. If there are more tests registered
    * with the SIMPLib_REGISTER_TEST() macro, the next test will execute. There are
    * lots of examples in the SIMPLib/Test folder to look at.
    */
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	  /* Test ITK to Dream3D data conversion*/
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	  //Create input image
	  const unsigned int dimension = 3;
	  int tol = 1;
	  //////////////////////////////////////////////////
	  typedef float PixelType;
	  typedef itk::Image<PixelType, dimension> ImageType;
	  ImageType::Pointer image = ImageType::New();
	  ImageType::PointType origin;
	  ImageType::SizeType size;
	  ImageType::DirectionType direction;
	  ImageType::SpacingType spacing;
	  for (unsigned int i = 0; i < dimension; i++)
	  {
		  origin[i] = 1.23 + float(i);
		  size[i] = 90 + i * 3;
		  spacing[i] = .45 + float(i)*.2;
	  }
	  direction.SetIdentity();
	  image->SetOrigin(origin);
	  image->SetDirection(direction);
	  image->SetSpacing(spacing);
	  image->SetRegions(size);
	  image->Allocate();
	  image->FillBuffer(12);
	  ImageType::IndexType index;
	  index.Fill(0);
	  float val=image->GetPixel(index);
	  // Create converter
	  typedef itk::ImageToDream3DDataFilter<PixelType, dimension> itkImageToDream3DDataFilterType;
	  itkImageToDream3DDataFilterType::Pointer filter = itkImageToDream3DDataFilterType::New();
	  filter->SetInput(image);
	  DataArrayPath dataArrayPath("TestContainer","TestAttributeMatrixName","TestAttributeArrayName");
	  filter->SetDataArrayPath(dataArrayPath);
	  filter->Update();
	  DataContainer::Pointer dc = filter->GetOutput();
	  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	  // Compare output to baseline result
	  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	  // Checks that data container pointer is not null
	  DREAM3D_REQUIRE_NE(dc.get(),0);
	  IGeometry::Pointer geom = dc->getGeometry();
	  //Verifies that output data container has the baseline name
	  DREAM3D_REQUIRE_EQUAL(dc->getName(), dataArrayPath.getDataContainerName());
	  //Verifies that output data container is an 'ImageGeom'
	  QString geomTypeName = geom->getGeometryTypeAsString();
	  DREAM3D_REQUIRE_EQUAL(geomTypeName, "ImageGeometry");
	  //Verifies that output data container contains the expected attribute matrix
	  bool containsMatrix = dc->doesAttributeMatrixExist(dataArrayPath.getAttributeMatrixName());
	  DREAM3D_REQUIRE_EQUAL(containsMatrix, true);
	  //Verifies that output data container contains the baseline data array
	  AttributeMatrix::Pointer am = dc->getAttributeMatrix(dataArrayPath.getAttributeMatrixName());
	  bool containsArray = am->doesAttributeArrayExist(dataArrayPath.getDataArrayName());
	  DREAM3D_REQUIRE_EQUAL(containsArray, true);
	  //Compares baseline image properties (size, spacing, origin) with DREAM3D image properties
	  ImageGeom::Pointer imageGeom = std::dynamic_pointer_cast<ImageGeom>(geom);
	  DREAM3D_REQUIRE_NE(imageGeom.get(), 0);
	  float r[3];
	  imageGeom->getResolution(r[0],r[1],r[2]);
	  float o[3];
	  imageGeom->getOrigin(o[0],o[1],o[2]);
	  size_t size_d[3];
	  imageGeom->getDimensions(size_d[0],size_d[1],size_d[2]);
	  for (int i = 0; i < dimension; i++)
	  {
		  float fspacing = (float)spacing[i];
		  DREAM3D_COMPARE_FLOATS(&r[i], &fspacing,tol);
		  float forigin = (float)origin[i];
		  DREAM3D_COMPARE_FLOATS(&o[i], &forigin,tol);
		  DREAM3D_REQUIRE_EQUAL(size_d[i], size[i]);
	  }
	  //Checks that if image is 2D, spacing and size along last dimension is '1'
	  float one = 1.0f;
	  if (dimension == 2)
	  {
		  DREAM3D_COMPARE_FLOATS(&r[2], &one,tol);
		  DREAM3D_REQUIRE_EQUAL(size_d[2], 1);
	  }
	  //Checks that array has same values as baseline
	  IDataArray::Pointer dataArray = am->getAttributeArray(dataArrayPath.getDataArrayName());
	  for (size_t i=0; i < dataArray->getSize(); i++)
	  {
		  float dval = static_cast<float*>(dataArray->getVoidPointer(0))[i];
		  float ival = image->GetBufferPointer()[i];
		  DREAM3D_COMPARE_FLOATS(&dval,&ival ,tol);
	  }
	  //Checks that input data pointer is the same as output baseline pointer
	  DREAM3D_REQUIRE_EQUAL(static_cast<float*>(dataArray->getVoidPointer(0)), image->GetBufferPointer());
    return EXIT_SUCCESS;
  }

  int Test2()
  {
	  const unsigned int Dimension = 3;
	  typedef float PixelType;
	  int tol = 1;
	  // Create test data (baseline)
	  QString attributeMatrixName = "TestAttributeMatrixName";
	  QString dataArrayName = "TestAttributeArrayName";
	  DataContainer::Pointer dc = DataContainer::New("TestContainer");
	  ImageGeom::Pointer imageGeom = ImageGeom::CreateGeometry(SIMPL::Geometry::ImageGeometry);
	  QVector<float> torigin(3, 0);
	  QVector<float> tspacing(3, 1);
	  QVector<size_t> tDims(3, 1);
	  for (size_t i = 0; i < Dimension; i++)
	  {
		  torigin[i] = 1.23 + float(i);
		  tspacing[i] = .45 + float(i)*.2;
		  tDims[i] = 90 + i * 3;
	  }
	  imageGeom->setResolution(tspacing[0], tspacing[1], tspacing[2]);
	  imageGeom->setOrigin(torigin[0], torigin[1], torigin[2]);
	  imageGeom->setDimensions(tDims[0], tDims[1], tDims[2]);
	  dc->setGeometry(imageGeom);
	  QVector<size_t> cDims(1, 0);
	  cDims[0] = 1;
	  AttributeMatrix::Pointer ma = dc->createAndAddAttributeMatrix(tDims, attributeMatrixName, SIMPL::AttributeMatrixType::Cell);
	  DataArray<PixelType>::Pointer data = DataArray<PixelType>::CreateArray(tDims, cDims, dataArrayName, true);
	  data->initializeWithValue(11.0);
	  ma->addAttributeArray(dataArrayName, data);
	  // Create filter
	  typedef itk::Image<PixelType, Dimension> ImageType;
	  typedef itk::Dream3DDataToImageFilter<PixelType, Dimension> FilterType;
	  FilterType::Pointer filter = FilterType::New();
	  filter->SetInput(dc);
	  filter->SetMatrixArrayName(attributeMatrixName.toStdString());
	  filter->SetDataArrayName(dataArrayName.toStdString());
	  filter->Update();
	  ImageType::Pointer image = filter->GetOutput();
	  // Check image properties (size, spacing, origin): must match baseline properties
	  ImageType::SizeType size;
	  size = image->GetLargestPossibleRegion().GetSize();
	  ImageType::PointType origin;
	  origin = image->GetOrigin();
	  ImageType::SpacingType spacing;
	  spacing = image->GetSpacing();
	  for (size_t i = 0; i < Dimension; i++)
	  {
		  float fspacing = (float)tspacing[i];
		  float ispacing = (float)spacing[i];
		  DREAM3D_COMPARE_FLOATS(&ispacing, &fspacing, tol);
		  float forigin = (float)torigin[i];
		  float iorigin = (float)origin[i];
		  DREAM3D_COMPARE_FLOATS(&iorigin, &forigin, tol);
		  DREAM3D_REQUIRE_EQUAL(size[i], tDims[i]);
	  }
	  // Check data pointer in image equals data pointer in Dream3D data container
	  DREAM3D_REQUIRE_EQUAL(data->getPointer(0), image->GetBufferPointer());
	  // Check pixel values in image match pixel values in Dream3D data container
	  for (size_t i = 0; i < data->getSize(); i++)
	  {
		  float dval = data->getPointer(0)[i];
		  float ival = image->GetBufferPointer()[i];
		  DREAM3D_COMPARE_FLOATS(&dval, &ival, tol);
	  }

	  return EXIT_SUCCESS;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void operator()()
  {
    int err = EXIT_SUCCESS;

    DREAM3D_REGISTER_TEST( TestFilterAvailability() );

    DREAM3D_REGISTER_TEST( Test1() )
	DREAM3D_REGISTER_TEST( Test2() )
    
	DREAM3D_REGISTER_TEST( RemoveTestFiles() )
  }

  private:
    ITKImageProcessingFilterTest(const ITKImageProcessingFilterTest&); // Copy Constructor Not Implemented
    void operator=(const ITKImageProcessingFilterTest&); // Operator '=' Not Implemented


};


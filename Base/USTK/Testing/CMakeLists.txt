##############################################################################
#
# Library:   TubeTK
#
# Copyright 2010 Kitware Inc. 28 Corporate Drive,
# Clifton Park, NY, 12065, USA.
#
# All rights reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#       http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
##############################################################################

include(${TubeTK_SOURCE_DIR}/CMake/MIDAS3.cmake)
set( MIDAS_REST_URL http://midas3.kitware.com/midas/api/rest )
set( MIDAS_KEY_DIR ${TubeTK_SOURCE_DIR}/MIDAS_Keys)

if( Slicer3_INSTALL_BIN_DIR )
  set( BASE_USTK_TESTS
    ${TubeTK_BINARY_DIR}/${Slicer3_INSTALL_BIN_DIR}/tubeBaseUSTKTests )
else()
  set( BASE_USTK_TESTS
    ${TubeTK_BINARY_DIR}/bin/tubeBaseUSTKTests )
endif()

set( TEMP ${TubeTK_BINARY_DIR}/Temporary )

set( tubeBaseUSTK_SRCS
  itkLabelMapToAcousticImpedanceImageFilterTest.cxx
  itkAcousticImpulseResponseImageFilterTest.cxx
  itkAngleOfIncidenceImageFilterTest.cxx
  itkGradientBasedAngleOfIncidenceImageFilterTest.cxx
  )

include_directories(
  ${TubeTK_SOURCE_DIR}/Utilities
  ${TubeTK_SOURCE_DIR}/Base/USTK
  ${TubeTK_SOURCE_DIR}/Base/Filtering
  )

add_executable( tubeBaseUSTKTests
  tubeBaseUSTKTests.cxx
  ${tubeBaseUSTK_SRCS}
  )
target_link_libraries( tubeBaseUSTKTests
  #TubeUSTK
  ${ITK_LIBRARIES}
  )

MIDAS_ADD_TEST( itkLabelMapToAcousticImpedanceImageFilterTest
  ${BASE_USTK_TESTS}
  --compare
    MIDAS{itkLabelMapToAcousticImpedanceImageFilterTest.mha.md5}
    ${TEMP}/itkLabelMapToAcousticImpedanceImageFilterTest.mha
  itkLabelMapToAcousticImpedanceImageFilterTest
    ${CMAKE_CURRENT_SOURCE_DIR}/LabelMapToAcousticImpedanceTestLookupTable.csv
    MIDAS{CIRSPhantomAbdominalLabels.nrrd.md5}
    ${TEMP}/itkLabelMapToAcousticImpedanceImageFilterTest.mha
  )

MIDAS_ADD_TEST( itkAngleOfIncidenceImageFilterTest
  ${BASE_USTK_TESTS}
    itkAngleOfIncidenceImageFilterTest
    MIDAS{PrimitiveObjects.mha.md5}
    ${TEMP}/PrimitveObjectsSheetnessImage.mha
    ${TEMP}/PrimitveObjectsAngleOfIncidenceImage.mha
    10
    10
    10
  )

MIDAS_ADD_TEST( itkGradientBasedAngleOfIncidenceImageFilterTest
  ${BASE_USTK_TESTS}
  --compareNumberOfPixelsTolerance 50
  --compare
      MIDAS{itkGradientBasedAngleOfIncidenceImageFilterTestGradientImageFilter.mha.md5}
      ${TEMP}/itkGradientBasedAngleOfIncidenceImageFilterTestGradientImageFilter.mha
  --compare
      MIDAS{itkGradientBasedAngleOfIncidenceImageFilterTestGradientRecursiveGaussianImageFilter.mha.md5}
      ${TEMP}/itkGradientBasedAngleOfIncidenceImageFilterTestGradientRecursiveGaussianImageFilter.mha
    itkGradientBasedAngleOfIncidenceImageFilterTest
      MIDAS{itkLabelMapToAcousticImpedanceImageFilterTest.mha.md5}
      ${TEMP}/itkGradientBasedAngleOfIncidenceImageFilterTestGradientImageFilter.mha
      ${TEMP}/itkGradientBasedAngleOfIncidenceImageFilterTestGradientRecursiveGaussianImageFilter.mha
      CURVILINEAR
      31.8
      -24.8
  )

MIDAS_ADD_TEST( itkGradientBasedAngleOfIncidenceImageFilterTest2
  ${BASE_USTK_TESTS}
  --compareNumberOfPixelsTolerance 50
  --compare
      MIDAS{itkGradientBasedAngleOfIncidenceImageFilterTest2GradientImageFilter.mha.md5}
      ${TEMP}/itkGradientBasedAngleOfIncidenceImageFilterTest2GradientImageFilter.mha
  --compare
      MIDAS{itkGradientBasedAngleOfIncidenceImageFilterTest2GradientRecursiveGaussianImageFilter.mha.md5}
      ${TEMP}/itkGradientBasedAngleOfIncidenceImageFilterTest2GradientRecursiveGaussianImageFilter.mha
    itkGradientBasedAngleOfIncidenceImageFilterTest
      MIDAS{itkLabelMapToAcousticImpedanceImageFilterTest.mha.md5}
      ${TEMP}/itkGradientBasedAngleOfIncidenceImageFilterTest2GradientImageFilter.mha
      ${TEMP}/itkGradientBasedAngleOfIncidenceImageFilterTest2GradientRecursiveGaussianImageFilter.mha
      CURVILINEAR
      136.5
      -90.8
  )

MIDAS_ADD_TEST( itkGradientBasedAngleOfIncidenceImageFilterTest3
  ${BASE_USTK_TESTS}
  --compareNumberOfPixelsTolerance 50
  --compare
      MIDAS{itkGradientBasedAngleOfIncidenceImageFilterTest3GradientImageFilter.mha.md5}
      ${TEMP}/itkGradientBasedAngleOfIncidenceImageFilterTest3GradientImageFilter.mha
  --compare
      MIDAS{itkGradientBasedAngleOfIncidenceImageFilterTest3GradientRecursiveGaussianImageFilter.mha.md5}
      ${TEMP}/itkGradientBasedAngleOfIncidenceImageFilterTest3GradientRecursiveGaussianImageFilter.mha
    itkGradientBasedAngleOfIncidenceImageFilterTest
      MIDAS{itkLabelMapToAcousticImpedanceImageFilterTest.mha.md5}
      ${TEMP}/itkGradientBasedAngleOfIncidenceImageFilterTest3GradientImageFilter.mha
      ${TEMP}/itkGradientBasedAngleOfIncidenceImageFilterTest3GradientRecursiveGaussianImageFilter.mha
      LINEAR
      0.0
      1.0
  )

MIDAS_ADD_TEST( itkGradientBasedAngleOfIncidenceImageFilterTest4
  ${BASE_USTK_TESTS}
  --compareNumberOfPixelsTolerance 50
  --compare
      MIDAS{itkGradientBasedAngleOfIncidenceImageFilterTest4GradientImageFilter.mha.md5}
      ${TEMP}/itkGradientBasedAngleOfIncidenceImageFilterTest4GradientImageFilter.mha
  --compare
      MIDAS{itkGradientBasedAngleOfIncidenceImageFilterTest4GradientRecursiveGaussianImageFilter.mha.md5}
      ${TEMP}/itkGradientBasedAngleOfIncidenceImageFilterTest4GradientRecursiveGaussianImageFilter.mha
    itkGradientBasedAngleOfIncidenceImageFilterTest
      MIDAS{itkLabelMapToAcousticImpedanceImageFilterTest.mha.md5}
      ${TEMP}/itkGradientBasedAngleOfIncidenceImageFilterTest4GradientImageFilter.mha
      ${TEMP}/itkGradientBasedAngleOfIncidenceImageFilterTest4GradientRecursiveGaussianImageFilter.mha
      LINEAR
      0.5
      0.5
  )

MIDAS_ADD_TEST( itkAcousticImpulseResponseImageFilterTest1
  ${BASE_USTK_TESTS}
  --compareNumberOfPixelsTolerance 50
  --compare
      MIDAS{itkAcousticImpulseResponseImageFilterTest1GradientImageFilter.mha.md5}
      ${TEMP}/itkAcousticImpulseResponseImageFilterTest1GradientImageFilter.mha
  --compare
      MIDAS{itkAcousticImpulseResponseImageFilterTest1GradientRecursiveGaussianImageFilter.mha.md5}
      ${TEMP}/itkAcousticImpulseResponseImageFilterTest1GradientRecursiveGaussianImageFilter.mha
    itkAcousticImpulseResponseImageFilterTest
      MIDAS{itkLabelMapToAcousticImpedanceImageFilterTest.mha.md5}
      ${TEMP}/itkAcousticImpulseResponseImageFilterTest1GradientImageFilter.mha
      ${TEMP}/itkAcousticImpulseResponseImageFilterTest1GradientRecursiveGaussianImageFilter.mha
      31.8
      -24.8
      1.0
  )

MIDAS_ADD_TEST( itkAcousticImpulseResponseImageFilterTest2
  ${BASE_USTK_TESTS}
  --compareNumberOfPixelsTolerance 50
  --compare
      MIDAS{itkAcousticImpulseResponseImageFilterTest2GradientImageFilter.mha.md5}
      ${TEMP}/itkAcousticImpulseResponseImageFilterTest2GradientImageFilter.mha
  --compare
      MIDAS{itkAcousticImpulseResponseImageFilterTest2GradientRecursiveGaussianImageFilter.mha.md5}
      ${TEMP}/itkAcousticImpulseResponseImageFilterTest2GradientRecursiveGaussianImageFilter.mha
    itkAcousticImpulseResponseImageFilterTest
      MIDAS{itkLabelMapToAcousticImpedanceImageFilterTest.mha.md5}
      ${TEMP}/itkAcousticImpulseResponseImageFilterTest2GradientImageFilter.mha
      ${TEMP}/itkAcousticImpulseResponseImageFilterTest2GradientRecursiveGaussianImageFilter.mha
      136.5
      -90.8
      1.0
  )

MIDAS_ADD_TEST( itkAcousticImpulseResponseImageFilterTest3
  ${BASE_USTK_TESTS}
  --compareNumberOfPixelsTolerance 50
  --compare
      MIDAS{itkAcousticImpulseResponseImageFilterTest3GradientImageFilter.mha.md5}
      ${TEMP}/itkAcousticImpulseResponseImageFilterTest3GradientImageFilter.mha
  --compare
      MIDAS{itkAcousticImpulseResponseImageFilterTest3GradientRecursiveGaussianImageFilter.mha.md5}
      ${TEMP}/itkAcousticImpulseResponseImageFilterTest3GradientRecursiveGaussianImageFilter.mha
    itkAcousticImpulseResponseImageFilterTest
      MIDAS{itkLabelMapToAcousticImpedanceImageFilterTest.mha.md5}
      ${TEMP}/itkAcousticImpulseResponseImageFilterTest3GradientImageFilter.mha
      ${TEMP}/itkAcousticImpulseResponseImageFilterTest3GradientRecursiveGaussianImageFilter.mha
      136.5
      -90.8
      0.5
  )

MIDAS_ADD_TEST( itkAcousticImpulseResponseImageFilterTest4
  ${BASE_USTK_TESTS}
  --compareNumberOfPixelsTolerance 50
  --compare
      MIDAS{itkAcousticImpulseResponseImageFilterTest4GradientImageFilter.mha.md5}
      ${TEMP}/itkAcousticImpulseResponseImageFilterTest4GradientImageFilter.mha
  --compare
      MIDAS{itkAcousticImpulseResponseImageFilterTest4GradientRecursiveGaussianImageFilter.mha.md5}
      ${TEMP}/itkAcousticImpulseResponseImageFilterTest4GradientRecursiveGaussianImageFilter.mha
    itkAcousticImpulseResponseImageFilterTest
      MIDAS{itkLabelMapToAcousticImpedanceImageFilterTest.mha.md5}
      ${TEMP}/itkAcousticImpulseResponseImageFilterTest4GradientImageFilter.mha
      ${TEMP}/itkAcousticImpulseResponseImageFilterTest4GradientRecursiveGaussianImageFilter.mha
      136.5
      -90.8
      2.0
  )

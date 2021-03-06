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

project( TubeFiltering )

find_package( ITK REQUIRED )
include( ${ITK_USE_FILE} )

set(TubeTK_Base_Filtering_H_Files
  itkAnisotropicCoherenceEnhancingDiffusionImageFilter.h
  itkAnisotropicDiffusionTensorFunction.h
  itkAnisotropicDiffusionTensorImageFilter.h
  itkAnisotropicEdgeEnhancementDiffusionImageFilter.h
  itkAnisotropicHybridDiffusionImageFilter.h
  itkStructureTensorRecursiveGaussianImageFilter.h
  itkSymmetricEigenVectorAnalysisImageFilter.h
  itkTubeEnhancingDiffusion2DImageFilter.h
  itkSheetnessMeasureImageFilter.h
)

set(TubeTK_Base_Filtering_TXX_Files
  itkAnisotropicCoherenceEnhancingDiffusionImageFilter.txx
  itkAnisotropicDiffusionTensorFunction.txx
  itkAnisotropicDiffusionTensorImageFilter.txx
  itkAnisotropicEdgeEnhancementDiffusionImageFilter.txx
  itkAnisotropicHybridDiffusionImageFilter.txx
  itkStructureTensorRecursiveGaussianImageFilter.txx
  itkTubeEnhancingDiffusion2DImageFilter.txx
  itkSheetnessMeasureImageFilter.txx
)

include_directories(
  ${TubeTK_SOURCE_DIR}/Utilities
  ${TubeTK_SOURCE_DIR}/Base/Numerics
  )

#set( TubeTK_Base_Filtering_SRCS
#  )

#add_library( TubeFiltering ${TubeTK_Base_Filtering_SRCS} )
#target_link_libraries( TubeFiltering ${ITK_LIBRARIES} )

#slicer3_set_plugins_output_path( TubeFiltering)
#set( TARGETS TubeFiltering )
#slicer3_install_plugins( ${TARGETS} )

if( BUILD_TESTING )
  add_subdirectory( Testing )
endif( BUILD_TESTING )

install( FILES
  ${TubeTK_Base_Filtering_H_Files}
  ${TubeTK_Base_Filtering_TXX_Files}
  DESTINATION include
  )

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


#-----------------------------------------------------------------------------
set(MODULE_NAME tubeAnisotropicDiffusiveDeformableRegistration)

find_package(SlicerExecutionModel REQUIRED)
include(${SlicerExecutionModel_USE_FILE})

find_package( ITK REQUIRED )
include( ${USE_ITK_FILE} )

find_package( VTK REQUIRED )
include( ${USE_VTK_FILE} )

#-----------------------------------------------------------------------------
SEMMacroBuildCLI(
  NAME ${MODULE_NAME}
  LOGO_HEADER ${Slicer_SOURCE_DIR}/Resources/NAMICLogo.h
  TARGET_LIBRARIES ${ITK_LIBRARIES} vtkCommon vtkGraphics vtkIO 
  INCLUDE_DIRECTORIES
    ${TubeTK_SOURCE_DIR}/Utilities
    ${TubeTK_SOURCE_DIR}/Base/Filtering
    ${TubeTK_SOURCE_DIR}/Base/Numerics
    ${TubeTK_SOURCE_DIR}/Base/Registration
  )

# TODO Check if this file is different from the own provided by Slicer: tubeCLISharedLibraryWrapper.cxx )

if( BUILD_TESTING )
  add_subdirectory( Testing )
endif( BUILD_TESTING )

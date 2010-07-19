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

##############################################################################
#
# Configure the following variables and move this file to the directory above
#   the tubetk source directory.
#
set( SITE_NAME "ginger.aylward.org" )
set( SITE_PLATFORM "WindowsXP-VS2010Exp" )
set( SITE_BUILD_TYPE "Release" )
set( SITE_CMAKE_GENERATOR "Visual Studio 10" )

set( SITE_MAKE_COMMAND "nmake" )
set( SITE_CMAKE_COMMAND "C:\\Program Files\\CMake 2.8\\bin\\cmake" )
set( SITE_QMAKE_COMMAND "C:\\Qt\\4.6.3\\bin\\qmake" )
set( SITE_CTEST_COMMAND "C:\\Program Files\\CMake 2.8\\bin\\ctest" )
set( SITE_MEMORYCHECK_COMMAND "" )
set( SITE_COVERAGE_COMMAND "" )
set( SITE_STYLE_COMMAND "" )

set( GIT_COMMAND 
  "C:\\Program Files\\Git\\bin\\git" )
set( SVNCOMMAND 
  "C:\\Program Files\\CollabNet\\Subversion Client\\svn" )
set( Subversion_SVN_EXECUTABLE "${SVNCOMMAND}" )

set( SITE_SOURCE_DIR 
  "C:\\Documents and Settings\\aylward\\My Documents\\src\\tubetk" )
set( SITE_BINARY_DIR 
  "C:\\Documents and Settings\\aylward\\My Documents\\src\\tubetk-Release" )

set( SITE_NIGHTLY ON )
set( SITE_NIGHTLY_STYLECHECK OFF )
set( SITE_NIGHTLY_COVERAGE OFF )
set( SITE_NIGHTLY_MEMORYCHECK OFF )
set( SITE_CONTINUOUS ON )
set( SITE_CONTINUOUS_STYLECHECK OFF )
set( SITE_CONTINUOUS_COVERAGE OFF )
set( SITE_CONTINUOUS_MEMORYCHECK OFF )


###########################################################################
# 
# The following advanced variables should only be changed by experts
#
set( SITE_BUILD_NAME "${SITE_PLATFORM}-${SITE_BUILD_TYPE}" )
set( SITE_UPDATE_COMMAND "${GIT_COMMAND}" )
set( SITE_SCRIPT_DIR "${SITE_SOURCE_DIR}/DashboardScripts" )

set(ENV{PATH} "${SITE_BINARY_DIR}/ModuleDescriptionParser-Build/${SITE_BUILD_TYPE};${SITE_BINARY_DIR}/GenerateCLP-Build/${SITE_BUILD_TYPE};${SITE_BINARY_DIR}/Insight-Build/bin/${SITE_BUILD_TYPE};${SITE_BINARY_DIR}/OpenIGTLink-Build/bin/${SITE_BUILD_TYPE};${SITE_BINARY_DIR}/TubeTK-Build/bin/${SITE_BUILD_TYPE};${SITE_BINARY_DIR}/TubeTK-Build/lib/TubeTK/Plugins/${SITE_BUILD_TYPE};$ENV{PATH}" )

set( SITE_CXX_FLAGS "" )
set( SITE_C_FLAGS "" )

set( COVERAGE_OPTIONS "" )
if( SITE_NIGHTLY_COVERAGE OR SITE_CONTINUOUS_COVERAGE )
  set( SITE_CXX_FLAGS "${SITE_CXX_FLAGS} ${COVERAGE_OPTIONS}" )
endif( SITE_NIGHTLY_COVERAGE OR SITE_CONTINUOUS_COVERAGE )

set( MEMORYCHECK_OPTIONS "" )

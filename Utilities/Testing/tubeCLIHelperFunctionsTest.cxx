/*=========================================================================

Library:   TubeTK

Copyright 2010 Kitware Inc. 28 Corporate Drive,
Clifton Park, NY, 12065, USA.

All rights reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#include <cstdlib>

#include "itkImage.h"

// Forward declaration required by CLIHelperFunctions
template< class pixelT, unsigned int dimensionT >
int DoIt( int argc, char *argv[] );

// Must include CLP before including tubeCLIHleperFunctions
#include "tubeCLIHelperFunctionsTestCLP.h"

// Includes tube::ParseArgsAndCallDoIt function
#include "tubeCLIHelperFunctions.h"

template< class pixelT, unsigned int dimensionT >
int DoIt( int argc, char *argv[] )
{
  PARSE_ARGS;

  return EXIT_SUCCESS;
}

int tubeCLIHelperFunctionsTest(int argc, char *argv[] )
{
  PARSE_ARGS;

  return tube::ParseArgsAndCallDoIt( inputVolume, argc, argv );
}

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

#include <iostream>
#include "tubeTestMain.h"

void RegisterTests()
{
#ifdef TubeTK_USE_VTK
  REGISTER_TEST( itkAnisotropicDiffusiveRegistrationGenerateTestingImages );
  REGISTER_TEST( itkAnisotropicDiffusiveRegistrationRegularizationTest );
#endif
  REGISTER_TEST( itkTubePointsToImageTest );
  REGISTER_TEST( itkImageToTubeRigidRegistrationTest );
  REGISTER_TEST( itkImageToTubeRigidRegistration2Test );
  REGISTER_TEST( itkImageToTubeRigidRegistrationPerformancesTest );
  REGISTER_TEST( itkImageToTubeRigidRegistration2PerformancesTest );
  REGISTER_TEST( itkTubeToTubeTransformFilterTest );
  REGISTER_TEST( itkImageToTubeRigidMetricTest );
  REGISTER_TEST( itkImageToTubeRigidMetricPerformancesTest );
  REGISTER_TEST( itkImageToTubeRigidMetric2Test );
  REGISTER_TEST( itkImageToTubeRigidMetric2PerformancesTest );
  REGISTER_TEST( itkSyntheticTubeImageGenerationsTest );
}


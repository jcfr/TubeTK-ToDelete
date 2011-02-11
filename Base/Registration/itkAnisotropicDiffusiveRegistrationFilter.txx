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
#ifndef __itkAnisotropicDiffusiveRegistrationFilter_txx
#define __itkAnisotropicDiffusiveRegistrationFilter_txx

#include "itkAnisotropicDiffusiveRegistrationFilter.h"
#include "itkSmoothingRecursiveGaussianImageFilter.h"

#include "vtkDataArray.h"
#include "vtkPointData.h"
#include "vtkPointLocator.h"
#include "vtkPolyDataNormals.h"

namespace itk
{

/**
 * Constructor
 */
template < class TFixedImage, class TMovingImage, class TDeformationField >
AnisotropicDiffusiveRegistrationFilter
< TFixedImage, TMovingImage, TDeformationField >
::AnisotropicDiffusiveRegistrationFilter()
{
}

/**
 * PrintSelf
 */
template < class TFixedImage, class TMovingImage, class TDeformationField >
void
AnisotropicDiffusiveRegistrationFilter
  < TFixedImage, TMovingImage, TDeformationField >
::PrintSelf( std::ostream& os, Indent indent ) const
{
  Superclass::PrintSelf( os, indent );
}

} // end namespace itk

#endif

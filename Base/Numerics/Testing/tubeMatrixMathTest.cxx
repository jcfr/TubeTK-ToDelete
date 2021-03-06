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
#include <cstdlib>

#include "itkMersenneTwisterRandomVariateGenerator.h"

#include "tubeMacro.h"
#include "tubeMatrixMath.h"

template< int dimensionT >
int Test( void )
{
  double epsilon = 0.00001;

  itk::Statistics::MersenneTwisterRandomVariateGenerator::Pointer rndGen
    = itk::Statistics::MersenneTwisterRandomVariateGenerator::New();
  rndGen->Initialize( 1 );

  int returnStatus = EXIT_SUCCESS;

  for( unsigned int count=0; count<1000; count++ )
    {
    vnl_vector<float> v1(dimensionT);
    for( unsigned int d=0; d<dimensionT; d++ )
      {
      v1[d] = rndGen->GetNormalVariate( 0.0, 1.0 );
      }
    vnl_vector<float> v2(dimensionT);
    if( dimensionT == 3 )
      {
      v2 = tube::GetOrthogonalVector( v1 );
      if( vnl_math_abs( dot_product( v1, v2 ) ) > epsilon )
        {
        std::cout << count << " : ";
        std::cout << "FAILURE: GetOrthogonalVector: DotProduct = "
                  << v1 << " .* " << v2 << " = "
                  << dot_product( v1, v2 ) << std::endl;
        returnStatus = EXIT_FAILURE;
        }

      vnl_vector<float> v3 = tube::GetCrossVector( v1, v2 );
      if( vnl_math_abs( dot_product( v1, v3 ) ) > epsilon ||
          vnl_math_abs( dot_product( v2, v3 ) ) > epsilon )
        {
        std::cout << count << " : ";
        std::cout << "FAILURE: GetCrossVector: DotProduct = "
          << dot_product( v1, v3 ) << " and "
          << dot_product( v2, v3 ) << std::endl;
        returnStatus = EXIT_FAILURE;
        }
      }
    else
      {
      for( unsigned int d=0; d<dimensionT; d++ )
        {
        v2[d] = rndGen->GetNormalVariate( 0.0, 1.0 );
        }
      }

    v2 = v2.normalize();
    vnl_vector<float> v4 = tube::ComputeLineStep( v1, 0.5, v2 );
    if( vnl_math_abs( tube::ComputeEuclideanDistanceVector( v1, v4 ) - 0.5 )
        > epsilon )
      {
      std::cout << count << " : ";
      std::cout << "FAILURE: ComputeLineStep = "
        << v1 << " + 0.5 * " << v2 << " != " << v4 << std::endl;
      std::cout << "FAILURE: ComputeEuclidenDistanceVector = "
        << tube::ComputeEuclideanDistanceVector( v1, v4 ) << std::endl;
      returnStatus = EXIT_FAILURE;
      }

    vnl_matrix<float> m1(dimensionT, dimensionT);
    for( unsigned int r=0; r<dimensionT; r++ )
      {
      for( unsigned int c=r; c<dimensionT; c++ )
        {
        m1(r,c) = rndGen->GetNormalVariate( 0.0, 1.0 );
        m1(c,r) = m1(r,c);
        }
      }

    vnl_matrix<float> eVects(dimensionT, dimensionT);
    vnl_vector<float> eVals(dimensionT);
    tube::Eigen( m1, eVects, eVals, true );
    for( unsigned int d=0; d<dimensionT; d++ )
      {
      v1 = m1 * eVects.get_column(d);
      if( vnl_math_abs( v1.magnitude() - vnl_math_abs(eVals[d]) ) > epsilon )
        {
        std::cout << count << " : ";
        std::cout << "FAILURE: Eigen : "
          << " v1 * M1 = " << v1
          << " and v1 norm = " << v1.magnitude()
          << " != " << eVals[d]
          << std::endl;
        returnStatus = EXIT_FAILURE;
        }
      }
    }

  return returnStatus;
}

int tubeMatrixMathTest(int tubeNotUsed(argc), char * tubeNotUsed(argv)[])
{

  if( Test<2>() == EXIT_FAILURE ||
      Test<3>() == EXIT_FAILURE ||
      Test<4>() == EXIT_FAILURE )
    {
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;

}

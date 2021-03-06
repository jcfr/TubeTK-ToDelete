/*=========================================================================

Library:   TubeTK

Copyright 2010 Kitware Inc. 28 Corporate Drive,
Clifton Park, NY, 12065, USA.

All rights reserved.

Licensed under the Apache License, Version 2.0 ( the "License" );
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

=========================================================================*/
#ifndef __itkImageToTubeRigidMetric_txx
#define __itkImageToTubeRigidMetric_txx

#include "itkImageToTubeRigidMetric.h"

namespace itk
{

/** Constructor */
template < class TFixedImage, class TMovingSpatialObject>
ImageToTubeRigidMetric<TFixedImage, TMovingSpatialObject>
::ImageToTubeRigidMetric()
{
  m_MaskImage = 0;
  m_Iteration = 1;
  m_Kappa = 1;
  m_RegImageThreshold = 0;
  m_Sampling = 30;

  m_Scale = 1;
  m_Factors.fill(1.0);
  m_RotationCenter.fill(0);
  m_Offsets = new vnl_vector<double>( 3, 0 );

  m_T = new vnl_matrix<double>( 3, 3 );
  m_T->set_identity();

  m_Extent = 3;     // TODO Check depedencies --> enum { ImageDimension = 3 };
  m_Verbose = true;
  m_DerivativeImageFunction = DerivativeImageFunctionType::New();

  this->m_FixedImage = 0;           // has to be provided by the user.
  this->m_MovingSpatialObject = 0;  // has to be provided by the user.
  this->m_Transform = 0;            // has to be provided by the user.
  this->m_Interpolator = 0;         // has to be provided by the user.
}

/** Destructor */
template < class TFixedImage, class TMovingSpatialObject>
ImageToTubeRigidMetric<TFixedImage, TMovingSpatialObject>
::~ImageToTubeRigidMetric()
{
  delete m_T;
  delete m_Offsets;
}

/** SetImageRange */
template < class TFixedImage, class TMovingSpatialObject>
void
ImageToTubeRigidMetric<TFixedImage, TMovingSpatialObject>
::ComputeImageRange( void )
{
  m_RangeCalculator = RangeCalculatorType::New();
  m_RangeCalculator->SetImage( this->m_FixedImage );
  m_RangeCalculator->Compute();
  m_ImageMin = m_RangeCalculator->GetMinimum();
  m_ImageMax = m_RangeCalculator->GetMaximum();

  if( m_Verbose )
    {
    std::cout << "ImageMin = " << m_ImageMin << std::endl;
    std::cout << "ImageMax = " << m_ImageMax << std::endl;
    }

  m_RegImageThreshold = m_ImageMax;
}

/** Initialize the metric  */
template < class TFixedImage, class TMovingSpatialObject>
void
ImageToTubeRigidMetric<TFixedImage, TMovingSpatialObject>
::Initialize( void ) throw ( ExceptionObject )
{
  m_Weight.clear();
  m_NumberOfPoints = 0;
  m_SumWeight = 0;

  if( !this->m_MovingSpatialObject || !this->m_FixedImage)
    {
    std::cout << "SubSampleTube : No tube/image net plugged in ! " << std::endl;
    return;
    }

  this->ComputeImageRange();
  this->SubSampleTube();
  this->ComputeCenterRotation();

  this->m_Interpolator->SetInputImage( this->m_FixedImage );
  m_DerivativeImageFunction->SetInputImage( this->m_FixedImage );
}

/** Subsample the MovingSpatialObject tubenet  */
// WARNING: I think there is a bug from Stephen method with the
// index incrementation. I currently replicated the exact same behaviour,
// but the method must be validated.
template < class TFixedImage, class TMovingSpatialObject>
void
ImageToTubeRigidMetric<TFixedImage, TMovingSpatialObject>
::SubSampleTube()
{
  double weight = 0.0;
  unsigned int tubeSize = 0;
  unsigned int step = m_Sampling / 2 - 1;

  vnl_matrix<double> biasV( 3, 3, 0 );
  TubeNetType::Pointer newTubeNet = TubeNetType::New();
  TubeNetType::ChildrenListType* tubeList = GetTubes();
  TubeNetType::ChildrenListType::iterator tubeIterator = tubeList->begin();
  for ( ; tubeIterator != tubeList->end(); ++tubeIterator )
    {
    TubeType::Pointer newTube = TubeType::New();
    TubeType* currTube =
      static_cast<TubeType*>( ( *tubeIterator ).GetPointer() );

    currTube->RemoveDuplicatePoints();
    currTube->ComputeTangentAndNormals();

    tubeSize = currTube->GetPoints().size();
    if ( tubeSize > m_Sampling )
      {
      typename std::vector<TubePointType>::iterator  tubePointIterator;
      int loopIndex = 0;
      unsigned int skippedPoints = 0;
      for ( tubePointIterator = currTube->GetPoints().begin();
            tubePointIterator != currTube->GetPoints().end();
            tubePointIterator++, ++loopIndex )
        {
        if( m_Sampling != 1 )
          {
          if ( std::distance(tubePointIterator, currTube->GetPoints().end() -1 )
               > step )
            {
            tubePointIterator +=
              (tubePointIterator == currTube->GetPoints().begin()) ? 0 : step;
            skippedPoints = ( loopIndex * ( step + 1 ) * 2 ) + 1;
            }
          else
            {
            skippedPoints = step + 2;
            tubePointIterator = currTube->GetPoints().end();
            }
          }

        // TODO Why +10 ?!
        if( tubePointIterator != currTube->GetPoints().end()
            && ( ( skippedPoints + 10 ) < tubeSize ) )
          {
          newTube->GetPoints().push_back( *( tubePointIterator ) );
          double val = -2 * ( tubePointIterator->GetRadius() );
          weight = 2.0 / ( 1.0 + exp( val ) );

          m_Weight.push_back( weight );

          vnl_matrix<double> tM( 3, 3 );
          vnl_vector_fixed<double, ImageDimension> v1;
          vnl_vector_fixed<double, ImageDimension> v2;

          for( unsigned int i = 0; i < ImageDimension; ++i )
            {
            v1[i] = ( *tubePointIterator ).GetNormal1()[i];
            v2[i] = ( *tubePointIterator ).GetNormal2()[i];
            }

          tM = outer_product( v1, v1 );
          tM = tM + outer_product( v2, v2 );
          biasV = biasV + ( weight * tM );
          for( unsigned int i = 0; i < ImageDimension; ++i )
            {
            m_RotationCenter[i] +=
              weight * ( tubePointIterator->GetPosition() )[i];
            }
          m_SumWeight += weight;
          m_NumberOfPoints++;
          }

        if( m_Sampling > 1 )
          {
          if ( std::distance(tubePointIterator, currTube->GetPoints().end() -1 )
               > step )
            {
            tubePointIterator += step;
            }
          else
            {
            tubePointIterator = currTube->GetPoints().end() - 1;
            }
          }
        }
      }
    newTubeNet->AddSpatialObject( newTube );
    }

  if ( m_Verbose )
    {
    std::cout << "Number of Points for the metric = "
              << m_NumberOfPoints << std::endl;
    }

  this->SetMovingSpatialObject( newTubeNet );
  delete tubeList;
}

/** Compute the Center of Rotation */
template < class TFixedImage, class TMovingSpatialObject>
void
ImageToTubeRigidMetric<TFixedImage, TMovingSpatialObject>
::ComputeCenterRotation()
{
  for ( unsigned int i = 0; i<ImageDimension; ++i )
    {
    m_RotationCenter[i] /= m_SumWeight;
    }

  if( m_Verbose )
    {
    std::cout << "Center of Rotation = "
              << m_RotationCenter[0] << "  " \
              << m_RotationCenter[1] << "  " \
              << m_RotationCenter[2] << std::endl;
    std::cout << "Extent = " << m_Extent << std::endl;
    }
}

/** Get tubes contained within the Spatial Object */
// WARNING:
// Method might use GetMaximumDepth from ITK.
// Patch pushed in ITKv4, waiting for validation.
template < class TFixedImage, class TMovingSpatialObject>
typename GroupSpatialObject<3>::ChildrenListType*
ImageToTubeRigidMetric<TFixedImage, TMovingSpatialObject>
::GetTubes() const
{
  if (!this->m_MovingSpatialObject)
    {
    return 0;
    }

  char childName[] = "Tube";
  return this->m_MovingSpatialObject->GetChildren( 999999, childName );
  //return this->m_MovingSpatialObject
  // ->GetChildren( this->m_MovingSpatialObject->GetMaximumDepth(), childName );
}

/** Get the match Measure */
// TODO Do not pass the parameter as arguments use instead
// the transform parameters previously set.
template < class TFixedImage, class TMovingSpatialObject>
typename ImageToTubeRigidMetric<TFixedImage, TMovingSpatialObject>::MeasureType
ImageToTubeRigidMetric<TFixedImage, TMovingSpatialObject>
::GetValue( const ParametersType & parameters ) const
{
  if( m_Verbose )
    {
    std::cout << "**** Get Value ****" << std::endl;
    std::cout << "Parameters = " << parameters << std::endl;
    }

  MeasureType matchMeasure = 0;
  double sumWeight = 0;
  double count = 0;
  double opR;

  std::list<double>::const_iterator weightIterator;
  weightIterator = m_Weight.begin();

  // TODO change the place where you set the parameters !
  //SetOffset( parameters[3], parameters[4], parameters[5] );
  //this->m_Transform->SetParameters( parameters );

  GroupSpatialObject<3>::ChildrenListType::iterator tubeIterator;
  TubeNetType::ChildrenListType* tubeList = GetTubes();
  for( tubeIterator = tubeList->begin();
       tubeIterator != tubeList->end();
       tubeIterator++ )
    {
    TubeType* currTube = static_cast<TubeType*>(
      ( *tubeIterator ).GetPointer() );

    std::vector<TubePointType>::iterator pointIterator;
    for( pointIterator = currTube->GetPoints().begin();
         pointIterator != currTube->GetPoints().end();
         ++pointIterator )
        {
        itk::Point<double, 3> inputPoint = pointIterator->GetPosition();
        /*static itk::Point<double, 3> point;
        Matrix<double, 3, 3> matrix =  GetTransform()->GetRotationMatrix();

        point =  matrix * inputPoint + GetTransform()->GetOffset();

        vnl_vector_fixed<double, 3> rotationOffset = matrix * m_RotationCenter;

        point[0] += m_RotationCenter[0] - rotationOffset[0];
        point[1] += m_RotationCenter[1] - rotationOffset[1];
        point[2] += m_RotationCenter[2] - rotationOffset[2];

        // TODO
        // Need to use interpolator intead

        itk::Index<3> index;
        index[0] = static_cast<unsigned int>( point[0] );
        index[1] = static_cast<unsigned int>( point[1] );
        index[2] = static_cast<unsigned int>( point[2] );*/

        if( this->IsInside( inputPoint ) )
          {
          sumWeight += *weightIterator;
          count++;
          opR = pointIterator->GetRadius();
          opR = std::max( opR, 0.5 );

          SetScale( opR * m_Kappa );

          Vector<double, 3> v2;
          for( unsigned int i = 0; i < 3; ++i )
            {
            v2[i] = pointIterator->GetNormal1()[i];
            }
          std::cout << "vector: " << v2[0] << " - " << v2[1] << " - " << v2[2] << std::endl;

            matchMeasure += *weightIterator * fabs(
              ComputeLaplacianMagnitude( &v2 ) );
            }
        else
          {
          matchMeasure -= m_ImageMax;
          }

        weightIterator++;
        }
    }

  std::cout << "SumWeight: " << sumWeight << std::endl;

  if( sumWeight == 0 )
    {
    std::cout << "GetValue: All the mapped image is outside ! " << std::endl;
    matchMeasure = -1;
    }
  else
    {
    float imageRatio = static_cast<float>( m_ImageMin / m_ImageMax );
    float normalizedMeasure = static_cast<float>( matchMeasure / sumWeight );
    matchMeasure = normalizedMeasure
                   - imageRatio
                   - static_cast<float>( m_ImageMin );
    }

  if( m_Verbose )
    {
    std::cout << "matchMeasure = " << matchMeasure << std::endl;
    }

  delete tubeList;
  return matchMeasure;
}

/** Compute the Laplacian magnitude */
// TODO FACTORIZE CODE --> See computeThirdDerivative
template < class TFixedImage, class TMovingSpatialObject>
double
ImageToTubeRigidMetric<TFixedImage, TMovingSpatialObject>
::ComputeLaplacianMagnitude( Vector<double, 3> *v ) const
{
  // We convolve the 1D signal defined by the direction v at point
  // m_CurrentPoint with a second derivative of a gaussian
  const double scaleSquared = std::pow( m_Scale, 2 );
  const double scaleExtentProduct = m_Scale * m_Extent;
  double result = 0;
  double wI = 0;
  unsigned int n = 0;

  itk::Index<3> index;
  for( double dist = -scaleExtentProduct; dist <= scaleExtentProduct; ++dist )
    {
    index[0] =
      static_cast<unsigned int>( m_CurrentPoint[0] + dist * v->GetElement(0) );
    index[1] =
      static_cast<unsigned int>( m_CurrentPoint[1] + dist * v->GetElement(1) );
    index[2] =
      static_cast<unsigned int>( m_CurrentPoint[2] + dist * v->GetElement(2) );

    double distSquared = std::pow( dist, 2 );
    itk::Size<3> size =
      this->m_FixedImage->GetLargestPossibleRegion().GetSize();
    if( index[0] >= 0 && ( index[0] < static_cast<unsigned int>( size[0] ) )
      && index[1] >= 0 && ( index[1] < static_cast<unsigned int>( size[1] ) )
      && index[2] >= 0 && ( index[2] < static_cast<unsigned int>( size[2] ) ) )
      {
      wI += ( -1 + ( distSquared / scaleSquared ) )
        * exp( -0.5 * distSquared / scaleSquared );
      n++;
      }
    }

  double error = wI / n;
  for( double dist = -scaleExtentProduct; dist <= scaleExtentProduct; ++dist )
    {
    double distSquared = std::pow( dist, 2 );
    wI = ( -1 + ( distSquared / scaleSquared ) )
      * exp( -0.5 * distSquared / scaleSquared ) - error;

    index[0] =
      static_cast<unsigned int>( m_CurrentPoint[0] + dist * v->GetElement(0) );
    index[1] =
      static_cast<unsigned int>( m_CurrentPoint[1] + dist * v->GetElement(1) );
    index[2] =
      static_cast<unsigned int>( m_CurrentPoint[2] + dist * v->GetElement(2) );

    itk::Size<3> size =
      this->m_FixedImage->GetLargestPossibleRegion().GetSize();
    if( index[0] >= 0 && ( index[0] < static_cast<unsigned int>( size[0] ) )
      && index[1] >= 0 && ( index[1] < static_cast<unsigned int>( size[1] ) )
      && index[2] >= 0 && ( index[2] < static_cast<unsigned int>( size[2] ) ) )
      {
      double value = this->m_FixedImage->GetPixel( index );
      result += value * wI;
      }
    }

  return result;
}

/** Compute the Hessian value at a given point */
template < class TFixedImage, class TMovingSpatialObject>
typename ImageToTubeRigidMetric< TFixedImage, TMovingSpatialObject>::MatrixType*
ImageToTubeRigidMetric<TFixedImage, TMovingSpatialObject>
::GetHessian( PointType point ) const
{
  int i, j, k, l, m;
  double xDist2, yDist2, zDist2;
  double wI, wTotalI=0, resI=0;
  MatrixType* hessian = new MatrixType( 3, 3, 0 );
  MatrixType w( 3, 3, 0.0 );
  MatrixType wTotal( 3, 3, 0.0 );

  int pointBounds[6];
  this->GetPointBounds( point, pointBounds );
  for( i = pointBounds[4]; i <= pointBounds[5]; ++i )
    {
    zDist2 = ( i - point[2] ) * ( i - point[2] ) * std::pow( m_Factors[2], 2 );
    for( j = pointBounds[2]; j <= pointBounds[3]; ++j )
      {
      yDist2 = ( j - point[1] ) * ( j - point[1] ) * std::pow( m_Factors[1], 2 );
      for( k = pointBounds[0]; k <= pointBounds[1]; ++k )
        {
        xDist2 = ( k - point[0] ) * ( k - point[0] ) * std::pow( m_Factors[0], 2 );
        if( xDist2 + yDist2 + zDist2 <= std::pow( m_Scale * m_Extent, 2 ) )
          {
          wI = static_cast<double>( exp( -0.5 * ( yDist2 + xDist2 + zDist2 )
            / std::pow( m_Scale, 2 ) ) );

          w( 0, 0 ) = ( 4 * xDist2 - 2 ) * wI;
          w( 0, 1 ) = ( 4 * ( k-point[0] )
            * ( j - point[1] ) * m_Factors[0] * m_Factors[1] ) * wI;
          w( 0, 2 ) = ( 4 * ( k-point[0] )
            * ( i - point[2] ) * m_Factors[0] * m_Factors[2] ) * wI;
          w( 1, 0 ) = w( 0, 1 );
          w( 1, 1 ) = ( 4 * yDist2 - 2 ) * wI;
          w( 1, 2 ) = ( 4 * ( j-point[1] )
            * ( i - point[2] ) * m_Factors[1] * m_Factors[2] ) * wI;
          w( 2, 0 ) = w( 0, 2 );
          w( 2, 1 ) = w( 1, 2 );
          w( 2, 2 ) = ( 4 * zDist2 - 2 ) * wI;

          itk::Size<3> size =
            this->m_FixedImage->GetLargestPossibleRegion().GetSize();

          if( i >= 0 && ( i < size[2] ) && j >= 0 && ( j < size[1] )
            && k >= 0 && ( k < size[0] ) )
            {
            wTotalI += static_cast<double>( fabs( wI ) );
            for( l = 0; l < 3; ++l )
              {
              for( m = 0; m < 3; ++m )
                {
                wTotal( l, m ) += fabs( w( l, m ) );
                }
              }

            itk::Index<3> index = {k, j, i};
            double value = this->m_FixedImage->GetPixel( index );
            resI += value * wI;

            for( l = 0; l < 3; ++l )
              {
              for( m = 0; m < 3; ++m )
                {
                ( *hessian )( l, m ) += value * w( l, m );
                }
              }
            }
          }
        }
      }
    }

  for( l = 0; l < 3; ++l )
    {
    for( m = 0; m < 3; ++m )
      {
      ( *hessian )( l, m ) /= wTotal( l, m );
      }
    }

  return hessian;
}

/** GetSecondDerivative */
template < class TFixedImage, class TMovingSpatialObject>
typename ImageToTubeRigidMetric< TFixedImage, TMovingSpatialObject>::VectorType*
ImageToTubeRigidMetric<TFixedImage, TMovingSpatialObject>
::GetSecondDerivatives( void ) const
{
  vnl_vector<double>* derivatives = new  vnl_vector<double>( 3 );
  itk::Index<3> index;
  PixelType value;
  double squaredDistance;
  double squaredDistances[3];
  double wI;          // TODO Rename these variables
  double wTotalI = 0; // TODO Rename these variables
  double weights[3];
  double result[3] = { 0, 0, 0 };
  double const gfact = -0.5 / ( std::pow( m_Scale, 2 ) );

  int currentPointBounds[6];
  this->GetCurrentPointBounds( currentPointBounds );
  this->ClampPointBoundsToImage( currentPointBounds );
  for( int i = currentPointBounds[4]; i <= currentPointBounds[5]; ++i )
    {
    squaredDistances[2] = std::pow( i - m_CurrentPoint[2], 2 );
    for( int j = currentPointBounds[2]; j <= currentPointBounds[3]; ++j )
      {
      squaredDistances[1] = std::pow( j - m_CurrentPoint[1], 2 );
      for( int k = currentPointBounds[0]; k <= currentPointBounds[1]; ++k )
        {
        squaredDistances[0] = std::pow( k - m_CurrentPoint[0], 2 );
        squaredDistance = squaredDistances[0] +
                          squaredDistances[1] +
                          squaredDistances[2];

        wI = exp( gfact * ( squaredDistance ) );
        weights[0] = ( 1 - squaredDistances[0] ) * wI;
        weights[1] = ( 1 - squaredDistances[1] ) * wI;
        weights[2] = ( 1 - squaredDistances[2] ) * wI;
        wTotalI += wI;

        // WARNING: value was set to 1000
        index[0]=k; index[1]=j; index[2]=i;
        value = this->m_FixedImage->GetPixel( index );
        result[0] += value * weights[0];
        result[1] += value * weights[1];
        result[2] += value * weights[2];
        }
      }
    }

  if (m_Verbose)
    {
    std::cout << "Result = "
              << result[0] << " : "
              << result[1] << " : "
              << result[2] << std::endl;
    }

  if( wTotalI == 0 )
    {
    derivatives->fill( 0 );
    }
  else
    {
    ( *derivatives )( 0 ) = result[0];
    ( *derivatives )( 1 ) = result[1];
    ( *derivatives )( 2 ) = result[2];
    }

  return derivatives;
}

/** GetDeltaAngles */
template < class TFixedImage, class TMovingSpatialObject>
void
ImageToTubeRigidMetric<TFixedImage, TMovingSpatialObject>
::GetDeltaAngles( const Point<double, 3> & x,
  const vnl_vector_fixed<double, 3> & dx,
  double angle[3] ) const
{
  vnl_vector_fixed<double, 3> tempV;
  vnl_vector_fixed<double, 3> pos;
  pos[0] = x[0];
  pos[1] = x[1];
  pos[2] = x[2];

  tempV = ( pos - ( *m_Offsets ) ) - ( m_RotationCenter );
  tempV.normalize();

  angle[0] = dx[1] * ( -tempV[2] ) + dx[2] * tempV[1];
  angle[1] = dx[0] * tempV[2] + dx[2] * ( -tempV[0] );
  angle[2] = dx[0] * ( -tempV[1] ) + dx[1] * tempV[0];
}

/** Set the offset */
template < class TFixedImage, class TMovingSpatialObject>
void
ImageToTubeRigidMetric<TFixedImage, TMovingSpatialObject>
::SetOffset( double oX, double oY, double oZ ) const
{
  if( ( *m_Offsets )( 0 ) == oX && ( *m_Offsets )( 1 ) == oY && ( *m_Offsets )( 2 ) == oZ )
    {
    return;
    }

  ( *m_Offsets )( 0 ) = oX;
  ( *m_Offsets )( 1 ) = oY;
  ( *m_Offsets )( 2 ) = oZ;
}

/** Transform a point */
template < class TFixedImage, class TMovingSpatialObject>
void
ImageToTubeRigidMetric<TFixedImage, TMovingSpatialObject>
::TransformPoint( vnl_vector<double> * in, vnl_vector<double> * out ) const
{
  vnl_vector<double>* tempV = new vnl_vector<double>( 3, 0 );
  vnl_vector<double>* tempV2 = new vnl_vector<double>( 3, 0 );

  ( *out ) = ( *in ) - ( m_RotationCenter );
  ( *tempV ) = ( *m_T ) * ( *out );

  ( *tempV2 ) = ( *tempV ) + ( *m_Offsets );
  ( *out ) = m_Scale * ( ( *tempV2 ) + ( m_RotationCenter ) );
}

/** Transform a vector */
template < class TFixedImage, class TMovingSpatialObject>
void
ImageToTubeRigidMetric<TFixedImage, TMovingSpatialObject>
::TransformVector( vnl_vector<double> * in, vnl_vector<double> * out )
{
  ( *out ) = m_Scale * ( ( *m_T ) * ( *in ) );
}

/** Set Angles */
template < class TFixedImage, class TMovingSpatialObject>
void
ImageToTubeRigidMetric<TFixedImage, TMovingSpatialObject>
::SetAngles( double alpha, double beta, double gamma ) const
{
  double ca = cos( alpha );
  double sa = sin( alpha );
  double cb = cos( beta );
  double sb = sin( beta );
  double cg = cos( gamma );
  double sg = sin( gamma );

  ( *m_T )( 0, 0 ) = ca * cb;
  ( *m_T )( 0, 1 ) = ca * sb * sg - sa * cg;
  ( *m_T )( 0, 2 ) = ca * sb * cg + sa * sg;
  ( *m_T )( 1, 0 ) = sa * cb;
  ( *m_T )( 1, 1 ) = sa * sb * sg + ca * cg;
  ( *m_T )( 1, 2 ) = sa * sb * cg - ca * sg;
  ( *m_T )( 2, 0 ) = -sb;
  ( *m_T )( 2, 1 ) = cb * sg;
  ( *m_T )( 2, 2 ) = cb * cg;

  *m_T = vnl_matrix_inverse<double>( *m_T ).inverse();
}

/** Get the Derivative Measure */
template < class TFixedImage, class TMovingSpatialObject>
void
ImageToTubeRigidMetric<TFixedImage, TMovingSpatialObject>
::GetDerivative( const ParametersType & parameters,
                 DerivativeType & derivative ) const
{
  derivative = DerivativeType( SpaceDimension );
  SetOffset( parameters[3], parameters[4], parameters[5] );
  this->m_Transform->SetParameters( parameters );

  if( m_Verbose )
    {
    std::cout << "**** Get Derivative ****" << std::endl;
    std::cout <<"parameters = "<< parameters << std::endl;
    }

  vnl_matrix<double> biasV( 3, 3, 0 );
  vnl_matrix<double> biasVI( 3, 3, 0 );

  double opR;

  std::list<double>::const_iterator weightIterator;
  weightIterator = m_Weight.begin();

  unsigned int count = 0;

  double dPosition[3] = { 0, 0, 0 };
  double dAngle[3] = { 0, 0, 0 };
  double dXProj1, dXProj2;

  vnl_vector<double> tV( 3 );
  vnl_matrix<double> tM( 3, 3 );
  vnl_vector<double> v1T( 3 );
  vnl_vector<double> v2T( 3 );
  double angleDelta[3];

  double sumWeight = 0;

  typedef itk::Vector<double, 3>    ITKVectorType;
  typedef std::list<ITKVectorType>  ListType;
  ListType dXTlist;
  itk::FixedArray<Point<double, 3>, 5000> XTlist;

  unsigned int listindex = 0;
  TubeNetType::ChildrenListType* tubeList = GetTubes();
  TubeNetType::ChildrenListType::iterator tubeIterator = tubeList->begin();
  for( ; tubeIterator != tubeList->end(); ++tubeIterator )
    {
    TubeType* currTube = static_cast<TubeType*>(
      ( *tubeIterator ).GetPointer() );

    std::vector<TubePointType>::iterator pointIterator;
    for( pointIterator = currTube->GetPoints().begin();
         pointIterator != currTube->GetPoints().end();
         ++pointIterator )
      {
      InputPointType inputPoint = pointIterator->GetPosition();
      itk::Point<double, 3> point;
      Matrix<double, 3, 3> matrix =  GetTransform()->GetRotationMatrix();

      point =  matrix * inputPoint + GetTransform()->GetOffset();

      vnl_vector<double> rotationOffset = matrix * m_RotationCenter;

      point[0] += m_RotationCenter[0] - rotationOffset[0];
      point[1] += m_RotationCenter[1] - rotationOffset[1];
      point[2] += m_RotationCenter[2] - rotationOffset[2];

      itk::Index<3> index;
      index[0] = static_cast<unsigned int>( point[0] );
      index[1] = static_cast<unsigned int>( point[1] );
      index[2] = static_cast<unsigned int>( point[2] );

      if( this->IsInside( inputPoint ) )
        {
        XTlist[listindex++] = m_CurrentPoint;
        sumWeight += *weightIterator;
        count++;
        opR = pointIterator->GetRadius();
        opR = std::max( opR, 0.5 );

        SetScale( opR * m_Kappa );

        Vector<double, 3> v1;
        Vector<double, 3> v2;
        for( unsigned int i = 0; i < 3; ++i )
          {
          v1[i] = pointIterator->GetNormal1()[i];
          v2[i] = pointIterator->GetNormal2()[i];
          }

        for( unsigned int i = 0; i < 3; ++i )
          {
          v1T[i] = this->m_Transform->TransformVector( v1 )[i];
          v2T[i] = this->m_Transform->TransformVector( v2 )[i];
          }

        v1 = this->m_Transform->TransformVector( v1 );
        v2 = this->m_Transform->TransformVector( v2 );

        dXProj1 = ComputeThirdDerivatives( &v1 );
        dXProj2 = ComputeThirdDerivatives( &v2 );

        Vector<double, 3> dXT;

        for( unsigned int i = 0; i < 3; ++i )
          {
          dXT[i] = ( dXProj1 * v1[i] + dXProj2 * v2[i] );
          }

        tM = outer_product( v1T, v1T );
        tM = tM + outer_product( v2T, v2T );

        tM = *weightIterator * tM;

        biasV += tM;

        dPosition[0] += *weightIterator * ( dXT[0] );
        dPosition[1] += *weightIterator * ( dXT[1] );
        dPosition[2] += *weightIterator * ( dXT[2] );

        dXTlist.push_back( dXT );
        }
      weightIterator++;
      }
    }

  biasVI = vnl_matrix_inverse<double>( biasV ).inverse();

  tV( 0 ) = dPosition[0];
  tV( 1 ) = dPosition[1];
  tV( 2 ) = dPosition[2];

  tV *= biasVI;

  dPosition[0] = tV( 0 );
  dPosition[1] = tV( 1 );
  dPosition[2] = tV( 2 );

  if( sumWeight == 0 )
    {
    biasV = 0;
    dAngle[0] = dAngle[1] = dAngle[2] = 0;
    derivative.fill(0);

    if( m_Verbose )
      {
      std::cout << "GetDerivative : sumWeight == 0 !" << std::endl;
      }
    return;
    }
  else
    {
    biasV = 1.0 / sumWeight * biasV;
    }

  biasVI = vnl_matrix_inverse<double>( biasV ).inverse();

  weightIterator = m_Weight.begin();
  ListType::iterator  dXTIterator = dXTlist.begin();

  listindex  = 0;

  this->SetOffset( dPosition[0], dPosition[1], dPosition[2] );

  while( dXTIterator != dXTlist.end() )
    {
    vnl_vector<double> dXT( 3 );
    dXT[0] = ( *dXTIterator )[0];
    dXT[1] = ( *dXTIterator )[1];
    dXT[2] = ( *dXTIterator )[2];

    dXT = dXT * biasVI;
    const Point<double, 3> & xT = XTlist[listindex++];

    GetDeltaAngles( xT, dXT, angleDelta );
    dAngle[0] += *weightIterator * angleDelta[0];
    dAngle[1] += *weightIterator * angleDelta[1];
    dAngle[2] += *weightIterator * angleDelta[2];
    weightIterator++;
    dXTIterator++;
    }

  dAngle[0] /= sumWeight * dXTlist.size();
  dAngle[1] /= sumWeight * dXTlist.size();
  dAngle[2] /= sumWeight * dXTlist.size();

  if( m_Verbose )
    {
    std::cout << "dA = " << dAngle[0] << std::endl;
    std::cout << "dB = " << dAngle[1] << std::endl;
    std::cout << "dG = " << dAngle[2] << std::endl;
    std::cout << "dX = " << dPosition[0] << std::endl;
    std::cout << "dY = " << dPosition[1] << std::endl;
    std::cout << "dZ = " << dPosition[2] << std::endl;
    }

  if( m_Iteration > 0 )
    {
    derivative[0] = dAngle[0];
    derivative[1] = dAngle[1];
    derivative[2] = dAngle[2];
    derivative[3] = dPosition[0];
    derivative[4] = dPosition[1];
    derivative[5] = dPosition[2];
    }

  delete tubeList;
}

/** Get both the match Measure and theDerivative Measure */
template < class TFixedImage, class TMovingSpatialObject>
void
ImageToTubeRigidMetric<TFixedImage, TMovingSpatialObject>
::GetValueAndDerivative( const ParametersType & parameters,
                         MeasureType&  Value,
                         DerivativeType  & Derivative ) const
{
  Value = GetValue( parameters );
  GetDerivative( parameters, Derivative );
}

template < class TFixedImage, class TMovingSpatialObject>
double
ImageToTubeRigidMetric<TFixedImage, TMovingSpatialObject>
::ComputeThirdDerivatives( Vector<double, 3> *v ) const
{
  // We convolve the 1D signal defined by the direction v at point
  // m_CurrentPoint with a second derivative of a gaussian
  double result = 0;
  double wI = 0;
  itk::Index<3> index;

  double wTotalX = 0;
  const double scaleSquared = std::pow( m_Scale, 2 );
  const double scaleExtentProduct = m_Scale * m_Extent;

  for( double dist = -scaleExtentProduct;
       dist <= scaleExtentProduct;
       dist += 0.1 )
    {
    wI = 2 * dist * exp( -0.5 * std::pow( dist, 2 ) / scaleSquared );

    wTotalX += fabs( wI );

    index[0] =
      static_cast<unsigned int>( m_CurrentPoint[0] + dist * v->GetElement(0) );
    index[1] =
      static_cast<unsigned int>( m_CurrentPoint[1] + dist * v->GetElement(1) );
    index[2] =
      static_cast<unsigned int>( m_CurrentPoint[2] + dist * v->GetElement(2) );

    itk::Size<3> size =
      this->m_FixedImage->GetLargestPossibleRegion().GetSize();
    if( index[0] >= 0 && ( index[0] < static_cast<unsigned int>( size[0] ) )
      && index[1] >= 0 && ( index[1] < static_cast<unsigned int>( size[1] ) )
      && index[2] >= 0 && ( index[2] < static_cast<unsigned int>( size[2] ) ) )
      {
      result += this->m_FixedImage->GetPixel( index ) * wI;
      }
    }

  return result / wTotalX;
}

/** Compute third derivatives at a point */
template < class TFixedImage, class TMovingSpatialObject>
typename ImageToTubeRigidMetric<TFixedImage,
  TMovingSpatialObject>::VectorType *
ImageToTubeRigidMetric<TFixedImage, TMovingSpatialObject>
::ComputeThirdDerivatives( void ) const
{
  vnl_vector<double>* derivatives  = new  vnl_vector<double>( 3 );
  itk::Index<3> index;
  PixelType value;
  double distances[3];
  double squaredDistance;
  double squaredDistances[3];
  double weights[3];
  double weightSum[3];
  double wI;
  double wTotalI = 0;
  double result[3] = { 0, 0, 0 };
  double const gfact = -0.5 / ( std::pow( m_Scale, 2 ) );

  double currentPointBounds[6];
  this->GetCurrentPointBounds( currentPointBounds );
  this->ClampPointBoundsToImage( currentPointBounds );
  for( int i = currentPointBounds[4]; i <= currentPointBounds[5]; ++i )
    {
    distances[2] = ( i - m_CurrentPoint[2] );
    squaredDistances[2] = std::pow( distances[2], 2 );
    for( int j = currentPointBounds[2]; j <= currentPointBounds[3]; ++j )
      {
      distances[1] = ( j - m_CurrentPoint[1] );
      squaredDistances[1] = std::pow( distances[1], 2 );
      for( int k = currentPointBounds[0]; k <= currentPointBounds[1]; ++k )
        {
        distances[0] = ( k - m_CurrentPoint[0] );
        squaredDistances[0] = std::pow( distances[0], 2 );
        squaredDistance = squaredDistances[0] +
                          squaredDistances[1] +
                          squaredDistances[2];

        wI = exp( gfact * ( squaredDistance ) );

        // Check what's are this constants ?!
        weights[0] = 4 * distances[0] * squaredDistances[0] * wI;
        weights[1] = 4 * distances[1] * squaredDistances[1] * wI;
        weights[2] = 4 * distances[2] * squaredDistances[2] * wI;

        wTotalI += wI;
        weightSum[0] += fabs( weights[0] );
        weightSum[1] += fabs( weights[1] );
        weightSum[2] += fabs( weights[2] );

        index[0]=k; index[1]=j; index[2]=i;
        value = this->m_FixedImage->GetPixel( index );
        result[0] += value * weights[0];
        result[1] += value * weights[1];
        result[2] += value * weights[2];
        }
      }
    }

  ( *derivatives )( 0 ) = ( weightSum[0] != 0 ) ? result[0] / weightSum[0] : 0;
  ( *derivatives )( 1 ) = ( weightSum[1] != 0 ) ? result[1] / weightSum[1] : 0;
  ( *derivatives )( 2 ) = ( weightSum[2] != 0 ) ? result[2] / weightSum[2] : 0;

  if( wTotalI == 0 )
    {
    derivatives->fill( 0 );
    }

  return derivatives;
}

/** Evaluate all derivatives at a point */
template < class TFixedImage, class TMovingSpatialObject>
typename ImageToTubeRigidMetric<TFixedImage,TMovingSpatialObject>::VectorType*
ImageToTubeRigidMetric<TFixedImage, TMovingSpatialObject>
::EvaluateAllDerivatives( void ) const
{
  vnl_vector<double>* derivatives  = new  vnl_vector<double>( 3 );
  itk::Index<3> index;
  PixelType value;
  double distances[3];
  double squaredDistance;
  double squaredDistances[3];
  double weights[3];
  double weightSum[3];
  double wI;
  double wTotalI = 0;
  double resI = 0;
  double result[3] = { 0, 0, 0 };
  double const gfact = -0.5 / ( std::pow( m_Scale, 2 ) );

  double currentPointBounds[6];
  this->GetCurrentPointBounds( currentPointBounds );
  this->ClampPointBoundsToImage( currentPointBounds );
  for( int i = currentPointBounds[4]; i <= currentPointBounds[5]; ++i )
    {
    distances[2] = ( i - m_CurrentPoint[2] );
    squaredDistances[2] = std::pow( distances[2], 2 );
    for( int j = currentPointBounds[2]; j <= currentPointBounds[3]; ++j )
      {
      distances[1] = ( j - m_CurrentPoint[1] );
      squaredDistances[1] = std::pow( distances[1], 2 );
      for( int k = currentPointBounds[0]; k <= currentPointBounds[1]; ++k )
        {
        distances[0] = ( k - m_CurrentPoint[0] );
        squaredDistances[0] = std::pow( distances[0], 2 );
        squaredDistance = squaredDistances[0] +
                          squaredDistances[1] +
                          squaredDistances[2];

        wI = exp( gfact * ( squaredDistance ) );

        weights[0] = 2 * distances[0] * wI;
        weights[1] = 2 * distances[1] * wI;
        weights[2] = 2 * distances[2] * wI;

        wTotalI += wI;
        weightSum[0] += fabs( weights[0] );
        weightSum[1] += fabs( weights[1] );
        weightSum[2] += fabs( weights[2] );

        resI += value * wI;
        index[0]=k; index[1]=j; index[2]=i;
        value = this->m_FixedImage->GetPixel( index );
        result[0] += value * weights[0];
        result[1] += value * weights[1];
        result[2] += value * weights[2];
        }
      }
    }

  ( *derivatives )( 0 ) = ( weightSum[0] != 0 ) ? result[0] / weightSum[0] : 0;
  ( *derivatives )( 1 ) = ( weightSum[1] != 0 ) ? result[1] / weightSum[1] : 0;
  ( *derivatives )( 2 ) = ( weightSum[2] != 0 ) ? result[2] / weightSum[2] : 0;

  if( wTotalI == 0 )
    {
    derivatives->fill( 0 );
    m_BlurredValue = 0;
    }
  else
    {
    m_BlurredValue = resI / wTotalI;
    }

  return derivatives;
}

/** Test whether the specified point is inside
 * Thsi method overload the one in the ImageMapper class
 * \warning This method cannot be safely used in more than one thread at
 * a time.
 * \sa Evaluate(); */
template < class TFixedImage, class TMovingSpatialObject>
bool
ImageToTubeRigidMetric<TFixedImage, TMovingSpatialObject>
::IsInside( const InputPointType & point ) const
{
  Matrix<double, 3, 3> matrix =  GetTransform()->GetRotationMatrix();
  m_CurrentPoint =  matrix * point + GetTransform()->GetOffset();

  Vector<double, 3>  m_CenterOfRotation;
  for( unsigned int i = 0; i < 3; i++ )
    {
    m_CenterOfRotation[i]= m_RotationCenter[i];
    }

  itk::Vector<double, 3> rotationOffset = matrix * m_CenterOfRotation;

  m_CurrentPoint[0] += m_CenterOfRotation[0] - rotationOffset[0];
  m_CurrentPoint[1] += m_CenterOfRotation[1] - rotationOffset[1];
  m_CurrentPoint[2] += m_CenterOfRotation[2] - rotationOffset[2];

  return ( this->m_Interpolator->IsInsideBuffer( m_CurrentPoint ) );
}

/** Compute the bounds at a given point */
template < class TFixedImage, class TMovingSpatialObject>
void
ImageToTubeRigidMetric<TFixedImage, TMovingSpatialObject>
::GetPointBounds(PointType point, int bounds[6])
{
  double const scaleExtentProd = m_Scale * m_Extent;

  bounds[0] = static_cast<int>( floor( point[0] - scaleExtentProd )
                                / m_Factors[0] );
  bounds[1] = static_cast<int>( ceil( point[0] + scaleExtentProd )
                                / m_Factors[0] );
  bounds[2] = static_cast<int>( floor( point[1] - scaleExtentProd )
                                / m_Factors[1] );
  bounds[3] = static_cast<int>( ceil( point[1] + scaleExtentProd )
                                / m_Factors[1] );
  bounds[4] = static_cast<int>( floor( point[2] - scaleExtentProd )
                                / m_Factors[2] );
  bounds[5] = static_cast<int>( ceil( point[2] + scaleExtentProd )
                                / m_Factors[2] );
}

/** Compute the bounds at the currently processed point */
template < class TFixedImage, class TMovingSpatialObject>
void
ImageToTubeRigidMetric<TFixedImage, TMovingSpatialObject>
::GetCurrentPointBounds(int bounds[6])
{
  this->GetPointBounds( m_CurrentPoint, bounds );
}

/** CLamp the point bounds to the fixed Image */
template < class TFixedImage, class TMovingSpatialObject>
void
ImageToTubeRigidMetric<TFixedImage, TMovingSpatialObject>
::ClampPointBoundsToImage(int bounds[6])
{
  SizeType size = this->m_FixedImage->GetLargestPossibleRegion().GetSize();

  bounds[0] = vnl_math_max( bounds[0], 0 );
  bounds[1] = vnl_math_min( bounds[1], static_cast<int>( size[0] ) - 1 );
  bounds[2] = vnl_math_max( bounds[2], 0 );
  bounds[3] = vnl_math_min( bounds[3], static_cast<int>( size[1] ) - 1 );
  bounds[4] = vnl_math_max( bounds[4], 0 );
  bounds[5] = vnl_math_min( bounds[5], static_cast<int>( size[2] ) - 1 );
}

} // end namespace itk

#endif

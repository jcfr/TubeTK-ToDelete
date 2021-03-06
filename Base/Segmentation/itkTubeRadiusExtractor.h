/*=========================================================================

Library:   TubeTK/VTree3D

Authors: Stephen Aylward, Julien Jomier, and Elizabeth Bullitt

Original implementation:
Copyright University of North Carolina, Chapel Hill, NC, USA.

Revised implementation:
Copyright Kitware Inc., Carrboro, NC, USA.

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
#ifndef __itkTubeRadiusExtractor_h
#define __itkTubeRadiusExtractor_h

#include <vector>

#include <vnl/vnl_vector.h>
#include <itkVesselTubeSpatialObject.h>

#include "itkTubeBlurImageFunction.h"

#include "tubeOptBrent1D.h"
#include "tubeOptParabolicFit1D.h"
#include "tubeSplineApproximation1D.h"

namespace itk
{

namespace tube
{

/**
 * This class extract the radius of a tube given an image
 *
 * /sa itkTubeRidgeExtractor
 */

template <class TInputImage>
class ITK_EXPORT RadiusExtractor : public Object
{
public:

  /**
   * Standard self typedef */
  typedef RadiusExtractor                                    Self;
  typedef Object                                             Superclass;
  typedef SmartPointer<Self>                                 Pointer;
  typedef SmartPointer<const Self>                           ConstPointer;

  typedef ::tube::OptParabolicFit1D                          OptimizerType;
  typedef ::tube::SplineApproximation1D                      SplineType;

  itkTypeMacro( RadiusExtractor, Object );
  itkNewMacro( RadiusExtractor );

  /**
   * Standard for the number of dimension
   */
  itkStaticConstMacro( ImageDimension, unsigned int,
    ::itk::GetImageDimension< TInputImage>::ImageDimension );

  typedef VesselTubeSpatialObject< TInputImage::ImageDimension >
                                                             TubeType;
  typedef typename TubeType::TubePointType                   TubePointType;

  typedef typename TubeType::PointType                       ITKPointType;
  typedef typename TubeType::VectorType                      ITKVectorType;

  /**
   * Kernel is a vector of points that sparsely represent a tube
   */
  typedef typename std::vector< TubePointType > KernArrayType;

  typedef typename std::vector< unsigned int >  KernArrayTubePointIndexType;

  /**
   * Type definition for the input image. */
  typedef TInputImage                                        ImageType;

  /**
   * Type definition for the input image pixel type. */
  typedef typename TInputImage::PixelType                    PixelType;

  /**
   * Defines the type of vectors used
   */
  typedef vnl_vector< double >                               VectorType;

  /**
   * Defines the type of matrix used
   */
  typedef vnl_matrix< double >                               MatrixType;

  /**
   * Set the input image */
  void SetInputImage( typename ImageType::Pointer inputImage );

  /**
   * Get the input image */
  itkGetConstObjectMacro( Image, ImageType );

  /*
   * Set Data Minimum */
  itkSetMacro( DataMin, double );

  /**
   * Get Data Minimum */
  itkGetMacro( DataMin, double );

  /*
   * Set Data Maximum */
  itkSetMacro( DataMax, double );

  /**
   * Get Data Maximum */
  itkGetMacro( DataMax, double );

  /**
   * Set Minimum Radius */
  void SetRadiusMin( double radiusMin );

  /**
   * Get Minimum Radius */
  itkGetMacro( RadiusMin, double );

  /**
   * Set Maximum Radius */
  void SetRadiusMax( double radiusMax );

  /**
   * Get Maximum Radius */
  itkGetMacro( RadiusMax, double );

  /**
   * Set Radius0 */
  void SetRadius0( double radius0 );

  /**
   * Get Radius0 */
  itkGetMacro( Radius0, double );

  /**
   * Set ThreshMedialness */
  itkSetMacro( ThreshMedialness, double );

  /**
   * Get ThreshMedialness */
  itkGetMacro( ThreshMedialness, double );

  /**
   * Set ThreshMedialness Start */
  itkSetMacro( ThreshMedialnessStart, double );

  /**
   * Get ThreshMedialness Start */
  itkGetMacro( ThreshMedialnessStart, double );

  /**
   * Set Extract Bright Tube (versus a Dark Tube) */
  itkSetMacro( ExtractBrightTube, bool );

  /**
   * Get Extract Bright Tube */
  itkGetMacro( ExtractBrightTube, bool );

  /**
   * Return the optimizer */
  OptimizerType & GetMedialnessOptimizer( void );

  /**
   * Return the optimizer */
  SplineType & GetMedialnessOptimizerSpline( void );

  /**
   *
   */
  void MeasuresAtPoint( TubePointType & pnt, double pntR,
    double & mness, double & bness, bool doBNess );

  /**
   * Compute medialness at a kernel array */
  void MeasuresInKernelArray( KernArrayType & kernArray,
    double pntR, double & mness, double & bness, bool doBNess );

  /**
   * Calculate the optimal scale
   */
  bool OptimalRadiusAtPoint( TubePointType & pnt, double & r0,
    double rMin, double rMax, double rStep, double rTolerance );

  /**
   * Calculate Radii
   */
  bool ExtractRadii( TubeType * tube );

  void SetIdleCallBack( bool ( *idleCallBack )() );
  void SetStatusCallBack( void ( *statusCallBack )( const char *,
      const char *, int ) );

protected:

  RadiusExtractor();
  virtual ~RadiusExtractor();
  RadiusExtractor( const Self& ) {}
  void operator=( const Self& ) {}

  void PrintSelf( std::ostream & os, Indent indent ) const;

  void ValuesInSubKernel( TubePointType pnt, double pntR,
    MatrixType & kernN, VectorType & kern, double & kernCnt );

  void ValuesInKernel( TubePointType pnt, double pntR,
    MatrixType & kernN, VectorType & kernPos, VectorType & kernNeg,
    VectorType & kernBrn, bool doBNess );

  void ValuesInFullKernelArray( TubeType * tube,
    KernArrayType & kernArray,
    KernArrayTubePointIndexType & kernArrayTubePointIndex );

  /**
   * Compute medialness at a kernel */
  void MeasuresInKernel( double pntR,
    VectorType & kernPos, VectorType & kernNeg, VectorType & kernBrn,
    double & mness, double & bness, bool doBNess );

  /**
   * Calculate Radii one way */
  void MeasuresInFullKernelArray( KernArrayType & kernArray,
    unsigned int kernPntStart, unsigned int KernPntEnd );

  void SmoothMeasuresInFullKernelArray( KernArrayType & kernArray );

  void ApplyMeasuresInFullKernelArray( TubeType * tube,
    KernArrayType & kernArray,
    KernArrayTubePointIndexType & kernArrayTubePointIndex );


private:

  typename ImageType::Pointer             m_Image;
  typename ImageType::IndexType           m_ImageXMin;
  typename ImageType::IndexType           m_ImageXMax;

  typename BlurImageFunction<ImageType>::Pointer  m_DataOp;
  double                                          m_DataMin;
  double                                          m_DataMax;

  double                                  m_MedialnessScaleStep;
  OptimizerType                           m_MedialnessOpt;
  SplineType                            * m_MedialnessOptSpline;

  unsigned int                            m_NumKernelPoints;
  unsigned int                            m_KernelPointSpacing;

  /** Determine if the algorithm extracts ridge or a valley */
  bool                                    m_ExtractBrightTube;

  double                                  m_Radius0;
  double                                  m_RadiusMin;
  double                                  m_RadiusMax;

  double                                  m_ThreshMedialness;
  double                                  m_ThreshMedialnessStart;

  ::tube::UserFunc<int, double> *         m_MedialnessFunc;

  unsigned int                            m_KernNumDirs;
  MatrixType                              m_KernX;

  bool ( *m_IdleCallBack )();
  void ( *m_StatusCallBack )( const char *, const char *, int );

};

} // end namespace tube

} // end namespace itk


#ifndef ITK_MANUAL_INSTANTIATION
#include "itkTubeRadiusExtractor.txx"
#endif

#endif /* __itkTubeRadiusExtractor_h */

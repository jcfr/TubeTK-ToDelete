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

#ifdef __BORLANDC__
#define ITK_LEAN_AND_MEAN
#endif

#include <iostream>
#include <fstream>

#include "itkImage.h"
#include "itkImageFileReader.h"

// TubeTK Filters
#include "itkTubeLabelOverlapMeasuresImageFilter.h"

// Must do a forward declaraction of DoIt before including
// tubeCLIHelperFunctions
template< class pixelT, unsigned int dimensionT >
int DoIt( int argc, char * argv[] );

// Must include CLP before including tubeCLIHleperFunctions
#include "tubeComputeBinaryImageSimilarityCLP.h"

// Includes tube::ParseArgsAndCallDoIt function
#include "tubeCLIHelperFunctions.h"

template< class pixelT, unsigned int dimensionT >
int DoIt( int argc, char * argv[] )
{
  PARSE_ARGS;

  typedef short                                               PixelType;
  typedef itk::Image< PixelType,  dimensionT >                ImageType;
  typedef itk::ImageFileReader< ImageType >                   ReaderType;

  typename ReaderType::Pointer reader1 = ReaderType::New();
  typename ReaderType::Pointer reader2 = ReaderType::New();

  //read input image
  reader1->SetFileName( inputVolume1.c_str() );
  reader2->SetFileName( inputVolume2.c_str() );

  try
    {
    reader1->Update();
    }
  catch( itk::ExceptionObject & err )
    {
    std::cerr << "ExceptionObject caught, image reader 1 !" << std::endl;
    std::cerr << err << std::endl;
    return EXIT_FAILURE;
    }

  try
    {
    reader2->Update();
    }
  catch( itk::ExceptionObject & err )
    {
    std::cerr << "ExceptionObject caught, image reader2 !" << std::endl;
    std::cerr << err << std::endl;
    return EXIT_FAILURE;
    }

  typename ImageType::Pointer image1 = reader1->GetOutput();
  typename ImageType::Pointer image2 = reader2->GetOutput();

  typedef itk::tube::LabelOverlapMeasuresImageFilter< ImageType >
    MetricFilterType;

  typename MetricFilterType::Pointer metric = MetricFilterType::New();
  metric->SetSourceImage( image1 );
  metric->SetTargetImage( image2 );
  metric->Update();

  if( resultsFile.size() == 0 )
    {
    std::cout << "Total Overlap = " << metric->GetTotalOverlap()
      << std::endl;
    std::cout << "Union Overlap (Jaccard Coefficient) = "
      << metric->GetUnionOverlap() << std::endl;
    std::cout << "Mean Overlap (Dice Coefficient) = "
      << metric->GetMeanOverlap() << std::endl;
    std::cout << "Volume Similarity = " << metric->GetVolumeSimilarity()
      << std::endl;
    std::cout << "False Negative Error = " << metric->GetFalseNegativeError()
      << std::endl;
    std::cout << "False Positive Error = " << metric->GetFalsePositiveError()
      << std::endl;
    }
  else
    {
    std::ofstream outFile;
    outFile.open( resultsFile.c_str() );
    outFile << "Total Overlap = " << metric->GetTotalOverlap()
      << std::endl;
    outFile << "Union Overlap (Jaccard Coefficient) = "
      << metric->GetUnionOverlap() << std::endl;
    outFile << "Mean Overlap (Dice Coefficient) = "
      << metric->GetMeanOverlap() << std::endl;
    outFile << "Volume Similarity = " << metric->GetVolumeSimilarity()
      << std::endl;
    outFile << "False Negative Error = " << metric->GetFalseNegativeError()
      << std::endl;
    outFile << "False Positive Error = " << metric->GetFalsePositiveError()
      << std::endl;
    outFile.close();
    }


  return EXIT_SUCCESS;
}

int main( int argc, char **argv )
{
  PARSE_ARGS;

  return tube::ParseArgsAndCallDoIt( inputVolume1, argc, argv );
}

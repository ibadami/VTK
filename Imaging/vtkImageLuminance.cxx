/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkImageLuminance.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 1993-2002 Ken Martin, Will Schroeder, Bill Lorensen 
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkImageLuminance.h"
#include "vtkObjectFactory.h"
#include "vtkImageProgressIterator.h"

#include <math.h>

vtkCxxRevisionMacro(vtkImageLuminance, "1.19");
vtkStandardNewMacro(vtkImageLuminance);

//----------------------------------------------------------------------------
// This method overrides information set by parent's ExecuteInformation.
void vtkImageLuminance::ExecuteInformation(vtkImageData *vtkNotUsed(inData), 
                                           vtkImageData *outData)
{
  outData->SetNumberOfScalarComponents(1);
}

//----------------------------------------------------------------------------
// This execute method handles boundaries.
// it handles boundaries. Pixels are just replicated to get values 
// out of extent.
template <class T>
static void vtkImageLuminanceExecute(vtkImageLuminance *self,
                                             vtkImageData *inData,
                                             vtkImageData *outData,
                                             int outExt[6], int id, T *)
{
  vtkImageIterator<T> inIt(inData, outExt);
  vtkImageProgressIterator<T> outIt(outData, outExt, self, id);
  float luminance;

  // Loop through ouput pixels
  while (!outIt.IsAtEnd())
    {
    T* inSI = inIt.BeginSpan();
    T* outSI = outIt.BeginSpan();
    T* outSIEnd = outIt.EndSpan();
    while (outSI != outSIEnd)
      {
      // now process the components
      luminance =  0.30 * *inSI++;
      luminance += 0.59 * *inSI++;
      luminance += 0.11 * *inSI++;
      *outSI = static_cast<T>(luminance);
      ++outSI;
      }
    inIt.NextSpan();
    outIt.NextSpan();
    }
}


//----------------------------------------------------------------------------
// This method contains a switch statement that calls the correct
// templated function for the input data type.  The output data
// must match input type.  This method does handle boundary conditions.
void vtkImageLuminance::ThreadedExecute(vtkImageData *inData, 
                                        vtkImageData *outData,
                                        int outExt[6], int id)
{
  vtkDebugMacro(<< "Execute: inData = " << inData 
  << ", outData = " << outData);
  
  // this filter expects that input is the same type as output.
  if (inData->GetNumberOfScalarComponents() != 3)
    {
    vtkErrorMacro(<< "Execute: input must have 3 components, but has " << inData->GetNumberOfScalarComponents());
    return;
    }
  
  // this filter expects that input is the same type as output.
  if (inData->GetScalarType() != outData->GetScalarType())
    {
    vtkErrorMacro(<< "Execute: input ScalarType, " << inData->GetScalarType()
    << ", must match out ScalarType " << outData->GetScalarType());
    return;
    }
  
  switch (inData->GetScalarType())
    {
    vtkTemplateMacro6(vtkImageLuminanceExecute, this, inData, outData, 
                      outExt, id, static_cast<VTK_TT *>(0));
    default:
      vtkErrorMacro(<< "Execute: Unknown ScalarType");
      return;
    }
}











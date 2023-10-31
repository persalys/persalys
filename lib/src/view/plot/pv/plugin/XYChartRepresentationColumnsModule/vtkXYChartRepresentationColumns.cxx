/*=========================================================================

  Program:   ParaView
  Module:    vtkXYChartRepresentationColumns.cxx

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkXYChartRepresentationColumns.h"

#include <vtkObjectFactory.h>
#include <vtkChartXY.h>

vtkStandardNewMacro(vtkXYChartRepresentationColumns)

//----------------------------------------------------------------------------
void vtkXYChartRepresentationColumns::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
void vtkXYChartRepresentationColumns::PrepareForRendering()
{
  this->Superclass::PrepareForRendering();
  vtkChartXY* chartXY = this->GetChart();
  chartXY->SetSelectionMethod(vtkChart::SELECTION_COLUMNS);
}

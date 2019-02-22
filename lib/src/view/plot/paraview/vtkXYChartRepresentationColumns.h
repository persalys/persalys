/*=========================================================================

  Program:   ParaView
  Module:    vtkXYChartRepresentationColumns.h

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class   vtkXYChartRepresentationColumns
 *
 * vtkXYChartRepresentationColumns is a specialisation of vtkXYChartRepresentation
 * that supports column selection.
 */

#ifndef vtkXYChartRepresentationColumns_h
#define vtkXYChartRepresentationColumns_h

#include <vtkXYChartRepresentation.h>

class VTK_EXPORT vtkXYChartRepresentationColumns : public vtkXYChartRepresentation
{
public:
  static vtkXYChartRepresentationColumns* New();
  vtkTypeMacro(vtkXYChartRepresentationColumns, vtkXYChartRepresentation);
  void PrintSelf(ostream& os, vtkIndent indent) override;

protected:
  vtkXYChartRepresentationColumns() = default;
  ~vtkXYChartRepresentationColumns() override = default;

  void PrepareForRendering() override;

private:
  vtkXYChartRepresentationColumns(const vtkXYChartRepresentationColumns&) = delete;
  void operator=(const vtkXYChartRepresentationColumns&) = delete;
};

#endif // vtkXYChartRepresentationColumns_h

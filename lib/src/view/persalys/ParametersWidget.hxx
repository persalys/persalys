//                                               -*- C++ -*-
/**
 *  @brief QWidget for Parameters tab of results windows
 *
 *  Copyright 2015-2023 EDF-Phimeca
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef PERSALYS_PARAMETERSWIDGET_HXX
#define PERSALYS_PARAMETERSWIDGET_HXX

#include "persalys/BaseTools.hxx"

#include <QGroupBox>

namespace PERSALYS
{
class PERSALYS_VIEW_API ParametersWidget : public QGroupBox
{
public:
  ParametersWidget(const QString title,                 // table title
                   const QStringList names,             // parameters names
                   const QStringList values,            // parameters values
                   const bool showGrid = false,         // show the grid of the table
                   const bool namesHasHeaderType = false, // parameters names display has table header
                   QWidget *parent = nullptr
                  );
  ParametersWidget(const QString title,                  // table title
                   const Parameters& analysisParameters, // parameters names and values
                   QWidget *parent = nullptr
                  );
};
}
#endif

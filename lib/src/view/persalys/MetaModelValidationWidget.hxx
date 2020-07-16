//                                               -*- C++ -*-
/**
 *  @brief QWidget for meta model result
 *
 *  Copyright 2015-2020 EDF-Phimeca
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
#ifndef PERSALYS_METAMODELVALIDATIONWIDGET_HXX
#define PERSALYS_METAMODELVALIDATIONWIDGET_HXX

#include "persalys/MetaModelValidationResult.hxx"

#include <openturns/Sample.hxx>

#include <QScrollArea>

namespace PERSALYS
{
class PERSALYS_VIEW_API MetaModelValidationWidget : public QScrollArea
{
  Q_OBJECT

public:
  MetaModelValidationWidget(const MetaModelValidationResult& result,
                            const OT::Sample& outputSample,
                            const OT::UnsignedInteger indexOutput,
                            const QString measure = "",
                            QWidget * parent = 0);
};
}
#endif

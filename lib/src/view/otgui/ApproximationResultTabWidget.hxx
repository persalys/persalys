//                                               -*- C++ -*-
/**
 *  @brief QTabWidget for the FORM results
 *
 *  Copyright 2015-2017 EDF-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef OTGUI_APPROXIMATIONRESULTTABWIDGET_HXX
#define OTGUI_APPROXIMATIONRESULTTABWIDGET_HXX

#include "otgui/ApproximationAnalysis.hxx"

#include <openturns/FORMResult.hxx>

#include <QTabWidget>

namespace OTGUI {
class OTGUI_API ApproximationResultTabWidget : public QTabWidget
{
  Q_OBJECT

public:
  ApproximationResultTabWidget(const OT::FORMResult& result,
                               const ApproximationAnalysis& analysis,
                               QWidget* parent = 0);

protected:
  void buildInterface();

private:
  OT::FORMResult result_;
  QWidget * parametersWidget_;
};
}
#endif
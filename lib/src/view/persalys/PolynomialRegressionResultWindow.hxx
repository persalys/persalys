//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow for the results of linear regression
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
#ifndef PERSALYS_POLYNOMIALREGRESSIONRESULTWINDOW_HXX
#define PERSALYS_POLYNOMIALREGRESSIONRESULTWINDOW_HXX

#include "persalys/ResultWindow.hxx"
#include "persalys/PolynomialRegressionAnalysisResult.hxx"

namespace PERSALYS
{
class PERSALYS_VIEW_API PolynomialRegressionResultWindow : public ResultWindow
{
  Q_OBJECT

public:
  PolynomialRegressionResultWindow(AnalysisItem* item, QWidget *parent = nullptr);

protected:
  void buildInterface();

private:
  PolynomialRegressionAnalysisResult result_;
  QString errorMessage_;
};
}
#endif

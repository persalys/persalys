//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow for the results of InferenceAnalysis
 *
 *  Copyright 2015-2018 EDF-Phimeca
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
#ifndef OTGUI_INFERENCERESULTWINDOW_HXX
#define OTGUI_INFERENCERESULTWINDOW_HXX

#include "otgui/ResultWindow.hxx"
#include "otgui/InferenceResult.hxx"
#include "otgui/InferenceResultWidget.hxx"

namespace OTGUI
{
class OTGUI_API InferenceResultWindow : public ResultWindow
{
  Q_OBJECT

public:
  InferenceResultWindow(AnalysisItem * item, QWidget * parent = 0);

protected:
  void buildInterface();

public slots:
  void updateInferenceResultWidget(QString);

private:
  InferenceResult result_;
  InferenceResultWidget * inferenceResultWidget_;
};
}
#endif

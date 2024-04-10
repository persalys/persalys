//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow for the results of InferenceAnalysis
 *
 *  Copyright 2015-2024 EDF-Phimeca
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
#ifndef PERSALYS_INFERENCERESULTWINDOW_HXX
#define PERSALYS_INFERENCERESULTWINDOW_HXX

#include "persalys/ResultWindow.hxx"
#include "persalys/InferenceResult.hxx"
#include "persalys/InferenceResultWidget.hxx"

namespace PERSALYS
{
class PERSALYS_VIEW_API InferenceResultWindow : public ResultWindow
{
  Q_OBJECT

public:
  InferenceResultWindow(AnalysisItem * item, QWidget *parent = nullptr);

protected:
  void buildInterface();

public slots:
  void updateInferenceResultWidget(QString);

private:
  double level_;
  InferenceResult result_;
  InferenceResultWidget * inferenceResultWidget_;
};
}
#endif

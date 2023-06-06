//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow for the results of an evaluation of the model
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
#ifndef PERSALYS_MODELEVALUATIONRESULTWINDOW_HXX
#define PERSALYS_MODELEVALUATIONRESULTWINDOW_HXX

#include "ResultWindow.hxx"
#include "persalys/AnalysisItem.hxx"
#include "persalys/DesignOfExperiment.hxx"

namespace PERSALYS
{
class PERSALYS_VIEW_API ModelEvaluationResultWindow : public ResultWindow
{
  Q_OBJECT

public:
  ModelEvaluationResultWindow(AnalysisItem * item, QWidget *parent = nullptr);

protected:
  virtual void setParameters(const Analysis& /*analysis*/, const QString& /*title*/)
  {
    /*has not parameters*/
  };
  void buildInterface();

private:
  DesignOfExperiment designOfExperiment_;
  OT::Scalar elapsedTime_;
};
}
#endif

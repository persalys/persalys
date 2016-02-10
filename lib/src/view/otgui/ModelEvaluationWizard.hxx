//                                               -*- C++ -*-
/**
 *  @brief QWizard to define an evaluation of the model
 *
 *  Copyright 2015-2016 EDF
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
#ifndef OTGUI_MODELEVALUATIONWIZARD_HXX
#define OTGUI_MODELEVALUATIONWIZARD_HXX

#include "otgui/InputTableModel.hxx"
#include "otgui/OTStudy.hxx"
#include "otgui/OTguiWizard.hxx"

#include <QWizard>
#include <QTableWidget>

namespace OTGUI {
class ModelEvaluationWizard : public OTguiWizard
{
  Q_OBJECT

public:
  ModelEvaluationWizard(OTStudy * otStudy, const PhysicalModel & physicalModel);
  ModelEvaluationWizard(const Analysis & analysis);

  void validate();

protected:
  void buildInterface();

public slots:
  void inputValueChanged(double value);

private:
  QTableWidget * table_;
  Analysis analysis_;
  OTStudy * otStudy_;
};
}
#endif
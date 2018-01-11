//                                               -*- C++ -*-
/**
 *  @brief QWizard to define a copula
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
#ifndef OTGUI_COPULADEFINITIONWIZARD_HXX
#define OTGUI_COPULADEFINITIONWIZARD_HXX

#include "otgui/OTguiWizard.hxx"

#include <QLineEdit>
#include <QButtonGroup>
#include <QLabel>

namespace OTGUI
{
class OTGUI_API CopulaDefinitionWizard : public OTguiWizard
{
  Q_OBJECT

public:
  CopulaDefinitionWizard(const PhysicalModel &model, QWidget *parent = 0);

  virtual bool validateCurrentPage();

  OT::Copula getCopula() const;

protected:
  void buildInterface();

public slots:
  void updateLineEdit(const OT::Description&, const OT::String&);

private:
  PhysicalModel physicalModel_;
  OT::Description copulaVariables_;
  QLineEdit * varLineEdit_;
  QButtonGroup * copulaChoice_;
  QLabel * errorMessageLabel_;
  bool pageValidity_;
};
}
#endif

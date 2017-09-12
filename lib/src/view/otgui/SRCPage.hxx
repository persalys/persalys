//                                               -*- C++ -*-
/**
 *  @brief QWizardPage to define src analysis
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
#ifndef OTGUI_SRCPAGE_HXX
#define OTGUI_SRCPAGE_HXX

#include "otgui/Analysis.hxx"
#include "otgui/LogSpinBox.hxx"
#include "otgui/UIntSpinBox.hxx"

#include <QWizardPage>

namespace OTGUI {
class OTGUI_API SRCPage : public QWizardPage
{
  Q_OBJECT

public:
  SRCPage(QWidget* parent=0);

  void initialize(const Analysis& analysis);
  Analysis getAnalysis(const OT::String& name, const PhysicalModel& physicalModel) const;

  virtual int nextId() const;
  virtual bool validatePage();

protected:
  void buildInterface();

private:
  LogSpinBox * sampleSizeSpinbox_;
  UIntSpinBox * blockSizeSpinbox_;
  QSpinBox * seedSpinbox_;
};
}
#endif

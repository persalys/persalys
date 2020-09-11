//                                               -*- C++ -*-
/**
 *  @brief Cleaning data wizard
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

#ifndef PERSALYS_DATACLEANINGWIZARD_HXX
#define PERSALYS_DATACLEANINGWIZARD_HXX

#include "persalys/Wizard.hxx"
#include "persalys/DataCleaningTools.hxx"
#include "persalys/ValueLineEdit.hxx"
#include "persalys/DataCleaningTableModel.hxx"

namespace PERSALYS
{
  class PERSALYS_VIEW_API DataCleaningWizard : public Wizard
  {
    Q_OBJECT
  public:
    DataCleaningWizard(DataCleaningTools* cleaner, QWidget * parent = 0);
    void keyPressEvent(QKeyEvent * event);
    void launch();

  protected:
    void buildInterface();
    void buildAnalysisPage();

  protected slots:
    void updateUserValues();

  private:
    DataCleaningTools * cleaner_;
    ValueLineEdit * lineEdit_;
    DataCleaningTableModel * tableModel_;
  };

}
#endif

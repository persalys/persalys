//                                               -*- C++ -*-
/**
 *  @brief SubWindow for model observations
 *
 *  Copyright 2015-2019 EDF-Phimeca
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
#ifndef PERSALYS_OBSERVATIONSWINDOW_HXX
#define PERSALYS_OBSERVATIONSWINDOW_HXX

#include "persalys/SubWindow.hxx"
#include "persalys/ObservationsItem.hxx"
#include "persalys/ExportableTableView.hxx"

#include <QVBoxLayout>

namespace PERSALYS
{
class PERSALYS_API ObservationsWindow : public SubWindow
{
  Q_OBJECT

public:
  ObservationsWindow(ObservationsItem *item, QWidget *parent)
  : SubWindow(item, parent)
  {
    QVBoxLayout * mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(new TitleLabel(tr("Model observations"), "user_manual/graphical_interface/deterministic_analysis/user_manual_deterministic_analysis.html#observationwindow"));

    // table
    mainLayout->addWidget(ExportableTableView::GetSampleTableViewWidget(getItem(), item->getDesignOfExperiment().getSample()));
  }
};
}
#endif

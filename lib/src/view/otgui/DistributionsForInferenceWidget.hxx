//                                               -*- C++ -*-
/**
 *  @brief QWidget used in the QWizard for inference
 *
 *  Copyright 2015-2016 EDF-Phimeca
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
#ifndef OTGUI_DISTRIBUTIONSINFERENCEWIDGET_HXX
#define OTGUI_DISTRIBUTIONSINFERENCEWIDGET_HXX

#include "otgui/DistributionsTableModel.hxx"
#include "otgui/TitledComboBox.hxx"

#include <QTableView>

namespace OTGUI {
class OTGUI_API DistributionsForInferenceWidget : public QWidget
{
  Q_OBJECT

public :
  DistributionsForInferenceWidget(const QStringList & distributions, QWidget* parent=0);

protected:
  void buildInterface();

public slots:
  void removeSelectedDistribution();
  void addSelectedDistribution(int);
signals:
  void distributionsListChanged(QStringList);

private:
  QStringList distributions_;
  QTableView * distributionsTableView_;
  DistributionsTableModel * tableModel_;
  TitledComboBox * addDistributionCombox_;
};
}
#endif
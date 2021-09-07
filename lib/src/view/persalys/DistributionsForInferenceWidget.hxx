//                                               -*- C++ -*-
/**
 *  @brief QWidget used in the QWizard for inference
 *
 *  Copyright 2015-2021 EDF-Phimeca
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
#ifndef PERSALYS_DISTRIBUTIONSINFERENCEWIDGET_HXX
#define PERSALYS_DISTRIBUTIONSINFERENCEWIDGET_HXX

#include "persalys/DistributionsTableModel.hxx"
#include "persalys/TitledComboBox.hxx"

#include <openturns/OTType.hxx>

#include <QTableView>
#include <QMetaType> // mandatory to specify it to avoid windows compilation problem

#if QT_VERSION < 0x060000
Q_DECLARE_METATYPE(OT::Description)
#endif

namespace PERSALYS
{
class PERSALYS_VIEW_API DistributionsForInferenceWidget : public QWidget
{
  Q_OBJECT

public :
  DistributionsForInferenceWidget(const QStringList & distributions, const OT::Description &variables, QWidget* parent = 0);

  void updateDistributions(const QStringList& distributions);

protected:
  void buildInterface();

public slots:
  void removeSelectedDistribution();
  void addSelectedDistribution(int);
signals:
  void distributionsListChanged(QStringList);
  void distributionsListChanged(const OT::Description &variables, const QStringList &dist);

private:
  OT::Description variables_;
  QStringList allDistributions_;
  QStringList distributions_;
  QTableView * tableView_;
  DistributionsTableModel * tableModel_;
  TitledComboBox * addComboBox_;
};
}
#endif

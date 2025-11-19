//                                               -*- C++ -*-
/**
 *  @brief QWidget used in the QWizard for inference
 *
 *  Copyright 2015-2025 EDF-Phimeca
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

#include "persalys/QtTools.hxx" // for Q_DECLARE_METATYPE(OT::Description)

namespace PERSALYS
{
class PERSALYS_VIEW_API DistributionsForInferenceWidget : public QWidget
{
  Q_OBJECT

public :
  DistributionsForInferenceWidget(const QStringList & distributions, const OT::Description &variables, QWidget* parent = nullptr, bool isCopulaInference=false);

  void updateDistributions(const QStringList& distributions);

protected:
  void buildInterface();

public slots:
  void removeSelectedDistribution();
  void addSelectedDistribution(int);
signals:
  void distributionsListChanged(QStringList);
  void distributionsListChanged(const OT::Description &variables, const QStringList &dist);
  void applySelectedDistributionsToAllVariablesRequested();

private:
  OT::Description variables_;
  QStringList distributions_;
  QStringList allDistributions_;
  bool isCopulaInference_                   = false;
  QTableView                * tableView_    = nullptr;
  DistributionsTableModel   * tableModel_   = nullptr;
  TitledComboBox            * addComboBox_  = nullptr;
};
}
#endif

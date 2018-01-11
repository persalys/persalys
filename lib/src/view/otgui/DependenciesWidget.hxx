//                                               -*- C++ -*-
/**
 *  @brief QWidget to define dependencies
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
#ifndef OTGUI_DEPENDENCIESWIDGET_HXX
#define OTGUI_DEPENDENCIESWIDGET_HXX

#include "otgui/ProbabilisticModelItem.hxx"
#include "otgui/ResizableStackedWidget.hxx"

#include <QTableView>
#include <QMetaType> // mandatory to specify it to avoid windows compilation problem

Q_DECLARE_METATYPE(OT::Copula)

namespace OTGUI
{
class OTGUI_API DependenciesWidget : public QWidget
{
  Q_OBJECT

public:
  DependenciesWidget(ProbabilisticModelItem *item, QWidget *parent = 0);

public slots:
  void removeCopula();
  void updateCopulaWidgets(const OT::ComposedCopula &copula);
  void selectedItemChanged(const QModelIndex &current, const QModelIndex &previous);
  void openCopulaDefinitionWizard();
signals:
  void addTableLine(const OT::Copula &copula);
  void updateTable();
  void removeTableLine(const QModelIndex &index);

protected:
  void buildInterface();
  void updatePlots();

private:
  PhysicalModel physicalModel_;
  ResizableStackedWidget * rightSideOfSplitterStackedWidget_;
  ResizableStackedWidget * copulaStackedWidget_;
  QTableView * tableView_;
};
}
#endif

//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow to define a probabilistic model
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
#ifndef OTGUI_PROBABILISTICMODELWINDOW_HXX
#define OTGUI_PROBABILISTICMODELWINDOW_HXX

#include "otgui/OTguiSubWindow.hxx"
#include "otgui/ProbabilisticModelItem.hxx"
#include "otgui/PlotWidget.hxx"
#include "otgui/InputTableProbabilisticModel.hxx"
#include "otgui/CopyableTableView.hxx"
#include "otgui/CollapsibleGroupBox.hxx"
#include "otgui/ValueLineEdit.hxx"
#include "otgui/OTStudy.hxx"
#include "otgui/MarginalsWidget.hxx"

#include <QGroupBox>
#include <QVBoxLayout>
#include <QTableView>
#include <QComboBox>
#include <QCheckBox>
#include <QLabel>
#include <QStackedWidget>

namespace OTGUI
{
class OTGUI_API ProbabilisticModelWindow : public OTguiSubWindow
{
  Q_OBJECT

public :
  ProbabilisticModelWindow(ProbabilisticModelItem * item, QWidget * parent = 0);

protected:
  void buildInterface();

public slots:
  void setCorrelationTabErrorMessage(const QString &);
  void reInitCorrelationErrorMessage(QTimeLine::State);
signals:
  void updateCorrelationTableData();

private:
  PhysicalModel physicalModel_;
  MarginalsWidget * marginalsWidget_;
  QLabel * correlationErrorMessage_;
  QList<QTimeLine*> qtimelineList_;
};
}
#endif

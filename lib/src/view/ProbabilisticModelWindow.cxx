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
#include "otgui/ProbabilisticModelWindow.hxx"

#include "otgui/CorrelationTableModel.hxx"
#include "otgui/SpinBoxDelegate.hxx"

using namespace OT;

namespace OTGUI
{

ProbabilisticModelWindow::ProbabilisticModelWindow(ProbabilisticModelItem * item, QWidget * parent)
  : OTguiSubWindow(item, parent)
  , physicalModel_(item->getPhysicalModel())
  , marginalsWidget_(new MarginalsWidget(item, this))
{
  connect(item, SIGNAL(stochasticInputListChanged()), marginalsWidget_, SLOT(updateProbabilisticModel()));
  connect(item, SIGNAL(inputListDefinitionChanged()), marginalsWidget_, SLOT(updateCurrentVariableDistributionWidgets()));
  connect(item, SIGNAL(inputListCorrelationChanged()), this, SIGNAL(updateCorrelationTableData()));

  buildInterface();
}


void ProbabilisticModelWindow::buildInterface()
{
  setWindowTitle(tr("Probabilistic model"));

  QTabWidget * rootTab = new QTabWidget;

  // First Tab: marginals
  rootTab->addTab(marginalsWidget_, tr("Marginals"));

  // Second Tab: correlation
  QWidget * tab = new QWidget;
  QVBoxLayout * tabLayout = new QVBoxLayout(tab);

  QGroupBox * groupBox = new QGroupBox(tr("Spearman's rank (Gaussian Copula)"));
  QVBoxLayout * groupBoxLayout = new QVBoxLayout(groupBox);

  CopyableTableView * correlationTableView = new CopyableTableView;
  SpinBoxDelegate * correlationDelegate = new SpinBoxDelegate(correlationTableView);
  correlationDelegate->setSpinBoxType(SpinBoxDelegate::correlation);
  correlationTableView->setItemDelegate(correlationDelegate);
  correlationTableView->setEditTriggers(QAbstractItemView::AllEditTriggers);

  CorrelationTableModel * correlationTableModel = new CorrelationTableModel(physicalModel_, correlationTableView);
  correlationTableView->setModel(correlationTableModel);
  connect(correlationTableModel, SIGNAL(errorMessageChanged(QString)), this, SLOT(setCorrelationTabErrorMessage(QString)));
  connect(marginalsWidget_, SIGNAL(updateCorrelationTableData()), correlationTableModel, SLOT(updateData()));
  connect(this, SIGNAL(updateCorrelationTableData()), correlationTableModel, SLOT(updateData()));

  groupBoxLayout->addWidget(correlationTableView);
  tabLayout->addWidget(groupBox);

  correlationErrorMessage_ = new QLabel;
  correlationErrorMessage_->setWordWrap(true);
  tabLayout->addWidget(correlationErrorMessage_);

  rootTab->addTab(tab, tr("Correlation"));

  setWidget(rootTab);
}


void ProbabilisticModelWindow::setCorrelationTabErrorMessage(const QString & message)
{
  correlationErrorMessage_->setText(QString("<font color=red>%1</font>").arg(message));
  QTimeLine * time = new QTimeLine(7000, this);
  qtimelineList_.push_back(time);
  connect(time, SIGNAL(stateChanged(QTimeLine::State)), this, SLOT(reInitCorrelationErrorMessage(QTimeLine::State)));
  time->start();
}


void ProbabilisticModelWindow::reInitCorrelationErrorMessage(QTimeLine::State state)
{
  if (state == QTimeLine::NotRunning)
  {
    if (qtimelineList_.isEmpty())
      return;
    // remove the the first item of the list
    qtimelineList_.removeFirst();
    // if another QTimeLine started before the end of the previous one: do nothing
    if (qtimelineList_.size())
      return;
    // else initialize error message
    if (correlationErrorMessage_)
      correlationErrorMessage_->setText("");
  }
}
}

//                                               -*- C++ -*-
/**
 *  @brief QStandardItem, observer of a limit state
 *
 *  Copyright 2015-2023 EDF-Phimeca
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
#include "persalys/LimitStateItem.hxx"

#include "persalys/StudyItem.hxx"

using namespace OT;

namespace PERSALYS
{

LimitStateItem::LimitStateItem(const LimitState & limitState)
  : Item(QString::fromUtf8(limitState.getName().c_str()), "LimitState")
  , Observer("LimitStateItem")
  , limitState_(limitState)
{
  limitState_.addObserver(this);
  limitState_.getPhysicalModel().addObserver(this);

  buildActions();
}


void LimitStateItem::buildActions()
{
  // new threshold exceedance action
  newThresholdExceedanceAction_ = createAction("ThresholdExceedance", getLimitState());

  // remove limit state action
  removeAction_ = new QAction(QIcon(":/images/window-close.png"), tr("Remove"), this);
  removeAction_->setStatusTip(tr("Remove the limit state"));
  connect(removeAction_, SIGNAL(triggered()), this, SLOT(removeLimitState()));

  // add actions
  appendSeparator(tr("Analysis"));
  appendAction(newThresholdExceedanceAction_);
  appendSeparator();
  appendAction(removeAction_);
}


void LimitStateItem::setData(const QVariant & value, int role)
{
  // rename
  if (role == Qt::EditRole)
    limitState_.getImplementation()->setName(value.toString().toUtf8().data());

  QStandardItem::setData(value, role);
}


LimitState LimitStateItem::getLimitState() const
{
  return limitState_;
}


void LimitStateItem::update(Observable* /*source*/, const String & message)
{
  // emit signals to LimitStateWindow

  if (message == "outputNumberChanged" ||
      message == "outputNameChanged" ||
      message == "outputSelectionChanged"
     )
  {
    emit outputListChanged();
  }
  else if (message == "operatorChanged")
  {
    emit operatorChanged();
  }
  else if (message == "thresholdChanged")
  {
    emit thresholdChanged();
  }
  else if (message == "analysisLaunched")
  {
    analysisInProgress_ = true;
  }
  else if (message == "analysisFinished" || message == "analysisBadlyFinished")
  {
    analysisInProgress_ = false;
  }
  else if (message == "objectRemoved")
  {
    emit removeRequested(row());
  }
}


void LimitStateItem::appendItem(const Analysis &analysis)
{
  appendAnalysisItem(analysis);
  analysis.getImplementation().get()->addObserver(this);
}


void LimitStateItem::removeLimitState()
{
  // check
  if (analysisInProgress_)
  {
    emit showErrorMessageRequested(tr("Can not remove a limit state when an analysis is running."));
    return;
  }
  // remove
  if (getParentStudyItem())
    getParentStudyItem()->getStudy().remove(LimitState(limitState_));
}
}

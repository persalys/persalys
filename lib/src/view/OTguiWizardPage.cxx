//                                               -*- C++ -*-
/**
 *  @brief Base class for wizard pages of otgui
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
#include "otgui/OTguiWizardPage.hxx"

namespace OTGUI
{

OTguiWizardPage::OTguiWizardPage(QWidget * parent)
  : QWizardPage(parent)
  , errorMessageLabel_(0)
  , qtimelineList_()
{
}


void OTguiWizardPage::setTemporaryErrorMessage(const QString& message)
{
  if (errorMessageLabel_)
    return;

  errorMessageLabel_->setText(QString("<font color=red>%1</font>").arg(message));

  QTimeLine * time = new QTimeLine(4000, this);
  qtimelineList_.push_back(time);
  connect(time, SIGNAL(stateChanged(QTimeLine::State)), this, SLOT(reInitErrorMessage(QTimeLine::State)));
  time->start();
}


void OTguiWizardPage::reInitErrorMessage(QTimeLine::State state)
{
  if (state == QTimeLine::NotRunning)
  {
    if (qtimelineList_.isEmpty())
      return;
    qtimelineList_.removeFirst();
    // if another QTimeLine started before the end of the previous one: do nothing
    if (qtimelineList_.size())
      return;
    // else initialize error message
    if (errorMessageLabel_)
      errorMessageLabel_->setText("");
  }
}
}

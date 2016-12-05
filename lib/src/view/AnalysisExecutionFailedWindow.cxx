//                                               -*- C++ -*-
/**
 *  @brief Base class for otgui error window
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
#include "otgui/AnalysisExecutionFailedWindow.hxx"

#include <QVBoxLayout>

using namespace OT;

namespace OTGUI {

AnalysisExecutionFailedWindow::AnalysisExecutionFailedWindow(AnalysisItem * item, const QString & errorMessage)
  : OTguiSubWindow(item)
{
  buildInterface();
  setErrorMessage(errorMessage);
}


void AnalysisExecutionFailedWindow::buildInterface()
{
  setWindowTitle(tr("Error window"));

  QWidget * mainWidget = new QWidget;
  QVBoxLayout * layout = new QVBoxLayout(mainWidget);

  QString message(tr("No results are available. The analysis was not launched or an error has occured during its execution.\n"));
  QLabel * label = new QLabel(message);
  label->setWordWrap(true);
  layout->addWidget(label);

  errorMessageLabel_ = new QLabel;
  errorMessageLabel_->setWordWrap(true);
  layout->addWidget(errorMessageLabel_);

  layout->addStretch();

  setWidget(mainWidget);
}
}
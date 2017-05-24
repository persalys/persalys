//                                               -*- C++ -*-
/**
 *  @brief Base class for otgui error window
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
#include "otgui/AnalysisExecutionFailedWindow.hxx"

#include "otgui/Controller.hxx"
#include "otgui/OTStudyItem.hxx"
#include "otgui/ParametersWidget.hxx"

#include <QVBoxLayout>

namespace OTGUI {

AnalysisExecutionFailedWindow::AnalysisExecutionFailedWindow(AnalysisItem* item, const QString& errorMessage)
  : OTguiSubWindow(item)
  , analysisErrorMessage_(errorMessage)
  , analysisItem_(item)
{
  buildInterface();
}


void AnalysisExecutionFailedWindow::buildInterface()
{
  setWindowTitle(tr("Analysis window"));

  QWidget * mainWidget = new QWidget;
  QVBoxLayout * mainLayout = new QVBoxLayout(mainWidget);

  // parameters widget
  QStringList paramNames;
  QStringList paramValues;
  AnalysisItem::GetAnalysisParameters(analysisItem_->getAnalysis(), paramNames, paramValues);
  if (!paramNames.isEmpty())
  {
    ParametersWidget * parametersWidget_ = new ParametersWidget(tr("Analysis parameters"), paramNames, paramValues);
    mainLayout->addWidget(parametersWidget_);
  }

  // progress bar
  progressBar_ = new QProgressBar;
  mainLayout->addWidget(progressBar_);
  connect(analysisItem_, SIGNAL(progressValueChanged(int)), progressBar_, SLOT(setValue(int)));

  // information message
  messageLabel_ = new QLabel;
  messageLabel_->setWordWrap(true);
  mainLayout->addWidget(messageLabel_);
  connect(analysisItem_, SIGNAL(messageChanged(QString)), messageLabel_, SLOT(setText(QString)));

  // initialization
  QString message;
  QString statusBarMessage;
  if (!analysisItem_->getAnalysis().getErrorMessage().empty())
  {
    // progress bar value
    progressBar_->setValue(100);

    // messages
    const QString errorMessage = tr("No results are available. An error has occured during the execution of the analysis.") + "\n";
    message = QString("%1%2%3").arg("<font color=red>").arg(errorMessage + analysisErrorMessage_).arg("</font>");
    statusBarMessage = tr("An error has occured during the execution");
  }
  else // if no error
  {
    // if no result <=> has not been launched yet
    if (!analysisItem_->getAnalysis().analysisLaunched())
    {
      // progress bar value
      progressBar_->setValue(0);

      // messages
      message = tr("The analysis is ready to be launched.") + "\n";
      statusBarMessage = tr("Ready to be launched");
    }
  }
  messageLabel_->setText(message);
  setErrorMessage(statusBarMessage);

  // run button
  QHBoxLayout * hLayout = new QHBoxLayout;
  hLayout->addStretch();

  runButton_ = new QPushButton(tr("Run"));
  runButton_->setIcon(QIcon(":/images/run-build.png"));
  connect(runButton_, SIGNAL(clicked(bool)), this, SLOT(launchAnalysis()));
  hLayout->addWidget(runButton_);

  // stop button
  stopButton_ = new QPushButton(tr("Stop"));
  stopButton_->setIcon(QIcon(":/images/process-stop.png"));
  stopButton_->setEnabled(false);
  connect(stopButton_, SIGNAL(clicked(bool)), this, SLOT(requestStop()));
  hLayout->addWidget(stopButton_);

  mainLayout->addLayout(hLayout);
  mainLayout->addStretch();

  setWidget(mainWidget);
}


void AnalysisExecutionFailedWindow::updateRunButtonAvailability(bool isAvailable)
{
  // impossible to launch an analysis when another one is already running
  if (!analysisItem_->getAnalysis().isRunning())
    runButton_->setEnabled(isAvailable);
}


void AnalysisExecutionFailedWindow::launchAnalysis()
{
  // enable stop button
  stopButton_->setEnabled(true);
  // To start indefinite/busy progress bar
  progressBar_->setRange(0, 0);
  progressBar_->setValue(10);

  // create controller
  Controller * controller = new Controller;
  connect(controller, SIGNAL(processFinished()), this, SLOT(processFinished()));

  // launch the analysis in a separate thread
  controller->launchAnalysis(analysisItem_->getAnalysis());

  // warn the study that an analysis is running
  analysisItem_->update(0, "analysisLaunched");

  // emit signal to disable run analysis/close study/import script...
  emit actionsAvailabilityChanged(false);
}


void AnalysisExecutionFailedWindow::requestStop()
{
  // add a message in case the analysis take too much time to end
  messageLabel_->setText(messageLabel_->text() + "\n" + tr("Stop in progress"));

  // disable stop button
  stopButton_->setEnabled(false);

  // stop the analysis
  analysisItem_->stopAnalysis();
}


void AnalysisExecutionFailedWindow::processFinished()
{
  // emit signal to enable run analysis/close study/import script...
  emit actionsAvailabilityChanged(true);
}
}
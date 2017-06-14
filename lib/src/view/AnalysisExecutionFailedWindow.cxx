//                                               -*- C++ -*-
/**
 *  @brief Window associated with an AnalysisItem
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
#include "otgui/ParametersWidget.hxx"

#include <QHBoxLayout>

namespace OTGUI {

AnalysisExecutionFailedWindow::AnalysisExecutionFailedWindow(AnalysisItem* item, const bool analysisInProgress)
  : OTguiSubWindow(item)
  , analysisInProgress_(analysisInProgress)
  , analysisItem_(item)
  , progressBar_(0)
  , runButton_(0)
  , stopButton_(0)
  , messageLabel_(0)
{
  buildInterface();
}


AnalysisExecutionFailedWindow::~AnalysisExecutionFailedWindow()
{
  analysisItem_ = 0;
}


void AnalysisExecutionFailedWindow::buildInterface()
{
  setWindowTitle(tr("Analysis window"));

  QWidget * mainWidget = new QWidget;
  QGridLayout * mainLayout = new QGridLayout(mainWidget);

  // analysis parameters widget
  QStringList paramNames;
  QStringList paramValues;
  AnalysisItem::GetAnalysisParameters(analysisItem_->getAnalysis(), paramNames, paramValues);
  if (!paramNames.isEmpty())
  {
    ParametersWidget * parametersWidget_ = new ParametersWidget(tr("Analysis parameters"), paramNames, paramValues);
    mainLayout->addWidget(parametersWidget_, 0, 0);
  }

  // progress bar
  progressBar_ = new QProgressBar;
  mainLayout->addWidget(progressBar_, 1, 0);
  connect(analysisItem_, SIGNAL(progressValueChanged(int)), progressBar_, SLOT(setValue(int)));

  // information message
  messageLabel_ = new QLabel;
  messageLabel_->setWordWrap(true);
  mainLayout->addWidget(messageLabel_, 2, 0);
  connect(analysisItem_, SIGNAL(messageChanged(QString)), messageLabel_, SLOT(setText(QString)));

  // initialization
  initializeWidgets();

  // buttons
  QHBoxLayout * hLayout = new QHBoxLayout;
  hLayout->addStretch();

  // - run button
  runButton_ = new QPushButton(tr("Run"));
  runButton_->setIcon(QIcon(":/images/system-run.png"));
  runButton_->setDisabled(analysisInProgress_);
  connect(runButton_, SIGNAL(clicked(bool)), this, SLOT(launchAnalysis()));
  hLayout->addWidget(runButton_);

  // - stop button
  stopButton_ = new QPushButton(tr("Stop"));
  stopButton_->setIcon(QIcon(":/images/process-stop.png"));
  stopButton_->setEnabled(false);
  connect(stopButton_, SIGNAL(clicked(bool)), this, SLOT(stopAnalysis()));
  hLayout->addWidget(stopButton_);

  mainLayout->addLayout(hLayout, 3, 0);
  mainLayout->setRowStretch(4, 1);

  setWidget(mainWidget);
}


void AnalysisExecutionFailedWindow::initializeWidgets()
{
  QString informationMessage;
  QString statusBarMessage;

  // if an error has occured
  if (!analysisItem_->getAnalysis().getErrorMessage().empty())
  {
    // progress bar value
    progressBar_->setValue(100);

    // messages
    QString errorMessage = tr("No results are available. An error has occured during the execution of the analysis.") + "\n";
    errorMessage += analysisItem_->getAnalysis().getErrorMessage().c_str();
    informationMessage = QString("<font color=red>%1</font>").arg(errorMessage);

    statusBarMessage = tr("An error has occured during the execution of the analysis");
  }
  else // if no error
  {
    // if no result <=> has not been launched yet
    if (!analysisItem_->getAnalysis().analysisLaunched())
    {
      // progress bar value
      progressBar_->setValue(0);

      // messages
      informationMessage = tr("The analysis is ready to be launched.") + "\n";

      statusBarMessage = tr("Ready to be launched");
    }
  }
  messageLabel_->setText(informationMessage);
  setErrorMessage(statusBarMessage);
}


void AnalysisExecutionFailedWindow::updateRunButtonAvailability(bool analysisInProgress)
{
  // impossible to launch an analysis when another one is already running
  if (!analysisItem_->getAnalysis().isRunning())
    runButton_->setDisabled(analysisInProgress);
}


void AnalysisExecutionFailedWindow::launchAnalysis()
{
  // enable stop button
  stopButton_->setEnabled(true);
  // start indefinite/busy progress bar
  progressBar_->setRange(0, 0);
  progressBar_->setValue(10);
  // messages
  messageLabel_->setText(tr("The analysis is running"));
  setErrorMessage("");

  // create controller
  Controller * controller = new Controller;
  connect(controller, SIGNAL(launchAnalysisRequested(Analysis)), analysisItem_, SLOT(processLaunched()));
  connect(controller, SIGNAL(processFinished()), analysisItem_, SLOT(processFinished()));

  // launch the analysis in a separate thread
  controller->launchAnalysis(analysisItem_->getAnalysis());
}


void AnalysisExecutionFailedWindow::stopAnalysis()
{
  // add a message in case the analysis take too much time to end
  messageLabel_->setText(messageLabel_->text() + "\n" + tr("Stop in progress"));

  // disable stop button
  stopButton_->setEnabled(false);

  // stop the analysis
  analysisItem_->stopAnalysis();
}
}
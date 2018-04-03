//                                               -*- C++ -*-
/**
 *  @brief Window associated with an AnalysisItem
 *
 *  Copyright 2015-2018 EDF-Phimeca
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
#include "otgui/AnalysisWindow.hxx"

#include "otgui/Controller.hxx"
#include "otgui/ParametersWidget.hxx"
#include "otgui/TranslationManager.hxx"

#include <QHBoxLayout>
#include <QScrollArea>

#ifdef OTGUI_HAVE_YACS
#include "otgui/YACSPhysicalModel.hxx"
#include <ResourceWidget.hxx>
#endif

namespace OTGUI
{

AnalysisWindow::AnalysisWindow(AnalysisItem* item, const bool analysisInProgress, QWidget * parent)
  : OTguiSubWindow(item, parent)
  , analysisInProgress_(analysisInProgress)
  , analysisItem_(item)
  , progressBar_(0)
  , runButton_(0)
  , stopButton_(0)
  , messageLabel_(0)
{
  buildInterface();
}


AnalysisWindow::~AnalysisWindow()
{
  analysisItem_ = 0;
}


void AnalysisWindow::buildInterface()
{
  setWindowTitle(tr("Analysis window"));

  QWidget * mainWidget = new QWidget;
  QGridLayout * mainLayout = new QGridLayout(mainWidget);

  QScrollArea * scrollArea = new QScrollArea;
  scrollArea->setWidgetResizable(true);

  // analysis parameters widget
  const Parameters analysisParameters(analysisItem_->getAnalysis().getImplementation()->getParameters());
  if (analysisParameters.getSize())
  {
    ParametersWidget * parametersWidget_ = new ParametersWidget(tr("Analysis parameters"), analysisParameters);
    mainLayout->addWidget(parametersWidget_, 0, 0);
  }

  // progress bar
  progressBar_ = new QProgressBar;
  mainLayout->addWidget(progressBar_, 1, 0);
  connect(analysisItem_, SIGNAL(progressValueChanged(int)), progressBar_, SLOT(setValue(int)));

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

  mainLayout->addLayout(hLayout, 2, 0);

  // information message
  messageLabel_ = new QLabel;
  messageLabel_->setWordWrap(true);
  messageLabel_->setTextFormat(Qt::PlainText);
  mainLayout->addWidget(messageLabel_, 3, 0);
  connect(analysisItem_, SIGNAL(messageChanged(QString)), messageLabel_, SLOT(setText(QString)));

  mainLayout->setRowStretch(4, 1);

  launchParameters_ = 0;
  analysisItem_->getAnalysis().acceptLaunchParameters(this);
  if (launchParameters_)
  {
    mainLayout->addWidget(launchParameters_, 4, 0);
  }

  scrollArea->setWidget(mainWidget);

  // initialization
  initializeWidgets();

  setWidget(scrollArea);
}


void AnalysisWindow::initializeWidgets()
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
    informationMessage = errorMessage;
    messageLabel_->setStyleSheet("color: red;");
    statusBarMessage = tr("An error has occured during the execution of the analysis");
  }
  else // if no error
  {
    // if no result <=> has not been launched yet
    if (!analysisItem_->getAnalysis().hasValidResult())
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


void AnalysisWindow::updateRunButtonAvailability(bool analysisInProgress)
{
  // impossible to launch an analysis when another one is already running
  runButton_->setDisabled(analysisInProgress);
}


void AnalysisWindow::launchAnalysis()
{
  // re re re check
  if (analysisItem_->getAnalysis().isRunning())
    return;

  // enable stop button
  stopButton_->setEnabled(true);
  // start indefinite/busy progress bar
  progressBar_->setRange(0, 0);
  progressBar_->setValue(10);
  // messages
  messageLabel_->setText(tr("The analysis is running"));
  messageLabel_->setStyleSheet("");
  setErrorMessage("");

  // create controller
  Controller * controller = new Controller;
  connect(controller, SIGNAL(launchAnalysisRequested(Analysis)), analysisItem_, SLOT(processLaunched()));
  connect(controller, SIGNAL(processFinished()), analysisItem_, SLOT(processFinished()));

  // launch the analysis in a separate thread
  controller->launchAnalysis(analysisItem_->getAnalysis());
}


void AnalysisWindow::stopAnalysis()
{
  // add a message in case the analysis take too much time to end
  messageLabel_->setText(messageLabel_->text() + "\n" + tr("Stop in progress"));

  // disable stop button
  stopButton_->setEnabled(false);

  // stop the analysis
  analysisItem_->stopAnalysis();
}


#ifdef OTGUI_HAVE_YACS
void AnalysisWindow::visitYACS(YACSPhysicalModel* model)
{
  ResourceWidget* rw = new ResourceWidget(model->getResourceModel());
  launchParameters_ = rw;
}
#endif
}

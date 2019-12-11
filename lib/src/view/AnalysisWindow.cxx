//                                               -*- C++ -*-
/**
 *  @brief Window associated with an AnalysisItem
 *
 *  Copyright 2015-2019 EDF-Phimeca
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
#ifdef PERSALYS_HAVE_YACS
#include "persalys/YACSPhysicalModel.hxx" //includes python.h. Has to be done before any Qt include
#include "persalys/YACSCouplingPhysicalModel.hxx"
#include <ydefx/ResourceWidget.hxx>
#endif

#include "persalys/Controller.hxx"   // !!! WARNING !!! THIS INCLUDE MUST BE THE VERY FIRST !!!

#include "persalys/AnalysisWindow.hxx"

#include "persalys/ParametersWidget.hxx"
#include "persalys/TranslationManager.hxx"
#include "persalys/QtTools.hxx"

#include "persalys/StudyManager.hxx"

#include <QHBoxLayout>
#include <QScrollArea>
#include <QMessageBox>

namespace PERSALYS
{

AnalysisWindow::AnalysisWindow(AnalysisItem* item, StudyManager *manager, QWidget * parent)
  : SubWindow(item, parent)
  , studyManager_(manager)
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
  studyManager_ = 0;
}


void AnalysisWindow::setErrorMessage(QString message)
{
  messageLabel_->setErrorMessage(message);
}


void AnalysisWindow::buildInterface()
{
  QVBoxLayout * widgetLayout = new QVBoxLayout(this);

  QWidget * mainWidget = new QWidget;
  QGridLayout * mainLayout = new QGridLayout(mainWidget);

  QScrollArea * scrollArea = new QScrollArea;
  scrollArea->setWidgetResizable(true);

  // analysis parameters widget
  const Parameters analysisParameters(analysisItem_->getAnalysis().getImplementation()->getParameters());
  if (analysisParameters.getSize())
  {
    ParametersWidget * parametersWidget_ = new ParametersWidget(tr("Analysis parameters"), analysisParameters);
    mainLayout->addWidget(parametersWidget_, 0, 0, 1, 3);
  }

  // progress bar
  progressBar_ = new QProgressBar;
  QPalette p = progressBar_->palette();
  p.setColor(QPalette::Highlight, ApplicationColor["darkColor"]);
  progressBar_->setPalette(p);
  mainLayout->addWidget(progressBar_, 1, 2);
  connect(analysisItem_, SIGNAL(progressValueChanged(int)), this, SLOT(updateProgressBar(int)));

  // buttons
  // - run button
  runButton_ = new QPushButton(tr("Run"));
  runButton_->setObjectName("runbutton");
  runButton_->setIcon(QIcon(":/images/system-run.png"));
  connect(runButton_, SIGNAL(clicked(bool)), this, SLOT(launchAnalysis()));
  mainLayout->addWidget(runButton_, 1, 0);

  // - stop button
  stopButton_ = new QPushButton(tr("Stop"));
  stopButton_->setIcon(QIcon(":/images/process-stop.png"));
  stopButton_->setEnabled(false);
  connect(stopButton_, SIGNAL(clicked(bool)), this, SLOT(stopAnalysis()));
  mainLayout->addWidget(stopButton_, 1, 1);
  mainLayout->setColumnStretch(2, 1);

  // information message
  messageLabel_ = new TemporaryLabel;
  messageLabel_->setTextFormat(Qt::PlainText);
  mainLayout->addWidget(messageLabel_, 2, 0, 1, 3);
  connect(analysisItem_, SIGNAL(messageChanged(QString)), messageLabel_, SLOT(setText(QString)));

  mainLayout->setRowStretch(4, 1);

  launchParameters_ = 0;
  analysisItem_->getAnalysis().acceptLaunchParameters(this);
  if (launchParameters_)
  {
    mainLayout->addWidget(launchParameters_, 4, 0, 1, 3);
  }

  scrollArea->setWidget(mainWidget);

  // initialization
  initializeWidgets();

  widgetLayout->addWidget(scrollArea);
}


void AnalysisWindow::initializeWidgets()
{
  QString informationMessage;
  QString statusBarMessage;

  // if an error has occurred
  if (!analysisItem_->getAnalysis().getErrorMessage().empty())
  {
    // progress bar value
    progressBar_->setValue(100);

    // messages
    QString errorMessage = tr("No results are available. An error has occurred during the execution of the analysis.") + "\n";
    errorMessage += analysisItem_->getAnalysis().getErrorMessage().c_str();
    messageLabel_->setErrorMessage(errorMessage);
  }
  else // if no error
  {
    // if no result <=> has not been launched yet
    if (!analysisItem_->getAnalysis().hasValidResult())
    {
      // progress bar value
      progressBar_->setValue(0);

      // messages
      messageLabel_->setMessage(tr("The analysis is ready to be launched.") + "\n");
    }
  }
}


void AnalysisWindow::launchAnalysis()
{
  // check if an analysis is running
  if (studyManager_->analysisInProgress())
  {
    QMessageBox::critical(parentWidget(), tr("Error"), tr("An analysis is already running"));
    return;
  }

  // enable stop button
  stopButton_->setEnabled(true);

  // launchParameters_ should never be enabled again after the analysis is launched
  if (launchParameters_)
    launchParameters_->setEnabled(false);

  // start indefinite/busy progress bar
  progressBar_->setRange(0, 0);
  progressBar_->setValue(10);
  // messages
  messageLabel_->setMessage(tr("The analysis is running"));

  // create controller
  Controller * controller = new Controller;
  connect(controller, SIGNAL(launchAnalysisRequested(Analysis)), analysisItem_, SLOT(processStatusChanged()));
  connect(controller, SIGNAL(processFinished()), analysisItem_, SLOT(processStatusChanged()));

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


void AnalysisWindow::updateProgressBar(const int value)
{
  if (progressBar_->maximum() == progressBar_->minimum())
  {
    progressBar_->setRange(0, 100);
    progressBar_->setValue(0.);
  }
  progressBar_->setValue(value);
}


#ifdef PERSALYS_HAVE_YACS
void AnalysisWindow::visitYACS(YACSPhysicalModel* model)
{
  ydefx::ResourceWidget* rw = new ydefx::ResourceWidget(model->jobParameters());
  launchParameters_ = rw;
}

void AnalysisWindow::visitYACS(YACSCouplingPhysicalModel* model)
{
  ydefx::ResourceWidget* rw = new ydefx::ResourceWidget(model->jobParameters());
  launchParameters_ = rw;
}


#endif
}

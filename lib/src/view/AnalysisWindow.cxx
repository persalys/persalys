//                                               -*- C++ -*-
/**
 *  @brief Window associated with an AnalysisItem
 *
 *  Copyright 2015-2026 EDF-Phimeca
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
#include "persalys/Controller.hxx"   // !!! WARNING !!! THIS INCLUDE MUST BE THE VERY FIRST !!!

#include "persalys/AnalysisWindow.hxx"

#include "persalys/ParametersWidget.hxx"
#include "persalys/TranslationManager.hxx"
#include "persalys/QtTools.hxx"

#include "persalys/StudyManager.hxx"

#include <QHBoxLayout>
#include <QScrollArea>
#include <QMessageBox>

using namespace OT;

namespace PERSALYS
{

AnalysisWindow::AnalysisWindow(AnalysisItem* item, StudyManager *manager, QWidget * parent)
  : SubWindow(item, parent)
  , studyManager_(manager)
  , analysisItem_(item)
{
  if (!item)
    throw InvalidArgumentException(HERE) << "AnalysisWindow: item is null";
  buildInterface();
}


void AnalysisWindow::setErrorMessage(QString message)
{
  messageWidget_->setMessage(message, ErrorWidget::Error, false, false);
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

  // buttons
  // - run button
  runButton_ = new QPushButton(tr("Run"));
  runButton_->setObjectName("runbutton");
  runButton_->setIcon(QIcon(":/images/system-run.png"));
  connect(runButton_, SIGNAL(clicked(bool)), this, SLOT(launchAnalysis()));
  mainLayout->addWidget(runButton_, 1, 0);

  // - stop button
  stopButton_ = new QPushButton(tr("Stop"));
  stopButton_->setToolTip(tr("Clean exit after current block"));
  stopButton_->setIcon(QIcon(":/images/kt-stop-all.png"));
  stopButton_->setEnabled(false);
  connect(stopButton_, SIGNAL(clicked(bool)), this, SLOT(stopAnalysis()));
  mainLayout->addWidget(stopButton_, 1, 1);

  // - interrupt button
  interruptButton_ = new QPushButton(tr("Interrupt"));
  interruptButton_->setToolTip(tr("Immediate dirty exit"));
  interruptButton_->setIcon(QIcon(":/images/process-stop.png"));
  interruptButton_->setVisible(analysisItem_->getAnalysis().canBeInterrupted());
  connect(interruptButton_, SIGNAL(clicked(bool)), this, SLOT(interruptAnalysis()));
  mainLayout->addWidget(interruptButton_, 1, 2);

  // - detach button
  detachButton_ = new QPushButton(tr("Detach"));
  detachButton_->setToolTip(tr("Continue running offline"));
  detachButton_->setIcon(QIcon(":/images/offline.png"));
  detachButton_->setVisible(analysisItem_->getAnalysis().canBeDetached());
  connect(detachButton_, SIGNAL(clicked(bool)), this, SLOT(detachAnalysis()));
  mainLayout->addWidget(detachButton_, 1, 3);
  mainLayout->setColumnStretch(4, 1);

  // progress bar
  progressBar_ = new QProgressBar;
  QPalette p = progressBar_->palette();
  p.setColor(QPalette::Highlight, ApplicationColor["darkColor"]);
  progressBar_->setPalette(p);
  mainLayout->addWidget(progressBar_, 2, 0, 1, 3);
  connect(analysisItem_, SIGNAL(progressValueChanged(int)), this, SLOT(updateProgressBar(int)));

  // information message
  messageWidget_ = new ErrorWidget;
  mainLayout->addWidget(messageWidget_, 3, 0, 1, 3);
  connect(analysisItem_, &AnalysisItem::messageChanged, messageWidget_, [this](const QString & msg){
    messageWidget_->setMessage(msg, ErrorWidget::Information, false, false);
  });

  mainLayout->setRowStretch(4, 1);

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
    QString message;
    if (analysisItem_->getAnalysis().getErrorMessage() == "__DETACHED_EXCEPTION__")
    {
      message = tr("No results yet, analysis has been detached. Click Run to reattach.");
      messageWidget_->setMessage(message, ErrorWidget::Information, false, false);
    }
    else
    {
      // progress bar value
      progressBar_->setValue(100);

      // messages
      message = tr("No results are available. An error has occurred during the execution of the analysis.") + "\n";
      message += analysisItem_->getAnalysis().getErrorMessage().c_str();
      messageWidget_->setMessage(message, ErrorWidget::Error, false, false);
    }
  }
  else // if no error
  {
    // if no result <=> has not been launched yet
    if (!analysisItem_->getAnalysis().hasValidResult())
    {
      // progress bar value
      progressBar_->setValue(0);

      // messages
      messageWidget_->setMessage(tr("The analysis is ready to be launched."), ErrorWidget::Information, false, false);
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

  // enable stop buttons
  stopButton_->setEnabled(true);
  interruptButton_->setEnabled(analysisItem_->getAnalysis().canBeInterrupted());
  runButton_->setEnabled(false);
  detachButton_->setEnabled(analysisItem_->getAnalysis().canBeDetached());

  // start indefinite/busy progress bar
  progressBar_->setRange(0, 0);
  progressBar_->setValue(10);
  // messages
  messageWidget_->setMessage(tr("The analysis is running"), ErrorWidget::Information, false, false);

  // create controller
  Controller * controller = new Controller;
  controller->setParent(this);
  connect(controller, SIGNAL(launchAnalysisRequested(Analysis)), analysisItem_, SLOT(processStatusChanged()));
  connect(controller, SIGNAL(processFinished()), analysisItem_, SLOT(processStatusChanged()));

  // launch the analysis in a separate thread
  controller->launchAnalysis(analysisItem_->getAnalysis());
}


void AnalysisWindow::stopAnalysis()
{
  // add a message in case the analysis take too much time to end
  messageWidget_->setMessage(messageWidget_->toPlainText() + "\n" + tr("Stop in progress"), ErrorWidget::Information, false, false);

  // disable stop buttons
  stopButton_->setEnabled(false);
  interruptButton_->setEnabled(false);
  detachButton_->setEnabled(false);

  // stop the analysis
  analysisItem_->stopAnalysis();
}


void AnalysisWindow::interruptAnalysis()
{
  // add a message in case the analysis take too much time to end
  messageWidget_->setMessage(messageWidget_->toPlainText() + "\n" + tr("Interrupt in progress"), ErrorWidget::Information, false, false);

  // disable stop buttons
  stopButton_->setEnabled(false);
  interruptButton_->setEnabled(false);
  detachButton_->setEnabled(false);

  // interrupt the analysis
  analysisItem_->interruptAnalysis();
}


void AnalysisWindow::detachAnalysis()
{
  if (analysisItem_->getAnalysis().canBeDetached())
  {
    // add a message in case the analysis take too much time to end
    messageWidget_->setMessage(messageWidget_->toPlainText() + "\n" + tr("Detach in progress"), ErrorWidget::Information, false, false);

    // disable stop buttons
    stopButton_->setEnabled(false);
    interruptButton_->setEnabled(false);
    detachButton_->setEnabled(false);

    // detach the analysis
    analysisItem_->detachAnalysis();
  }
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


}

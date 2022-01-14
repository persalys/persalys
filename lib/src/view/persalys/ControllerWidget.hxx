//                                               -*- C++ -*-
/**
 *  @brief QWidget with a progressBar which allows to stop analysis
 *
 *  Copyright 2015-2022 EDF-Phimeca
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
#ifndef PERSALYS_CONTROLLERWIDGET_HXX
#define PERSALYS_CONTROLLERWIDGET_HXX

#include <QVBoxLayout>
#include <QProgressBar>
#include <QPushButton>
#include <QDateTime>
#include <QLabel>
#include <QScrollArea>

namespace PERSALYS
{
class ControllerWidget : public QWidget
{
  Q_OBJECT

public:
  ControllerWidget(const QString& analysisName, QWidget* parent = 0)
    : QWidget(parent)
  {
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    QVBoxLayout * widgetLayout = new QVBoxLayout(this);

    QScrollArea * scrollArea = new QScrollArea;
    scrollArea->setWidgetResizable(true);
    QFrame * frame = new QFrame;
    QVBoxLayout * mainLayout = new QVBoxLayout(frame);

    // analysis name
    QString dateFormat("d MMMM yyyy h:mm:ss");
    QLabel * aLabel = new QLabel(tr("Run the analysis '%1'.\nLaunch time: %2.").arg(analysisName).arg(QDateTime::currentDateTime().toString(dateFormat)));
    aLabel->setWordWrap(true);
    mainLayout->addWidget(aLabel);

    // progress bar
    progressBar_ = new QProgressBar;
    //To start indefinite/busy progress bar
    progressBar_->setRange(0, 0);
    progressBar_->setValue(10);
    mainLayout->addWidget(progressBar_);

    // information message
    messageLabel_ = new QLabel;
    messageLabel_->setWordWrap(true);
    mainLayout->addWidget(messageLabel_);

    // stop button
    QHBoxLayout * hLayout = new QHBoxLayout;
    hLayout->addStretch();
    QPushButton * stopButton = new QPushButton(tr("Stop"));
    connect(stopButton, SIGNAL(clicked(bool)), this, SIGNAL(stopRunningRequested()));
    hLayout->addWidget(stopButton);
    mainLayout->addLayout(hLayout);

    mainLayout->addStretch();

    scrollArea->setWidget(frame);
    widgetLayout->addWidget(scrollArea);
  }

public slots:
  void changeProgressBarValue(int value)
  {
    progressBar_->setRange(0, 100);
    progressBar_->setValue(value);
  }


  void setMessage(const QString message)
  {
    messageLabel_->setText(message);
  }

signals:
  void stopRunningRequested();

private:
  QLabel * messageLabel_;
  QProgressBar * progressBar_;
};
}
#endif
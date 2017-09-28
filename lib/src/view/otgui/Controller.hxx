//                                               -*- C++ -*-
/**
 *  @brief QObject to run an analysis in a separate thread
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
#ifndef OTGUI_CONTROLLER_HXX
#define OTGUI_CONTROLLER_HXX

#include "otgui/Analysis.hxx"

#include <openturns/PythonWrappingFunctions.hxx>

#include <QThread>
#include <QMetaType> // mandatory to specify it to avoid windows compilation problem

namespace OTGUI
{
class Worker : public QObject
{
  Q_OBJECT

public slots:
  void process(Analysis analysis)
  {
    analysis.run();
    emit processFinished();
  }

signals:
  void processFinished();
};


class Controller : public QObject
{
  Q_OBJECT

public:
  Controller()
  {
    qRegisterMetaType<OTGUI::PhysicalModel>("PhysicalModel");
    qRegisterMetaType<OTGUI::Analysis>("Analysis");

    // new thread
    QThread * workerThread = new QThread;
    connect(workerThread, SIGNAL(finished()), workerThread, SLOT(deleteLater()));

    // worker
    Worker * worker = new Worker;
    worker->moveToThread(workerThread);

    // connections
    connect(this, SIGNAL(launchAnalysisRequested(Analysis)), worker, SLOT(process(Analysis)));
    connect(worker, SIGNAL(processFinished()), this, SIGNAL(processFinished()));
    connect(worker, SIGNAL(processFinished()), workerThread, SLOT(quit()));
    connect(worker, SIGNAL(processFinished()), worker, SLOT(deleteLater()));
    connect(worker, SIGNAL(processFinished()), this, SLOT(deleteLater()));

    // start thread
    workerThread->start();
  }


  void launchAnalysis(Analysis analysis)
  {
    emit launchAnalysisRequested(analysis);

    // Warning : NOT create worker as attribut and then write here worker.process():
    // otherwise the run method WON'T be launch in a separate thread !
  };

signals:
  void launchAnalysisRequested(Analysis analysis);
  void processFinished();
};
}
#endif
//                                               -*- C++ -*-
/**
 *  @brief QObject to run an analysis in a separate thread
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
#ifndef PERSALYS_CONTROLLER_HXX
#define PERSALYS_CONTROLLER_HXX

#include "persalys/Analysis.hxx"

#include <QObject>

namespace PERSALYS
{
class Worker : public QObject
{
  Q_OBJECT

public slots:
  void process(Analysis analysis);

signals:
  void processFinished();
};


class Controller : public QObject
{
  Q_OBJECT

public:
  Controller();
  void launchAnalysis(Analysis analysis);

signals:
  void launchAnalysisRequested(Analysis analysis);
  void processFinished();
};
}
#endif

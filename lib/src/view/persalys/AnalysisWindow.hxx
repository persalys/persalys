//                                               -*- C++ -*-
/**
 *  @brief Window associated with an AnalysisItem
 *
 *  Copyright 2015-2020 EDF-Phimeca
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
#ifndef PERSALYS_ANALYSISWINDOW_HXX
#define PERSALYS_ANALYSISWINDOW_HXX

#include "persalys/SubWindow.hxx"
#include "persalys/AnalysisItem.hxx"
#include "persalys/LaunchParametersVisitor.hxx"
#include "persalys/TemporaryLabel.hxx"

#include <QPushButton>
#include <QProgressBar>

namespace PERSALYS
{
class StudyManager;
class PERSALYS_API AnalysisWindow : public SubWindow, LaunchParametersVisitor
{
  Q_OBJECT

public:
  AnalysisWindow(AnalysisItem *item, StudyManager *manager, QWidget * parent = 0);

  virtual ~AnalysisWindow();

  void setErrorMessage(QString message);

protected:
  void buildInterface();
  void initializeWidgets();
#ifdef PERSALYS_HAVE_YACS
  virtual void visitYACS(YACSPhysicalModel* model);
  virtual void visitYACS(YACSCouplingPhysicalModel* model);
#endif

protected slots:
  void launchAnalysis();
  void stopAnalysis();
  void updateProgressBar(const int value);

private:
  StudyManager * studyManager_;
  AnalysisItem * analysisItem_;
  QProgressBar * progressBar_;
  QPushButton * runButton_;
  QPushButton * stopButton_;
  TemporaryLabel * messageLabel_;
  QWidget * launchParameters_;
};
}
#endif

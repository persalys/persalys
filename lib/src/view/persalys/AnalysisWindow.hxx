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
#ifndef PERSALYS_ANALYSISWINDOW_HXX
#define PERSALYS_ANALYSISWINDOW_HXX

#include "persalys/SubWindow.hxx"
#include "persalys/AnalysisItem.hxx"
#include "persalys/ErrorWidget.hxx"

#include <QPushButton>
#include <QProgressBar>

namespace PERSALYS
{
class StudyManager;
class PERSALYS_VIEW_API AnalysisWindow : public SubWindow
{
  Q_OBJECT

public:
  AnalysisWindow(AnalysisItem *item, StudyManager *manager, QWidget *parent = nullptr);

  void setErrorMessage(QString message);

protected:
  void buildInterface();
  void initializeWidgets();

protected slots:
  void launchAnalysis();
  void stopAnalysis();
  void detachAnalysis();
  void updateProgressBar(const int value);

private:
  StudyManager  * studyManager_   = nullptr;
  AnalysisItem  * analysisItem_   = nullptr;
  QProgressBar  * progressBar_    = nullptr;
  QPushButton   * runButton_      = nullptr;
  QPushButton   * stopButton_     = nullptr;
  QPushButton   * detachButton_   = nullptr;
  ErrorWidget   * messageWidget_  = nullptr;
};
}
#endif

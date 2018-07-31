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
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef OTGUI_ANALYSISWINDOW_HXX
#define OTGUI_ANALYSISWINDOW_HXX

#include "otgui/OTguiSubWindow.hxx"
#include "otgui/AnalysisItem.hxx"
#include "otgui/LaunchParametersVisitor.hxx"

#include <QPushButton>
#include <QProgressBar>

namespace OTGUI
{
class OTGUI_API AnalysisWindow : public OTguiSubWindow, LaunchParametersVisitor
{
  Q_OBJECT

public:
  AnalysisWindow(AnalysisItem* item, const bool analysisInProgress = false, QWidget * parent = 0);

  virtual ~AnalysisWindow();

protected:
  void buildInterface();
  void initializeWidgets();
#ifdef OTGUI_HAVE_YACS
  virtual void visitYACS(YACSPhysicalModel* model);
#endif

public slots:
  void updateRunButtonAvailability(bool);

protected slots:
  void launchAnalysis();
  void stopAnalysis();
  void updateProgressBar(const int value);

private:
  bool analysisInProgress_;
  AnalysisItem * analysisItem_;
  QProgressBar * progressBar_;
  QPushButton * runButton_;
  QPushButton * stopButton_;
  QLabel * messageLabel_;
  QWidget * launchParameters_;
};
}
#endif

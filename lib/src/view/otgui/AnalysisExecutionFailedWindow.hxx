//                                               -*- C++ -*-
/**
 *  @brief Base class for otgui error window
 *
 *  Copyright 2015-2016 EDF-Phimeca
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
#ifndef OTGUI_ANALYSISEXECUTIONFAILEDWINDOW_HXX
#define OTGUI_ANALYSISEXECUTIONFAILEDWINDOW_HXX

#include "otgui/OTguiSubWindow.hxx"
#include "otgui/AnalysisItem.hxx"

namespace OTGUI {
class OTGUI_API AnalysisExecutionFailedWindow : public OTguiSubWindow
{
  Q_OBJECT

public:
  AnalysisExecutionFailedWindow(AnalysisItem * item, const QString & errorMessage);

protected:
  void buildInterface();
};
}
#endif
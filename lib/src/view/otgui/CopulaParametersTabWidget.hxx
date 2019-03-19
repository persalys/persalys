//                                               -*- C++ -*-
/**
 *  @brief QTabWidget for the parameters and PDF/CDF copula
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
#ifndef OTGUI_COPULAPARAMETERSTABWIDGET_HXX
#define OTGUI_COPULAPARAMETERSTABWIDGET_HXX

#include "otgui/DataSample.hxx"

#include <openturns/Distribution.hxx>

#include <QTabWidget>
#include <QLabel>

namespace OTGUI
{
class OTGUI_API CopulaParametersTabWidget : public QTabWidget
{
  Q_OBJECT

public:
  CopulaParametersTabWidget(const OT::Distribution& distribution,
                            const OT::Sample& sample,
                            const DataSample::SampleCollection& kendallPlotData,
                            const bool displaySetting = true,
                            QWidget* parent = 0);

protected:
  void buildInterface();

public slots:
  void openUrl();
  void setCurrentIndexWithoutSignal(int);

private:
  OT::Distribution distribution_;
  OT::Sample sample_;
  OT::PersistentCollection<OT::Sample> kendallPlotData_;
  bool displaySetting_;
  QLabel * analysisErrorMessageLabel_;
};
}
#endif

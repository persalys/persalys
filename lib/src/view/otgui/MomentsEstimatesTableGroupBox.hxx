//                                               -*- C++ -*-
/**
 *  @brief QGroupBox for moments estimates table
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
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef OTGUI_MOMENTASESTIMATESTABLEGROUPBOX_HXX
#define OTGUI_MOMENTASESTIMATESTABLEGROUPBOX_HXX

#include "otgui/DataAnalysisResult.hxx"
#include "otgui/ResizableStackedWidget.hxx"

#include <QGroupBox>

namespace OTGUI
{
class OTGUI_API MomentsEstimatesTableGroupBox : public QGroupBox
{
  Q_OBJECT

public:
  MomentsEstimatesTableGroupBox(const DataAnalysisResult & result,
                                const bool isConfidenceIntervalRequired,
                                const double levelConfidenceInterval,
                                const OT::Indices& variablesIndices,
                                QWidget* parent = 0);

protected:
  QWidget * getMomentsEstimateTableView(const DataAnalysisResult& result, const OT::UnsignedInteger outputIndex);

public slots:
  void setCurrentIndexStackedWidget(int index);

private:
  ResizableStackedWidget * stackedWidget_;
  bool isConfidenceIntervalRequired_;
  double levelConfidenceInterval_;
};
}
#endif

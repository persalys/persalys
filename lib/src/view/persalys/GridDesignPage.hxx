//                                               -*- C++ -*-
/**
 *  @brief QWizardPage to define deterministic designs of experiments
 *
 *  Copyright 2015-2025 EDF-Phimeca
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
#ifndef PERSALYS_DETERMINISTICDESIGNPAGE_HXX
#define PERSALYS_DETERMINISTICDESIGNPAGE_HXX

#include "persalys/Analysis.hxx"
#include "persalys/ResizableHeaderlessTableView.hxx"
#include "persalys/ErrorWidget.hxx"
#include "persalys/ExperimentTableModel.hxx"

#include <QWizardPage>
#include <QLabel>

namespace PERSALYS
{
class PERSALYS_VIEW_API GridDesignPage : public QWizardPage
{
  Q_OBJECT

public:
  explicit GridDesignPage(QWidget *parent = nullptr);

  void initialize(const Analysis& analysis);
  Analysis getAnalysis() const;
  bool validatePage() override;

signals:
  void showTime();

protected:
  void buildInterface();
  void resizeEvent(QResizeEvent * event) override;

private:
  ResizableHeaderlessTableView  * tableView_          = nullptr;
  ExperimentTableModel          * tableModel_         = nullptr;
  QLabel                        * DOESizeLabel_       = nullptr;
  QLabel                        * DOETimeLabel_       = nullptr;
  ErrorWidget                   * errorWidget_  = nullptr;
};
}
#endif

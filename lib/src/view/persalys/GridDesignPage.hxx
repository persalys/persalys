//                                               -*- C++ -*-
/**
 *  @brief QWizardPage to define deterministic designs of experiments
 *
 *  Copyright 2015-2023 EDF-Phimeca
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
#include "persalys/TemporaryLabel.hxx"
#include "persalys/ExperimentTableModel.hxx"

#include <QWizardPage>

namespace PERSALYS
{
class PERSALYS_VIEW_API GridDesignPage : public QWizardPage
{
  Q_OBJECT

public:
  GridDesignPage(QWidget *parent = 0);

  void initialize(const Analysis& analysis);
  Analysis getAnalysis();
  bool validatePage();

signals:
  void showTime();

protected:
  void buildInterface();
  virtual void resizeEvent(QResizeEvent * event);

private:
  ResizableHeaderlessTableView * tableView_;
  ExperimentTableModel * tableModel_;
  QLabel * DOESizeLabel_;
  QLabel * DOETimeLabel_;
  TemporaryLabel * errorMessageLabel_;
};
}
#endif

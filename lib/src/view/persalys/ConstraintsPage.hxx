//                                               -*- C++ -*-
/**
 *  @brief QWizardPage to define optimization constraints
 *
 *  Copyright 2015-2024 EDF-Phimeca
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

#ifndef PERSALYS_CONSTRAINTSPAGE_HXX
#define PERSALYS_CONSTRAINTSPAGE_HXX

#include "persalys/OptimizationAnalysis.hxx"
#include "persalys/ConstraintsTableModel.hxx"
#include "persalys/TemporaryLabel.hxx"

#include <QWizardPage>
#include <QTableView>


namespace PERSALYS
{
  class PERSALYS_VIEW_API ConstraintsPage: public QWizardPage
  {
    Q_OBJECT

    friend class TestOptimizationWizard;

  public:
    ConstraintsPage(QWidget* parent = 0);
    bool validatePage() override;
    void resizeEvent(QResizeEvent *event) override;
    ConstraintsTableModel* getTableModel() const {return cstrTableModel_;};

  signals:
    void constraintsDefined();
    void removeInputLine(QModelIndex);

  public slots:
    void initialize(OptimizationAnalysis&);
    void removeInputLine();
    void addInputLine();

  private:
    void updateView();
    ConstraintsTableModel* cstrTableModel_;
    QTableView * cstrTableView_;
    TemporaryLabel* errorMessageLabel_;
  };


}
#endif

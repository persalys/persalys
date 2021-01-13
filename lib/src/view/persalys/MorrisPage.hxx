//                                               -*- C++ -*-
/**
 *  @brief QWizardPage to define Morris analysis
 *
 *  Copyright 2015-2021 EDF-Phimeca
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
#ifndef PERSALYS_MORRISPAGE_HXX
#define PERSALYS_MORRISPAGE_HXX

#include "persalys/Analysis.hxx"
#include "persalys/TemporaryLabel.hxx"
#include "persalys/UIntSpinBox.hxx"
#include "persalys/MorrisTableModel.hxx"

#include <QTableView>
#include <QWizardPage>

namespace PERSALYS
{
class PERSALYS_VIEW_API MorrisPage : public QWizardPage
{
  Q_OBJECT

public:
  MorrisPage(QWidget* parent = 0);

  void initialize(const Analysis& analysis);
  MorrisAnalysis getAnalysis() const;

  virtual int nextId() const;
  virtual bool validatePage();

protected:
  void buildInterface();

private:
  QTableView * tableView_;
  MorrisTableModel * tableModel_;
  TemporaryLabel * errorMessageLabel_;
};
class PERSALYS_VIEW_API MorrisSecondPage : public QWizardPage
{
  Q_OBJECT

public:
  MorrisSecondPage(QWidget* parent = 0);

  void initialize(const Analysis& analysis);
  int getTrajectoriesNumber() const;
  int getLevel() const;
  int getSeed() const;

protected:
  void buildInterface();

protected slots:
  void updateNbSimuLabel();

private:
  OT::UnsignedInteger nbInputs_;
  UIntSpinBox * trajNbSpinbox_;
  UIntSpinBox * levelSpinbox_;
  QSpinBox * seedSpinbox_;
  QLabel * nbSimuLabel_;
};
}
#endif

//                                               -*- C++ -*-
/**
 *  @brief QWizardPage to define Morris analysis
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
#ifndef OTGUI_MORRISPAGE_HXX
#define OTGUI_MORRISPAGE_HXX

#include "otgui/Analysis.hxx"
#include "otgui/TemporaryLabel.hxx"
#include "otgui/UIntSpinBox.hxx"
#include "otgui/MorrisTableModel.hxx"

#include <QTableView>
#include <QWizardPage>

namespace OTGUI
{
class OTGUI_API MorrisPage : public QWizardPage
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
class OTGUI_API MorrisSecondPage : public QWizardPage
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

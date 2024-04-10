//                                               -*- C++ -*-
/**
 *  @brief QWizardPage to define the limit state and the method of reliability analysis
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
#ifndef PERSALYS_INTRORELIABILITYPAGE_HXX
#define PERSALYS_INTRORELIABILITYPAGE_HXX

#include "persalys/LimitState.hxx"
#include "persalys/Analysis.hxx"

#include <QComboBox>
#include <QWizardPage>
#include <QLabel>
#include <QButtonGroup>
#include <QStandardItemModel>
#include <QMetaType> // mandatory to specify it to avoid windows compilation problem


#if QT_VERSION < 0x060000
Q_DECLARE_METATYPE(PERSALYS::LimitState)
#endif

namespace PERSALYS
{

class PERSALYS_VIEW_API ReliabilityIntroPage : public QWizardPage
{
  Q_OBJECT

public:
  enum Method {MonteCarlo, FORM_IS, FORM, SORM};

  ReliabilityIntroPage(QWidget* parent = 0);

  virtual int nextId() const;

  void initialize(const Analysis& analysis, QList<LimitState> limitStatesList);
  LimitState getLimitState() const;
  int getMethodId() const;

public slots:
  void changeLimitStateLabel(int);
signals:
  void methodChanged(int);

private:
  QComboBox * limitStatesComboBox_;
  QStandardItemModel * limitStatesComboBoxModel_;
  QLabel * limitStateLabel_;
  QButtonGroup * methodGroup_;
};
}
#endif

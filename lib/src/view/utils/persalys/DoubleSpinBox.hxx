//                                               -*- C++ -*-
/**
 *  @brief QDoubleSpinBox
 *
 *  Copyright 2015-2022 EDF-Phimeca
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
/// this class was taken here:
/// http://www.matthiaspospiech.de/blog/2009/01/03/qt-spinbox-widget-with-scientific-notation/

#ifndef PERSALYS_DOUBLESPINBOX_HXX
#define PERSALYS_DOUBLESPINBOX_HXX

#include <QDoubleSpinBox>
#include <QDoubleValidator>
#include "persalys/PersalysPrivate.hxx"

namespace PERSALYS
{
class PERSALYS_UTILS_API DoubleSpinBox : public QDoubleSpinBox
{
  Q_OBJECT

public:
  DoubleSpinBox(QWidget * parent = 0);

  int decimals() const;
  void setDecimals(int value);

  QString textFromValue(double value) const;
  double valueFromText(const QString & text) const;

private:
  static bool isIntermediateValueHelper(qint64 num, qint64 minimum, qint64 maximum, qint64 *match = 0);

  int dispDecimals;
  QChar delimiter, thousand;
  QDoubleValidator * v;

private:
  void initLocalValues(QWidget *parent);
  bool isIntermediateValue(const QString &str) const;
  QVariant validateAndInterpret(QString &input, int &pos, QValidator::State &state) const;
  QValidator::State validate(QString &text, int &pos) const;
  void fixup(QString &input) const;
  QString stripped(const QString &t, int *pos) const;
  double round(double value) const;
};
}
#endif
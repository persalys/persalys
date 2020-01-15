//                                               -*- C++ -*-
/**
 *  @brief Qt tools
 *
 *  Copyright 2015-2020 EDF-Phimeca
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
#ifndef PERSALYS_QTTOOLS_HXX
#define PERSALYS_QTTOOLS_HXX

#include "persalys/PhysicalModel.hxx"

#include <openturns/OTType.hxx>

#include <QColor>
#include <QMap>
#include <QObject>

namespace PERSALYS
{
/**
 * @class SignalBlocker
 *
 * Class to block signals of widgets.
 */
class PERSALYS_API SignalBlocker
{
  QObject* blockedObject_;
  bool previousBlockingStatus_;
public :
  SignalBlocker(QObject* blockedObject, bool block = true);
  ~SignalBlocker();
};

class PERSALYS_API SimpleException : public std::exception
{
  const QString text_;
public :
  SimpleException(const QString& text);
  virtual ~SimpleException() throw();
  QString text() const;
  virtual const char* what() const throw();
};

class PERSALYS_API QtOT
{
public:
  static QStringList DescriptionToStringList(const OT::Description & description);
  static OT::Description StringListToDescription(const QStringList& stringList);
  static QString PointToString(const OT::Point& point);
  static QStringList GetVariableAxisLabels(const PhysicalModel &model, const OT::Description &variableNames);
};

static const QMap<QString, QString> ApplicationColor{
  {"darkColor", "#003366"},
  {"mediumColor", "#336699"},
  {"lightColor", "#6699cc"}};
static const QColor DefaultHistogramColor = QColor(127, 172, 210);
static const int MaxVisibleVariableNumber = 12;
static const int RowNumberToScrollTable = 11;
}
#endif

//                                               -*- C++ -*-
/**
 *  @brief Qt tools
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
#ifndef OTGUI_QTTOOLS_HXX
#define OTGUI_QTTOOLS_HXX

#include "otgui/OTGuiprivate.hxx"
#include <QObject>
#include <openturns/OTType.hxx>

namespace OTGUI
{
/**
 * @class SignalBlocker
 *
 * Class to block signals of widgets.
 */
class OTGUI_API SignalBlocker
{
  QObject* blockedObject_;
  bool previousBlockingStatus_;
public :
  SignalBlocker(QObject* blockedObject, bool block = true);
  ~SignalBlocker();
};

class OTGUI_API SimpleException : public std::exception
{
  const QString text_;
public :
  SimpleException(const QString& text);
  virtual ~SimpleException() throw();
  QString text() const;
  virtual const char* what() const throw();
};

class OTGUI_API QtOT
{
public:
  static QStringList DescriptionToStringList(const OT::Description & description);
  static OT::Description StringListToDescription(const QStringList& stringList);
  static QString PointToString(const OT::Point& point);
};
}
#endif

//                                               -*- C++ -*-
/**
 *  @brief Qt tools
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
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "otgui/QtTools.hxx"

using namespace OT;

namespace OTGUI
{

// SignalBlocker class
SignalBlocker::SignalBlocker(QObject *blockedObject, bool block)
  : blockedObject_(blockedObject)
{
  previousBlockingStatus_ = blockedObject_->blockSignals(block);
}


SignalBlocker::~SignalBlocker()
{
  blockedObject_->blockSignals(previousBlockingStatus_);
}

// SimpleException class
SimpleException::SimpleException(const QString &text)
  : text_(text)
{
}


SimpleException::~SimpleException() throw() {}


QString SimpleException::text() const
{
  return text_;
}


const char *SimpleException::what() const throw()
{
  return text_.toStdString().c_str();
}


QStringList QtOT::DescriptionToStringList(const Description& description)
{
  UnsignedInteger size(description.getSize());
  QStringList result;
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    result.append(QString::fromUtf8(description[i].c_str()));
  }
  return result;
}


Description QtOT::StringListToDescription(const QStringList& stringList)
{
  Description result(stringList.size());
  for (int i = 0; i < stringList.size(); ++i)
  {
    result[i] = stringList[i].toUtf8().constData();
  }
  return result;
}


QString QtOT::PointToString(const Point& point)
{
  QString resu;
  for (UnsignedInteger j = 0; j < point.getSize(); ++j)
  {
    resu += QString::number(point[j]);
    if (j < point.getSize() - 1)
      resu += "; ";
  }
  return resu;
}
}

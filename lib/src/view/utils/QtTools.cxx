//                                               -*- C++ -*-
/**
 *  @brief Qt tools
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
#include "persalys/QtTools.hxx"

#include <QTranslator>

using namespace OT;

namespace PERSALYS
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
  const std::string message(text_.toStdString());
  const std::string::size_type size = message.size();
  const char *src = message.c_str();
  char *buffer = new char[size + 1];
  std::copy(src, src + size + 1, buffer);
  return buffer;
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


QStringList QtOT::GetVariableAxisLabels(const PhysicalModel &model, const Description &variableNames)
{
  QStringList labels;
  for (UnsignedInteger i = 0; i < variableNames.getSize(); ++i)
  {
    const String varName(variableNames[i]);

    // get description
    QString varDescription;
    if (model.hasInputNamed(varName))
      varDescription = QString::fromUtf8(model.getInputByName(varName).getDescription().c_str());
    else if (model.hasOutputNamed(varName))
      varDescription = QString::fromUtf8(model.getOutputByName(varName).getDescription().c_str());

    labels << (varDescription.isEmpty() ? QString::fromUtf8(varName.c_str()) : varDescription);
  }
  return labels;
}

QString QtOT::FormatDuration(double seconds)
{
  // convert seconds
  double sec = std::fmod(seconds,60.);
  int minutes = seconds / 60;
  int hours = minutes / 60;
  minutes %= 60;
  int days = hours / 24;
  hours %= 24;

  // set text: XXX d XX h XX m XX s || XXX ms
  QString duration;
  if (days)
  {
    duration += QString::number(days);
    duration += QT_TR_NOOP("d");
  }
  if (hours)
  {
    if (days)
      duration += " ";
    duration += QString::number(hours);
    duration += "h";
  }
  if (minutes)
  {
    if (hours || days)
      duration += " ";
    duration += QString::number(minutes);
    duration += "m";
  }
  if (sec && !days) // Conditional seconds display
  {
    if (sec < 1.) // Display ms
    {
      duration += QString::number((int)(sec*1000));
      duration += "ms";
    }
    else
    {
      int precision = 3;
      if (seconds > 60.) // Round seconds if > 1 mn and add space after minutes
      {
        precision = 2;
        if (days || hours || minutes)
          duration += " ";
      }
      duration += QString::number(sec, 'g', precision);
      duration += "s";
    }
  }
  return duration;
}

}

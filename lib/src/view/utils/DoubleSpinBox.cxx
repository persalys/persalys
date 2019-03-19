//                                               -*- C++ -*-
/**
 *  @brief QDoubleSpinBox
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
#include "otgui/DoubleSpinBox.hxx"

#include <QLineEdit>
#include <QDebug>

#include <limits>

namespace OTGUI
{

DoubleSpinBox::DoubleSpinBox(QWidget * parent)
  : QDoubleSpinBox(parent)
  , dispDecimals(15)
  , delimiter(' ')
  , thousand(' ')
  , v(0)
{
  initLocalValues(parent);

  QDoubleSpinBox::setDecimals(15);
  setDecimals(15);

  // set Range to maximum possible values
  double doubleMax = std::numeric_limits<double>::max();
  setRange(-doubleMax, doubleMax);

  v = new QDoubleValidator(this);
  v->setLocale(QLocale::C);
  v->setDecimals(1000); // (standard anyway)
  v->setNotation(QDoubleValidator::ScientificNotation);
  this->lineEdit()->setValidator(v);
}


void DoubleSpinBox::initLocalValues(QWidget *parent)
{
  setLocale(QLocale::C);
  locale().setNumberOptions(QLocale::OmitGroupSeparator | QLocale::RejectGroupSeparator);
//   const QString str = (parent ? parent->locale() : locale()).toString(4567.1);
  // for now use only QLocale::C
  const QString str = locale().toString(4567.1);
  if (str.size() == 6)
  {
    delimiter = str.at(4);
    thousand = QChar((ushort)0);
  }
  else if (str.size() == 7)
  {
    thousand = str.at(1);
    delimiter = str.at(5);
  }
  Q_ASSERT(!delimiter.isNull());
}


int DoubleSpinBox::decimals() const
{
  return dispDecimals;
}


void DoubleSpinBox::setDecimals(int value)
{
  dispDecimals = value;
}


QString DoubleSpinBox::textFromValue(double value) const
{
  // convert to string -> Using exponential display with internal decimals
  QString str = locale().toString(value, 'g', dispDecimals);
  // remove thousand sign
  if (qAbs(value) >= 1000.0)
  {
    str.remove(thousand);
  }
  return str;
}


double DoubleSpinBox::valueFromText(const QString &text) const
{
  QString copy = text;
  int pos = this->lineEdit()->cursorPosition();
  QValidator::State state = QValidator::Acceptable;
  return validateAndInterpret(copy, pos, state).toDouble();
}


// this function is never used...?
double DoubleSpinBox::round(double value) const
{
  const QString strDbl = locale().toString(value, 'g', dispDecimals);
  return locale().toDouble(strDbl);
}


// overwritten virtual function of QAbstractSpinBox
QValidator::State DoubleSpinBox::validate(QString &text, int &pos) const
{
  QValidator::State state;
  validateAndInterpret(text, pos, state);
  return state;
}


// overwritten virtual function of QAbstractSpinBox
void DoubleSpinBox::fixup(QString &input) const
{
  input.remove(thousand);
}


// reimplemented function, copied from QDoubleSpinBoxPrivate::isIntermediateValue
bool DoubleSpinBox::isIntermediateValue(const QString &str) const
{
//   qDebug() << "input is" << str << minimum() << maximum();
  qint64 dec = 1;

  for (int i = 0; i < decimals(); ++i)
    dec *= 10;

  const QLatin1Char dot('.');

  /*!
  * determine minimum possible values on left and right of Decimal-char
  */
  // I know QString::number() uses CLocale so I use dot
  const QString minstr = QString::number(minimum(), 'f', QDoubleSpinBox::decimals());
  qint64 min_left = minstr.left(minstr.indexOf(dot)).toLongLong();
  qint64 min_right = minstr.mid(minstr.indexOf(dot) + 1).toLongLong();

  const QString maxstr = QString::number(maximum(), 'f', QDoubleSpinBox::decimals());
  qint64 max_left = maxstr.left(maxstr.indexOf(dot)).toLongLong();
  qint64 max_right = maxstr.mid(maxstr.indexOf(dot) + 1).toLongLong();

  /*!
    * determine left and right long values (left and right of delimiter)
    */
  const int dotindex = str.indexOf(delimiter);
  const bool negative = maximum() < 0;
  qint64 left = 0, right = 0;
  bool doleft = true;
  bool doright = true;
  // no separator -> everything in left
  if (dotindex == -1)
  {
    left = str.toLongLong();
    doright = false;
  }
  // separator on left or contains '+'
  else if (dotindex == 0 || (dotindex == 1 && str.at(0) == QLatin1Char('+')))
  {
    // '+' at negative max
    if (negative)
    {
//       qDebug() << __FILE__ << __LINE__ << "returns false";
      return false;
    }
    doleft = false;
    right = str.mid(dotindex + 1).toLongLong();
  }
  // contains '-'
  else if (dotindex == 1 && str.at(0) == QLatin1Char('-'))
  {
    // '-' at positive max
    if (!negative)
    {
//       qDebug() << __FILE__ << __LINE__ << "returns false";
      return false;
    }
    doleft = false;
    right = str.mid(dotindex + 1).toLongLong();
  }
  else
  {
    left = str.left(dotindex).toLongLong();
    if (dotindex == str.size() - 1)
    {
      // nothing right of Separator
      doright = false;
    }
    else
    {
      right = str.mid(dotindex + 1).toLongLong();
    }
  }
  // left > 0, with max < 0 and no '-'
  if ((left >= 0 && max_left < 0 && !str.startsWith(QLatin1Char('-')))
      // left > 0, with min > 0
      || (left < 0 && min_left >= 0))
  {
//     qDebug("returns false");
    return false;
  }

  qint64 match = min_left;
  if (doleft && !isIntermediateValueHelper(left, min_left, max_left, &match))
  {
//     qDebug() << __FILE__ << __LINE__ << "returns false";
    return false;
  }
  if (doright)
  {
//     qDebug("match %lld min_left %lld max_left %lld", match, min_left, max_left);
    if (!doleft)
    {
      if (min_left == max_left)
      {
        const bool ret = isIntermediateValueHelper(qAbs(left),
                         negative ? max_right : min_right,
                         negative ? min_right : max_right);
//         qDebug() << __FILE__ << __LINE__ << "returns" << ret;
        return ret;
      }
      else if (qAbs(max_left - min_left) == 1)
      {
        const bool ret = isIntermediateValueHelper(qAbs(left), min_right, negative ? 0 : dec)
                         || isIntermediateValueHelper(qAbs(left), negative ? dec : 0, max_right);
//         qDebug() << __FILE__ << __LINE__ << "returns" << ret;
        return ret;
      }
      else
      {
        const bool ret = isIntermediateValueHelper(qAbs(left), 0, dec);
//         qDebug() << __FILE__ << __LINE__ << "returns" << ret;
        return ret;
      }
    }
    if (match != min_left)
    {
      min_right = negative ? dec : 0;
    }
    if (match != max_left)
    {
      max_right = negative ? 0 : dec;
    }
    qint64 tmpl = negative ? max_right : min_right;
    qint64 tmpr = negative ? min_right : max_right;
    const bool ret = isIntermediateValueHelper(right, tmpl, tmpr);
//     qDebug() << __FILE__ << __LINE__ << "returns" << ret;
    return ret;
  }
//   qDebug() << __FILE__ << __LINE__ << "returns true";
  return true;
}


/*!
    \internal Multi purpose function that parses input, sets state to
    the appropriate state and returns the value it will be interpreted
    as.
*/
// reimplemented function, copied from QDoubleSpinBoxPrivate::validateAndInterpret
QVariant DoubleSpinBox::validateAndInterpret(QString &input,
    int &pos,
    QValidator::State &state) const
{
  /*! return 'cachedText' if
   *   input = cachedText, or input Empty
   */

  static QString cachedText;
  static QValidator::State cachedState;
  static QVariant cachedValue;

  if (cachedText == input && !input.isEmpty())
  {
    state = cachedState;
//     qDebug() << "cachedText was" << "'" << cachedText << "'" << "state was "
//              << state << " and value was " << cachedValue;
    return cachedValue;
  }
  const double max = maximum();
  const double min = minimum();

// removes prefix & suffix
  QString copy = stripped(input, &pos);
//   qDebug() << "input" << input << "copy" << copy;

  int len = copy.size();
  double num = min;
  const bool plus = max >= 0;
  const bool minus = min <= 0;

  // Test possible 'Intermediate' reasons
  switch (len)
  {
    case 0:
      // Length 0 is always 'Intermediate', except for min=max
      if (max != min)
      {
        state = QValidator::Intermediate;
      }
      else
      {
        state = QValidator::Invalid;
      }
      goto end;
    case 1:
      // if only char is '+' or '-'
      if (copy.at(0) == delimiter
          || (plus && copy.at(0) == QLatin1Char('+'))
          || (minus && copy.at(0) == QLatin1Char('-')))
      {
        state = QValidator::Intermediate;
        goto end;
      }
      break;
    case 2:
      // if only chars are '+' or '-' followed by Comma separator (delimiter)
      if (copy.at(1) == delimiter
          && ((plus && copy.at(0) == QLatin1Char('+')) || (minus && copy.at(0) == QLatin1Char('-'))))
      {
        state = QValidator::Intermediate;
        goto end;
      }
      break;
    default:
      break;
  } // end switch


  // First char must not be thousand-char
  if (copy.at(0) == thousand)
  {
//     qDebug() << __FILE__ << __LINE__<< "state is set to Invalid";
    state = QValidator::Invalid;
    goto end;
  }
  // Test possible 'Invalid' reasons
  else if (len > 1)
  {
    const int dec = copy.indexOf(delimiter); // position of delimiter
    // if decimal separator (delimiter) exists
    if (dec != -1)
    {
      // not two delimiters after one other (meaning something like ',,')
      if (dec + 1 < copy.size() && copy.at(dec + 1) == delimiter && pos == dec + 1)
      {
        copy.remove(dec + 1, 1); // typing a delimiter when you are on the delimiter
      }              // should be treated as typing right arrow
      // too many decimal points
      if (copy.size() - dec > QDoubleSpinBox::decimals() + 1)
      {
//         qDebug() << __FILE__ << __LINE__<< "state is set to Invalid";
        state = QValidator::Invalid;
        goto end;
      }
      // after decimal separator no thousand char
      for (int i = dec + 1; i < copy.size(); ++i)
      {
        if (copy.at(i).isSpace() || copy.at(i) == thousand)
        {
//           qDebug() << __FILE__ << __LINE__<< "state is set to Invalid";
          state = QValidator::Invalid;
          goto end;
        }
      }
      // if no decimal separator exists
    }
    else
    {
      const QChar &last = copy.at(len - 1);
      const QChar &secondLast = copy.at(len - 2);
      // group of two thousand or space chars is invalid
      if ((last == thousand || last.isSpace())
          && (secondLast == thousand || secondLast.isSpace()))
      {
        state = QValidator::Invalid;
//         qDebug() << __FILE__ << __LINE__<< "state is set to Invalid";
        goto end;
      }
      // two space chars is invalid
      else if (last.isSpace() && (!thousand.isSpace() || secondLast.isSpace()))
      {
        state = QValidator::Invalid;
//         qDebug() << __FILE__ << __LINE__<< "state is set to Invalid";
        goto end;
      }
    }
  } // end if (len > 1)

  // block of remaining test before 'end' mark
  {
    bool ok = false;
    bool notAcceptable = false;

    // convert 'copy' to double, and check if that was 'ok'
    QLocale loc(locale());
    num = loc.toDouble(copy, &ok);
//     qDebug() << __FILE__ << __LINE__ << loc << copy << num << ok;

    // conversion to double did fail
    if (!ok)
    {
      // maybe thousand char was responsable
      if (thousand.isPrint())
      {
        // if no thousand sign is possible, then
        // something else is responable -> Invalid
        if (max < 1000 && min > -1000 && copy.contains(thousand))
        {
          state = QValidator::Invalid;
//             qDebug() << __FILE__ << __LINE__<< "state is set to Invalid";
          goto end;
        }

        // two thousand-chars after one other are not valid
        len = copy.size();
        for (int i = 0; i < len - 1; ++i)
        {
          if (copy.at(i) == thousand && copy.at(i + 1) == thousand)
          {
//             qDebug() << __FILE__ << __LINE__<< "state is set to Invalid";
            state = QValidator::Invalid;
            goto end;
          }
        }

        // remove thousand-chars
        const int s = copy.size();
        copy.remove(thousand);
        pos = qMax(0, pos - (s - copy.size()));

        num = loc.toDouble(copy, &ok);
        qDebug() << thousand << num << copy << ok;

        // if conversion still not valid, then reason unknown -> Invalid
        if (!ok)
        {
          state = QValidator::Invalid;
//           qDebug() << __FILE__ << __LINE__<< "state is set to Invalid";
          goto end;
        }
        notAcceptable = true; // -> state = Intermediate
      } // endif: (thousand.isPrint())
    }

    // no thousand sign, but still invalid for unknown reason
    if (!ok)
    {
      state = QValidator::Invalid;
//       qDebug() << __FILE__ << __LINE__<< "state is set to Invalid";
    }
    // number valid and within valid range
    else if (num >= min && num <= max)
    {
      if (notAcceptable)
      {
        state = QValidator::Intermediate; // conversion to num initially failed
      }
      else
      {
        state = QValidator::Acceptable;
      }
//       qDebug() << __FILE__ << __LINE__<< "state is set to "
//                   << (state == QValidator::Intermediate ? "Intermediate" : "Acceptable");
    }
    // when max and min is the same the only non-Invalid input is max (or min)
    else if (max == min)
    {
      state = QValidator::Invalid;
//       qDebug() << __FILE__ << __LINE__<< "state is set to Invalid";
    }
    else
    {
      // value out of valid range (coves only special cases)
      if ((num >= 0 && num > max) || (num < 0 && num < min))
      {
        state = QValidator::Invalid;
//         qDebug() << __FILE__ << __LINE__<< "state is set to Invalid";
      }
      else
      {
        // invalid range, further test with 'isIntermediateValue'
        if (isIntermediateValue(copy))
        {
          state =  QValidator::Intermediate;
        }
        else
        {
          state =  QValidator::Invalid;
        }
//         qDebug() << __FILE__ << __LINE__<< "state is set to "
//                  << (state == QValidator::Intermediate ? "Intermediate" : "Acceptable");
      }
    }
  }

end:
  // if something went wrong, set num to something valid
  if (state != QValidator::Acceptable)
  {
    num = max > 0 ? min : max;
  }

  // save (private) cache values
  cachedText = prefix() + copy + suffix();
  cachedState = state;
  cachedValue = QVariant(num);
  // return resulting valid num
  return QVariant(num);
}


/*!
    \internal
    Strips any prefix/suffix from \a text.
*/
// reimplemented function, copied from QAbstractSpinBoxPrivate::stripped
QString DoubleSpinBox::stripped(const QString &t, int *pos) const
{
  QString text = t;
  QString prefixtext = prefix();
  QString suffixtext = suffix();

  if (specialValueText().size() == 0 || text != specialValueText())
  {
    int from = 0;
    int size = text.size();
    bool changed = false;
    if (prefixtext.size() && text.startsWith(prefixtext))
    {
      from += prefixtext.size();
      size -= from;
      changed = true;
    }
    if (suffixtext.size() && text.endsWith(suffixtext))
    {
      size -= suffixtext.size();
      changed = true;
    }
    if (changed)
      text = text.mid(from, size);
  }

  const int s = text.size();
  text = text.trimmed();
  if (pos)
    (*pos) -= (s - text.size());
  return text;
}


// reimplemented function, copied from qspinbox.cpp
bool DoubleSpinBox::isIntermediateValueHelper(qint64 num, qint64 min, qint64 max, qint64 *match)
{
//   qDebug("%lld %lld %lld", num, min, max);

  if (num >= min && num <= max)
  {
    if (match)
      *match = num;
//     qDebug("returns true 0");
    return true;
  }
  qint64 tmp = num;

  int numDigits = 0;
  int digits[10];
  if (tmp == 0)
  {
    numDigits = 1;
    digits[0] = 0;
  }
  else
  {
    tmp = qAbs(num);
    for (int i = 0; tmp > 0; ++i)
    {
      digits[numDigits++] = tmp % 10;
      tmp /= 10;
    }
  }

  int failures = 0;
  qint64 number;
  for (number = max; number >= min; --number)
  {
    tmp = qAbs(number);
    for (int i = 0; tmp > 0;)
    {
      if (digits[i] == (tmp % 10))
      {
        if (++i == numDigits)
        {
          if (match)
            *match = number;
//           qDebug("returns true 1");
          return true;
        }
      }
      tmp /= 10;
    }
    if (failures++ == 500000)
    {
      //upper bound
      if (match)
        *match = num;
//       qDebug("returns true 2");
      return true;
    }
  }
//   qDebug("returns false");
  return false;
}
}

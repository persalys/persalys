//                                               -*- C++ -*-
/**
 *  @brief base tools
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
#include "persalys/BaseTools.hxx"

#include <boost/locale.hpp>

#include <stdint.h>
#include <chrono>

using namespace OT;

namespace PERSALYS
{

Parameters::Parameters()
  : pairsCollection_()
{
}


Parameters::~Parameters()
{
}


void Parameters::add(const String& name, const String& value)
{
  std::pair<String, String> newPair;
  newPair.first = name;
  newPair.second = value;
  pairsCollection_.add(newPair);
}


void Parameters::add(const String& name, const double& value)
{
  add(name, OSS() << value);
}


void Parameters::add(const String& name, const UnsignedInteger& value)
{
  add(name, OSS() << value);
}


void Parameters::add(const String& name, const Point& values)
{
  add(name, GetOTPointStr(values, ";"));
}


void Parameters::add(const Parameters& parameters)
{
  for (UnsignedInteger i = 0; i < parameters.getSize(); ++i)
    add(parameters[i].first, parameters[i].second);
}


UnsignedInteger Parameters::getSize() const
{
  return pairsCollection_.getSize();
}


std::pair<String, String> Parameters::operator[](const UnsignedInteger index) const
{
  return pairsCollection_[index];
}


bool Parameters::operator==(const Parameters& other) const
{
  if (getSize() != other.getSize())
    return false;
  bool equality = true;
  for (UnsignedInteger i = 0; i < getSize(); ++i)
  {
    equality &= (pairsCollection_[i].first == other[i].first);
    equality &= (pairsCollection_[i].second == other[i].second);
    if (!equality)
      return false;
  }
  return true;
}


String Parameters::GetOTSampleStr(const Sample& values)
{
  OSS sampleOss;
  sampleOss << "[";
  for (UnsignedInteger i = 0; i < values.getSize(); ++i)
  {
    sampleOss << "[";
    Point pt(values[i]);
    std::copy(pt.begin(), pt.end(), OSS_iterator<Scalar>(sampleOss, ","));
    sampleOss << "]";
    if (i < (values.getSize() - 1))
      sampleOss << ",\n";
  }
  sampleOss << "]\n";

  return sampleOss;
}


String Parameters::GetOTPointStr(const Point& values, const String& separator)
{
  String valuesStr = "[";
  for (UnsignedInteger i = 0; i < values.getSize(); ++i)
  {
    valuesStr += OSS() << values[i];
    if (i < values.getSize() - 1)
      valuesStr += separator + " ";
  }
  valuesStr += "]";
  return valuesStr;
}


String Parameters::GetOTIndicesStr(const Indices& values)
{
  String valuesStr = "[";
  for (UnsignedInteger i = 0; i < values.getSize(); ++i)
  {
    valuesStr += OSS() << values[i];
    if (i < values.getSize() - 1)
      valuesStr += ", ";
  }
  valuesStr += "]";
  return valuesStr;
}


String Parameters::GetOTDescriptionStr(const Description& values, const bool quote)
{
  String valuesStr = "[";
  for (UnsignedInteger i = 0; i < values.getSize(); ++i)
  {
    valuesStr += OSS() << (quote ? "'" : "") << values[i] << (quote ? "'" : "");
    if (i < values.getSize() - 1)
      valuesStr += ", ";
  }
  valuesStr += "]";
  return valuesStr;
}


String Parameters::GetOTBoolCollectionStr(const Interval::BoolCollection& values)
{
  String valuesStr = "[";
  for (UnsignedInteger i = 0; i < values.getSize(); ++i)
  {
    valuesStr += values[i] ? "True" : "False";
    if (i < values.getSize() - 1)
      valuesStr += ", ";
  }
  valuesStr += "]";
  return valuesStr;
}


String Parameters::GetOTCorrelationMatrixStr(const CorrelationMatrix &correlationMatrix)
{
  OSS oss;
  oss.setPrecision(12);

  oss << "R = ot.CorrelationMatrix(" << correlationMatrix.getNbRows() << ")\n";

  for (UnsignedInteger row = 0; row < correlationMatrix.getNbRows(); ++ row)
  {
    for (UnsignedInteger col = row + 1; col < correlationMatrix.getNbRows(); ++ col)
    {
      if (correlationMatrix(row, col) != 0.0)
      {
        oss << "R[" << row << ", " << col << "] = " << correlationMatrix(row, col) << "\n";
      }
    }
  }
  return oss;
}


String Parameters::GetOTNormalCopulaStr(const Distribution &distribution)
{
  OSS oss;
  if (distribution.getImplementation()->getClassName() != "NormalCopula")
    return oss;
  oss <<  GetOTCorrelationMatrixStr(distribution.getCorrelation());
  oss << "copula = ot.NormalCopula(ot.NormalCopula.GetCorrelationFromSpearmanCorrelation(R))\n";
  return oss;
}


Description Parameters::GetOTIntervalDescription(const Interval& interval)
{
  Description resu(interval.getDimension());

  for (UnsignedInteger i = 0; i < interval.getDimension(); ++i)
  {
    String intervalStr_i;

    intervalStr_i += (interval.getFiniteLowerBound()[i] ? "[" : "]");
    if (interval.getFiniteLowerBound()[i])
      intervalStr_i += OSS() << interval.getLowerBound()[i];
    else
      intervalStr_i += "-∞";
    intervalStr_i += ", ";
    if (interval.getFiniteUpperBound()[i])
      intervalStr_i += OSS() << interval.getUpperBound()[i];
    else
      intervalStr_i += "+∞";
    intervalStr_i += (interval.getFiniteUpperBound()[i] ? "]" : "[");

    resu[i] = intervalStr_i;
  }
  return resu;
}


Sample Tools::ImportSample(const String& fileName)
{
  std::vector< String > separatorsList(3);
  separatorsList[0] = " ";
  separatorsList[1] = ",";
  separatorsList[2] = ";";
  Sample sampleFromFile;

  for (UnsignedInteger i = 0; i < separatorsList.size(); ++ i)
  {
    // import sample from the file
    sampleFromFile = Sample::ImportFromTextFile(fileName, separatorsList[i]);
    if (sampleFromFile.getSize())
      break;
  }
  if (!sampleFromFile.getSize())
    throw InvalidArgumentException(HERE) << "The file " << fileName << " does not contain a sample and/or the file encoding is not valid (use utf-8)";

  // check description encoding : only utf-8 is valid
  const String descriptionStr = sampleFromFile.getDescription().__str__();
  if (!IsUTF8(descriptionStr.c_str(), descriptionStr.size()))
    throw InvalidArgumentException(HERE) << "The file must contain utf-8 characters";

  return sampleFromFile;
}


String Tools::GetLocaleString(const String& str)
{
  boost::locale::generator g;
  g.locale_cache_enabled(true);
  std::locale loc = g(boost::locale::util::get_system_locale());

  return boost::locale::conv::from_utf<char>(str, loc);;
}


// code found at https://unicodebook.readthedocs.io/guess_encoding.html#is-utf-8
int Tools::IsUTF8(const char *data, size_t size)
{
  const unsigned char *str = (unsigned char*)data;
  const unsigned char *end = str + size;
  unsigned char byte;
  unsigned int code_length, i;
  uint32_t ch;
  while (str != end)
  {
    byte = *str;
    if (byte <= 0x7F)
    {
      /* 1 byte sequence: U+0000..U+007F */
      str += 1;
      continue;
    }

    if (0xC2 <= byte && byte <= 0xDF)
      /* 0b110xxxxx: 2 bytes sequence */
      code_length = 2;
    else if (0xE0 <= byte && byte <= 0xEF)
      /* 0b1110xxxx: 3 bytes sequence */
      code_length = 3;
    else if (0xF0 <= byte && byte <= 0xF4)
      /* 0b11110xxx: 4 bytes sequence */
      code_length = 4;
    else
    {
      /* invalid first byte of a multibyte character */
      return 0;
    }

    if (str + (code_length - 1) >= end)
    {
      /* truncated string or invalid byte sequence */
      return 0;
    }

    /* Check continuation bytes: bit 7 should be set, bit 6 should be
     * unset (b10xxxxxx). */
    for (i=1; i < code_length; i++)
    {
      if ((str[i] & 0xC0) != 0x80)
        return 0;
    }

    if (code_length == 2)
    {
      /* 2 bytes sequence: U+0080..U+07FF */
      ch = ((str[0] & 0x1f) << 6) + (str[1] & 0x3f);
      /* str[0] >= 0xC2, so ch >= 0x0080.
               str[0] <= 0xDF, (str[1] & 0x3f) <= 0x3f, so ch <= 0x07ff */
    }
    else if (code_length == 3)
    {
      /* 3 bytes sequence: U+0800..U+FFFF */
      ch = ((str[0] & 0x0f) << 12) + ((str[1] & 0x3f) << 6) +
          (str[2] & 0x3f);
      /* (0xff & 0x0f) << 12 | (0xff & 0x3f) << 6 | (0xff & 0x3f) = 0xffff,
               so ch <= 0xffff */
      if (ch < 0x0800)
        return 0;

      /* surrogates (U+D800-U+DFFF) are invalid in UTF-8:
               test if (0xD800 <= ch && ch <= 0xDFFF) */
      if ((ch >> 11) == 0x1b)
        return 0;
    }
    else if (code_length == 4)
    {
      /* 4 bytes sequence: U+10000..U+10FFFF */
      ch = ((str[0] & 0x07) << 18) + ((str[1] & 0x3f) << 12) +
          ((str[2] & 0x3f) << 6) + (str[3] & 0x3f);
      if ((ch < 0x10000) || (0x10FFFF < ch))
        return 0;
    }
    str += code_length;
  }
  return 1;
}


void Tools::ComputeBounds(const InputCollection& inputs, Point& startingPoint, Interval& bounds)
{
  const UnsignedInteger nbInputs = inputs.getSize();
  startingPoint = Point(nbInputs);
  bounds = Interval(nbInputs);

  Point lowerBounds(nbInputs);
  Point upperBounds(nbInputs);

  for (UnsignedInteger i = 0; i < nbInputs; ++i)
  {
    startingPoint[i] = inputs[i].getValue();

    if (!inputs[i].isStochastic())
    {
      lowerBounds[i] = -0.1;
      upperBounds[i] = 0.1;
      if (startingPoint[i] != 0)
      {
        lowerBounds[i] = startingPoint[i] - 0.1 * std::abs(startingPoint[i]);
        upperBounds[i] = startingPoint[i] + 0.1 * std::abs(startingPoint[i]);
      }
    }
    else
    {
      const Distribution distribution = inputs[i].getDistribution();
      // lower bound
      if (distribution.getRange().getFiniteLowerBound()[0])
        lowerBounds[i] = distribution.getRange().getLowerBound()[0];
      else
        lowerBounds[i] = distribution.computeQuantile(0.05)[0];
      // upper bound
      if (distribution.getRange().getFiniteUpperBound()[0])
        upperBounds[i] = distribution.getRange().getUpperBound()[0];
      else
        upperBounds[i] = distribution.computeQuantile(0.95)[0];

      // check if the interval contains the starting point
      if (!Interval(lowerBounds[i], upperBounds[i]).contains(Point(1, startingPoint[i])))
        startingPoint[i] = (upperBounds[i] + lowerBounds[i]) * 0.5;
    }
  }
  bounds.setLowerBound(lowerBounds);
  bounds.setUpperBound(upperBounds);
}


// TimeCriteria methods

void TimeCriteria::setStartTime(const Scalar startTime)
{
  stopRequested_ = false;
  startTime_ = startTime;
}
Scalar TimeCriteria::getStartTime() const
{
  return startTime_;
}
void TimeCriteria::setMaxElapsedTime(const Scalar seconds)
{
  maximumElapsedTime_ = seconds;
}
void TimeCriteria::stop()
{
  stopRequested_ = true;
}
void TimeCriteria::incrementElapsedTime()
{
  elapsedTime_ = Now() - startTime_;
}
Scalar TimeCriteria::getElapsedTime() const
{
  return elapsedTime_;
}
Scalar TimeCriteria::Now()
{
  std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
  return 1e-3 * std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
}
}

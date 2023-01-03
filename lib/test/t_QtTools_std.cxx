//                                               -*- C++ -*-
/**
 *  @brief The test file for QtTools
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

#include <QtTest/QtTest>

using namespace OT;

namespace PERSALYS
{
class TestQtTools : public QObject
{
  Q_OBJECT
public:
  TestQtTools()
  {}
private slots:
  void testConversion()
  {
    double duration = 0.834;
    QVERIFY2(QtOT::FormatDuration(duration) == "834ms", "The strings must be equal");
    duration += 1.;
    QVERIFY2(QtOT::FormatDuration(duration) == QString("1.83s"), "The strings must be equal");
    duration += 10.;
    QVERIFY2(QtOT::FormatDuration(duration) == QString("11.8s"), "The strings must be equal");
    duration += 360.;
    QVERIFY2(QtOT::FormatDuration(duration) == QString("6m 12s"), "The strings must be equal");
    duration += 7200.;
    QVERIFY2(QtOT::FormatDuration(duration) == QString("2h 6m 12s"), "The strings must be equal");
    duration += 86400.;
    QVERIFY2(QtOT::FormatDuration(duration) == QString("1d 2h 6m"), "The strings must be equal");
  }
};
}

QTEST_MAIN(PERSALYS::TestQtTools)
#include "t_QtTools_std.moc"

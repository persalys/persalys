/**
 *  @brief QObject to translate words
 *
 *  Copyright 2015-2017 EDF-Phimeca
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
#ifndef OTGUI_TRANSLATIONMANAGER_HXX
#define OTGUI_TRANSLATIONMANAGER_HXX

#include "otgui/OTGuiprivate.hxx"

#include <QObject>

#ifndef Q_MOC_RUN
#include <boost/bimap.hpp>
#endif

namespace OTGUI
{
class OTGUI_API TranslationManager : public QObject
{
  Q_OBJECT

public:
#ifndef Q_MOC_RUN
  typedef boost::bimap< QString, QString > bimap_type;
  typedef bimap_type::value_type type;
#endif

  static QString GetTranslatedDistributionName(const std::string& name);
  static QString GetTranslatedDistributionParameterName(const std::string& name);
  static std::string GetDistributionName(const QString& name);
  static QStringList GetAvailableDistributions();

private:
  static void InitializeDistributionsNames();
  static void InitializeDistributionsParametersNames();
#ifndef Q_MOC_RUN
  static bimap_type DistributionsNames_;
  static bimap_type DistributionsParametersNames_;
#endif
};
}
#endif

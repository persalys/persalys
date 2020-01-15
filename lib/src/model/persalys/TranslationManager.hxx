/**
 *  @brief QObject to translate words
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
#ifndef PERSALYS_TRANSLATIONMANAGER_HXX
#define PERSALYS_TRANSLATIONMANAGER_HXX

#include "persalys/PersalysPrivate.hxx"

#include <QObject>

#ifndef Q_MOC_RUN
#include <boost/bimap.hpp>
#endif

namespace PERSALYS
{
class PERSALYS_API TranslationManager : public QObject
{
  Q_OBJECT

public:
#ifndef Q_MOC_RUN
  typedef boost::bimap< QString, QString > bimap_type;
  typedef bimap_type::value_type type;
#endif

  static QString GetTranslatedDistributionName(const std::string& name);
  static QString GetTranslatedCopulaName(const std::string& name);
  static QString GetTranslatedDistributionParameterName(const std::string& name);
  static std::string GetDistributionName(const QString& name);
  static std::string GetCopulaName(const QString& name);
  static QStringList GetTranslatedContinuousDistributions();
  static QStringList GetTranslatedDiscreteDistributions();
  static QStringList GetAvailableCopulas();
  static QString GetTranslatedParameterName(const std::string& name);
  static QString GetTranslatedErrorMessage(const std::string& message);

private:
  static void InitializeDistributionsNames();
  static void InitializeCopulasNames();
  static void InitializeDistributionsParametersNames();
  static void InitializeParametersNames();
  static void InitializeErrorMessages();
#ifndef Q_MOC_RUN
  static bimap_type DistributionsNames_;
  static bimap_type CopulasNames_;
  static bimap_type DistributionsParametersNames_;
  static bimap_type ParametersNames_;
  static bimap_type ErrorMessages_;
#endif
};
}
#endif

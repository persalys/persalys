//                                               -*- C++ -*-
/**
 *  @brief Specific QWizard for persalys
 *
 *  Copyright 2015-2024 EDF-Phimeca
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
#ifndef PERSALYS_WIZARD_HXX
#define PERSALYS_WIZARD_HXX

#include "persalys/Analysis.hxx"
#include "persalys/DesignOfExperiment.hxx"
#include "persalys/DesignOfExperimentDefinitionItem.hxx"

#include <QWizard>
#include <QMetaType> // mandatory to specify it to avoid windows compilation problem

#include "persalys/QtTools.hxx" // for Q_DECLARE_METATYPE(PERSALYS::DesignOfExperiment)
                                //     Q_DECLARE_METATYPE(PERSALYS::DesignOfExperimentDefinitionItem*)

namespace PERSALYS
{
class PERSALYS_VIEW_API Wizard : public QWizard
{
  Q_OBJECT

public:
  Wizard(QWidget *parent = nullptr);

public slots:
  void showHelp();
signals:
  void analysisChanged(const Analysis & analysis);

protected:
  QString docLink_ = "";
};
}
#endif

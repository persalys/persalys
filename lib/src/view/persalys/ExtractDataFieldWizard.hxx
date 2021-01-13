//                                               -*- C++ -*-
/**
 *  @brief QWizard to extract data of a Field
 *
 *  Copyright 2015-2021 EDF-Phimeca
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
#ifndef PERSALYS_EXTRACTDATAFIELDWIZARD_HXX
#define PERSALYS_EXTRACTDATAFIELDWIZARD_HXX

#include "persalys/FieldMonteCarloAnalysis.hxx"
#include "persalys/Analysis.hxx"
#include "persalys/DataModel.hxx"
#include "persalys/TemporaryLabel.hxx"

#include <QDialog>

namespace PERSALYS
{

class PERSALYS_VIEW_API ExtractDataFieldWizard : public QDialog
{
  Q_OBJECT

public:

  ExtractDataFieldWizard(const Analysis& analysis, QWidget *parent = 0);

  DataModel getDataModel();

public slots:
  virtual void done(int r);
  void updateSelectedNodes(const QList<int>& nodes);

private:
  FieldMonteCarloAnalysis * analysis_;
  QList<int> nodes_;
  TemporaryLabel * errorMessageLabel_;
};
}
#endif

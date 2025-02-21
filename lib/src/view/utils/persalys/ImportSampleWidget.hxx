//                                               -*- C++ -*-
/**
 *  @brief QWidget to import sample
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
#ifndef PERSALYS_IMPORTSAMPLEWIDGET_HXX
#define PERSALYS_IMPORTSAMPLEWIDGET_HXX

#include "persalys/ExportableTableView.hxx"
#include "persalys/TemporaryLabel.hxx"
#include "persalys/DataImport.hxx"

#include <openturns/OTType.hxx>

#include <QLineEdit>
#include <QButtonGroup>

namespace PERSALYS
{
class PERSALYS_UTILS_API ImportSampleWidget : public QWidget
{
  Q_OBJECT

public:
  friend class ImportedDesignPage;
  friend class MeshDefinitionWizard;
  friend class ImportObservationsPage;
  friend class TestMeshDefinitionWizard;
  friend class EditValuesWizard;
  friend class ImportedDistributionPage;
  friend class DataFieldModelWindow;

  ImportSampleWidget(QWidget *parent = nullptr, bool chooseOrder = false);

protected:
  void buildInterface();
  void setData(const QString & fileName);
  void updateWidgets(const OT::Sample& fileSample, const OT::Description& variableNames, const OT::Indices& variablecolumns, const OT::Description &comboItems);
  void updateWidgets(const OT::Sample& fileSample, const OT::Description& variableNames, const OT::Indices& variablecolumns);
  OT::Indices getColumns(const OT::Description& names) const;
  OT::Sample getData() const;
  Tools::DataOrder getDataOrder() const;

public slots:
  void openFileRequested();
signals:
  void updateTableRequested(const QString & fileName);
  void checkColumnsRequested();

protected:
  bool tableValidity_;
  QLineEdit * filePathLineEdit_ = nullptr;
  ExportableTableView * dataPreviewTableView_ = nullptr;
  QLabel * DOESizeLabel_ = nullptr;
  TemporaryLabel * errorMessageLabel_ = nullptr;
  bool chooseOrder_ = false;
  QButtonGroup * orderButtonGroup_ = nullptr;
};
}
#endif

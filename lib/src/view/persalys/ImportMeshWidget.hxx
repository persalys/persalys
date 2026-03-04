//                                               -*- C++ -*-
/**
 *  @brief QWidget to import mesh
 *
 *  Copyright 2015-2025 EDF-Phimeca
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
#ifndef PERSALYS_IMPORTMESHWIDGET_HXX
#define PERSALYS_IMPORTMESHWIDGET_HXX

#include "persalys/ExportableTableView.hxx"
#include "persalys/ErrorWidget.hxx"

#include <openturns/OTType.hxx>

#include <QLineEdit>
#include <QButtonGroup>
#include <QLabel>

namespace PERSALYS
{
class PERSALYS_VIEW_API  ImportMeshWidget : public QWidget
{
  Q_OBJECT

public:
  friend class MeshDefinitionWizard;
  friend class TestMeshDefinitionWizard;

  explicit ImportMeshWidget(QWidget *parent = nullptr);

protected:
  void buildInterface();
  void setData(const QString & fileName);
  void updateWidgets(const OT::Sample& fileSample, const OT::Description& variableNames, const OT::Indices& variablecolumns, const OT::Description &comboItems);
  void updateWidgets(const OT::Sample& fileSample, const OT::Description& variableNames, const OT::Indices& variablecolumns);
  OT::Indices getColumns(const OT::Description& names) const;
  OT::Sample getData() const;

public slots:
  void openFileRequested();
signals:
  void updateTableRequested(const QString & fileName);
  void checkColumnsRequested();

private:
  bool                tableValidity_          = false;

  QLineEdit           * filePathLineEdit_     = nullptr;
  ExportableTableView * dataPreviewTableView_ = nullptr;
  QLabel              * DOESizeLabel_         = nullptr;
  ErrorWidget         * errorWidget_          = nullptr;
};
}
#endif

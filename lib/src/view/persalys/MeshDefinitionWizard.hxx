//                                               -*- C++ -*-
/**
 *  @brief QWizard to define a mesh
 *
 *  Copyright 2015-2026 EDF-Phimeca
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
#ifndef PERSALYS_MESHDEFINITIONWIZARD_HXX
#define PERSALYS_MESHDEFINITIONWIZARD_HXX

#include "persalys/Wizard.hxx"
#include "persalys/CustomStandardItemModel.hxx"
#include "persalys/ImportMeshWidget.hxx"
#include "persalys/GridMeshModel.hxx"
#include "persalys/ImportedMeshModel.hxx"
#include "persalys/CopyableTableView.hxx"
#include "persalys/ErrorWidget.hxx"

#include <QButtonGroup>
#include <QResizeEvent>

namespace PERSALYS
{
class PERSALYS_VIEW_API MeshDefinitionWizard : public Wizard
{
  Q_OBJECT

  friend class TestMeshDefinitionWizard;

public:
  enum Method {Grid, Import};
  explicit MeshDefinitionWizard(const MeshModel& mesh, QWidget* parent = nullptr, bool isDataField = false);

  MeshModel getMesh() const;
  bool validateCurrentPage() override;

protected:
  void buildInterface();
  void resizeEvent(QResizeEvent * event) override;

public slots:
  void setTable(const QString& fileName);
  void checkColumns();

private:
  MeshModel               mesh_;
  OT::Bool                isDataField_          = false;
  GridMeshModel           gridMesh_;
  ImportedMeshModel       importedMesh_;

  QButtonGroup            * methodGroup_        = nullptr;
  ImportMeshWidget        * sampleWidget_       = nullptr;
  CopyableTableView       * tableView_          = nullptr;
  CustomStandardItemModel * tableModel_         = nullptr;
  ErrorWidget             * errorWidget_  = nullptr;
};
}
#endif

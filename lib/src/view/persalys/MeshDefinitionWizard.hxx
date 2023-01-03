//                                               -*- C++ -*-
/**
 *  @brief QWizard to define a mesh
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
#ifndef PERSALYS_MESHDEFINITIONWIZARD_HXX
#define PERSALYS_MESHDEFINITIONWIZARD_HXX

#include "persalys/Wizard.hxx"
#include "persalys/CustomStandardItemModel.hxx"
#include "persalys/ImportSampleWidget.hxx"
#include "persalys/GridMeshModel.hxx"
#include "persalys/ImportedMeshModel.hxx"
#include "persalys/CopyableTableView.hxx"

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
  MeshDefinitionWizard(const MeshModel& mesh, QWidget* parent = 0);

  MeshModel getMesh() const;
  virtual bool validateCurrentPage();

protected:
  void buildInterface();
  virtual void resizeEvent(QResizeEvent * event);

public slots:
  void setTable(const QString& fileName);
  void checkColumns();

private:
  MeshModel mesh_;
  QButtonGroup * methodGroup_;
  ImportSampleWidget * sampleWidget_;
  CopyableTableView * tableView_;
  CustomStandardItemModel * tableModel_;
  GridMeshModel gridMesh_;
  ImportedMeshModel importedMesh_;
  TemporaryLabel * errorMessageLabel_;
};
}
#endif

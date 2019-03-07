//                                               -*- C++ -*-
/**
 *  @brief QWizard to define a mesh
 *
 *  Copyright 2015-2018 EDF-Phimeca
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
#ifndef OTGUI_MESHDEFINITIONWIZARD_HXX
#define OTGUI_MESHDEFINITIONWIZARD_HXX

#include "otgui/Wizard.hxx"
#include "otgui/TemporaryLabel.hxx"
#include "otgui/MeshModel.hxx"
#include "otgui/ExportableTableView.hxx"
#include "otgui/CustomStandardItemModel.hxx"

#include <QLineEdit>
#include <QButtonGroup>

namespace OTGUI
{
class OTGUI_API MeshDefinitionWizard : public Wizard
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
  void setTable(const QString& fileName);
  void setData(const QString & fileName);

public slots:
  void openFileRequested();

private:
  MeshModel mesh_;
  OT::Indices columns_;
  QButtonGroup * methodGroup_;
  CustomStandardItemModel * tableModel_;
  QLineEdit * filePathLineEdit_;
  ExportableTableView * dataPreviewTableView_;
  QLabel * sizeLabel_;
  TemporaryLabel * errorMessageLabel_;
};
}
#endif

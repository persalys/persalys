//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow to display the data model
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
#ifndef PERSALYS_DATAFIELDMODELWINDOW_HXX
#define PERSALYS_DATAFIELDMODELWINDOW_HXX

#include "persalys/DataFieldModelItem.hxx"
#include "persalys/DataFieldModel.hxx"
#include "persalys/SampleTableModel.hxx"
#include "persalys/EditableExportableTableView.hxx"
#include "persalys/SubWindow.hxx"
#include "persalys/ErrorWidget.hxx"
#include "persalys/ImportSampleWidget.hxx"

#include <QLineEdit>
#include <QResizeEvent>
#include <QButtonGroup>
#include <QCheckBox>


namespace PERSALYS
{

//Fwd declaration
class EditableExportableTableView;

class PERSALYS_VIEW_API DataFieldModelWindow : public SubWindow
{
  Q_OBJECT

public:
  explicit DataFieldModelWindow(DataFieldModelItem * item, QWidget *parent = nullptr);
  void showEvent(QShowEvent*) override;

protected:
  void buildInterface();
  void setTable(const QString& fileName);

public slots:
  void openFileRequested();
  void updateProcessSample();
  void launchCleaningWizard();

private:
  DataFieldModel              dataModel_;
  bool                        forceUpdateProcessSample_ = false;

  SampleTableModel            * tableModel_             = nullptr;
  QLineEdit                   * filePathLineEdit_       = nullptr;
  QButtonGroup                * orderButtonGroup_       = nullptr;
  EditableExportableTableView * dataTableView_          = nullptr;
  ErrorWidget              * errorWidget_      = nullptr;
  QCheckBox                   * includeMeshCB_          = nullptr;
};

} // namespace PERSALYS

#endif // PERSALYS_DATAFIELDMODELWINDOW_HXX

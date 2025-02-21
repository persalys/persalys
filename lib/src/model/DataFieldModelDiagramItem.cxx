//                                               -*- C++ -*-
/**
 *  @brief QStandardItem, observer of data model diagram
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
#include "persalys/DataFieldModelDiagramItem.hxx"
#include "persalys/DataFieldModelDefinitionItem.hxx"
#include "persalys/DataMeshItem.hxx"
#include "persalys/StudyItem.hxx"

#include <QDebug>

using namespace OT;

namespace PERSALYS
{

  DataFieldModelDiagramItem::DataFieldModelDiagramItem(const DataFieldModel& dataModel)
    : DataFieldModelItem(dataModel, "DataFieldModelDiagramItem")
  {
    setData(QString::fromUtf8(dataModel.getName().c_str()), Qt::DisplayRole);

    buildActions();
  }


  void DataFieldModelDiagramItem::buildActions()
  {
    // define data model action
    defineAction_ = new QAction(tr("Define the model"), this);
    defineAction_->setStatusTip(tr("Define the data model"));
    connect(defineAction_, SIGNAL(triggered(bool)), this, SLOT(appendDataFieldModelItem()));

    // new analyses
    newDataAnalysis_ = createAction("FieldKarhunenLoeveAnalysis", getDataFieldModel());

    // remove data model
    removeAction_ = new QAction(QIcon(":/images/window-close.png"), tr("Remove"), this);
    removeAction_->setStatusTip(tr("Remove the data model"));
    connect(removeAction_, SIGNAL(triggered()), this, SLOT(removeDataFieldModel()));

    // add actions
    appendAction(defineAction_);
    appendSeparator();
    appendAction(removeAction_);
  }


  void DataFieldModelDiagramItem::update(Observable* /*source*/, const String& message)
  {
    if (message == "analysisLaunched")
    {
      analysisInProgress_ = true;
    }
    else if (message == "analysisFinished" || message == "analysisBadlyFinished")
    {
      analysisInProgress_ = false;
    }
    else if (message == "objectRemoved")
    {
      if (hasChildren())
        qDebug() << "DataFieldModelDiagramItem::update(objectRemoved) has not to contain child\n";
      emit removeRequested(row());
    }
    else if (message == "processSampleChanged")
      updateDiagramBoxesValidity();
  }


  void DataFieldModelDiagramItem::fill()
  {
    // model definition item
    // if (dataModel_.getProcessSample().getSize())
    DataMeshItem * meshItem = new DataMeshItem(getDataFieldModel());
    appendRow(meshItem);
    emit windowRequested(meshItem);
    appendDataFieldModelItem();
    updateDiagramBoxesValidity();
  }


  void DataFieldModelDiagramItem::appendDataFieldModelItem()
  {
    // do nothing if the item already exists
    QModelIndexList listIndexes = model()->match(this->index(), Qt::UserRole, "DataFieldModelDefinitionItem", 1, Qt::MatchRecursive);
    if (listIndexes.size() == 1 && listIndexes[0].parent() == this->index())
    {
      // emit signal to the study tree to display the window
      emit changeCurrentItemRequested(listIndexes[0]);
      return;
    }

    // new item
    DataFieldModelDefinitionItem * dmItem = new DataFieldModelDefinitionItem(getDataFieldModel(), this);
    appendRow(dmItem);

    // emit signal to the StudyTreeView to create a window
    emit windowRequested(dmItem);

    // disable the definition action
    defineAction_->setDisabled(true);
  }


  void DataFieldModelDiagramItem::appendItem(const Analysis& analysis)
  {
    appendAnalysisItem(analysis);
    analysis.getImplementation().get()->addObserver(this);
    analysis.getImplementation().get()->addObserver(getParentStudyItem());
  }

  void DataFieldModelDiagramItem::removeDataFieldModel()
  {
    // check
    if (analysisInProgress_)
    {
      emit showErrorMessageRequested(tr("Can not remove a data field model when an analysis is running."));
      return;
    }
    // remove
    if (getParentStudyItem())
      getParentStudyItem()->getStudy().remove(DataFieldModel(dataModel_));
  }

  void DataFieldModelDiagramItem::setData(const QVariant & value, int role)
  {
    // rename
    if (role == Qt::EditRole)
      dataModel_.getImplementation()->setName(value.toString().toUtf8().data());

    QStandardItem::setData(value, role);
}

  void DataFieldModelDiagramItem::updateDiagramBoxesValidity()
  {
    // emit signals to DataFielModelDiagramWindow
    // to update diagram (arrow color and button availability)
    emit modelValidityChanged(dataModel_.isValid());
  }
}

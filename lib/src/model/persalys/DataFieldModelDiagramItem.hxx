//                                               -*- C++ -*-
/**
 *  @brief QStandardItem, observer of field data model diagram
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
#ifndef PERSALYS_DATAFIELDMODELDIAGRAMITEM_HXX
#define PERSALYS_DATAFIELDMODELDIAGRAMITEM_HXX

#include "persalys/DataFieldModelItem.hxx"

namespace PERSALYS
{
  class PERSALYS_MODEL_API DataFieldModelDiagramItem : public DataFieldModelItem
  {
    Q_OBJECT

    friend class DataFieldModelDefinitionItem;
    friend class DataFieldModelDiagramWindow;

  public:
    DataFieldModelDiagramItem(const DataFieldModel& dataModel);

    void setData(const QVariant & value, int role);

    virtual void update(Observable* source, const OT::String& message);

    void fill();
    virtual void appendItem(const Analysis& analysis);
    void updateDiagramBoxesValidity();

  protected:
    void buildActions();

  public slots:
    void appendDataFieldModelItem();
    void removeDataFieldModel();
  signals:
    // signal for diagram
    void modelValidityChanged(bool);

  private:
    QAction * defineAction_ = nullptr;
    QAction * newDataAnalysis_ = nullptr;
    QAction * removeAction_ = nullptr;
  };
}
#endif

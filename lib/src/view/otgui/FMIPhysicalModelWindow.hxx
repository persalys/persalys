//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow to define a FMI physical model
 *
 *  Copyright 2015-2017 EDF-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef OTGUI_FMIPHYSICALMODELWINDOW_HXX
#define OTGUI_FMIPHYSICALMODELWINDOW_HXX

#include "otgui/OTguiSubWindow.hxx"
#include "otgui/PhysicalModelItem.hxx"
#include "otgui/CopyableTableView.hxx"
#include "otgui/ListWidgetWithCheckBox.hxx"
#include "otgui/FMUInfo.hxx"
#include "otgui/FMIPhysicalModel.hxx"
#include "otgui/DoubleSpinBox.hxx"
#include "otgui/DifferentiationTableModel.hxx"

#include <QLineEdit>
#include <QCheckBox>
#include <QComboBox>
#include <QAbstractTableModel>
#include <QSortFilterProxyModel>
#include <QItemDelegate>
#include <QTextEdit>
#include <QTreeView>
#include <QMouseEvent>

namespace OTGUI {


class OTGUI_API DataTableModel : public QAbstractTableModel
{
  Q_OBJECT

public:
  DataTableModel(const PhysicalModel & physicalModel, QObject * parent = 0);

  int columnCount(const QModelIndex & parent = QModelIndex()) const;
  int rowCount(const QModelIndex & parent = QModelIndex()) const;
  QVariant data(const QModelIndex & index, int role) const;
  bool setData(const QModelIndex & index, const QVariant & value, int role);
  Qt::ItemFlags flags(const QModelIndex & index) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

  bool isValid();
  void loadData(const FMUInfo & info);

  OT::Description getVariableNames() const;
  OT::Indices getIO() const;
  OT::Description getProperties() const;
public slots:
  void updateOutputValues();

signals:
  void errorMessageChanged(QString);
  void ioCountChanged();

private:
  OT::Description filterIOVariables(const OT::UnsignedInteger io) const;
  OT::Description getInputVariableNames() const;
  OT::Description getOutputVariableNames() const;

  PhysicalModel physicalModel_;
  OT::Description properties_;
  OT::Description variableNames_;
  OT::Description descriptions_;
  OT::Indices variability_;
  OT::Indices causality_;
  OT::Indices inputOutput_;//0:none; 1:input; 2:output
  
  OT::Description startKeys_;
  OT::Point startValues_;
};



class OTGUI_API EnumDelegate : public QItemDelegate
{
  Q_OBJECT
public:
  EnumDelegate(QStringList & enumLabels, QObject *parent = 0)
    : QItemDelegate(parent)
    , enumLabels_(enumLabels)
  {
  }

  QWidget *createEditor(QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index) const
  {
    QComboBox * editor = new QComboBox(parent);
    editor->addItems(enumLabels_);
    return editor;
  }


  void setEditorData(QWidget * editor, const QModelIndex & index) const
  {
    QComboBox * comboBox = static_cast<QComboBox*>(editor);
    int value = index.model()->data(index, Qt::EditRole).toInt();
    comboBox->setCurrentIndex(value);
    comboBox->setEnabled(comboBox->currentIndex() >= 0);
  }


  void setModelData(QWidget * editor, QAbstractItemModel * model, const QModelIndex & index) const
  {
    QComboBox * comboBox = static_cast<QComboBox*>(editor);
    model->setData(index, QString::number(comboBox->currentIndex()), Qt::EditRole);
  }

private:
  QStringList enumLabels_;
};


class OTGUI_API FloatDelegate : public QItemDelegate
{
  Q_OBJECT
public:
  FloatDelegate(QObject *parent = 0)
    : QItemDelegate(parent)
  {
  }

  QWidget *createEditor(QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index) const
  {
    DoubleSpinBox * editor = new DoubleSpinBox(parent);
    return editor;
  }


  void setEditorData(QWidget * editor, const QModelIndex & index) const
  {
    DoubleSpinBox * comboBox = static_cast<DoubleSpinBox*>(editor);
    double value = index.model()->data(index, Qt::EditRole).toDouble();
    comboBox->setValue(value);
  }


  void setModelData(QWidget * editor, QAbstractItemModel * model, const QModelIndex & index) const
  {
    DoubleSpinBox * comboBox = static_cast<DoubleSpinBox*>(editor);
    model->setData(index, QString::number(comboBox->value()), Qt::EditRole);
  }

};

class DataFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
  DataFilterProxyModel(QObject *parent = 0);

  void setVariabilityFilter(const QList<int> & variabilityFilter);
  void setCausalityFilter(const QList<int> & causalityFilter);
  void setIOFilter(const QList<int> & ioFilter);
protected:
  virtual bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;

private:
  QList<int> variabilityFilter_;
  QList<int> causalityFilter_;
  QList<int> ioFilter_;
};



class TreeItem
{
public:
  explicit TreeItem(const QList<QVariant> &data, TreeItem *parentItem = 0);
  ~TreeItem();

  void appendChild(TreeItem *child);

  TreeItem *child(int row);
  int childCount() const;
  int columnCount() const;
  QVariant data(int column) const;
  int row() const;
  TreeItem *parentItem();

private:
  QList<TreeItem*> m_childItems;
  QList<QVariant> m_itemData;
  TreeItem *m_parentItem;
};


class TreeModel : public QAbstractItemModel
{
  Q_OBJECT

public:
  explicit TreeModel(const QString &data, QObject *parent = 0);
  ~TreeModel();

  QVariant data(const QModelIndex &index, int role) const;
  Qt::ItemFlags flags(const QModelIndex &index) const;
  QVariant headerData(int section, Qt::Orientation orientation,
                      int role = Qt::DisplayRole) const;
  QModelIndex index(int row, int column,
                    const QModelIndex &parent = QModelIndex()) const;
  QModelIndex parent(const QModelIndex &index) const;
  int rowCount(const QModelIndex &parent = QModelIndex()) const;
  int columnCount(const QModelIndex &parent = QModelIndex()) const;

  void setVariableNames(const OT::Description & variableNames);
private:
  void setupModelData(const OT::Description & variableNames);

  TreeItem *rootItem_;
};



class DeselectableTreeView : public QTreeView
{
public:
  DeselectableTreeView(QWidget *parent = 0) : QTreeView(parent) {}

private:
  virtual void mousePressEvent(QMouseEvent *event)
  {
    QModelIndex item = indexAt(event->pos());
    bool selected = selectionModel()->isSelected(indexAt(event->pos()));
    QTreeView::mousePressEvent(event);

    if (!item.isValid() || selected)
    {
      clearSelection();
      const QModelIndex index;
      selectionModel()->setCurrentIndex(index, QItemSelectionModel::Select);
    }
  }
};



class OTGUI_API FMIPhysicalModelWindow : public OTguiSubWindow
{
  Q_OBJECT

public :
  FMIPhysicalModelWindow(PhysicalModelItem * item);

public slots:
  void selectImportFileDialogRequested();
  void loadModel(const FMUInfo & info);
  void updateFilters();
  void clearFilters();
  void updateIOCount();
  void treeSelect(const QModelIndex & current, const QModelIndex & previous);
  void updateVariablesTableModel();
  void evaluateOutputs();
  void updateDifferentiationTableModel();
  void applyDifferentiationStepToAllInputs(double);

private:
  FMIPhysicalModel * getFMIPhysicalModel() const;

  PhysicalModel physicalModel_;
  QLineEdit * FMUfileNameEdit_;
  QLineEdit * filterTextEdit_;
  QCheckBox * matchCaseCheckBox_;
  QComboBox * searchFieldComboBox_;
  ListWidgetWithCheckBox * variabilityField_;
  ListWidgetWithCheckBox * causalityField_;
  ListWidgetWithCheckBox * ioField_;
  QCheckBox * filterCausalityComboBox_[7];
  CopyableTableView * variablesTableView_;
  DataTableModel * variablesTableModel_;
  DataFilterProxyModel * proxyModel_;
  TreeModel *tree_model_;
  QTableView * propertiesTable_;
  QLabel * ioCountLabel_;
  QTabWidget * tabWidget_;

  CopyableTableView * differentiationTableView_;
  DifferentiationTableModel * differentiationTableModel_;
};
}
#endif

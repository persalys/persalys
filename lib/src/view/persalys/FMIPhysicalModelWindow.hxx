//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow to define a FMI physical model
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
#ifndef PERSALYS_FMIPHYSICALMODELWINDOW_HXX
#define PERSALYS_FMIPHYSICALMODELWINDOW_HXX

#include "persalys/SubWindow.hxx"
#include "persalys/PhysicalModelItem.hxx"
#include "persalys/CopyableTableView.hxx"
#include "persalys/ListWidgetWithCheckBox.hxx"
#include "persalys/FMUInfo.hxx"
#include "persalys/FMIPhysicalModel.hxx"
#include "persalys/TemporaryLabel.hxx"

#include <QLineEdit>
#include <QCheckBox>
#include <QComboBox>
#include <QAbstractTableModel>
#include <QSortFilterProxyModel>
#include <QItemDelegate>
#include <QTextEdit>
#include <QTreeView>
#include <QMouseEvent>

namespace PERSALYS
{


class PERSALYS_VIEW_API DataTableModel : public QAbstractTableModel
{
  Q_OBJECT

public:
  DataTableModel(const PhysicalModel & physicalModel, QObject *parent = nullptr);

  int columnCount(const QModelIndex & parent = QModelIndex()) const override;
  int rowCount(const QModelIndex & parent = QModelIndex()) const override;
  QVariant data(const QModelIndex & index, int role) const override;
  bool setData(const QModelIndex & index, const QVariant & value, int role) override;
  Qt::ItemFlags flags(const QModelIndex & index) const override;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

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



class PERSALYS_VIEW_API EnumDelegate : public QItemDelegate
{
  Q_OBJECT
public:
  EnumDelegate(QStringList & enumLabels, QObject *parent = nullptr)
    : QItemDelegate(parent)
    , enumLabels_(enumLabels)
  {
  }

  QWidget *createEditor(QWidget * parent, const QStyleOptionViewItem & /*option*/, const QModelIndex & /*index*/) const override
  {
    QComboBox * editor = new QComboBox(parent);
    editor->addItems(enumLabels_);
    return editor;
  }


  void setEditorData(QWidget * editor, const QModelIndex & index) const override
  {
    QComboBox * comboBox = static_cast<QComboBox*>(editor);
    int value = index.model()->data(index, Qt::EditRole).toInt();
    comboBox->setCurrentIndex(value);
    comboBox->setEnabled(comboBox->currentIndex() >= 0);
  }


  void setModelData(QWidget * editor, QAbstractItemModel * model, const QModelIndex & index) const override
  {
    QComboBox * comboBox = static_cast<QComboBox*>(editor);
    model->setData(index, QString::number(comboBox->currentIndex()), Qt::EditRole);
  }

private:
  QStringList enumLabels_;
};


class DataFilterProxyModel : public QSortFilterProxyModel
{
  Q_OBJECT

public:
  DataFilterProxyModel(QObject *parent = nullptr);

  void setVariabilityFilter(const QList<int> & variabilityFilter);
  void setCausalityFilter(const QList<int> & causalityFilter);
  void setIOFilter(const QList<int> & ioFilter);
protected:
  bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;

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
  explicit TreeModel(const QString &data, QObject *parent = nullptr);
  ~TreeModel();

  QVariant data(const QModelIndex &index, int role) const override;
  Qt::ItemFlags flags(const QModelIndex &index) const override;
  QVariant headerData(int section, Qt::Orientation orientation,
                      int role = Qt::DisplayRole) const override;
  QModelIndex index(int row, int column,
                    const QModelIndex &parent = QModelIndex()) const override;
  QModelIndex parent(const QModelIndex &index) const override;
  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  int columnCount(const QModelIndex &parent = QModelIndex()) const override;

  void setVariableNames(const OT::Description & variableNames);
private:
  void setupModelData(const OT::Description & variableNames);

  TreeItem *rootItem_;
};



class DeselectableTreeView : public QTreeView
{
public:
  DeselectableTreeView(QWidget *parent = nullptr)
  : QTreeView(parent)
  {
    // style sheet
    const QString styleSheet = " QTreeView::item:selected { background-color: #a5d3a1;\
                                                            color: doubledarkgray;\
                                 }\
                                 QTreeView::branch:selected { background-color: #a5d3a1;\
                                 }\
                                 QTreeView::branch:has-children:!has-siblings:closed,\
                                 QTreeView::branch:closed:has-children:has-siblings { border-image: none;\
                                                                                      image: url(:/images/branch-closed.png);\
                                 }\
                                 QTreeView::branch:open:has-children:!has-siblings,\
                                 QTreeView::branch:open:has-children:has-siblings { border-image: none;\
                                                                                    image: url(:/images/branch-opened.png);\
                                 }\
                                 QTreeView::branch:has-siblings:adjoins-item { border-image: url(:/images/branch-more.png) 0;\
                                 }\
                                 QTreeView::branch:!has-children:!has-siblings:adjoins-item { border-image: url(:/images/branch-end.png) 0;\
                                 }\
                               ";

    setStyleSheet(styleSheet);
  }

private:
  void mousePressEvent(QMouseEvent *event) override
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



class PERSALYS_VIEW_API FMIPhysicalModelWindow : public SubWindow
{
  Q_OBJECT

public :
  FMIPhysicalModelWindow(PhysicalModelItem * item, QWidget *parent = nullptr);

public slots:
  void selectImportFileDialogRequested();
  void loadModel(const FMUInfo & info);
  void updateFilters();
  void updatePersistentEditor();
  void clearFilters();
  void updateIOCount();
  void treeSelect(const QModelIndex & current, const QModelIndex & previous);
  void updateVariablesTableModel();
  void evaluateOutputs();

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
  TemporaryLabel * errorMessageLabel_;
};
}
#endif

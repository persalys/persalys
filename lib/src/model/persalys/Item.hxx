//                                               -*- C++ -*-
/**
 *  @brief QStandardItem, observer
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
#ifndef PERSALYS_ITEM_HXX
#define PERSALYS_ITEM_HXX

#include "persalys/ItemFactory.hxx"

#include <QStandardItem>

namespace PERSALYS
{
class AnalysisItem;
class PERSALYS_MODEL_API Item : public ItemFactory, public QStandardItem
{
  Q_OBJECT

public:
  Item(const QString& name, const QString& type);

  virtual ~Item();

  void emitShowWindowRequested();
  void emitRemoveWindowRequested();

  QList< QAction* > getActions();
  void appendAction(QAction *action);
  void appendSeparator(const QString& text = "");
  void insertAction(int i, QAction* action);

  QVariant data(int role) const override;
  virtual void appendRow(Item * item);
  virtual void insertRow(int row, Item * item);
  virtual void removeRow(int row);

  bool analysisInProgress() const;

protected slots:
  void requestRemoveChild(int);
signals:
  void showWindowRequested();
  void dataExportRequested();

  void removeRequested(int);
  void removeWindowRequested();

  // signals for StudyManager
  void showErrorMessageRequested(QString);
  void changeCurrentItemRequested(QModelIndex);
  void windowRequested(AnalysisItem*, bool createConnections = true);
  void windowRequested(Item*);

  void exportRequested();
  void saveRequested(StudyItem* item);
  void saveAsRequested(StudyItem* item);
  void closeRequested(StudyItem* item);


protected:
  Item * getTitleItemNamed(const QString &analysisName);
  void appendAnalysisItem(const Analysis &analysis);

protected:
  bool analysisInProgress_;

private:
  QList< QAction* > menuActions_;
};
}
#endif

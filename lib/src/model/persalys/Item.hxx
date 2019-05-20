//                                               -*- C++ -*-
/**
 *  @brief QStandardItem, observer
 *
 *  Copyright 2015-2019 EDF-Phimeca
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

#include "persalys/Analysis.hxx"

#include <QStandardItem>
#include <QAction>

namespace PERSALYS
{
class StudyItem;
class PERSALYS_API Item : public QObject, public QStandardItem
{
  Q_OBJECT

public:
  Item(const QString& name, const QString& type);

  virtual ~Item();

  void emitShowWindowRequested();
  void emitRemoveWindowRequested();

  QList< QAction* > getActions();
  void appendAction(QAction* action);
  void appendSeparator(const QString& text = "");
  void insertAction(int i, QAction* action);

  virtual QVariant data(int role) const;
  virtual void appendRow(Item * item);
  virtual void insertRow(int row, Item * item);
  virtual void removeRow(int row);

  StudyItem * getParentStudyItem();
  bool analysisInProgress() const;

protected slots:
  void requestRemoveChild(int);
  void setAnalysisInProgress(bool);
signals:
  void showWindowRequested();
  void dataExportRequested();
  void showErrorMessageRequested(QString);
  void analysisRequested(Item*, const Analysis&, const bool isGeneralWizard = false);
  void removeRequested(int);
  void removeWindowRequested();
  void analysisInProgressStatusChanged(bool);

protected:
  Item * getTitleItemNamed(const QString& name, const QString& typeName);

protected:
  StudyItem * parentStudyItem_;
  bool analysisInProgress_;

private:
  QList< QAction* > menuActions_;
};
}
#endif

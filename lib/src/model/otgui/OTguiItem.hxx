//                                               -*- C++ -*-
/**
 *  @brief QStandardItem, observer
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
#ifndef OTGUI_OTGUIITEM_HXX
#define OTGUI_OTGUIITEM_HXX

#include "otgui/Analysis.hxx"

#include <QStandardItem>
#include <QAction>

namespace OTGUI
{
class OTStudyItem;
class OTGUI_API OTguiItem : public QObject, public QStandardItem
{
  Q_OBJECT

public:
  OTguiItem(const QString& name, const QString& type);

  virtual ~OTguiItem();

  QList< QAction* > getActions();
  void appendAction(QAction* action);
  void appendSeparator(const QString& text = "");
  void insertAction(int i, QAction* action);

  virtual QVariant data(int role) const;
  virtual void appendRow(OTguiItem * item);
  virtual void insertRow(int row, OTguiItem * item);
  virtual void removeRow(int row);

  OTStudyItem * getParentOTStudyItem();

protected slots:
  void requestRemoveChild(int);
  void setAnalysisInProgress(bool);
signals:
  void dataExportRequested();
  void emitErrorMessageRequested(QString);
  void analysisRequested(OTguiItem*, const Analysis&, const bool isGeneralWizard = false);
  void removeRequested(int);
  void removeWindowRequested();
  void analysisInProgressStatusChanged(bool);

protected:
  OTguiItem * getTitleItemNamed(const QString& name, const QString& typeName);

protected:
  OTStudyItem * parentOTStudyItem_;
  bool analysisInProgress_;

private:
  QList< QAction* > menuActions_;
};
}
#endif

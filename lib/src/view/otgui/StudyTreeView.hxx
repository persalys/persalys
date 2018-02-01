//                                               -*- C++ -*-
/**
 *  @brief QTreeView to list studies
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
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef OTGUI_STUDYTREEVIEW_HXX
#define OTGUI_STUDYTREEVIEW_HXX

#include "otgui/StudyTreeViewModel.hxx"
#include "otgui/OTguiSubWindow.hxx"
#include "otgui/AnalysisWizard.hxx"
#include "otgui/DataModelDefinitionItem.hxx"
#include "otgui/DesignOfExperimentDefinitionItem.hxx"

#include <QTreeView>
#include <QMouseEvent>

namespace OTGUI
{
class OTGUI_API StudyTreeView : public QTreeView
{
  Q_OBJECT

public:
  StudyTreeView(QWidget * parent = 0);

  OTguiItem * getCurrentItem() const;

  virtual void mousePressEvent(QMouseEvent*);

public slots:
  void onCustomContextMenu(const QPoint& point);
  void selectedItemChanged(const QModelIndex& currentIndex, const QModelIndex& previousIndex);

  void modifyStudySubItemsExpansion(OTStudyItem* item);

signals:
  void studyCreated(OTStudyItem*);
  void itemSelectionChanged(QStandardItem*);

private:
  StudyTreeViewModel * treeViewModel_;
  bool analysisInProgress_;
};
}
#endif

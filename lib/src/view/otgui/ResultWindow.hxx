//                                               -*- C++ -*-
/**
 *  @brief Base class QMdiSubWindow for the results
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
#ifndef OTGUI_RESULTWINDOW_HXX
#define OTGUI_RESULTWINDOW_HXX

#include "otgui/SubWindow.hxx"
#include "otgui/AnalysisItem.hxx"
#include "otgui/ParametersWidget.hxx"

#include <QScrollArea>
#include <QListWidget>
#include <QStyledItemDelegate>

namespace OTGUI
{

// custom QStyledItemDelegate for VariablesListWidget
class OTGUI_API ListWidgetItemDelegate : public QStyledItemDelegate
{
  public:
    ListWidgetItemDelegate(QObject* parent = 0)
        : QStyledItemDelegate(parent)
    {
    }
    QSize sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index) const
    {
      QSize result = QStyledItemDelegate::sizeHint (option, index);
      result.setHeight(result.height() * 2);
      return result;
    }
};

// custom QListWidget for all result windows
class OTGUI_API VariablesListWidget : public QListWidget
{
public:
  VariablesListWidget(QWidget * parent = 0)
    : QListWidget(parent)
  {
    // style sheet
    const QString styleSheet = "QListWidget::item:selected { background-color: #a5d3a1;\
                                                             color: doubledarkgray; }";

    setStyleSheet(styleSheet);
    ListWidgetItemDelegate * delegate = new ListWidgetItemDelegate(this);
    setItemDelegateForColumn(0, delegate);
  }
};


// base class for all result windows
class OTGUI_API ResultWindow : public SubWindow
{
  Q_OBJECT

public:
  ResultWindow(Item * item, QWidget * parent = 0);

protected:
  virtual void setParameters(const Analysis& analysis, const QString& title);
  virtual void setModelDescription(const Analysis& analysis);
//   QString getModelDescription(const Analysis& analysis);

protected:
  QScrollArea * parametersWidget_;
  QWidget * modelDescriptionWidget_;
};
}
#endif

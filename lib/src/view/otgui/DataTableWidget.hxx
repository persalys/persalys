//                                               -*- C++ -*-
/**
 *  @brief Generic data table
 *
 *  Copyright 2015-2016 EDF-Phimeca
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
#ifndef OTGUI_DATATABLEWIDGET_HXX
#define OTGUI_DATATABLEWIDGET_HXX

#include <QtGui/QTableWidget>

namespace OTGUI {

class DataTableWidget : public QTableWidget
{
  Q_OBJECT

public:

  /// default constructor
  DataTableWidget(QWidget * parent = 0);
  
  /// cosntructor from number of rows and columns
  DataTableWidget(int rows, int columns, QWidget * parent = 0);
  
//   /// write text
//   void setItemText (int row, int column, const QString & text, int alignement = Qt::AlignLeft | Qt::AlignVCenter);
//   void setItemBoldText (int row, int column, const QString & text);
// 
//   /// write value
//   void setItemValue (int row, int column, const double value, int alignement = Qt::AlignRight | Qt::AlignVCenter);
// 
//   /// write gray line with title
//   QTableWidgetItem * writeSectionLine (int row, const QString & title);
//   
//   /// precision accessor
//   void setPrecision(int precision);
//   
  
public slots:
  void copy();

protected:
//   QTableWidgetItem *createItem (int row, int column);
// 
//   virtual void mouseMoveEvent(QMouseEvent *event);
//   virtual void mousePressEvent(QMouseEvent *event);

  /// return selection range
  QTableWidgetSelectionRange selectedRange() const;

  /// format the text
  QString getFormattedText() const;

  /// drag start position
//   QPoint dragStartPosition_;

  QAction *copyAction_;

private:
  void initialize();
  
  void buildActions();
  
//   int precision_;
};
}
#endif
//                                               -*- C++ -*-
/**
 *  @brief QGroupBox to select outputs
 *
 *  Copyright 2015-2020 EDF-Phimeca
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
#ifndef PERSALYS_OUTPUTSSELECTIONGROUPBOX_HXX
#define PERSALYS_OUTPUTSSELECTIONGROUPBOX_HXX

#include "persalys/ListWidgetWithCheckBox.hxx"
#include "persalys/TitledComboBox.hxx"

#include <openturns/Description.hxx>

#include <QGroupBox>
#include <QLabel>

namespace PERSALYS
{
class PERSALYS_API OutputsSelectionGroupBox : public QGroupBox
{
  Q_OBJECT

public:
  OutputsSelectionGroupBox(QWidget* parent = 0);
  OutputsSelectionGroupBox(bool pluralText, QWidget* parent = 0);
  OutputsSelectionGroupBox(const OT::Description& outputsNames, const OT::Description& interestVariables, QWidget* parent = 0);

  QStringList getSelectedOutputsNames() const;
  void updateComboBoxModel(const OT::Description& outputsNames, const OT::Description& interestVariables);

signals:
  void outputsSelectionChanged(QStringList);

private slots:
  void updateLabel(const QStringList& variables);

private:
  bool pluralText_;
  QLabel * namesLabel_;
  ListWidgetWithCheckBox * outputsListWidget_;
  TitledComboBox * outputsComboBox_;
};
}
#endif

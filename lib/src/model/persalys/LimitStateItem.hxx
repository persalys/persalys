//                                               -*- C++ -*-
/**
 *  @brief QStandardItem, observer of a limit state
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
#ifndef PERSALYS_LIMITSTATEITEM_HXX
#define PERSALYS_LIMITSTATEITEM_HXX

#include "persalys/LimitState.hxx"
#include "persalys/AnalysisItem.hxx"

namespace PERSALYS
{
class PERSALYS_API LimitStateItem : public Item, public Observer
{
  Q_OBJECT

public:
  LimitStateItem(const LimitState & limitState);

  LimitState getLimitState() const;

  void setData(const QVariant & value, int role);

  virtual void update(Observable * source, const OT::String & message);

  virtual void appendItem(const Analysis& analysis);

protected:
  void buildActions();

public slots:
  void removeLimitState();
signals:
  void outputListChanged();
  void operatorChanged();
  void thresholdChanged();

private:
  LimitState limitState_;
  QAction * newThresholdExceedanceAction_;
  QAction * removeAction_;
};
}
#endif

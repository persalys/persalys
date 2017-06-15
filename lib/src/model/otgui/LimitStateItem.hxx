//                                               -*- C++ -*-
/**
 *  @brief QStandardItem, observer of a limit state
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
#ifndef OTGUI_LIMITSTATEITEM_HXX
#define OTGUI_LIMITSTATEITEM_HXX

#include "otgui/LimitState.hxx"
#include "otgui/AnalysisItem.hxx"

namespace OTGUI {
class OTGUI_API LimitStateItem : public OTguiItem, public Observer
{
  Q_OBJECT

public:
  LimitStateItem(const LimitState & limitState);

  LimitState getLimitState() const;

  void setData(const QVariant & value, int role);

  virtual void update(Observable * source, const OT::String & message);

  void appendAnalysisItem(Analysis& analysis);

protected:
  void buildActions();

public slots:
  void createNewThresholdExceedance();
  void removeLimitState();
signals:
  void outputListChanged();
  void operatorChanged();
  void thresholdChanged();
  void newAnalysisItemCreated(AnalysisItem*);

private:
  LimitState limitState_;
  QAction * newThresholdExceedance_;
  QAction * removeLimitState_;
};
}
#endif
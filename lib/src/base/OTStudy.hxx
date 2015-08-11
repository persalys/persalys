// OTStudy.hxx

#ifndef OTSTUDY_H
#define OTSTUDY_H

#include "PhysicalModel.hxx"
#include "Calculus.hxx"
#include "ParametricCalculus.hxx"
#include "Observer.hxx"

#include <vector>
#include <string>

namespace OTGUI {
class OTStudy : public OT::PersistentObject, public Observable
{
public:
  static std::vector<OTStudy*> GetInstances();
  static OTStudy * FindInstance(const std::string & name);
  static void SetInstanceObserver(Observer * observer);

  OTStudy(const std::string & name);
  OTStudy(const OTStudy & other);
  virtual OTStudy * clone() const;

  virtual ~OTStudy();

  std::string getName() const;
  void setName(const std::string & name);

  std::vector<PhysicalModel> getPhysicalModels() const;
  void addPhysicalModel(const PhysicalModel & physicalModel);

  std::vector<Calculus> getCalculuses() const;
  void addCalculus(const Calculus & calculus);

  std::string dump();

private:
  static std::vector<OTStudy*> Instances_;
  static Observer * InstanceObserver_;
  std::string name_;
  std::vector<PhysicalModel> physicalModels_;
  std::vector<Calculus> calculuses_;
};
}
#endif
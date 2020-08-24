#ifndef MYSCHEDULER_H
#define MYSCHEDULER_H

#include "Objects.h"
#include "Scheduler.h"
#include <vector> 
#include <string> 
#include <map> 
#include <queue> 
#include <set> 

class MyScheduler: public Scheduler {
  struct City{
    //City(std::string name, unsigned int population);
    std::string name; 
    unsigned int population; 
    std::set<Route*> edges; 
    bool          factory;
    unsigned int  vaccines;
  };
  
  struct iRoute{
    std::string out; 
    std::string in; 
    unsigned int totalDays; 
    unsigned int id; 
    unsigned int doses; 
  };
  struct Compare {
    bool operator () (const iRoute* a, const iRoute* b) const {
    /*if(a == nullptr || b == nullptr) {
      throw std::invalid_argument("Null pointer in result set.");
    }*/
    return a->totalDays < b->totalDays;
    }
  };
  // Member Variables
  unsigned int                        mDeadline;
  std::map<std::string, City*> mCities;
  std::map<std::string, iRoute*> correctroute;
  std::vector<std::string>            mFactories;
  std::vector<Route*>                  mRoutes;
  std::vector<Shipment> mSchedule; 
  std::priority_queue<iRoute*,std::vector<iRoute*> ,Compare> possibleRoutes;
  std::set<std::string> namecheck;  
  std::set<std::string> nextcities; 
public:
  // Constructor
  MyScheduler(unsigned int deadline,
  std::map<std::string, unsigned int> cities,
  std::vector<std::string>            factories,
  std::vector<Route>                  routes); 
  //City(std::string name, unsigned int population);
  // Destructor?

  // Required Member Function
  std::vector<Shipment> schedule();
  bool in_city_two(std::string name);
  bool name_is_factory(std::string name);
  void set_route();
  unsigned int total_doses(std::string name); 
};

#endif

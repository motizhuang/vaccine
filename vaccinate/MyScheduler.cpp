#include "MyScheduler.h"
#include <vector> 
#include <string> 
#include <map>
#include <queue> 
#include <set> 
#include "Objects.h"
#include <iostream> 

Scheduler* Scheduler::create(
  unsigned int                        deadline,
  std::map<std::string, unsigned int> cities,
  std::vector<std::string>            factories,
  std::vector<Route>                  routes
) 
{
  return new MyScheduler(deadline,cities,factories,routes);
}
MyScheduler::~MyScheduler(){
  for(const auto& [name,city] : mCities){
    (void) name; 
    /*for(iRoute* route: city->correctroute)
      delete route; */
    for(Route* route: city->edges){
      delete route; 
    }
    delete city; 
  }
  /*for(const auto& route: mRoutes){
    delete route; 
  }*/
  for(const auto& need: todelete){
    delete need; 
  }
}
MyScheduler::MyScheduler(unsigned int deadline,
std::map<std::string, unsigned int> cities,
  std::vector<std::string>            factories,
  std::vector<Route>                  routes){
    mDeadline = deadline;  
    mFactories = factories; 
    for(const auto& [name, pop]: cities) {//create cities 
      City info; 
      info.name=name; 
      info.population=pop; 
      //std::set<Route*> edges; 
      //info.factory=false;
      //info.vaccines=0;
      info.doses_needed=info.population;
      info.count=0;
      mCities[name]=new City(info);
      //mCities[name] = new MyScheduler::City::City(name, pop);
    }
    for(const auto& mroute: routes){//create routes 
      Route info; 
      info.id=mroute.id; 
      info.city1=mroute.city1;
      info.city2=mroute.city2; 
      info.days=mroute.days;
      info.load=mroute.load;
      info.cost=mroute.cost; 
      mCities[info.city1]->edges.insert(new Route(info));
      mCities[info.city2]->edges.insert(new Route(info));
      //std::cout<<info.id<<'\n';
    }
    std::cout<<"made it out"<<'\n';
    
    std::cout<<"made it out"<<'\n';
    set_route(); //create correct route 
    for(const auto& factory: factories){
      set_doses_needed(factory); 
      //std::cout<<factory<<" needs "<<mCities[factory]->doses_needed<<'\n';
    }
}
void MyScheduler::set_route(){
  //for(const auto& [name,pop] : mCities){
  for(const auto& factories : mFactories){
    for(Route* edge : mCities[factories]->edges){
      //need name of city going to and the time it takes 
      iRoute info; 
      info.out=factories; 
      if(edge->city1==factories)
        info.in=edge->city2; 
      else
        info.in = edge->city1; 
      // std::cout<<"route ends: "<<info.out<<'\n';
      info.totalDays = edge->days;  
      info.id = edge->id; 
      possibleRoutes.push(new iRoute(info)); 
    }
    namecheck.insert(factories); 
  }
  //std::cout<<"made it out"<<'\n';
  while(!possibleRoutes.empty()){
    // for(std::string list : namecheck){
    //   std::cout<<list;
    // }
    // std::cout<<'\n';
    // std::cout<<possibleRoutes.top()->in<<'\n';
    if(!in_city_two(possibleRoutes.top()->in)){//possiblity: pops mati and mamburao has already been popped, for tandag those are the only cities connected to it
      namecheck.insert(possibleRoutes.top()->in);
      //std::cout<<"Name of city being viewed: "<<possibleRoutes.top()->in<<'\n';
      std::string in = possibleRoutes.top()->in;
      std::string out = possibleRoutes.top()->out; 
      iRoute* xroute = possibleRoutes.top();
      // std::cout<<"Name of source city: "<<possibleRoutes.top()->out<<'\n';
      mCities[out]->correctroute.insert(possibleRoutes.top());
      //correctroute[out]=possibleRoutes.top();
      for(Route* edge : mCities[in]->edges){//the city at the road we chose directs to 
        iRoute info; //need to make sure don't go backwards
        if(edge->city1!=out&&edge->city2!=out){
          info.out=in; 
          if(edge->city1==in)
            info.in = edge->city2; 
          else 
            info.in =edge->city1; 
          info.totalDays = edge->days+xroute->totalDays;
          // std::cout<<"City: "<<info.in<<'\n';
          // std::cout<<info.totalDays<<'\n';
          info.id = edge->id; 
          possibleRoutes.push(new iRoute(info));
        }
      }
    }
    else{
      //delete possibleRoutes.top();
      todelete.push_back(possibleRoutes.top());
      possibleRoutes.pop();
    }
  }
  // for(const auto& [name,city]: mCities){
  //   (void) city;
  //   for(iRoute* xroute: mCities[name]->correctroute){//trying to access routes. so if 
  //     //std::cout<<city->doses_needed<<" Doses needed in "<<name<<'\n';
  //     std::cout<<name<<" has roads leading to "<<xroute->in<<'\n';
  //   }
  // }
}
bool MyScheduler::in_city_two(std::string name){
  for(std::string list : namecheck){
    if(list==name)
    return true; 
  }
  //std::cout<<"made it out in city two"<<'\n';
  return false; 
}
bool MyScheduler::name_is_factory(std::string name){
  for(const auto& factory : mFactories){
    if(name==factory)
    return true; 
  }
  //std::cout<<"made it out name is factory"<<'\n';
  return false; 
}
unsigned int MyScheduler::set_doses_needed(std::string name){//updating the total vaccines in the city
    if(mCities[name]->correctroute.empty())
      return mCities[name]->population; 
  mCities[name]->doses_needed = mCities[name]->population;
  //std::cout<<name<<"needs"<<mCities[name]->doses_needed<<'\n';
  for(iRoute* route: mCities[name]->correctroute)
    mCities[name]->doses_needed += set_doses_needed(route->in);
  return mCities[name]->doses_needed;
}
std::vector<Shipment> MyScheduler::schedule(){
  unsigned int thisday =0; 
  while(thisday-1!=mDeadline){//because can deliver on the day of deadline
    //for(const auto& [name,city]: mCities){
      //std::cout<<"where it's going: "<<route->in<<" where it's coming from: "<<route->out<<'\n';
      if(thisday==0)
        for(const auto& name: mFactories){
          //if(name_is_factory(name)){//shipments being sent on day 1
          receiving.insert(name);
            for(iRoute* route: mCities[name]->correctroute/* const auto& [mname,mroute]: correctroute*/){
            // std::cout<<"NEW SHIPMENT"<<'\n';
              //receiving.insert(name); 
              Shipment mshipment; 
              mshipment.route_id = route->id; 
              mshipment.source = name; 
              mshipment.day = 0;
              //mshipment.day=route->totalDays; // double check this 
              mshipment.doses=mCities[route->in]->doses_needed; //total_doses(name); //doesn't seem to be working, wait you don't even know which one this is...
              // std::cout<<"total doses sent: "<<mshipment.doses<<'\n';
              // std::cout<<"where it's going: "<<route->in<<" with route totalTime of: "<<route->totalDays<<'\n';
              // std::cout<<"where it's coming from: "<<route->out<<'\n';
              // std::cout<<"TODAY IS: "<<thisday<<'\n';
              route->doses=mshipment.doses; 
              mSchedule.push_back(mshipment); //could make a priority queue and pushback 
                
            //std::cout<<route->id<<" shipment doses "<<mshipment.doses<<'\n';
            // for(const auto& [name,route]: correctroute)
            // std::cout<<"Route ID "<<route->id<<"'s totalDays: "<<route->totalDays<<" carrying "<<route->doses<<" doses."<<'\n';
            }
        }
          //}
      for(std::string name: receiving){
        mCities[name]->count=0; 
        for(iRoute* newroute: mCities[name]->correctroute){
          if(thisday>newroute->totalDays){
            mCities[name]->count+=1; 
          }
          if(thisday == newroute->totalDays) {
            receiving.insert(newroute->in);
            for(iRoute* xroute: mCities[newroute->in]->correctroute){
          //   std::cout<<"NEW SHIPMENT"<<'\n';
          //   std::cout<<"where it's going to: "<<newroute->in<<'\n';
          // std::cout<<"what time: "<<thisday<<'\n';
          // std::cout<<"where it's coming from: "<<newroute->out<<'\n';
              //receiving.insert(xroute->out);
              Shipment mshipment; 
              mshipment.route_id = xroute->id; 
              mshipment.source = xroute->out; 
              mshipment.day = thisday; 
              mshipment.doses=mCities[xroute->in]->doses_needed; 
              xroute->doses=mshipment.doses; 
              std::cout<<"shipping this much: "<<xroute->doses<<" from: "<<xroute->out<<'\n';
              mSchedule.push_back(mshipment); 
            }
          }
        }
        /*if(count==mCities[name]->correctroute.size()){
          receiving.erase(name);
          std::cout<<"IS this the problem?"<<'\n';
        }*/
      }
      std::set<std::string> temp; 

      for(std::string name: receiving){
        if(mCities[name]->count==mCities[name]->correctroute.size())
        temp.insert(name);
      }
      for(std::string name: temp){
        receiving.erase(name);
      }
      temp.clear();
      //std::cout<<"how many times"<<'\n';
      /*for(iRoute* route: city->correctroute)
        if(thisday == route->totalDays) {
          
          for(iRoute* newroute: mCities[route->in]->correctroute){
          //   std::cout<<"NEW SHIPMENT"<<'\n';
          //   std::cout<<"where it's going to: "<<newroute->in<<'\n';
            
          // std::cout<<"what time: "<<thisday<<'\n';
          // std::cout<<"where it's coming from: "<<newroute->out<<'\n';
            Shipment mshipment; 
            mshipment.route_id = newroute->id; 
            mshipment.source = newroute->out; 
            mshipment.day = thisday; 
            mshipment.doses=mCities[newroute->in]->doses_needed; //route->in is the city we are talking about 
            //std::cout<<mshipment.doses<<'\n';
        
            newroute->doses=mshipment.doses; 
            std::cout<<"shipping this much: "<<newroute->doses<<'\n';
            mSchedule.push_back(mshipment); 
          }
        }*/
    //}
    thisday+=1;  
  }
  std::cout<<"DEADLINE: "<<mDeadline<<'\n';
  return mSchedule; 
}
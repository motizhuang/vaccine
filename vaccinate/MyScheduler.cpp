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
MyScheduler::MyScheduler(unsigned int deadline,
std::map<std::string, unsigned int> cities,
  std::vector<std::string>            factories,
  std::vector<Route>                  routes){
    mDeadline = deadline;  
    mFactories = factories; 
    for(const auto& mroute: routes){//create routes 
      Route info; 
      info.id=mroute.id; 
      info.city1=mroute.city1;
      info.city2=mroute.city2; 
      info.days=mroute.days;
      info.load=mroute.load;
      info.cost=mroute.cost; 
      mRoutes.push_back(new Route(info));
      //std::cout<<info.id<<'\n';
    }
    std::cout<<"made it out"<<'\n';
    for(const auto& [name, pop]: cities) {//create cities 
      City info; 
      info.name=name; 
      info.population=pop; 
      //std::set<Route*> edges; 
      //info.factory=false;
      //info.vaccines=0;
      mCities[name]=new City(info);
      //mCities[name] = new MyScheduler::City::City(name, pop);
      for(const auto& route: mRoutes){
        if(route->city1==name||route->city2==name){
          mCities[name]->edges.insert(route); 
        }
      }
      for(const auto& factory: factories){
        if(name==factory)
        mCities[name]->factory = true; 
      }
    }
    std::cout<<"made it out"<<'\n';
    set_route(); //create correct route 
    for(const auto& factory: factories)
    set_doses_needed(factory); 
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
  std::cout<<"made it out"<<'\n';
  while(!possibleRoutes.empty()){
    if(!in_city_two(possibleRoutes.top()->in)){
      namecheck.insert(possibleRoutes.top()->in);
      std::cout<<"Name of city being viewed: "<<possibleRoutes.top()->in<<'\n';
      std::string in = possibleRoutes.top()->in;
      std::string out = possibleRoutes.top()->out; 
      std::cout<<"Name of source city: "<<possibleRoutes.top()->out<<'\n';
      mCities[out]->correctroute.insert(possibleRoutes.top());
      correctroute[out]=possibleRoutes.top();
      for(Route* edge : mCities[in]->edges){//the city at the road we chose directs to 
        iRoute info; //need to make sure don't go backwards
        if(edge->city1!=out&&edge->city2!=out){
          info.out=in; 
          if(edge->city1==in)
            info.in = edge->city2; 
          else 
            info.in =edge->city1; 
          info.totalDays = edge->days+correctroute[out]->totalDays;
          info.id = edge->id; 
          possibleRoutes.push(new iRoute(info));
        }
      }
    }
    else
      possibleRoutes.pop();
  }
  for(std::string list : namecheck){
    std::cout<<"namecheck: "<<list<<'\n';
  }
  // for(iRoute* xroute: mCities["Cadiz"]->correctroute){//trying to access routes. so if 
  //   std::cout<<"Cadiz correctroutes: "<<xroute->in<<'\n';
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
  //int pass=0; 
  //for(const auto& [cityname,city]: mCities){
    if(mCities[name]->correctroute.empty())
      return mCities[name]->population; 
  /*for(const auto& [routename,router]: correctroute){//trying to access routes. so if 
    if(name==routename)
      pass++; 
    (void) router;
  }
  if(pass==0){
      return mCities[name]->population;  
  }*/
  mCities[name]->doses_needed = mCities[name]->population;
  for(iRoute* route: mCities[name]->correctroute)
    mCities[name]->doses_needed += set_doses_needed(route->in);
  /*for(const auto& [routename,route]: correctroute){
    if(route->out==name)
    mCities[name]->doses_needed += set_doses_needed(route->in);
  }*/
  return mCities[name]->doses_needed;
}
std::vector<Shipment> MyScheduler::schedule(){
  unsigned int thisday =0; 
  while(thisday-1!=mDeadline){//because can deliver on the day of deadline
    for(const auto& [name,city]: mCities){
      (void) city;
      //std::cout<<"where it's going: "<<route->in<<" where it's coming from: "<<route->out<<'\n';
      if(thisday==0)
        if(name_is_factory(name)){//shipments being sent on day 1
          for(iRoute* route: mCities[name]->correctroute/* const auto& [mname,mroute]: correctroute*/){
            std::cout<<"NEW SHIPMENT"<<'\n';
            Shipment mshipment; 
            mshipment.route_id = route->id; 
            mshipment.source = name; 
            mshipment.day = 0;
            mshipment.day=route->totalDays; // double check this 
            mshipment.doses=mCities[route->in]->doses_needed; //total_doses(name); //doesn't seem to be working, wait you don't even know which one this is...
            std::cout<<"total doses sent: "<<mshipment.doses<<'\n';
            std::cout<<"where it's going: "<<route->in<<" with route totalTime of: "<<route->totalDays<<'\n';
            std::cout<<"where it's coming from: "<<route->out<<'\n';
            std::cout<<"TODAY IS: "<<thisday<<'\n';
            route->doses=mshipment.doses; 
            mSchedule.push_back(mshipment); //could make a priority queue and pushback 
              
          //std::cout<<route->id<<" shipment doses "<<mshipment.doses<<'\n';
          // for(const auto& [name,route]: correctroute)
          // std::cout<<"Route ID "<<route->id<<"'s totalDays: "<<route->totalDays<<" carrying "<<route->doses<<" doses."<<'\n';
          }
        }
      for(iRoute* route: mCities[name]->correctroute)
        if(thisday == route->totalDays) {
          
          for(iRoute* newroute: mCities[route->in]->correctroute){
            std::cout<<"NEW SHIPMENT"<<'\n';
            std::cout<<"where it's going to: "<<newroute->in<<'\n';
            
          std::cout<<"what time: "<<thisday<<'\n';
          std::cout<<"where it's coming from: "<<newroute->out<<'\n';
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
        }
    }
    thisday+=1;  
  }
  std::cout<<"DEADLINE: "<<mDeadline<<'\n';
  return mSchedule; 
}
/*bool have_enough_doses(){
  return true; 
}*/

// MyScheduler Member Functions

//need to remember to add the already traversed times 
  /*while(!nextcities.empty()){
  for(const auto& set: nextcities)
    for(Route* edge : mCities[set]->edges){
      possibleRoutes.push(edge); //basically dijkstra's
      nextcities.erase(set);  //to recycle nextcities
      while(!possibleRoutes.empty()){
        if(!in_city_two(possibleRoutes.top()->city2)){
          correctroute[set]=possibleRoutes.top(); 
        //vector of routes .push(queue.top())
          namecheck.insert(possibleRoutes.top()->city2); 
          nextcities.insert(possibleRoutes.top()->city2);
        }else
        possibleRoutes.pop(); 
      }
    }
  } */
  //now do for inserted routes 
    //when popping from priority queue, pop only if city 1 is factory 


//else
      /*if(name_is_factory(name)){
        Shipment mshipment; 
        mshipment.route_id = route->id; 
        mshipment.source = name; 
        mshipment.day = thisday; 
        mshipment.doses=mCities[name]->vaccines-mCities[name]->population; 
        mSchedule.push_back(mshipment); 
      }*/
    //mCities[route->city2]->vaccines = mshipment.doses; 
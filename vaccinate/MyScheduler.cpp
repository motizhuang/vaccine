#include "MyScheduler.h"
#include <vector> 
#include <string> 
#include <map>
#include <queue> 
#include <set> 
#include "Objects.h"

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
    }
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
    set_route(); //create correct route 

}
void MyScheduler::set_route(){
  //for(const auto& [name,pop] : mCities){
  for(const auto& factories : mFactories){
    for(Route* edge : mCities[factories]->edges){
      //need name of city going to and the time it takes 
      iRoute info; 
      if(edge->city1==factories){
      info.out=factories; 
      info.in=edge->city2; }
      else{
        info.out=edge->city2; 
        info.in = factories; 
      }
      info.totalDays = edge->days;  
      info.id = edge->id; 
      possibleRoutes.push(new iRoute(info)); 
    }
    namecheck.insert(factories); 
  }
  while(!possibleRoutes.empty()){
    if(!in_city_two(possibleRoutes.top()->in)){
      correctroute[possibleRoutes.top()->out]=possibleRoutes.top();//does this guarantee? yeah, just checking the best after checking if already found best
      for(Route* edge : mCities[possibleRoutes.top()->in]->edges){//the city at the road we chose directs to 
        iRoute info; //need to make sure you don't insert the route to possibleRoute.out() 
        if(edge->city1!=possibleRoutes.top()->out&&edge->city2!=possibleRoutes.top()->out){
          if(edge->city1==possibleRoutes.top()->in){
            info.out=possibleRoutes.top()->in;
            info.in = edge->city2; 
          }
          else {
            info.out=edge->city2;
            info.in =edge->city1; 
          }
        }
        info.totalDays = edge->days+correctroute[possibleRoutes.top()->out]->totalDays;  
        info.id = edge->id; 
        possibleRoutes.push(new iRoute(info));
        possibleRoutes.pop();
      }
      //nextcities.insert(possibleRoutes.top()->in); 
      namecheck.insert(possibleRoutes.top()->in); 
    }
    else
      possibleRoutes.pop(); 
  }
}//need to remember to add the already traversed times 
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
bool MyScheduler::in_city_two(std::string name){
  for(std::string list : namecheck){
    if(list==name)
    return true; 
  }
  return false; 
}
bool MyScheduler::name_is_factory(std::string name){
  for(const auto& factory : mFactories){
    if(name==factory)
    return true; 
  }
  return false; 
}
unsigned int MyScheduler::total_doses(std::string name){//updating the total vaccines in the city 
  for(const auto& [routename,route]: correctroute){//trying to access routes. so if 
  if(name!=routename)//the last city in the chain won't have a correct route because correct route names go from start to end city. 
  return 0; 
  (void) route; 
}
  unsigned int total = mCities[correctroute[name]->in]->population; 
  return total+=total_doses(correctroute[name]->in);
}
std::vector<Shipment> MyScheduler::schedule(){
  unsigned int thisday =0; 
  while(thisday-1!=mDeadline){//because can deliver on the day of deadline
    if(thisday==0)
    for(const auto& [name,route]: correctroute){
      if(name_is_factory(name)){ 
        Shipment mshipment; 
        mshipment.route_id = route->id; 
        mshipment.source = name; 
        mshipment.day = 0; 
        mshipment.doses=total_doses(name); 
        route->doses=mshipment.doses; 
        mSchedule.push_back(mshipment); 
      if(thisday == route->totalDays) {
        mCities[route->in]->vaccines = route->doses; 
        Shipment mshipment; 
        mshipment.route_id = correctroute[route->in]->id; 
        mshipment.source = route->in; 
        mshipment.day = thisday; 
        mshipment.doses=mCities[route->in]->vaccines-mCities[route->in]->population; //route->in is the city we are talking about 
        correctroute[route->in]->doses=mshipment.doses; 
        mSchedule.push_back(mshipment); 
      }
      //else
      /*if(name_is_factory(name)){
        Shipment mshipment; 
        mshipment.route_id = route->id; 
        mshipment.source = name; 
        mshipment.day = thisday; 
        mshipment.doses=mCities[name]->vaccines-mCities[name]->population; 
        mSchedule.push_back(mshipment); 
      }*/
    }
    //mCities[route->city2]->vaccines = mshipment.doses; 
    }
  thisday+=1;  
  }
  return mSchedule; 
}
/*bool have_enough_doses(){
  return true; 
}*/

// MyScheduler Member Functions

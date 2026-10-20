// RED Spice - local copper repair. Prefer a quick acceptable path; never wait
// for a globally optimal board layout or change a net's electrical identity.
#include "Circuit.hpp"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <limits>
#include <queue>

namespace
{
  using namespace hbs;
  constexpr double epsilon = 1e-8;
  double extent(Position a, Position b)
  { return std::max(std::abs(a.x-b.x), std::abs(a.y-b.y)); }
  double cross(Position a, Position b) { return a.x*b.y-a.y*b.x; }
  double point_distance(Position p, Position a, Position b)
  {
    Position d=b-a;
    double length=d.x*d.x+d.y*d.y;
    double t=length ? std::clamp(((p.x-a.x)*d.x+(p.y-a.y)*d.y)/length,0.0,1.0) : 0;
    return std::hypot(p.x-a.x-t*d.x,p.y-a.y-t*d.y);
  }
  double segment_distance(Position a, Position b, Position c, Position d)
  {
    double ab1=cross(b-a,c-a), ab2=cross(b-a,d-a);
    double cd1=cross(d-c,a-c), cd2=cross(d-c,b-c);
    if (((ab1>0 && ab2<0)||(ab1<0 && ab2>0)) &&
        ((cd1>0 && cd2<0)||(cd1<0 && cd2>0))) return 0;
    return std::min({point_distance(a,c,d),point_distance(b,c,d),
                     point_distance(c,a,b),point_distance(d,a,b)});
  }
  struct Work
  {
    const RoutingBudget &limits;
    RoutingResult &result;
    std::chrono::steady_clock::time_point deadline;
    Work(const RoutingBudget &b, RoutingResult &r):limits(b),result(r),
      deadline(std::chrono::steady_clock::now()+std::chrono::milliseconds(b.milliseconds)) {}
    bool Spend()
    {
      if(result.limited)return false;
      if(result.checks>=limits.max_checks ||
         (result.checks%64==0 && std::chrono::steady_clock::now()>=deadline))
        {result.limited=true;return false;}
      ++result.checks;return true;
    }
    bool Expand()
    {
      if(result.expanded>=limits.max_expansions) {result.limited=true;return false;}
      ++result.expanded;return Spend();
    }
  };
  struct Block
  {
    Position a,b;
    unsigned layers;
    double clearance;
  };
  unsigned layer_mask(ILink::Layer layer) {return layer==ILink::TOP?1:2;}
  bool near_box(const Block &block, Position a, Position b, double extra=0)
  {
    double gap=block.clearance+extra;
    return std::max(block.a.x,block.b.x)+gap>=std::min(a.x,b.x) &&
      std::min(block.a.x,block.b.x)-gap<=std::max(a.x,b.x) &&
      std::max(block.a.y,block.b.y)+gap>=std::min(a.y,b.y) &&
      std::min(block.a.y,block.b.y)-gap<=std::max(a.y,b.y);
  }
  std::vector<Block> copper(const Track::Layout &layout)
  {
    std::vector<Block> out;
    std::vector<bool> used(layout.nodes.size(),false);
    for(const auto &edge:layout.segments)
      {
        const auto &a=layout.nodes[edge.from],&b=layout.nodes[edge.to];
        bool via=a.second!=b.second;
        out.push_back({a.first,b.first,layer_mask(a.second)|layer_mask(b.second),via?.6:.25});
        used[edge.from]=used[edge.to]=true;
      }
    for(size_t i=0;i<layout.nodes.size();++i)
      if(!used[i])out.push_back({layout.nodes[i].first,layout.nodes[i].first,
                                 layer_mask(layout.nodes[i].second),.25});
    return out;
  }
  struct Pad {Track *net;Block block;};
  struct Obstacles
  {
    std::map<Track*,std::vector<Block> > tracks;
    std::vector<Pad> pads;
    std::vector<Block> Nearby(Track *net, Position a, Position b, unsigned layer, Work &work) const
    {
      std::vector<Block> out;
      for(const auto &entry:tracks)
        if(entry.first!=net)
          for(const auto &block:entry.second)
            {
              if(!work.Spend())return {};
              if((block.layers&layer) && near_box(block,a,b,4))out.push_back(block);
            }
      for(const auto &pad:pads)
        {
          if(!work.Spend())return {};
          if(pad.net!=net && near_box(pad.block,a,b,4))out.push_back(pad.block);
        }
      return out;
    }
  };
  void simplify(std::vector<Position> &path)
  {
    std::vector<Position> out;
    for(Position p:path)
      {
        if(!out.empty() && extent(p,out.back())<epsilon)continue;
        while(out.size()>1)
          {
            Position a=out.back()-out[out.size()-2],b=p-out.back();
            if(std::abs(cross(a,b))>epsilon || a.x*b.x+a.y*b.y<0)break;
            out.pop_back();
          }
        out.push_back(p);
      }
    path.swap(out);
  }
  struct Search
  {
    Work &work;
    std::vector<Block> blocks;
    bool Clear(Position a, Position b)
    {
      if(!work.Spend())return false;
      for(const auto &block:blocks)
        {
          if(!work.Spend())return false;
          if(near_box(block,a,b) && segment_distance(a,b,block.a,block.b)<block.clearance-epsilon)
            return false;
        }
      return true;
    }
    bool Simple(Position a, Position b, std::vector<Position> &out)
    {
      Position d=b-a;
      double diagonal=std::min(std::abs(d.x),std::abs(d.y));
      Position v{std::copysign(diagonal,d.x),std::copysign(diagonal,d.y)};
      const Position corners[]={a+v,b-v,{a.x,b.y},{b.x,a.y}};
      for(Position corner:corners)
        if(Clear(a,corner) && Clear(corner,b))
          {out={a,corner,b};simplify(out);return true;}
      return false;
    }
    bool Route(Position a, Position b, std::vector<Position> &out)
    {
      if(!std::isfinite(a.x) || !std::isfinite(a.y) || !std::isfinite(b.x) || !std::isfinite(b.y) ||
         std::max({std::abs(a.x),std::abs(a.y),std::abs(b.x),std::abs(b.y)})>1000000)return false;
      if(Simple(a,b,out))return true;
      if(work.result.limited)return false;
      int left=int(std::floor(std::min(a.x,b.x)))-3;
      int top=int(std::floor(std::min(a.y,b.y)))-3;
      int width=int(std::ceil(std::max(a.x,b.x)))-left+4;
      int height=int(std::ceil(std::max(a.y,b.y)))-top+4;
      if(width<=0 || height<=0 || width>32 || height>32)return false;
      int count=width*height;
      std::vector<double> cost(count,std::numeric_limits<double>::infinity());
      std::vector<int> parent(count,-1);
      std::vector<std::vector<Position> > prefix(count),suffix(count);
      struct Entry {double priority,cost;int index;};
      struct Later {bool operator()(const Entry &a,const Entry &b) const
        {return a.priority!=b.priority?a.priority>b.priority:a.index>b.index;}};
      std::priority_queue<Entry,std::vector<Entry>,Later> open;
      auto point=[&](int index){return Position{double(left+index%width),double(top+index/width)};};
      auto estimate=[&](Position p){double x=std::abs(p.x-b.x),y=std::abs(p.y-b.y);
        return std::max(x,y)+.41421356237*std::min(x,y);};
      for(int y=0;y<2;++y)for(int x=0;x<2;++x)
        {
          int sx=int(std::floor(a.x))+x,sy=int(std::floor(a.y))+y;
          int start=(sy-top)*width+sx-left;
          if(Simple(a,point(start),prefix[start]))
            {cost[start]=std::hypot(a.x-sx,a.y-sy);open.push({cost[start]+1.2*estimate(point(start)),cost[start],start});}
          int gx=int(std::floor(b.x))+x,gy=int(std::floor(b.y))+y;
          int goal=(gy-top)*width+gx-left;
          Simple(point(goal),b,suffix[goal]);
        }
      const int dx[]={1,1,0,-1,-1,-1,0,1},dy[]={0,1,1,1,0,-1,-1,-1};
      while(!open.empty())
        {
          if(!work.Expand())return false;
          Entry item=open.top();open.pop();
          int current=item.index;
          if(item.cost!=cost[current])continue;
          if(!suffix[current].empty())
            {
              std::vector<int> chain;
              for(int p=current;p!=-1;p=parent[p])chain.push_back(p);
              std::reverse(chain.begin(),chain.end());
              out=prefix[chain.front()];
              for(int p:chain)out.push_back(point(p));
              out.insert(out.end(),suffix[current].begin(),suffix[current].end());
              simplify(out);return true;
            }
          Position p=point(current);
          for(int dir=0;dir<8;++dir)
            {
              int x=current%width+dx[dir],y=current/width+dy[dir];
              if(x<0 || y<0 || x>=width || y>=height)continue;
              int next=y*width+x;
              double bend=0;
              if(parent[current]!=-1)
                {Position before=p-point(parent[current]);
                 if(before.x!=dx[dir] || before.y!=dy[dir])bend=.12;}
              double value=cost[current]+(dir%2?1.41421356237:1)+bend;
              if(value>=cost[next] || !Clear(p,point(next)))continue;
              cost[next]=value;parent[next]=current;
              open.push({value+1.2*estimate(point(next)),value,next});
            }
        }
      return false;
    }
  };
  bool repair(Track::Layout &layout, const RoutingAnchor &anchor, Position pin,
              Track *net, const Obstacles &obstacles, Work &work)
  {
    size_t node=anchor.node,edge=Track::NoNode,degree=0;
    if(node>=layout.nodes.size() || extent(layout.nodes[node].first,anchor.before)>epsilon)return false;
    auto layer=layout.nodes[node].second;
    std::vector<std::vector<size_t> > adjacent(layout.nodes.size());
    std::vector<bool> attached(layout.nodes.size(),false),via(layout.nodes.size(),false);
    bool stable=false;
    for(size_t i=0;i<layout.segments.size();++i)
        {
          if(!work.Spend())return false;
          auto e=layout.segments[i];
          adjacent[e.from].push_back(i);adjacent[e.to].push_back(i);
          if(layout.nodes[e.from].second!=layout.nodes[e.to].second)via[e.from]=via[e.to]=true;
          if(e.from==node || e.to==node) {++degree;edge=i;}
        }
    if(!degree)return false;
    for(size_t i=0;i<layout.attachment_nodes.size();++i)
      {
        size_t n=layout.attachment_nodes[i];
        if(n<attached.size())attached[n]=true;
        if(i!=anchor.attachment && n==node)stable=true;
      }
    stable=stable || via[node];
    size_t start=node,target=node;
    if(degree==1 && !stable)
      {
        double remaining=6;
        size_t cursor=node;
        std::vector<bool> removed(layout.segments.size(),false);
        std::vector<Track::Segment> kept;
        for(size_t steps=0;steps<48;++steps)
          {
            if(!work.Spend())return false;
            auto e=layout.segments[edge];removed[edge]=true;
            size_t next=e.from==cursor?e.to:e.from;
            Position a=layout.nodes[cursor].first,b=layout.nodes[next].first;
            double length=extent(a,b);
            target=next;
            if(length>remaining+epsilon)
              {
                // Preserve the remote portion of a long segment, exactly.
                Position cut={a.x+(b.x-a.x)*remaining/length,a.y+(b.y-a.y)*remaining/length};
                target=layout.nodes.size();layout.nodes.push_back({cut,layer});
                kept.push_back({target,next});break;
              }
            remaining-=length;
            if(remaining<epsilon || attached[next] || via[next] || adjacent[next].size()!=2)break;
            cursor=next;
            edge=adjacent[next][0]==edge?adjacent[next][1]:adjacent[next][0];
          }
        for(size_t i=0;i<removed.size();++i)if(!removed[i])kept.push_back(layout.segments[i]);
        layout.segments.swap(kept);
        layout.nodes[node].first=pin;
      }
    else
      {
        // A shared node or a via stays fixed. Give this terminal its own short
        // branch, leaving the other terminals and layer changes in place.
        start=layout.nodes.size();layout.nodes.push_back({pin,layer});
        layout.attachment_nodes[anchor.attachment]=start;
      }
    Position goal=layout.nodes[target].first;
    Search search{work,obstacles.Nearby(net,pin,goal,layer_mask(layer),work)};
    if(work.result.limited)return false;
    std::vector<Position> path;
    if(!search.Route(pin,goal,path))return false;
    size_t previous=start;
    for(size_t i=1;i+1<path.size();++i)
      {
        size_t next=layout.nodes.size();layout.nodes.push_back({path[i],layer});
        layout.segments.push_back({previous,next});previous=next;
      }
    if(previous!=target)layout.segments.push_back({previous,target});
    return true;
  }

  void compact(Track::Layout &layout, const Track::Layout &before)
  {
    std::vector<bool> used(layout.nodes.size(),false),old_copper(before.nodes.size(),false);
    for(const auto &e:before.segments)old_copper[e.from]=old_copper[e.to]=true;
    for(const auto &e:layout.segments)used[e.from]=used[e.to]=true;
    for(size_t node:layout.attachment_nodes)if(node<used.size())used[node]=true;
    std::vector<size_t> remap(used.size(),Track::NoNode);
    std::vector<std::pair<Position,ILink::Layer> > nodes;
    for(size_t i=0;i<used.size();++i)
      // Preserve pre-existing free routing starts, but discard corners made
      // obsolete by this repair. Repeated nudges must not grow stale copper.
      if(used[i] || (i<old_copper.size() && !old_copper[i]))
        {remap[i]=nodes.size();nodes.push_back(layout.nodes[i]);}
    for(auto &e:layout.segments) {e.from=remap[e.from];e.to=remap[e.to];}
    for(size_t &node:layout.attachment_nodes)if(node!=Track::NoNode)node=remap[node];
    layout.nodes.swap(nodes);
  }
}

hbs::RoutingSnapshot hbs::Circuit::CaptureRouting(const std::set<IComponent*> &components,
                                                const std::set<Track*> &excluded) const
{
  RoutingSnapshot result;
  if(components.empty())return result;
  size_t scanned=0;
  for(const auto &entry:tracks)
    {
      Track *track=entry.second;
      if(excluded.count(track) || track->GetNbrSteps()>4096 || track->GetSegments().size()>8192)continue;
      const auto &attachments=track->GetAttachments();
      for(size_t i=0;i<attachments.size();++i)
        {
          if(++scanned>20000 || result.size()>=128)return result;
          const auto &a=attachments[i];
          if(!components.count(a.component) || a.node==Track::NoNode || a.node>=track->GetNbrSteps())continue;
          Position pin=a.component->GetPinPosition(a.pin);
          Position copper=track->GetNodePosition(a.node);
          for(const auto &segment:track->GetSegments())
            if(segment.from==a.node || segment.to==a.node)
              {result.push_back({track,a.component,a.pin,i,a.node,copper,pin});break;}
        }
    }
  return result;
}

hbs::RoutingResult hbs::Circuit::Reroute(const RoutingSnapshot &snapshot, const RoutingBudget &limits)
{
  RoutingResult result;
  if(snapshot.empty())return result;
  Work work(limits,result);
  std::set<IComponent*> live;
  std::map<Track*,std::vector<RoutingAnchor> > jobs;
  std::set<Track*> live_tracks;
  for(const auto &entry:circuit) {if(!work.Spend())return result;live.insert(entry.second);}
  for(const auto &entry:tracks) {if(!work.Spend())return result;live_tracks.insert(entry.second);}
  for(const auto &anchor:snapshot)
    {
      if(!work.Spend())return result;
      if(!live.count(anchor.component) || !live_tracks.count(anchor.track))continue;
      const auto &attachments=anchor.track->GetAttachments();
      if(anchor.attachment>=attachments.size())continue;
      const auto &a=attachments[anchor.attachment];
      if(a.component!=anchor.component || a.pin!=anchor.pin || a.node!=anchor.node)continue;
      Position pin=anchor.component->GetPinPosition(anchor.pin);
      double distance=extent(pin,anchor.pin_before),gap=extent(pin,anchor.before);
      if(distance>epsilon && distance<=5+epsilon && gap>epsilon && gap<=5+epsilon)
        {jobs[anchor.track].push_back(anchor);++result.pending;}
    }
  if(jobs.empty())return result;
  Obstacles obstacles;
  std::map<IComponent*,std::map<size_t,Track*> > owners;
  size_t nodes=0,edges=0;
  for(const auto &entry:tracks)
    {
      if(!work.Spend())return result;
      nodes+=entry.second->GetNbrSteps();edges+=entry.second->GetSegments().size();
      if(nodes>20000 || edges>20000) {result.limited=true;return result;}
      obstacles.tracks[entry.second]=copper(entry.second->GetLayout());
      for(const auto &a:entry.second->GetAttachments())
        {if(!work.Spend())return result;owners[a.component][a.pin]=entry.second;}
    }
  for(const auto &entry:circuit)
    if(!dynamic_cast<Track*>(entry.second))
      for(size_t pin=1;pin<=entry.second->GetPinCount();++pin)
        {
          if(!work.Spend())return result;
          Position p=entry.second->GetPinPosition(pin);
          auto owner=owners.find(entry.second);
          Track *net=nullptr;
          if(owner!=owners.end())
            {auto found=owner->second.find(pin);if(found!=owner->second.end())net=found->second;}
          obstacles.pads.push_back({net,{p,p,3,.6}});
        }
  for(const auto &job:jobs)
    {
      if(!work.Spend())break;
      Track::Layout before=job.first->GetLayout(),candidate=before;
      bool valid=true;
      for(const auto &anchor:job.second)
        if(!repair(candidate,anchor,anchor.component->GetPinPosition(anchor.pin),job.first,obstacles,work))
          {valid=false;break;}
      if(!valid || result.limited)continue;
      compact(candidate,before);
      auto blocks=copper(candidate);
      if(!work.Spend())break;
      job.first->SetLayout(candidate);
      obstacles.tracks[job.first].swap(blocks);
      result.repaired+=job.second.size();result.pending-=job.second.size();
    }
  return result;
}

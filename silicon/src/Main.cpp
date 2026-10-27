#include "Editor.hpp"
#include <cstdlib>
#include <iostream>
#include <stdexcept>

static void Usage(){std::cerr<<"BCAD / mode circuit integre\n"
  "  bcad dessin.rsi                    ouvrir un buffer CI dans BCAD\n"
  "  bcad --ic dessin.rsi --check [--simulate N] [A=0|1|U|Z|B ...]\n"
  "  bcad-ic-cli dessin.rsi --check     outil sans LibLapin\n"
  "  --render image.ppm   --spice chip.cir   --csv trace.csv\n"
  "  --examples dossier  --save dessin.rsi\n";}
int silicon::CommandLine(int argc,char **argv){
  try{
    silicon::Document doc=silicon::Example("nmos-inverter");std::string file,render,spice,csv,save;int ticks=0;bool headless=false;
    std::vector<std::string> inputs;
    for(int a=1;a<argc;++a){std::string arg=argv[a];
      if(arg=="--help"){Usage();return 0;}
      if(arg=="--new"){doc=silicon::Document();continue;}
      if(arg=="--check"){headless=true;continue;}
      if(arg=="--example" || arg=="--render" || arg=="--spice" || arg=="--csv" || arg=="--simulate" || arg=="--save" || arg=="--examples"){
        if(++a==argc)throw std::runtime_error("Argument manquant: "+arg);std::string value=argv[a];
        if(arg=="--example")doc=silicon::Example(value);
        if(arg=="--render"){render=value;headless=true;}
        if(arg=="--spice"){spice=value;headless=true;}
        if(arg=="--csv"){csv=value;headless=true;}
        if(arg=="--save"){save=value;headless=true;}
        if(arg=="--simulate"){size_t used=0;ticks=std::stoi(value,&used);if(used!=value.size() || ticks<1 || ticks>100000)throw std::runtime_error("Nombre de pas attendu entre 1 et 100000");headless=true;}
        if(arg=="--examples"){for(const std::string &name:silicon::Examples())silicon::Example(name).Save(value+"/"+name+".rsi");return 0;}
      }
      else if(arg.find('=')!=std::string::npos)inputs.push_back(arg);
      else if(!arg.empty() && arg[0]=='-')throw std::runtime_error("Option inconnue: "+arg);
      else {if(!file.empty())throw std::runtime_error("Un seul dessin attendu");file=arg;doc=silicon::Document::Load(file);}
    }
    silicon::Editor editor(doc,file);
    if(!save.empty())editor.Save(save);
    if(!headless){
#ifdef REDSILICON_CLI
      throw std::runtime_error("Ce mode est sans fenetre; utiliser --help ou ouvrir le fichier avec bcad dessin.rsi");
#else
      return silicon::RunWindow(editor);
#endif
    }
    const auto &nets=editor.nets;
    for(const silicon::Diagnostic &d:nets.diagnostics)std::cerr<<(d.error?"ERREUR":"NOTE")<<" ["<<d.at.x<<","<<d.at.y<<"] "<<d.message<<'\n';
    std::cout<<nets.nodes<<" noeuds / "<<nets.devices.size()<<" dispositifs / "<<nets.ports.size()<<" broches\n";
    if(!nets.Valid())return 2;
    for(const std::string &arg:inputs){size_t at=arg.find('=');editor.simulator->Drive(arg.substr(0,at),silicon::ParseState(arg.substr(at+1)));}
    for(int t=0;t<ticks;++t)editor.Step();
    if(ticks)for(const silicon::Port &p:nets.ports)std::cout<<p.name<<'='<<silicon::Name(editor.simulator->Get(p.node))<<" ("<<editor.simulator->Voltage(p.node)<<" V)\n";
    if(!spice.empty())silicon::WriteFile(spice,nets.Spice(doc.title));
    if(!csv.empty())silicon::WriteFile(csv,editor.simulator->Csv());
    if(!render.empty()){silicon::Image image(editor.width,editor.height);editor.Render(image);silicon::WriteFile(render,image.Ppm());}
    return ticks && !editor.simulator->converged?3:0;
  }catch(const std::exception &e){std::cerr<<"bcad IC: "<<e.what()<<'\n';return 1;}
}
#ifndef BCAD_IC_LIBRARY
int main(int argc,char **argv){return silicon::CommandLine(argc,argv);}
#endif

#include "Editor.hpp"
#include <algorithm>
#include <cmath>
#include <sstream>
#include <stdexcept>

namespace silicon {
Image::Image(int w,int h):width(w),height(h) {
  if(w<1 || h<1 || w>4096 || h>4096)throw std::runtime_error("Taille image invalide");
  pixels.assign(size_t(w)*h,0x141923ff);
}
void Image::Pixel(int x,int y,Color c){if(x<0 || y<0 || x>=width || y>=height)return;
  unsigned a=c&255;Color &p=pixels[y*width+x];if(a==255){p=c;return;}
  Color r=255;for(int shift:{8,16,24})r|=((((c>>shift)&255)*a+((p>>shift)&255)*(255-a))/255)<<shift;p=r;
}
void Image::Line(int x,int y,int bx,int by,Color c){
  // Liang-Barsky clipping bounds the work for distant geometry.
  double dx=double(bx)-x,dy=double(by)-y,t0=0,t1=1;
  double p[]={-dx,dx,-dy,dy},q[]={double(x),double(width-1-x),double(y),double(height-1-y)};
  for(int k=0;k<4;++k){if(p[k]==0){if(q[k]<0)return;}else{double r=q[k]/p[k];if(p[k]<0)t0=std::max(t0,r);else t1=std::min(t1,r);if(t0>t1)return;}}
  int x2=int(std::lround(x+dx*t1)),y2=int(std::lround(y+dy*t1));x=int(std::lround(x+dx*t0));y=int(std::lround(y+dy*t0));
  int ax=std::abs(x2-x),ay=-std::abs(y2-y),sx=x<x2?1:-1,sy=y<y2?1:-1,err=ax+ay;
  for(;;){Pixel(x,y,c);if(x==x2 && y==y2)break;int e=2*err;if(e>=ay){err+=ay;x+=sx;}if(e<=ax){err+=ax;y+=sy;}}
}
void Image::Box(int x,int y,int w,int h,Color c,bool fill){if(w<1 || h<1)return;
  if(fill){for(int py=std::max(0,y);py<std::min(height,y+h);++py)for(int px=std::max(0,x);px<std::min(width,x+w);++px)Pixel(px,py,c);}
  else {Line(x,y,x+w-1,y,c);Line(x,y,x,y+h-1,c);Line(x+w-1,y,x+w-1,y+h-1,c);Line(x,y+h-1,x+w-1,y+h-1,c);}
}
void Image::Text(int x,int y,const std::string &text,Color color,int scale){
  static const std::string alphabet="ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-+./:()[]=!?_<>#,;";
  static const char *font[]={
    "0E11111F111111","1E11111E11111E","0E11101010110E","1E11111111111E","1F10101E10101F","1F10101E101010","0E11101711110F","1111111F111111","0E04040404040E","0702020212120C","11121418141211","1010101010101F","111B1515111111","11191513111111","0E11111111110E","1E11111E101010","0E11111115120D","1E11111E141211","0F10100E01011E","1F040404040404","1111111111110E","11111111110A04","11111115151B11","11110A040A1111","11110A04040404","1F01020408101F",
    "0E11131519110E","040C040404040E","0E11010204081F","1E01010E01011E","02060A121F0202","1F10101E01011E","0E10101E11110E","1F010204080808","0E11110E11110E","0E11110F01010E",
    "0000001F000000","0004041F040400","00000000000C0C","01010204081010","000C0C000C0C00","02040808080402","08040202020408","0E08080808080E","0E02020202020E","00001F001F0000","04040404040004","0E110204040004","0000000000001F","02040810080402","08040201020408","0A0A1F0A1F0A0A","00000000000C08","000C0C000C0C08"};
  int origin=x;
  for(size_t pos=0;pos<text.size();++pos){unsigned char c=text[pos];if(c==0xc3 && pos+1<text.size()){unsigned char tail=text[++pos];if(tail>=0x80 && tail<=0x85)c='A';else if(tail==0x87)c='C';else if(tail>=0x88 && tail<=0x8b)c='E';else if(tail>=0x8c && tail<=0x8f)c='I';else if(tail>=0x92 && tail<=0x96)c='O';else if(tail>=0x99 && tail<=0x9c)c='U';else if(tail>=0xa0 && tail<=0xa5)c='a';else if(tail==0xa7)c='c';else if(tail>=0xa8 && tail<=0xab)c='e';else if(tail>=0xac && tail<=0xaf)c='i';else if(tail>=0xb2 && tail<=0xb6)c='o';else if(tail>=0xb9 && tail<=0xbc)c='u';else c='?';}if(c=='\n'){x=origin;y+=10*scale;continue;}if(c>='a' && c<='z')c-=32;size_t glyph=alphabet.find(char(c));
    if(glyph<sizeof(font)/sizeof(*font)){std::string bits=font[glyph];for(int row=0;row<7;++row){unsigned value=std::stoul(bits.substr(row*2,2),nullptr,16);for(int col=0;col<5;++col)if(value&(1u<<(4-col)))Box(x+col*scale,y+row*scale,scale,scale,color);}}
    x+=6*scale;if(x>width+300)break;
  }
}
std::string Image::Ppm() const {std::ostringstream o;o<<"P6\n"<<width<<' '<<height<<"\n255\n";std::string r=o.str();r.reserve(r.size()+pixels.size()*3);for(Color c:pixels){r.push_back(char(c>>24));r.push_back(char(c>>16));r.push_back(char(c>>8));}return r;}
}

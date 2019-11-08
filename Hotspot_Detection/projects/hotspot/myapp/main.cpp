#include <bits/stdc++.h>
using namespace std;
long double  power(long double  x, unsigned int y){long double  res = 1;while (y > 0){ if (y & 1){res = res*x;} y = y>>1;x = x*x;}return res;}
const int inf=1e9;
#define PI 3.14159

struct coord{
    long double  x,y;
    coord(){
        this->x=0;
        this->y=0;
    }
    coord(long double  a,long double  b){
        this->x=a;
        this->y=b;
    }
};
int find_bottom_left(vector<coord> &pos){
    long double  minl=inf;
    for(int i=0;i<pos.size();i++){
        if(minl>min(pos[i].x,pos[i].y) ){
            minl=min(pos[i].x,pos[i].y);
        }
    }
    return minl;
}
long double  scalefactor;
void remove_negetive(vector<coord> &pos){
    scalefactor=find_bottom_left(pos);
    for(int i=0;i<pos.size();i++){
        pos[i].x-=scalefactor;
        pos[i].y-=scalefactor;
    }
}

pair<int,int> getplanearea(vector<coord> &pos){
  long double  xmin,xmax,ymin,ymax;
  for(int i=0;i<pos.size();i++){
    xmin=min(xmin,pos[i].x);
    ymin=min(ymin,pos[i].y);
    xmax=min(xmax,pos[i].x);
    ymax=min(ymax,pos[i].y);
  }
  int a=xmax-xmin;
  int b=ymax-ymin;
  int edge=max(abs(a),abs(b));
  return {edge*edge,edge};
}

/////////////////////////////////////////////////////// COUNT GRID ////////////////////////////////////
int modP,areaS,sidelength;
int lcell,N,total_countGrid_cells,total_cubicGrid_cells;

// A SINGLE CELL CELLCOUNT NUMBER OF POINTS FROM XMIN TO XMAX, YMIN TO YMAX IN A SINGLE CELL
struct countGrid_Cell{
    long double  Xmin,Xmax,Ymin,Ymax;
    int cell_count;
};

// needed to specify a limit to the count grid
countGrid_Cell grid[5000][5000];

////////////////////////////////////////////// MECC MFCC LLR /////////////////////////////////
bool lieincircle(long double  a,long double  b,int r,const coord *p){
  // (x-a)2+(y-b)2=r2
  long double  rd=(p->x-a)*(p->x-a)+(p->y-b)*(p->y-b);
  return rd<=r*r;
}
pair<long double ,vector<coord> > MECC(long double  x,long double  y, int radius,vector<coord> &p){
  vector<coord> meccpoints;
  map<pair<int,int>, int> mp;
  for(int m=0;m<p.size();m++){
    int i=p[m].x/lcell;
    int j=p[m].y/lcell;

    mp[{i,j}]++;

    coord *a=new coord(grid[i][j].Xmin,grid[i][j].Ymin);
    coord *b=new coord(grid[i][j].Xmin,grid[i][j].Ymax);
    coord *c=new coord(grid[i][j].Xmax,grid[i][j].Ymin);
    coord *d=new coord(grid[i][j].Xmax,grid[i][j].Ymax);

    if(lieincircle(x,y,radius,a) || lieincircle(x,y,radius,b) || lieincircle(x,y,radius,c) || lieincircle(x,y,radius,d)){
      meccpoints.push_back(p[m]);
    }
  }

  long double  area=lcell*lcell*mp.size();
  return {area,meccpoints};
}

pair<long double ,vector<coord> >  MFCC(long double  x,long double  y, int radius,vector<coord> &p){
  map<pair<int,int>, int> mp;
  vector<coord> mfccpoints;
  for(int m=0;m<p.size();m++){
    int i=p[m].x/lcell;
    int j=p[m].y/lcell;

    mp[{i,j}]++;

    coord *a=new coord(grid[i][j].Xmin,grid[i][j].Ymin);
    coord *b=new coord(grid[i][j].Xmin,grid[i][j].Ymax);
    coord *c=new coord(grid[i][j].Xmax,grid[i][j].Ymin);
    coord *d=new coord(grid[i][j].Xmax,grid[i][j].Ymax);

    if(lieincircle(x,y,radius,a) && lieincircle(x,y,radius,b) && lieincircle(x,y,radius,c) && lieincircle(x,y,radius,d)){
      mfccpoints.push_back(p[m]);
    }
  }

  long double  area=lcell*lcell*mp.size();
  return {area,mfccpoints};
}
long double  log_LRGrid_Upperbound(long double  areaMecc,long double  areaMfcc,int nMecc,int nMfcc){
  int uc=nMecc;
  int lc=nMfcc;
  long double  ub=areaMecc*modP/areaS;
  long double  lb=areaMfcc*modP/areaS;

  if(!(uc>lb))return log(0);

  long double  a=uc/lb;
  a=power(a,uc);
  long double  b;
  if(lc>=ub){
    b=modP-lc;
    b/=(modP-ub);
    b=power(b,(modP-uc) );
  }
  else{
    b=1;
  }
  return log(a*b);
}


class Circle{
public:
  long double  x,y,radius;
  int noofpoints;
  Circle(long double  x,long double  y,long double  radius){
    this->x=x;
    this->y=y;
    this->radius=radius;
  }
  /*
    there's a confusion in this
    do we have to take areaS and modP respect to all dataset(all points)
    or we should take the minimized filtered set
    we are going with global ones.
  */
  long double  logLikelihood_circle(){
      long double  areaCircle=3.14*radius*radius;
      long double  B=(areaCircle/areaS)*modP;
      int C=noofpoints;
      if(!C>B)return log(0);
      long double  a=(C/B);
      a=power(a,C);
      long double  b=(modP-C)/(modP-B);
      b=power(b,(modP-C));
      return log(a*b);
  }
};

/////////////////////////////////////////////////////// PHASES //////////////////////////////////////////////////////////
vector<pair<pair<int,int>,vector<coord> > > Filter_Phase(vector<coord> pos,int thetha){
  for(int i=0;i<pos.size();i++){
    int x=pos[i].x/lcell;
    int y=pos[i].y/lcell;
    if(grid[x][y].cell_count==0){
        grid[x][y].Xmin=x*lcell;
        grid[x][y].Ymin=y*lcell;
        grid[x][y].Xmax=x*lcell+lcell-0.00001;
        grid[x][y].Ymax=y*lcell+lcell-0.00001;
    }
    grid[x][y].cell_count++;
  }
  vector<pair<pair<int,int>,vector<coord> > > filtered_set;
  int sett=0;

  while(!pos.empty()){
    long double  maxxLLR=-1;
    int maxxX,maxxY,maxxR,xx,yy;

    for(int i=0;i<N;i++){
      for(int j=0;j<N;j++){
        for(int r=1;r<=N/2;r++){

          long double  cenx=(grid[i][j].Xmin+grid[i][j].Xmax)/2;
          long double  ceny=(grid[i][j].Ymin+grid[i][j].Ymax)/2;
          long double  areaMecc,areaMfcc;
          vector<coord> cirmecc,cirmfcc;

          pair<long double ,vector<coord> > p1=MECC(cenx,ceny,r,pos);
          pair<long double ,vector<coord> > p2=MFCC(cenx,ceny,r,pos);

          areaMecc=p1.first;
          areaMfcc=p2.first;
          cirmecc=p1.second;
          cirmfcc=p2.second;
          int nMecc=cirmecc.size();
          int nMfcc=cirmfcc.size();

          /*
            here there's a confusion do we have to take the area of mecc as the area of mecc circle
            or the area of the grid colloection formaed by mecc squares.
            here we are going with the area that is made up by the grip squares.
            if we take area as area of circle long double  areaMecc=PI*r*r;
            if we take area as area of circle long double  areaMfcc=PI*r*r;
          */

          long double  llr=log_LRGrid_Upperbound(nMecc,nMfcc,areaMecc,areaMfcc);
          if(llr>=thetha && llr>maxxLLR){
              maxxLLR=llr;
              maxxX=cenx;maxxY=ceny;maxxR=r;xx=i;yy=j;
          }
        }
      }
    }
    if(maxxLLR!=-1){
      filtered_set.push_back({{xx,yy},{}});
      for(int i=0;i<pos.size();i++){
        if(lieincircle(maxxX,maxxY,maxxR,&pos[i])){
          filtered_set[sett].second.push_back(pos[i]);
          //erase takes n to be hcanged
          pos.erase(pos.begin()+i);
          i--;
        }
      }
      ++sett;
    }

  }
  return filtered_set;
}

// This is diffrent from  MECC /////////////////////////////////////////////////////////////////

Circle b_md(vector<coord> R) {
    if (R.size() == 0) {
        return Circle(0, 0, -1);
    } else if (R.size() == 1) {
        return Circle(R[0].x, R[0].y, 0);
    } else if (R.size() == 2) {
        return Circle((R[0].x+R[1].x)/2.0,
                 (R[0].y+R[1].y)/2.0,
            abs(R[0].x-R[1].x+R[0].y-R[1].y)/2.0);
    } else {
        long double  D = (R[0].x - R[2].x)*(R[1].y - R[2].y) - (R[1].x - R[2].x)*(R[0].y - R[2].y);
        long double  p0 = (((R[0].x - R[2].x)*(R[0].x + R[2].x) + (R[0].y - R[2].y)*(R[0].y + R[2].y)) / 2 * (R[1].y - R[2].y) - ((R[1].x - R[2].x)*(R[1].x + R[2].x) + (R[1].y - R[2].y)*(R[1].y + R[2].y)) / 2 * (R[0].y - R[2].y))/D;
        long double  p1 = (((R[1].x - R[2].x)*(R[1].x + R[2].x) + (R[1].y - R[2].y)*(R[1].y + R[2].y)) / 2 * (R[0].x - R[2].x) - ((R[0].x - R[2].x)*(R[0].x + R[2].x) + (R[0].y - R[2].y)*(R[0].y + R[2].y)) / 2 * (R[1].x - R[2].x))/D;
        return Circle(p0, p1, abs(R[0].x - p0+R[0].y - p1) );
    }
}

Circle b_minidisk(vector<coord>& P, int i, vector<coord> R) {
    if (i == P.size() || R.size() == 3) {
        return b_md(R);
    }
    else {
        Circle D = b_minidisk(P, i+1, R);
        if (abs(P[i].x-D.x+P[i].y-D.y) > D.radius) {
            R.push_back(P[i]);
            D = b_minidisk(P, i+1, R);
        }
        return D;
    }
}

Circle SEC(vector<coord> P) {
    random_shuffle(P.begin(), P.end());
    return b_minidisk(P, 0, vector<coord>());
}

bool is_pointliesincell(pair<int,int> c,long double  x,long double  y){
    return (c.first/lcell==x && c.second/lcell==y);
}
vector<pair<Circle,long double > > Refine_Phase(vector<pair<pair<int,int>,vector<coord> > > fset,int thetha,int rmin){

  vector<pair<Circle,long double > > candidate_circles;
  for(int i=0;i<fset.size();i++){
    pair<int,int> cellcenter=fset[i].first;
    int r=inf;
    long double  cenx=(grid[cellcenter.first][cellcenter.second].Xmin+grid[cellcenter.first][cellcenter.second].Xmax)/2;
    long double  ceny=(grid[cellcenter.first][cellcenter.second].Ymin+grid[cellcenter.first][cellcenter.second].Ymax)/2;
    Circle maxC(0,0,0);
    long double  maxllr=-1;

    while(fset[i].second.size()!=0 && r>=rmin){

      Circle C=SEC(fset[i].second);
      C.noofpoints=fset[i].second.size();

      if(is_pointliesincell(cellcenter,C.x,C.y) && C.radius>=rmin){
        r=C.radius;
        long double  llr=C.logLikelihood_circle();
        if(maxllr<llr){
          maxllr=llr;
          maxC=C;
        }
      }
      // farthest from center of cellcount
      long double  dis=-1;int mpos;
      for(int j=0;j<fset[i].second.size();j++){
        if(dis>abs(fset[i].second[j].x-cenx)+abs(fset[i].second[j].y-ceny)){
          dis=abs(fset[i].second[j].x-cenx)+abs(fset[i].second[j].y-ceny);
          mpos=j;
        }
      }
      //erase takes n
      fset[i].second.erase(fset[i].second.begin()+mpos);
    }
    if(maxllr!=-1){
      candidate_circles.push_back({maxC,maxllr});
    }
  }
  return candidate_circles;
}

long double  rand_double () {
   return rand()/(long double )RAND_MAX;
}
/*
int getPoissonRandom(int u)
{
  long double  L=exp((-1)*u);
  long double  p = 1.0;
  int k = 0;

  do {
    //cout<<"1"<<endl;
    k++;
    p *= rand_long double ();
  } while (p > L);

  return k - 1;
}
*/
void generatemontecarlo(int m,long double  montecarlo[],long double  areaS,long double  rmin,long double  thetha)
{
    int i,j;
    for(i=0;i<m;i++)
    {
        vector<coord> crd;
        for(j=0;j<100;j++)
        {
            long double  lat=fmod(rand(),sidelength);
            long double  lon=fmod(rand(),sidelength);
            crd.push_back({lat,lon});
            //cout<<lat<<" "<<lon<<endl;
        }
        vector<pair<pair<int,int>,vector<coord> > > fset= Filter_Phase(crd,thetha);
        vector<pair<Circle,long double > > Ctemp=Refine_Phase(fset,thetha,rmin);

        long double  maxll=-inf;
        for(j=0;j<Ctemp.size();j++){
            if(Ctemp[i].second>maxll)
                maxll=Ctemp[i].second;
        }
        montecarlo[i]=maxll;
    }
}

int main()
{

    freopen("C:\\Users\\aakas\\Documents\\Geograhically-Robust-hotspot-Detection\\Hotspot_Detection\\projects\\hotspot\\myapp\\static\\myapp\\inputgen.txt","r",stdin);
    freopen("C:\\Users\\aakas\\Documents\\Geograhically-Robust-hotspot-Detection\\Hotspot_Detection\\projects\\hotspot\\myapp\\static\\myapp\\outputgen.txt","w",stdout);

    long double thetha,rmin,alphaP;
    int msim;
    thetha=0;
    rmin=2;
    alphaP=0.1;
    msim=5;

    int n;
    cin>>n;
    //cout<<n<<endl;

    vector<coord> points(n);

    for(int i=0;i<n;i++){
      string x;
      cin>>x;
      points[i].x=stod(x);
      cin>>x;
      points[i].y=stod(x);
    }
    for(auto i : points){
        cout<<i.x<<endl;
        cout<<i.y<<" "<<endl;
        cout<<25<<endl;
    }
    return 0;

    remove_negetive(points);
    pair<int,int> p=getplanearea(points);
    areaS=p.first;
    sidelength=p.second;
    modP=points.size();


    //ceil to be chaged
    long double lcell=ceil(rmin/2);
    int N=sidelength/lcell;
    int total_countGrid_cells=N*N;
    total_cubicGrid_cells=N*N*N;

    //3 PHASES
    vector<pair<pair<int,int>,vector<coord> > > fset= Filter_Phase(points,thetha);
    // circles, logirithmic value
    vector<pair<Circle,long double > > candidate_circles=Refine_Phase(fset,thetha,rmin);

    srand(time(NULL));
    long double  montecarlo[msim];
    for(int i=0;i<msim;i++)
        montecarlo[i]=0;

    generatemontecarlo(msim,montecarlo,areaS,rmin,thetha);
    sort(montecarlo,montecarlo+msim,greater<long double >());

    vector<Circle> hotspotCircles;
    for(int i=0;i<candidate_circles.size();i++){
        // will use binary search to search position of log in the array but
        // for the time being but here we are brute forcing to get solution
        for(int j=0;j<msim;j++){
            if(candidate_circles[i].second>montecarlo[j]){
                long double  pval=((long double )j)/((long double )msim+1);
                if(pval<=alphaP){
                    hotspotCircles.push_back(candidate_circles[i].first);
                }
                break;
            }
        }
    }
    for(int i=0;i<hotspotCircles.size();i++){
        cout<< hotspotCircles[i].x<<" "<<hotspotCircles[i].y<<" "<<hotspotCircles[i].radius<<endl;
    }
    return 0;
}

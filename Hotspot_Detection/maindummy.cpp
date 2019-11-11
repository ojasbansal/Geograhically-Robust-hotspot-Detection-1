#include <bits/stdc++.h>
#include <windows.h>
#include <conio.h>

using namespace std;
long double  power(long double  x, unsigned long y){long double  res = 1;while (y > 0){ if (y & 1){res = res*x;} y = y>>1;x = x*x;}return res;}
const long inf=1e9;
#define PI 3.14159
long double thetha,rmin,alphaP;

void gotoxy (int x, int y)
    {
         COORD coordinates;     // coordinates is declared as COORD
         coordinates.X = x;     // defining x-axis
         coordinates.Y = y;     //defining  y-axis
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),coordinates);
    }
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

long double pxmax,pxmin,pymax,pymin;

void find_bottom_left(vector<coord> &pos){
    pxmin=inf,pymin=inf;
    for(long i=0;i<pos.size();i++){
        pxmin=min(pos[i].x,pxmin);
        pymin=min(pos[i].y,pymin);
    }
}
pair<double,double> sf;//minimum subtractor
void remove_negetive(vector<coord> &pos){
    find_bottom_left(pos);
    sf = { pxmin,pymin };
   // sf.first--;sf.second--;
    pxmin=inf,pymin=inf,pxmax=0,pymax=0;
    for(long i=0;i<pos.size();i++){
        pos[i].x-=sf.first;
        pos[i].y-=sf.second;
        pxmin=min(pos[i].x,pxmin);
        pymin=min(pos[i].y,pymin);
        pxmax=max(pos[i].x,pxmax);
        pymax=max(pos[i].y,pymax);
    }
}
long double scale;
void variate(vector<coord>&pos){
    long double factorX,factorY;
    int n= pos.size();
    long double xmax = INT_MIN;
    long double xmin = INT_MAX;
    long double ymax = INT_MIN;
    long double ymin = INT_MAX;
    
    for(int i=0;i<n;i++){
      xmax= max(xmax, pos[i].x);
      ymax= max(ymax, pos[i].y);
      xmin= min(xmin, pos[i].x);
      ymin= min(ymin, pos[i].y);
    }
    cout << xmax << " " << xmin << " " << ymin << " " << ymax << endl;
    factorX= 20/(xmax-xmin);
    factorY= 20/(ymax-ymin);
    //if(factorX <100) factorX= 100/factorX;
    //if(factorY < 100) factorY=100/factorY;
    cout << factorX << "      " << factorY << endl;
    scale=min(factorX,factorY);
    for(int i=0;i<n;i++){
      pos[i].x*=scale;
      pos[i].y*=scale;
    }
    for(int i=0;i<n;i++){
      xmax= max(xmax, pos[i].x);
      ymax= max(ymax, pos[i].y);
      xmin= min(xmin, pos[i].x);
      ymin= min(ymin, pos[i].y);
    }
    cout << xmax << " " << xmin << " " << ymin << " " << ymax << endl;
    
}

void scaleback(long double &xx, long double &yy, long double &rr){
    xx/=scale;
    yy/=scale;
    xx+=sf.first;
    yy+=sf.second;
    rr/=scale;
  }

pair<int,int> getplanearea(vector<coord> &pos){
  long a=pxmax-pxmin;
  long b=pymax-pymin;
  long edge=max(abs(a),abs(b));
  return {edge*edge,edge};
}

/////////////////////////////////////////////////////// COUNT GRID ////////////////////////////////////
long modP,areaS,sidelength;
long lcell,N,total_countGrid_cells,total_cubicGrid_cells;

// A SINGLE CELL CELLCOUNT NUMBER OF POINTS FROM XMIN TO XMAX, YMIN TO YMAX IN A SINGLE CELL
struct countGrid_Cell{
    long double  Xmin,Xmax,Ymin,Ymax;
    long cell_count;
    countGrid_Cell(){cell_count=0;}
};

// needed to specify a limit to the count grid
countGrid_Cell grid[5000][5000];

class Circle{
public:
  long double  x,y,radius;
  long noofpoints;
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
      long C=noofpoints;
      if(!C>B)return log(0);
      long double  a=(C/B);
      a=C*log(a);
      long double  b=(modP-C)/(modP-B);
      b=(modP-C)*log(b);
      return a+b;
  }
};

////////////////////////////////////////////// MECC MFCC LLR /////////////////////////////////


long double  log_LRGrid_Upperbound(long nMecc,long nMfcc,long double  areaMecc,long double  areaMfcc){
  long uc=nMecc;
  long lc=nMfcc;
  long double  ub=areaMecc*modP/areaS;
  long double  lb=areaMfcc*modP/areaS;

  if(!(uc>lb))return -inf;

  long double  a=uc/lb;
  a=uc*log(a);
  long double  b;
  if(lc>=ub){
    b=modP-lc;
    b/=(modP-ub);
    b=(modP-uc)*log(b);
  }
  else{
    b=0;//log1=0
  }
  return a+b;
}
bool lieincircle(long double  a,long double  b,long r,long double x,long double y){
  long double  rd=(x-a)*(x-a)+(y-b)*(y-b);
  return rd*1.0<=r*r;
}
bool Grid_lieincircle(long double  a,long double  b,long r,long double x,long double y){
  long double  rd=(x-a)*(x-a)+(y-b)*(y-b);
  rd=sqrt(rd);//distance of grid center from circle center
  rd-=r;//extra distance need to cover to the center of grid
  if(rd<=lcell/2)return true;
  return false;
}

long checker(long double cx,long double cy,long radius,long i,long j,int var){
    if(var==0){
      coord *a=new coord(grid[i][j].Xmin,grid[i][j].Ymin);
        coord *b=new coord(grid[i][j].Xmin,grid[i][j].Ymax);
        coord *c=new coord(grid[i][j].Xmax,grid[i][j].Ymin);
        coord *d=new coord(grid[i][j].Xmax,grid[i][j].Ymax);
      while((lieincircle(cx,cy,radius,a->x,a->y) || lieincircle(cx,cy,radius,b->x,b->y) || lieincircle(cx,cy,radius,c->x,c->y) || lieincircle(cx,cy,radius,d->x,d->y))==false){
        radius++;
      }
    }
    else{
        coord *a=new coord(grid[i][j].Xmin,grid[i][j].Ymin);
        coord *b=new coord(grid[i][j].Xmin,grid[i][j].Ymax);
        coord *c=new coord(grid[i][j].Xmax,grid[i][j].Ymin);
        coord *d=new coord(grid[i][j].Xmax,grid[i][j].Ymax);
      while((lieincircle(cx,cy,radius,a->x,a->y) && lieincircle(cx,cy,radius,b->x,b->y) && lieincircle(cx,cy,radius,c->x,c->y) && lieincircle(cx,cy,radius,d->x,d->y))==false){
        radius++;
      }
    }
    return radius;
}

vector< pair<long,long> > MECC_MFCC(long double cx,long double cy,long x,long y,int var){
  map<pair<int,int>, int> mp;

  queue<pair<long,long> > q;
  int dx[3]={0,1,-1};

  vector< pair<long,long> > ans(50);
  for(int i=0;i<ans.size();i++){ans[i].second=0;ans[i].first=0;}
  long radius=1;

  radius=checker(cx,cy,radius,x,y,var);
  q.push({x,y});
  mp[{x,y}]=1;
  ans[radius].second=1;
  ans[radius].first=grid[x][y].cell_count;

  while(!q.empty() && radius<N/2){
    int a=q.front().first;
    int b=q.front().second;

    q.pop();
    for(int xd=0;xd<3 && radius<N/2;xd++){
        for(int xy=0;xy<3;xy++){
            int i=a+dx[xd];
            int j=b+dx[xy];
            if( i>=N || i<0 || j>=N || j<0 || mp[{i,j}]>0 )continue;
            radius=checker(cx,cy,radius,i,j,var);
            if(radius>=N/2)break;

            q.push({i,j});
            mp[{i,j}]++;
            ans[radius].second++;
            ans[radius].first+=grid[i][j].cell_count;
        }
    }
  }
  for(int i=1;i<ans.size();i++){
    ans[i].first+=ans[i-1].first;
    ans[i].second+=ans[i-1].second;
  }
  return ans;
}
/////////////////////////////////////////////////////// PHASES //////////////////////////////////////////////////////////
vector<pair<pair<int,int>,vector<coord> > > Filter_Phase(vector<coord> pos,long thetha){
  long double gridx=0;
  for (int i=0;i<=N;i++){
    long double gridy=0;
    for(int j=0;j<=N;j++){
        grid[i][j].Xmin=gridx;
        grid[i][j].Xmax=gridx+lcell-.00001;
        grid[i][j].Ymin=gridy;
        grid[i][j].Ymax=gridy+lcell-.00001;
        gridy+=lcell;
        grid[i][j].cell_count=0;
    }
    gridx+=lcell;
  }


  for(long i=0;i<pos.size();i++){
    long x=pos[i].x/lcell;
    long y=pos[i].y/lcell;
    grid[x][y].cell_count++;
  }

  vector<pair<pair<int,int>,vector<coord> > > filtered_set;
  long sett=0;
//cout<<N<<endl;
  while(!pos.empty()){
    long double  maxxLLR=-1;
    long double maxxX,maxxY,maxxR,gridcenter_x,gridcenter_y,pts;

vector<pair<long,long>> p1;
vector<pair<long,long>> p2;

    for(long i=0;i<N;i++){
      for(long j=0;j<N;j++){

        long double  cenx=(grid[i][j].Xmin+grid[i][j].Xmax)/2;
        long double  ceny=(grid[i][j].Ymin+grid[i][j].Ymax)/2;

        p1=MECC_MFCC(cenx,ceny,i,j,0);
        p2=MECC_MFCC(cenx,ceny,i,j,1);

        for(long r=ceil(rmin);r<p1.size();r++){

          long double areaMecc=lcell*lcell*p1[r].second;
          long double areaMfcc=lcell*lcell*p2[r].second;

          if(areaMfcc==0)continue;

          long nMecc=p1[r].first;
          long nMfcc=p2[r].first;

          long double  llr=log_LRGrid_Upperbound(nMecc,nMfcc,areaMecc,areaMfcc);

          if(llr>=thetha && llr>maxxLLR){
              maxxLLR=llr;
              maxxX=cenx;maxxY=ceny;maxxR=r;gridcenter_x=i;gridcenter_y=j;
            pts=nMfcc;
          }
        }

      }
    }
    cout<<pos.size()<<endl;

    if(maxxLLR>=thetha){
            cout<<"traped : ";

      filtered_set.push_back({{gridcenter_x,gridcenter_y},{}});
      int flag=0;
      for(long i=0;i<pos.size();i++){
        if(lieincircle(maxxX,maxxY,maxxR,pos[i].x,pos[i].y)){
          filtered_set[sett].second.push_back(pos[i]);
         // gotoxy(pos[i].x,pos[i].y);cout<<"*";
          long rx=pos[i].x/lcell;
          long ry=pos[i].y/lcell;
          grid[rx][ry].cell_count--;
          pos.erase(pos.begin()+i);
          i--;
          flag++;
        }
      }
      if(flag)
        ++sett;
      else{
        filtered_set.pop_back();
        for(long i=0;i<pos.size();i++){
            cout<<pos[i].x<<" "<<pos[i].y<<endl;
        }
        //   chod h
        cout<<"filter Breakout : "<<filtered_set.size()<<endl;
        return filtered_set;
      }
    }
    else break;
  }
  cout<<"filter done : "<<filtered_set.size()<<endl;
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

Circle b_minidisk(vector<coord>& P, long i, vector<coord> R) {
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
    double Xmin=grid[c.first][c.second].Xmin-1;//for approximate ans
    double Xmax=grid[c.first][c.second].Xmax+1;
    double Ymin=grid[c.first][c.second].Ymin-1;
    double Ymax=grid[c.first][c.second].Ymax+1;
    if(x>=Xmin && x<=Xmax){
        if(y>=Ymin && y<=Ymax)return true;
    }
    return false;
}
vector<pair<Circle,long double > > Refine_Phase(vector<pair<pair<int,int>,vector<coord> > > fset,long double thetha,long double rmin){

  vector<pair<Circle,long double > > candidate_circles;
  for(long i=0;i<fset.size();i++){
    pair<int,int> cellcenter=fset[i].first;
    long double r=inf;
    long double  cenx=(grid[cellcenter.first][cellcenter.second].Xmin+grid[cellcenter.first][cellcenter.second].Xmax)/2;
    long double  ceny=(grid[cellcenter.first][cellcenter.second].Ymin+grid[cellcenter.first][cellcenter.second].Ymax)/2;

    Circle maxC(0,0,0);
    long double  maxllr=-1;

    while(fset[i].second.size()!=0 && r>=rmin){

    //cout<<fset[i].second.size()<<endl;
      Circle C=SEC(fset[i].second);
      C.noofpoints=fset[i].second.size();
      r=C.radius;

      if(is_pointliesincell(cellcenter,C.x,C.y) && C.radius>=rmin){
        r=C.radius;
        long double  llr=C.logLikelihood_circle();
        if(maxllr<llr){
          maxllr=llr;
          maxC=C;
        }
      }
      // farthest from center of cellcount
      long double  dis=-1;long mpos=-1;
      for(long j=0;j<fset[i].second.size();j++){
        if(dis<abs(fset[i].second[j].x-cenx)+abs(fset[i].second[j].y-ceny)){
          dis=abs(fset[i].second[j].x-cenx)+abs(fset[i].second[j].y-ceny);
          mpos=j;
        }
      }
      //erase takes n
      if(mpos!=-1)
        fset[i].second.erase(fset[i].second.begin()+mpos);
    }

    if(maxllr!=-1){
      candidate_circles.push_back({maxC,maxllr});
    }
  }
  cout<<"refine done : "<<candidate_circles.size()<<endl;
  return candidate_circles;
}

/*
long getPoissonRandom(long u)
{
  long double  L=exp((-1)*u);
  long double  p = 1.0;
  long k = 0;

  do {
    //cout<<"1"<<endl;
    k++;
    p *= rand_long double ();
  } while (p > L);

  return k - 1;
}
*/
long double  rand_double () {
   return rand()/(long double )RAND_MAX;
}
void generatemontecarlo(long m,long double  montecarlo[],long double  areaS,long double  rmin,long double  thetha)
{
    long i,j;
    for(i=0;i<m;i++)
    {
        vector<coord> crd;
        for(j=0;j<5000;j++)
        {
            long double  lat=fmod(rand(),sidelength);
            if(lat<0)lat=-lat;
            long double  lon=fmod(rand(),sidelength);
            if(lon<0)lat=-lon;
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

    freopen("inputgen.txt","r",stdin);
    //freopen("C:\\Users\\aakas\\Documents\\Geograhically-Robust-hotspot-Detection\\Hotspot_Detection\\projects\\hotspot\\myapp\\static\\myapp\\outputgen.txt","w",stdout);

    long msim;
    thetha=0;
    rmin=2.5;
    alphaP=0.001;
    msim=5;

    long n;
    cin>>n;
    //cout<<n<<endl;

    vector<coord> points(5000);

    for(long i=0;i<5000;i++){
      string x;
      cin>>x;
      points[i].x=stod(x);
      cin>>x;
      points[i].y=stod(x);
    }
    remove_negetive(points);
    variate(points);
    pair<int,int> p=getplanearea(points);
    areaS=p.first;
    sidelength=p.second;
    modP=points.size();

    //ceil to be chaged
    lcell=ceil(rmin/2);
    N=ceil(sidelength/lcell);
    total_countGrid_cells=N*N;
    total_cubicGrid_cells=N*N*N;


    //cout<<areaS<<" "<<N<<" "<<total_countGrid_cells<<endl;

    //3 PHASES
    vector<pair<pair<int,int>,vector<coord> > > fset= Filter_Phase(points,thetha);
    // circles, logirithmic value
    vector<pair<Circle,long double > > candidate_circles=Refine_Phase(fset,thetha,rmin);
    srand(time(NULL));
    long double  montecarlo[msim];
    for(long i=0;i<msim;i++)
        montecarlo[i]=0;

    generatemontecarlo(msim,montecarlo,areaS,rmin,thetha);
    sort(montecarlo,montecarlo+msim,greater<long double >());

    vector<Circle> hotspotCircles;
    for(long i=0;i<candidate_circles.size();i++){
        // will use binary search
        for(long j=0;j<msim;j++){
            if(candidate_circles[i].second>montecarlo[j]){
                long double  pval=((long double )j)/((long double )msim+1);
                if(pval<=alphaP){
                    hotspotCircles.push_back(candidate_circles[i].first);
                }
                break;
            }
        }
    }

    for(long i=0;i<hotspotCircles.size();i++){
         scaleback(hotspotCircles[i].x,hotspotCircles[i].y,hotspotCircles[i].radius);
         cout << hotspotCircles[i].x<<" " <<hotspotCircles[i].y<<" " <<hotspotCircles[i].radius << endl;
    }
    return 0;
}

//  AIzaSyBUuOtNyFrqUt5QwgwIhJPMtcaW8Hps6Tk --map api
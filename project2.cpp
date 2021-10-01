#include<bits/stdc++.h>
using namespace std;

class Car {
public:
	int x, y, s, dir;
	double p;
	bool newCar;
};
 
vector<Car> car;
int station[4][2] = {{360, 680}, {660, 658}, {330, 350}, {640, 310}};
double AVGmax = 0.0, AVGmin = 0.0;

bool arrival(double lambda) {
	double x = rand() / (RAND_MAX + 1.0);	//1.0!!!
	if(x < 1 - (double)exp(-lambda * 0.001))
		return true;
	else
		return false;
}

double power(int cx, int cy, int sx, int sy) {
	double distance = sqrt((sx - cx)*(sx - cx) + (sy - cy)*(sy - cy));
	return 100 - 33 - 20 * log10(distance);
}

void dir_turn(int i, int d) {
	int dir[4][5] {{0, 0, 0, 1, 3}, {1, 1, 1, 2, 0}, {2, 2, 2, 3, 1}, {3, 3, 3, 0, 2}};
	int x = rand() % 5;
	car[i].dir = dir[d][x];
}

void move(int i) {
	if(car[i].dir == 0)
		car[i].y += 10;
	else if(car[i].dir == 1)
		car[i].x -= 10;
	else if(car[i].dir == 2)
		car[i].y -= 10;
	else if(car[i].dir == 3)
		car[i].x += 10;
}

int check(int policy,int i, int t){
    int h = 0;
    car[i].p = power(car[i].x, car[i].y, station[car[i].s][0], station[car[i].s][1]);	//!!!why?
    double Pmax = 10.0, Pmin = 10.0, Pold = car[i].p;
    for(int j = 0; j < 4; ++j){
        double Pnew = power(car[i].x, car[i].y, station[j][0], station[j][1]);
        switch (policy) {
	        case 0:		//best(AVGmax)
	        	if(Pnew > Pold)
	        		h = 1;
	        	break;
	        case 1:		//threshold
	        	if(Pnew > Pold && Pold < 15.0)
	                h = 1;
	        	break;
			case 2:		//entropy
	        	if(Pnew > Pold + 13.0)
					h = 1;
	        	break;
			case 3:		//my
	        	if(t % 10 == 0 && Pnew > Pold)
	        		h = 1;
	        	break;
	        case 4:		//to get AVGmin
	        	if(Pold < Pmin)
	        		h = 1;
	        	break;
        }
        if(h == 1 && Pnew > car[i].p){
            car[i].p = Pnew;
            car[i].s = j;
        }
    }
    return h;
}

int main() {
	srand(time(NULL));
	fstream file;
    file.open("handoff.csv", ios::out | ios::trunc);
    double totalPower = 0.0;
	int entrance[36][2] = {{100, 0}, {200, 0}, {300, 0}, {400, 0}, {500, 0}, {600, 0}, {700, 0}, {800, 0}, {900, 0},
							{1000, 100}, {1000, 200}, {1000, 300}, {1000, 400}, {1000, 500}, {1000, 600}, {1000, 700}, {1000, 800}, {900, 900},
							{900, 1000}, {800, 1000}, {700, 1000}, {600, 1000}, {500, 1000}, {400, 1000}, {300, 1000}, {200, 1000}, {100, 1000},	
							{0, 900}, {0, 800}, {0, 700}, {0, 600}, {0, 500}, {0, 400}, {0, 300}, {0, 200}, {0, 100}};
	int enterStation[36] = {2,2,2,2,3,3,3,3,3,3,3,3,3,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,2,2,2,2,2};
	int totalHandoff = 0, totalCar = 0;
	for(int ts = 0; ts < 86400; ++ts) {															//*****sec*****
		int handoff = 0;
		for(int e = 0; e < 36; ++e) {
			for(int tms = 0; tms < 1000; ++tms) {
				if(arrival((double)1/2)) {														//*****lambda*****
					Car temp;
					temp.x = entrance[e][0];
					temp.y = entrance[e][1];
					temp.s = enterStation[e];
					temp.dir = e / 9;															//0:up, 1:left, 2:down, 3:right
					temp.p = power(temp.x, temp.y, station[temp.s][0], station[temp.s][1]);
					temp.newCar = true;	
					car.push_back(temp);
				}
			}
		}
		for(int i = 0; i < car.size(); ++i) {
			if(car[i].newCar == false && (car[i].y > 1000 || car[i].x < 0 || car[i].y < 0 || car[i].x > 1000))
				car.erase(car.begin() + i);
			if(car[i].newCar == false && car[i].x % 100 == 0 && car[i].y % 100 == 0)
				dir_turn(i, car[i].dir);										 
			if(car[i].newCar == false)
				move(i);
			car[i].newCar = false;
			int ho = check(0, i, ts);														 	//*****policy*****
			handoff += ho;
			totalHandoff += ho;
			totalPower += car[i].p;
		}
		totalCar += car.size();
		file << handoff << endl;
	}
	double AvgPower = totalPower / (double)totalCar;
	cout << "Handoff = " << totalHandoff << endl << "Average Power = " << AvgPower;
	file.close();
	return 0;
}

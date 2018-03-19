#include <iostream>
#include <stdlib.h>
#include <vector>
#include <fstream>

using namespace std;

struct Coord{
	double x;
	double y;
};

class Nation{
	public:
		string name;
		vector<double> xVals;
		vector<double> yVals;
};

//Override << to allow for easy printing of a nation object.
//Was used for testing, not really used right now!
ostream& operator << (ostream& os, const Nation& n){
	os << n.name << ": ";
	vector<double>::const_iterator i1;
	vector<double>::const_iterator i2 = n.yVals.begin();
	for(i1 = n.xVals.begin(); i1 != n.xVals.end(); ++i1){
		os << "(" << *i1 << ", " << *i2 << ") ";
		++i2;
	}
	
	return os << endl;
}

//Check to see if q is on the line segment p1p2
bool onLine(Coord p1, Coord q, Coord p2) {
    if (q.x <= max(p1.x, p2.x) && q.x >= min(p1.x, p2.x) &&
        q.y <= max(p1.y, p2.y) && q.y >= min(p1.y, p2.y))
       return true;
    return false;
}

/* Check to see if the three points provided are:
	- Colinear (returns 0)
	- Clockwise (returns 1)
	- Counter-clockwise (returns 2)
	
	The equation assgined to the variable VAL was provided
	by https://www.geeksforgeeks.org/orientation-3-ordered-points/
	as they explained how the formula works.
*/
double orient(Coord p1, Coord p2, Coord p3) {
    double val = (p2.y-p1.y)*(p3.x-p2.x) - (p2.x-p1.x)*(p3.y-p2.y);
    
    if (val == 0) return 0;  // colinear
    return (val > 0)? 1: 2; // clock or counterclock wise
}
 
//returns true if both passed lines, expressed by coord pairs, intersect
//p1 and p2 coorelate to the points on the first line
//where p1 is the arbitrary point outside of the island nation
//and p2 is the point in question
//q1 and q2 coorelate to a border of an island
bool doIntersect(Coord p1, Coord p2, Coord q1, Coord q2)
{
	//Find the different orientations related to the four points
    double o1 = orient(p1, p2, q1);
    double o2 = orient(p1, p2, q2);
    double o3 = orient(q1, q2, p1);
    double o4 = orient(q1, q2, p2);
 
    /* General case for intersection:
    	if the points p1, p2, q1 (orientation o1)
    		and p1, p2, q2 (orientation o2) have different orientations
    	AND
    	if the points q1, q2, p1 (orientation o3)
    		and q1, q2, p2 (orientation o4) have different orientations
 		
    	then the lines intersect
    */
    if (o1 != o2 && o3 != o4)
        return true;
 
    /* Special cases for intersection:
    	* p1, p2 and q1 are colinear and q1 is on the line p1p2
    	* p1, p2 and q2 are colinear and q2 lies on the line p1p2
    	* q1, q2 and p1 are colinear and p1 lies on the line q1q2
    	* q1, q2 and p2 are colinear and p2 lies on the line q1q2
    	
    	Note: Orientation function above returns a 0 when colinear
    */
    
    if (o1 == 0 && onLine(p1, q1, p2)) return true; 
    else if (o2 == 0 && onLine(p1, q2, p2))	return true;
    else if (o3 == 0 && onLine(q1, p1, q2)) return true;
    else if (o4 == 0 && onLine(q1, p2, q2)) return true;
    else return false;
}

//removes spaces from a string
string remSpace(string line){
	int i = 0; int j = 0;
	while(line[i]){
		if(line[i] != ' ' ){
			line[j++] = line[i];
		}
		i++;
	}
	line[j] = '\0';		
	return line;
}

//calls the doIntersect function, if true then it'll return a 1
//if false then it'll return a 0
int checkIntersect(Coord p1, Coord p2, Coord q1, Coord q2){
	if(doIntersect(p1,p2,q1,q2)) return 1;
	else return 0;
} 

bool inNation(Nation n, Coord c){
	Coord randPoint = {-1234512512.54321,-5432125151.12345};
	
	vector<double>::const_iterator i1;
	vector<double>::const_iterator i2 = n.yVals.begin();	
	int i = 1; 
	
	//Intersection counter, if 0 or even then it's not in the nation
	//if odd then it is in the nation
	int interCount = 0; 
	
	for(i1 = n.xVals.begin(); i1 != n.xVals.end(); ++i1){
		if(i++ >= 2){
			Coord p1 = {*(i1-1), *(i2-1)};
			Coord p2 = {*i1, *i2};
			interCount += checkIntersect(randPoint, c, p1, p2);
		}
		++i2;
	}
	
	//final border
	Coord p1 = {*(n.xVals.end()-1), *(n.yVals.end()-1)};
	Coord p2 = {*(n.xVals.begin()), *(n.yVals.begin())};
	interCount += checkIntersect(randPoint, c, p1, p2);
	
	cout << n.name << " " << interCount << endl;
	if(interCount == 0 || interCount % 2 == 0)
		return false;
	else
		return true;
}

int main() {
	vector<Nation> islandVec;
	size_t pos = 0;
	
	ifstream infile("map-test.csv");
	string line;
	int i = 0;
	
	while(getline(infile, line)){
		Nation n;
		while((pos = line.find(",")) != string::npos){
			string token = line.substr(0,pos);
			line.erase(0, pos+1);
			token = remSpace(token);
			if(i == 0) n.name = token;
			else if(i >= 2) {
				double val = atof(token.c_str());
				if(i % 2 == 0) n.xVals.push_back(val);
				else n.yVals.push_back(val);
			}
			i++;
		}
		line = remSpace(line.substr(0, line.find('\0')));
		double val = atof(line.c_str());
		n.yVals.push_back(val);
		islandVec.push_back(n);
		i = 0;
	} 
	
	vector<Coord> signalVec;
	ifstream infile2("signals.csv");
	string line2;
	
	while(getline(infile2, line2)){
		while((pos = line2.find(",")) != string::npos){
			string token = line2.substr(0,pos);
			line2.erase(0, pos+1);
			token = remSpace(token);
			line2 = remSpace(line2);
				
			Coord c = {atof(token.c_str()), atof(line2.c_str())};
			
			signalVec.push_back(c);
		}
	}
	
	bool totFlag = false;
	vector<Coord>::const_iterator j;
	for(j = signalVec.begin(); j != signalVec.end(); ++j){
		Coord c = *j;
		vector<Nation>::const_iterator i1;
		for(i1 = islandVec.begin(); i1 != islandVec.end(); ++i1){
			Nation n = *i1;
			bool flag = inNation(n,c);
			if(flag) {
				string str = n.name;
				totFlag = true;
				cout << "("<< c.x << ", " << c.y << ") is in " 
					<< str << "." << endl;
			}
		}
		if(!totFlag)
			cout << "("<< c.x << ", " <<c.y<< ") is in the open ocean." << endl;
		totFlag = false;
	}
	
	return 0;
}

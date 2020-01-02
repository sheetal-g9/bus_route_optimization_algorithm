#include<bits/stdc++.h>
using namespace std;

#define int long long

// p_id (x, y) : point with index id is (x, y)
struct point 			
{
	long double x, y;
	int id;
	bool operator == (point &other)
	{
		return (x == other.x && y == other.y && id == other.id);
	}
	bool operator<(point &other)
	{
		return tie(x, y) < tie(other.x, other.y);
	}
};

// ax + by + c = 0
struct line 
{
	long double a, b, c;
};

// returns square of distance b/w two pts;
long double dist_point(point a, point b)	
{
	return (a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y); 
}

// return a line passing through two points
line build_line(point a, point b)
{
	line temp;
	temp.a = b.y - a.y;
	temp.b = a.x - b.x;
	temp.c = a.y * b.x - a.x * b.y;
	return temp;
};

// stores bus-routes
struct route
{
	vector<int> bus_stops;
	int cnt = 0;
};

int n, bus, capacity, num_emp, kms, pts;
long double time_duration, bus_speed;
string st_time1, end_time1;
string st_time2, end_time2;
string st_time3, end_time3;
point office;
vector<point> points;
vector<bool> exist;
vector<int> stops;
vector<vector<long double>> graph, dist;
vector<route> routes;
vector<string> names;

const long double INF = 1e12;

// bool within(point a, point b, point foot)
// {
// 	long double dist1 = dist_point(a, foot);
// 	long double dist2 = dist_point(b, foot);
// 	long double dist3 = dist_point(a, b);
// 	dist1 = sqrt(dist1), dist2 = sqrt(dist2), dist3 = sqrt(dist3);
// 	return (abs(dist1 + dist2 - dist3) <= 1e-5);
// }

// returns true if c lies on line segment a and b
bool within(point a, point b, point c)
{
	long double eps = 1e-5;
	if(abs((c.y - a.y) * (b.x - c.x) - (c.x-a.x) * (b.y - c.y)) <= eps && ((c.y - a.y) * (b.y - c.y) >=0))
		return true;
	return false;
}

// returns a vectors consisting of points lying withing a range of line ab
vector<point> order_of_points(point a, point b, long double range)
{
	line ab = build_line(a, b);
	
	vector<point> worthy;
	
	for(int i = 0; i < n; i++)
	{
		if(!exist[i] || points[i] == a || points[i] == b)
			continue;

		long double nu = abs(ab.a * points[i].x + ab.b * points[i].y + ab.c);
		long double de = range * sqrt(ab.a * ab.a + ab.b * ab.b);
		long double distt = (ab.a * points[i].x + ab.b * points[i].y + ab.c) / (-1.0 * (ab.a * ab.a + ab.b * ab.b));
		point foot;
		foot.x = ab.a * distt + points[i].x;
		foot.y = ab.b * distt + points[i].y;
		if(nu <= de && within(a, b, foot))
			worthy.push_back(points[i]);
	}
	return worthy;
}


// convex hull 
bool cmp(point a, point b) {
    return a.x < b.x || (a.x == b.x && a.y < b.y);
}

bool cw(point a, point b, point c) {
    return a.x * (b.y-c.y) + b.x * (c.y-a.y) + c.x * (a.y-b.y) < 0;
}

bool ccw(point a, point b, point c) {
    return a.x * (b.y - c.y) + b.x * (c.y - a.y) + c.x * (a.y-b.y) > 0;
}
// returns convex hull of given points
vector<point> convex_hull(vector<point>& a){
    
    vector<point> hull = a;
    if (hull.size() == 1)
        return hull;

    sort(hull.begin(), hull.end(), &cmp);
    point p1 = hull[0], p2 = hull.back();
    vector<point> up, down;
    up.push_back(p1);
    down.push_back(p1);
    for (int i = 1; i < (int)hull.size(); i++) {
        if (i == hull.size() - 1 || cw(p1, hull[i], p2)) {
            while (up.size() >= 2 && !cw(up[up.size()-2], up[up.size()-1], hull[i]))
                up.pop_back();
            up.push_back(hull[i]);
        }
        if (i == hull.size() - 1 || ccw(p1, hull[i], p2)) {
            while(down.size() >= 2 && !ccw(down[down.size()-2], down[down.size()-1], hull[i]))
                down.pop_back();
            down.push_back(hull[i]);
        }
    }

    hull.clear();
    for (int i = 0; i < (int)up.size(); i++)
        hull.push_back(up[i]);
    for (int i = down.size() - 2; i > 0; i--)
        hull.push_back(down[i]);
    return hull;
}

// comparator to keep points sorted in a clockwise manner relative to office
struct ckw_cmp
{
	bool operator()(point p1, point p2)
	{
		point v1, v2;
		v1.x = p1.x - office.x;
		v1.y = p1.y - office.y;
		v2.x = p2.x - office.x;
		v2.y = p2.y - office.y;
		long double cp = v1.x * v2.y - v1.y * v2.x;
		if(cp == 0)
			return dist[p1.id][office.id] < dist[p2.id][office.id];
		return cp < 0;
	}
};

// comparator to sort points such that points farther from offices comes first 
bool dist_sort(point p1, point p2)
{
	return dist[p1.id][office.id] > dist[p2.id][office.id];	
}

// removing the bus stop with zero employees and inserting a new route to list of routes
void update_exist(set<point, ckw_cmp> &certain, int emp_cnt)
{
	route new_route;
	for(auto i : certain)
	{
		new_route.bus_stops.push_back(i.id);
		if(!stops[i.id])
			exist[i.id] = 0, pts--;
	}
	exist[office.id] = 1;
	pts++;
	new_route.cnt = emp_cnt;
	routes.push_back(new_route);
}

// generates bus routes
void iteration()
{
	int emp_cnt = 0;
	vector<point> cur_points;
	for(int i = 0; i < n; i++)
		if(exist[i])
			cur_points.push_back(points[i]);
	vector<point> hull = convex_hull(cur_points);
	set<point, ckw_cmp> certain;
	vector<point> consider;
	point prev_pt = office;
	long double cur_dis = 0;
	int space = capacity;
	certain.insert(office);
	bool first = 0;
	for(auto i : hull)
	{
		if(i == office)
			continue;
		if(cur_dis + dist[prev_pt.id][i.id] + dist[i.id][office.id] <= kms && space)
		{
			first = 1;
			certain.insert(i);
			int val = min(space, stops[i.id]);
			stops[i.id] -= val;
			space -= val;
			emp_cnt += val;
		}
		else 
		{
			if(first)
				break;
			else
				continue;
		}
		vector<point> inline_points = order_of_points(prev_pt, i, 10000);
		for(auto j : inline_points)		
			consider.push_back(j);
		cur_dis += dist[prev_pt.id][i.id];
		prev_pt = i;
	}
	cur_dis += dist[prev_pt.id][office.id];
	vector<point> inline_points = order_of_points(prev_pt, office, 10000);
	for(auto j : inline_points)		
		consider.push_back(j);
	sort(begin(consider), end(consider), dist_sort);
	for(auto i : consider)
	{
		auto temp = certain.insert(i);
		if(!temp.second)
			continue;
		auto itp = temp.first;
		auto itn = itp;
		--itp;
		auto itc = itn;
		++itn;
		if(itn == end(certain))
			itn = begin(certain);
		long double new_dis = cur_dis - dist[itp -> id][itn -> id];
		new_dis = new_dis + dist[itp -> id][itc -> id] + dist[itc -> id][itn -> id];
		if(new_dis > kms || !space)
		{
			certain.erase(itc);
			break;
		}
		else
		{
			cur_dis = new_dis;
			int val = min(space, stops[itc -> id]);
			stops[itc -> id] -= val;
			space -= val;
			emp_cnt += val;
		}
	}
	update_exist(certain, emp_cnt);
}

// all pair shortest path algorithm
void floyd_warshall()
{
	int n = graph.size();
	dist = graph;
	for (int k = 0; k < n; ++k) 
	    for (int i = 0; i < n; ++i) 
	        for (int j = 0; j < n; ++j) 
	            dist[i][j] = min(dist[i][j], dist[i][k] + dist[k][j]); 
}

// converts time( hh:mm string) to min(int)
int time_to_min_util(string time_)
{
	int hh, mm;
	if(time_[2] == ':')
	{
		hh = (time_[0] - '0') * 10 + (time_[1] - '0');
		mm = (time_[3] - '0') * 10 + (time_[4] - '0');
	}
	else
	{
		hh = (time_[0] - '0');
		mm = (time_[2] - '0') * 10 + (time_[3] - '0');
	}
	return hh * 60 + mm;
}

// reverse of above function
string min_to_time_util(int minute)
{
	string time_;
	time_ = to_string(minute / 60);
	time_ += ":";
	string min1 = to_string(minute % 60);
	if(min1.size() == 1)
		min1 = "0" + min1; 
	time_ += min1;
	return time_;
}

void print_route(vector<int>& cur_route)
{
	long double tm1 = (long double)time_to_min_util(st_time1);
	long double tm2 = (long double)time_to_min_util(st_time2);
	long double tm3 = (long double)time_to_min_util(st_time3);

	int bs = 0;
	cout << "Shift 1\t\t\t\tShift 2\t\t\t\tNight Shift\t\t\t\t\tBus Stop\n";
	for(auto j : cur_route)
	{
		long double val = dist[bs][j] / (bus_speed + 100); 
		tm1 += val;
		tm2 += val;
		tm3 += val;
		cout << min_to_time_util(ceil(tm1)) << "\t\t\t\t" << min_to_time_util(ceil(tm2)) << "\t\t\t\t" << min_to_time_util(ceil(tm3)) << "\t\t\t\t" << names[j] << "\n";
		bs = j;
		tm1 += 0.33333;
		tm2 += 0.33333;
		tm3 += 0.33333;
	}
	tm1 += dist[bs][office.id] / (bus_speed + 100); 
	tm2 += dist[bs][office.id] / (bus_speed + 100); 
	tm3 += dist[bs][office.id] / (bus_speed + 100); 
	cout << min_to_time_util(ceil(tm1)) << "\t\t\t\t" << min_to_time_util(ceil(tm2)) << "\t\t\t\t" << min_to_time_util(ceil(tm3)) << "\t\t\t\t" << names[office.id] << "\n";	
}

void get_input_matrix()
{
	for (int i = 0; i < n; ++i)
	{
		for(int j = 0; j < n; ++j)
		{
			if(graph[j][i] == 0)
			{
				swap(graph[j], graph[i]);
				break;
			}
		}
	}
}

int32_t main()
{
	cout.precision(10);
	cerr.precision(10);
	#ifndef ONLINE_JUDGE
	// for getting input from input.txt
	freopen("input.txt", "r", stdin);
	// for writing output to output.txt
	freopen("output.txt", "w", stdout);
	cout << "OUTPUT \n";
	#endif

	cin >> office.x >> office.y;
	office.id = 0;
	
	// cout << "Number of bus stops : ";
	cin >> n;
	points.resize(n);
	exist.resize(n,1);
	stops.resize(n,0);
	dist.resize(n, vector<long double>(n));
	graph.resize(n, vector<long double>(n));
	names.resize(n);
	
	// cout << "Enter co-ordinates of bus stops\n";
	string ex;
	getline(cin, ex);

	for(int i = 0; i < n; i++)
	{
		getline(cin, names[i]);
		point p;
		cin >> p.x >> p.y;
		p.id = i;
		points[i] = p;
		getline(cin, ex);
	}
	
	// cout << "Number of buses : ";
	cin >> bus;

	// cout << "Maximum capacity of a bus : ";
	cin >> capacity;
	
	// cout << "Time  : "
	cin >> st_time1 >> end_time1;
	time_duration = time_to_min_util(end_time1) - time_to_min_util(st_time1);
	cin >> st_time2 >> end_time2;
	cin >> st_time3 >> end_time3;
	// cout << "Number of Employees : ";
	cin >> num_emp;
	
	// cout << "Index of boarding stops of Employees\n";
	for (int i = 0; i < n; ++i)
		cin >> stops[i];
	
	for(int i = 0; i < n; i++)
		for(int j = 0; j < n; j++)
			cin >> graph[i][j];
	get_input_matrix();
	cerr << graph[0][0] << " " << endl;
	floyd_warshall();
	for(int i = 0; i < n; i++)
		for(int j = 0; j < i; j++)
		{
			graph[i][j] = min(graph[i][j], graph[j][i]);
			graph[j][i] = graph[i][j];
		}

	cin >> kms;

	bus_speed = (kms) / time_duration;

	int t = 20;
	pts = n;

	// cout << "Total kms run/day : ";
	while(t-- && pts > 1)
	{
		iteration();
	}

	for(int i = 0; i < n; i++)
		for(int j = 0; j < n; j++)
			cerr << graph[i][j] << " \n"[j == n - 1];
	pts = 1;
	sort(begin(routes), end(routes), [&](route r1, route r2){
		return r1.cnt > r2.cnt;
	});
	int sz = 0;
	for(auto i : routes)
	{
		if(i.bus_stops.size() == 1)
			continue;
		sz++;
	}
	for(int i = 0; i < min(sz, bus); i++)
	{
		if(routes[i].bus_stops.size() == 1)
			continue;
		cout << "Route " << pts++ << "\n";
		cout << "----------------------------------------------------\n";
		cout << "Number of Employees taken: " << routes[i].cnt << "\n";			
		print_route(routes[i].bus_stops);
		cout << "----------------------------------------------------\n";
	}
	if(bus < sz)
	{
		cout << "\n\nxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n";
		cout << "Error :- Due to fewer number of buses following generated routes cannot be covered : ";

		for(int i = bus; i < sz; ++i)
		{
			if(routes[i].bus_stops.size() == 1)
				continue;
			cout << "Route " << pts++ << "\n";
			cout << "\n\nxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n";
			cout << "Number of Employees left: " << routes[i].cnt << "\n";			
			print_route(routes[i].bus_stops);
			cout << "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n";
		}
		cout << "\n";
	}
	
	bool chk = 0;
	for(int i = 1; i < n; i++)
		chk |= exist[i];
	if(chk)
	{
		cout << "\n\nxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n";
		cout << "Error :- Under given constraints these locations cannot be covered : ";
		
		for(int i = 1; i < n; i++)
			if(exist[i])
				cout << names[i] << " ";
		cout << "\n";
		cout << "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n";
	}

	return 0;
}



/*
											INPUT

763132.1869148843 1415939.1400030896
20
Bosch Bidadi, Bengaluru
763132.1869148843  1415939.1400030896
Devegowda Petrol Bunk, Bengaluru
777776.3688518326  1429920.8631497987
Hoskeralli, Bengaluru
775201.9653375052  1429891.080382486
Channasandra RNSIT, Bengaluru
773304.8061703905  1427661.2472977254
Kathriguppe, Bengaluru
777438.2354220599  1430710.3368906518
Kamakya Theatre, Bengaluru
777056.0623013019  1430035.713058344
PESIT Collage, Bengaluru
789218.6061219915  1423312.0945444703
Katherguppe Circle, Bengaluru
776683.9818221548  1430248.2379606676
Bata Show Room, Bengaluru
783296.8318305017  1452048.2641508547
Ittamadu, Bengaluru
776177.9399029084  1429963.98339432
Rajarajeshwari temple, Bengaluru
771986.9881333619  1430859.9112484732
Jantha Bazar, Bengaluru
779597.5540256634  1436008.2512950187
Rajarajeshwarinagar Double Road, Bengaluru
781287.4322356434  1433821.7135571998
Kanthi Sweets RR Nagar, Bengaluru
773528.3943916513  1429078.6459262224
Chowdeshwari Talkies, Bengaluru
776991.7355536399  1441648.7936304198
Jayanagar, Bengaluru
779959.046157304  1430898.9307355725
Kadirenahallli, Bengaluru
778075.8859046573  1429184.2047316984
Mantri Apartment, Bengaluru
786850.9580598711  1429662.0243343462
Kodipalya (Uttarahalli main Road), Bengaluru
774069.9201475626  1427874.8064627687
Uttarahalli road Kengeri, Bengaluru
769852.4590372324  1428502.2953201465
6
32
6:15 7:35
29
0 1 5 2 5 2 1 1 1 1 1 1 1 1 1 1 1 1 1 1 
0 33266 31947 22478 33265 32968 41453 32669 57852 32659 23045 31856 33089 23862 46649 36401 34845 50945 23184 18438 
31711 0 3920 6201 2025 914 16742 1267 25565 2098 7515 8279 7898 6760 21129 3085 1592 11450 5480 10242 
30427 3638 0 4586 3700 2928 31413 3607 31048 1440 6231 10167 10880 4497 19845 6773 5217 15075 3865 8627 
21692 5939 4586 0 6619 6027 27230 6381 33800 4359 6002 13730 13836 1590 22597 9144 5806 16412 721 4056 
36296 6751 9216 12334 6208 6952 16428 6931 21759 7762 11342 4473 0 13128 12596 4268 7972 10168 11613 14847 
22606 6439 4288 1714 6538 6528 28735 5942 32737 4860 4812 12539 13772 0 21534 9645 6307 16913 1973 5562 
45206 20273 19398 22063 20273 19975 46006 19677 15627 19667 18535 7766 11757 20731 0 14567 21853 20026 22769 22040 
34770 3194 6978 9259 3111 3627 14024 4325 23290 5156 11076 6004 5612 9818 14127 0 4004 8732 8538 14581 
56941 32009 31133 33798 25447 31711 35184 31412 0 31402 30270 18830 20655 32466 15404 23465 27211 36674 34504 33776 
30858 1881 1440 4359 1943 1478 31844 1674 31479 0 6661 9341 8960 4918 20276 5016 3460 13318 3638 8400 
20467 7535 5600 5067 7535 7237 29742 6939 30722 6928 0 10652 11885 3734 19519 10670 9114 18972 5772 3505 
32483 9861 12485 15134 9318 10062 19583 10041 17718 10872 12016 0 4496 13802 8554 7335 11082 13323 15840 15521 
33104 1521 5313 5802 3186 2307 15989 2660 26578 3491 8907 9292 8911 6361 22522 3430 0 10697 5081 9843 
48625 11309 15100 16132 11276 12094 9205 12447 28137 13278 18695 12413 9536 16691 18973 8638 10555 0 15411 35111 
22398 5218 3865 721 5898 5306 27196 5660 34506 3638 6708 14435 13116 1848 23303 8423 5085 15691 0 4762 
17635 11007 8612 4041 11007 10709 25577 10411 34194 8385 5313 14124 15357 5424 22991 14142 9832 35069 4746 0 
31686 1648 3894 6644 0 888 32671 1241 24606 2072 7489 7896 7515 6438 13713 3117 3227 11375 5923 10994 
30886 801 3095 5789 968 0 31872 442 25430 1273 6690 9130 8749 5638 20304 3936 2380 12238 5069 10195 
40062 34178 32859 28353 34177 33880 0 33581 37840 33571 33828 22116 19239 29737 47442 18312 19869 14700 28307 26548 
31218 2128 2164 5083 2295 1329 32203 0 31839 714 7021 9764 9383 5642 20636 5263 2508 13565 4362 9124 
92000
*/
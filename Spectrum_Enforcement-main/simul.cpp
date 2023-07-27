// C++/CSIM Model of M/M/1 queue

#include "cpp.h"			// class definitions
#include "stdio.h"
#include <map>
#include <math.h>
#include <set>
#include <iostream>
#include <stdlib.h>


#include <fstream>  
#include <ctime>
#include <cstdint>

#include <cmath>
#include <algorithm>
#include <functional>
#include <vector>
#include <random>
#include <chrono>
#include <time.h>
#include <sstream>
#include <ctime>        // std::time
#include <cstdlib>      // std::rand, std::srand

//#include <boost/random/variate_generator.hpp>
//#include <gsl/gsl_rng.h>
//#include <gsl/gsl_randist.h>




#define MAX_VOL 1070

//#define MAX_VOL 2140
#define IAR_TM 2.0
#define SRV_TM 1.0

#define AUI 5.0
//#define AUI 10.0
#define MI 4
#define PI 3.14159265
#define INF 100000 //Infinity



using namespace std;


event done("done");			// the event named done
event init("init");
facility f("facility");			// the facility named f
table tbl("resp tms");			// table of response times
qhistogram qtbl("num in sys", 10l);	// qhistogram of number in system
int cnt;				// count of remaining processes

//No. of times new volunteers are selected/No. of Monitoring Intervals.
int iter = 0;

//Counter to check if it is a MI
int MI_count = 0;

int no_of_MI = 0;
//Battery computation time
float batt_compute = 100.0;

//Battery idle time
float batt_idle = 10.0;

//ACcess Time
float access_time = 10.0;

//Area boundaries
int bdry_X = 1000;//kk
int bdry_Y = 500;

//maximum battery level for every volunteer
int max_battery_level = 7*3600;
//int max_battery_level = 7*10000;
int trust_threshold = 42; //simulb

//Set the burnout period
int burnout = 0;//5;//100;

//Total number of volunteers
int total_volunteers = 0;

//Total number of authorized users
int total_authorized_users = 10;

//Total number of Malicious Users
int total_malicious_users = 7;


//Total Number of channels/region
int channel_count_per_region = 5;

//Total Number of regions
int region_count = 2;

//Total number of corrupt volunteers
int total_corrupt_volunteers = 20;

//Corrupt Volunteers count
int corrupt_volunteer_count = 0;

//Percentage of corrupt volunteers
float corrupt_percent = 0.5;////

int global_k;

//REPUTATION Constants s
/* float beta1 = 5;
float beta2 = 5;
float rep_threshold = -20.0;
float beta3 = 10; */

float beta1 = 1;
float beta2 = 1;
float rep_threshold = -10.0;
float beta3 = 0.5;

//Weighted Sum Parameters

float weight1 = 0.25;
float weight2 = 0.75;

int reverse1 = 0;


extern "C" void sim(int, char **);


//###### ---------Structures------------ #######

struct channel
{
		std::string permission;
		vector <std::string> access;
		
};

struct Point
{
	int x;
	int y;
};

//Structs to define a volunteer

struct params
{
	float loc_likelihood;
	float trust;
};

struct channel_vol_detection
{
	int id;
	float detection_capability;
};

struct vol
{
	int x; 
	int y;
	//std::map <int, struct params> region_list; //map of region-specific volunteer parameters
	int bin_value;
	
	int corrupt;
	float average_speed;
	//vector <int> channel_preference;
	std::map< int, vector <struct channel_vol_detection> > vol_channel_detection; //region_id -> list of channel detection probabilities
	
};
struct Authorized_U
{
	int x; 
	int y;
};

struct Malicious_U
{
	int x;
	int y;
};



std::map< int, vector <struct vol_ID_Qual> > channel_preference;
//structure for parameters of a volunteer wrt. a region
struct vol_for_region
{
	int x;
	int y;
	int curr_region; // current region of the volunteer
	int prev_region; // previous region of the volunteer
	
	//parameters for calculating location likelihood
	float soj_time; //Sojourn time of volunteer
	float ret_time; //Return time of volunteer
	
	float sojourn_avg; //average sojourn time of a volunteer in a given region
	float normal_sojourn_avg; //Normal sojourn time average
	
	float p_r;
	float p_r_avg;
	
	float sep_factor;
	float qualification;
	bool selected;
	
	//parameters for calculating trust
	int bin_value;
	int corrupt;
	
	//float TP;
	float TN;
	//float FP;
	float FN;
	
	vector <float> TP;
	//vector <float> TN;
	vector <float> FP;
	//vector <float> FN;  
	
	vector <float> qualification_vector; //stores vector of floating qualification values
	
	vector <int> channel_preference_vector;
	
	//map for mapping channel no to volunteer capability 
	
	
	int channel_index; //map <region -> channel_index>
	
	vector <float> trust;
	vector <float> reputation;
	vector <float> normal_reputation;
	
	float accuracy;
	
	float normal_pravg;
	float normal_accuracy;
	
	std::map< int, vector <struct vol_ID_Qual> > channel_preference; //channel_id -> (vol_id, qualification)
	
	float shortest_time;
	float normal_shortest_time;
	
	//Residency Parameters
	float residence_time;
	float residence_probability;
	
	//alpha parameters
	float alpha10;
	float sigma;
	
	//Volunteer_channel detection capability
	vector <struct channel_vol_detection> vol_det_cap;
	std::map< int, float> detection_capability_map;
	
	
	
};

//Struct for region

struct region
{
	struct Point bdry1;
	struct Point bdry2;
	struct Point bdry3;
	struct Point bdry4;
};

//struct for volunteer id - qualification pair
struct vol_qual
{
	int id;
	float qual;
};



struct volunteer_reputation
{
	int id;
	float reputation;
};

struct vol_ID_Qual
{
	int vol_id;
	float qualification;
};

struct vol_hit
{
	int v;
	int hit;
};

//Function Prototypes

void volunteer(int); //creates volunteers
void print_vol_list(); //prints the list of volunteers
void battery(int); //creates battery process for each procedure
void theory(); 
void init_regions(); //initializes the regions
bool event_over(); //function to check if the simulation is over
void print_battery_list();
bool onSegment(Point, Point, Point);
int orientation(Point, Point, Point);
bool doIntersect(Point p1, Point q1, Point p2, Point q2);
bool isInsidePolygon(Point polygon[], int n, Point p) ; //Function that checks if a point lies inside a given polygon
bool isInsideRegion(int,int);

bool isInsideRegion_AU_MU(int x, int y, int region); 

float residence_param(float, float); // Calculates the residence parameter of a volunteer for a given region
void print_region_vol_list(); 
float separation_factor(int, int, int); // Calculates the separation_factor for the current position of a given volunteer
//Point centroid(Point polygon[], int vertexCount); //Calculates the centroid of a polygon
void Secretary(std::map <int, float> &A, int k, int region); //Secretary algorithm
void classic_secretary(std:: map <int, float> &A, int k, int region); //Classic secretary 
void Secretary2(std:: map <int, float> &A, int k, int region);
void StableMarriage(std:: map <int, float> &A, int k, int region);
void StableMarriage2(std:: map <int, float> &A, int k, int region);
void random_algo(std:: map <int, float> &A, int k, int region); //Random Algorithm
void no_of_volunteers ();

void random_algo2(std:: map <int, float> &A, int k, int region);

int binomial(double p, int n); // Returns a Binomial random variable
double rand_val(int seed);    
void init_trust();   

void init_channels(); 
void authorized_user(int);
void AU_access(int);
void print_channel_list();

void malicious_user(int);

void MU_access(int);

void assign_channels(int region, int k, std::map< int, float > sum_happiness);

void assign_channels_round_robin(int region, int k, std::map< int, float > sum_happiness);

bool compareBydetection_capability(const struct channel_vol_detection &a, const struct channel_vol_detection &b);

void comb(int, int); //Calcuates combinations

void Deferred_Acceptance(std:: map< int, std::map <int, float> > &A, int k);
void Deferred_Acceptance2(std:: map< int, std::map <int, float> > &A, int k);
void Deferred_Acceptance_channelPropose(std:: map< int, std::map <int, float> > &A, int k);

void assign_channels_round_robin(int region, int k);//assign_channels_round_robin

float shortest_time_calc(int region, int x, int y, float average_speed);

std::map <int, struct vol> volunteer_list; //List of all volunteers

std::map <int, struct Authorized_U> AU_list; //List of Authorized Users

std::map <int, struct Malicious_U> MU_list; //List of Authorized Users

std::map <int, struct region> region_list; //List of all regions

std::map<int, float> battery_list; // list of battery values of all volunteers.


std::map< int, std::map <int, std::vector <struct vol_ID_Qual> > > region_channel_preference; //region -> (channel list)

std::map< int, std::map <int, struct vol_for_region> > region_volunteer_list; //nested map of region-specific volunteers

std::map< int, std:: map <int, float> >qualification_list;

std::vector <struct vol_qual> selected_volunteers;

//std:: map<int, std:: vector <struct vol_qual> > VS_regionList;

std::map<int, std:: map<int, float> >VS_regionList;

std::map< int, vector <float> > hit_ratio_list; //Hit Ratio list for all regions

std::map< int, vector <float> > hit_ratio_list_optimal; //Hit Ratio list optimal for all regions

std::map< int, vector <float> > hit_ratio_list_optimal_overall; //Hit Ratio list optimal overall for all regions

std::map <int, vector <struct channel> > channel_list;

//global trust parameters
std::map <int, float> TP_round;
std::map <int, float> FP_round;
std::map <int, float> TN_round;
std::map <int, float> FN_round;

std::map <int, float> accuracy_round;
std::map <int, float> sensitivity_round;
std::map <int, float> specificity_round;

std::map< int, vector <float> > accuracy_list; 
std::map< int, vector <float> > sensitivity_list; 
std::map< int, vector <float> > specificity_list; 

std::map <int, float> channel_TP_round;
std::map <int, float> channel_FP_round;
std::map <int, float> channel_accuracy_round;

std::map< int, vector<float> > channel_accuracy_list; //channel no -> <accuracy list>



std::map <int, int> channel_counter_region; //Map <region ->channel_index_assigned>

std::map< int, vector<int> > volunteer_channel_assignment_counter; //Map: volunteer-> vector of channel assignment counts

std::map< int, vector<float> > volunteer_channel_reputation_mean; //Map: volunteer-> vector 

std::map< int, vector<float> > volunteer_channel_reputation_mean_CMAB_ready; //Map: volunteer-> vector

std::map< int, vector <int> > combination_map; //Map index -> <combinations vector>


//Data Structures for assigning channels to volunteers in Secretary Algorithm
std::map< int, vector <float> > volunteer_channel_mean_reputation; //vol_id --> <vector of mean reputations to monitor channels>


std::map< int, vector <struct volunteer_reputation> > channel_volunteer_list;

std::map< int, std::map< int, float > > channel_vol_qualification; //channel_id -> (volunteer_id -> qualification for the channel)

//List of volunteers who applied to a given channel in a given region
std::map< int, std::map< int, vector<int> > > application_list;//region -> channel -> [List of volunteer ids]

std::map< int, vector<struct vol_hit> > volunteer_hit_map;

std::map< int, vector<struct vol_hit> > volunteer_hit_map_overall;

std::map< int, vector< float > > happiness_score_list;

void init_volunteer_channel_assignment_counter()
{
	for (int i = 1; i<= volunteer_list.size(); i++)
	{
		for (int j = 0; j < channel_count_per_region; j++)
		{
			volunteer_channel_assignment_counter[i].push_back(0);
		}
	}
}

void update_mean_reputation()
{
	for (int i = 1; i <=  region_list.size(); i++) //check value of i
 	{
		for (int v = 1; v<= volunteer_list.size(); v++)
		{
			for (int c = 0; c< channel_count_per_region; c++)
			{
				volunteer_channel_reputation_mean[v][c] += region_volunteer_list[i][v].reputation[c]; //sum of reputations
			}
		}			
	}
	//calculating mean of reputations
	for (int v = 1; v<= volunteer_list.size(); v++)
	{
		for (int c = 0; c< channel_count_per_region; c++)
		{
			volunteer_channel_reputation_mean[v][c] /= region_list.size();
		}
	}
}



void init_volunteer_channel_reputation_mean()
{
	for (int i = 1; i<= volunteer_list.size(); i++)
	{
		for (int j = 0; j < channel_count_per_region; j++)
		{
			volunteer_channel_reputation_mean[i].push_back(0.0);
		}
	}
}

void init_current_channel_index()
{
	for (int i = 1; i<= region_list.size(); i++)
	{
		channel_counter_region[i] = 0;
	}
}

void no_of_volunteers ()
{
	srand(clock1*100.0);
	total_volunteers = 1 + rand()%(MAX_VOL-1);
	//total_volunteers = total_volunteers*2;
	
	
	//total_volunteers = 200;
	/*for (int i = 0; i < MAX_VOL; i++)
	{
		rand_val(i);
		int value = binomial(0.5, 100);
		if(value >50)
		{
			total_volunteers++;
		}
	}*/
	
	cout<<"\n\nTotal Number of volunteers::"<<total_volunteers<<endl;
}

//Initialize the channels
void init_channels()
{
	for (int i = 1; i <=  region_list.size(); i++) //check value of i
 	{
		vector <struct channel> channel_vector;
				cout<<"\nprint here";
		for (int j = 0; j < channel_count_per_region; j++)
		{
			std::string permission = "";
			std::string access = "";
			struct channel ch;
			ch.permission = "";
			
			
			channel_vector.push_back(ch);	
		}
		channel_list[i] = channel_vector;
		

		
	}
}


void init_trust()
{
	for (int i = 1; i<= region_list.size(); i++)
	{
		TP_round[i] = 0.0;
		TN_round[i] = 0.0;
		FP_round[i] = 0.0;
		FN_round[i] = 0.0;
		
		accuracy_round[i] = 0.0;
		sensitivity_round[i] = 0.0;
		specificity_round[i] = 0.0;	
	}
	
}
void init_trust2()
{
	for (int c = 0; c< channel_count_per_region; c++)
	{
		channel_TP_round[c] = 0.0;
		channel_FP_round[c] = 0.0;
		channel_accuracy_round[c] = 0.0;
	}
}
bool compareVolunteer_Qual(const struct vol_ID_Qual &a, const struct vol_ID_Qual &b)
{
	return a.qualification > b.qualification;
}

bool compareByVolHitCount(const struct vol_hit &a, const struct vol_hit &b)
{
	return a.hit > b.hit;
}

// ############## ---------------------Main -------------------------- ##############
void sim(int argc, char *argv[])
{
	float t1;
	t1 = clock1;
	int random;
	init_regions(); // Initializing the regions
	
	init_trust(); //Initializing the trust parameters
	
	init_trust2();
	
	no_of_volunteers (); //Initializing the total number of volunteers
	
	init_channels(); //Initializing the channels
	
	set_model_name("Spectrum Enforcement");
	create("sim");
	//cnt = MAX_VOL;
	cnt = 3;
	
	for(int i = 1; i <= total_volunteers; i++) 
	{
		hold(2.0);
		volunteer(i);		// generate next volunteer
		
	}
	cout<<"hiyo";
	
	//cout<<"\nchannel list size:"<<channel_list.size()<<"\n"; 
	//good
	 //Generate authorized users
	/* for(int i = 1; i <= total_authorized_users; i++) 
	{
		hold(2);
		authorized_user(i);		// generate next authorized user
		
	}
	//Generate malicious users
	for(int i = 1; i <= total_malicious_users; i++) 
	{
		hold(2);
		malicious_user(i); //generate next malicious user
	} */
	 
	init_volunteer_channel_assignment_counter(); 
	
	init_volunteer_channel_reputation_mean();
	
	hold(400);
	cout<<"\n==========+++++++++++no of volunters:"<<volunteer_list.size()<<"\n";
	for (int i = 1; i<= region_list.size(); i++)
	{
		for (int v = 1; v <= volunteer_list.size(); v++)
		{
			
			region_volunteer_list[i][v].x = volunteer_list[v].x;
			region_volunteer_list[i][v].y = volunteer_list[v].y;
			region_volunteer_list[i][v].bin_value = volunteer_list[v].bin_value;
			region_volunteer_list[i][v].corrupt = volunteer_list[v].corrupt;
			region_volunteer_list[i][v].channel_index = 0;
			region_volunteer_list[i][v].sojourn_avg = 0.0;
			region_volunteer_list[i][v].sigma = 0.0;
			region_volunteer_list[i][v].alpha10 = 0.0;
			
			region_volunteer_list[i][v].residence_time = 0.0;
			region_volunteer_list[i][v].residence_probability = 0.0;
				
			//cout <<"\n fill halt";
			for (int c = 0; c< channel_count_per_region; c++)
			{

				region_volunteer_list[i][v].TP.push_back(0.0);
				region_volunteer_list[i][v].FP.push_back(0.0);
				region_volunteer_list[i][v].trust.push_back(0.0);
				region_volunteer_list[i][v].reputation.push_back(1.0);
				region_volunteer_list[i][v].normal_reputation.push_back(1.0);
			}
			///cout<<"\nvalues of TP:";
			for (int c = 0; c< channel_count_per_region; c++)
			{
				//cout<<region_volunteer_list[i][v].TP[c]<<",";
			}
			cout<<"\n";
			//updating the current and previous region of every volunteer (for every region)
			for (int k = 1; k <= region_list.size(); k++)
			{
				if (isInsideRegion(v, k) == true)
				{
					region_volunteer_list[i][v].prev_region = region_volunteer_list[i][v].curr_region;
					region_volunteer_list[i][v].curr_region = k;
					break;
				}
			}
			
			//Fill the channel preference vector
			//1. Fill
			for (int c = 0; c< channel_count_per_region; c++)
			{
				region_volunteer_list[i][v].channel_preference_vector.push_back(c);
			}
			//2. Shuffle
			std::random_shuffle( region_volunteer_list[i][v].channel_preference_vector.begin(), region_volunteer_list[i][v].channel_preference_vector.end() );
			
		}
	}
	
	//while (cnt > 0)
	while (event_over() == false)
	{
		//cout<<"\nEvent over"<<event_over();
		//cout<<"hello there";
		hold(AUI);
		
		//print_channel_list();
		std::cout<<"hello";
		//fill the region_volunteer list for all regions.
		/* for (int i = 1; i<= region_list.size(); i++)
		{
			for (int v = 1; v <= volunteer_list.size(); v++)
			{
				if (isInsideRegion(v, i) == true)
				{
					region_volunteer_list[i][v].residence_time += 1.0;
				}
			}
		} */
		
		for (int i = 1; i<= region_list.size(); i++)
		{
			for (int v = 1; v <= volunteer_list.size(); v++)
			{
				
				region_volunteer_list[i][v].x = volunteer_list[v].x;
				region_volunteer_list[i][v].y = volunteer_list[v].y;
				region_volunteer_list[i][v].bin_value = volunteer_list[v].bin_value;
				region_volunteer_list[i][v].corrupt = volunteer_list[v].corrupt;
				
				//Update volunteer channel detection capability vector
				region_volunteer_list[i][v].vol_det_cap = volunteer_list[v].vol_channel_detection[i];
				
				//Store the detection capabilities in a map
				
				for (int dc = 0; dc < region_volunteer_list[i][v].vol_det_cap.size(); dc++)
				{
					int channel = region_volunteer_list[i][v].vol_det_cap[dc].id;
					float det_cap = region_volunteer_list[i][v].vol_det_cap[dc].detection_capability;
					region_volunteer_list[i][v].detection_capability_map[channel] = det_cap;
				}
				
				//Multiply the volunteer detection capability with the shortest time to reach the region.
				float shortest_time_to_region = shortest_time_calc(i, region_volunteer_list[i][v].x, region_volunteer_list[i][v].y, volunteer_list[v].average_speed);
				//cout<<"\nShortest time to region"<<i<<": "<< shortest_time_to_region;
				for (int dc = 0; dc < region_volunteer_list[i][v].vol_det_cap.size(); dc++)
				{
					int channel = region_volunteer_list[i][v].vol_det_cap[dc].id;
					//cout<<"\nChannel "<<channel<<":  "<<region_volunteer_list[i][v].vol_det_cap[dc].detection_capability<<"\n";
					region_volunteer_list[i][v].vol_det_cap[dc].detection_capability = (float)region_volunteer_list[i][v].vol_det_cap[dc].detection_capability * (float)(1/shortest_time_to_region);
					//cout<<"\nChannel "<<channel<<":  "<<region_volunteer_list[i][v].vol_det_cap[dc].detection_capability<<"\n";
				}
				
				
				//Test print
				/* cout<<"TESTIN TESTING#################\n#################\n********************\n((((((((((((\n";
				cout <<"\nRegion "<<i<<"\n";
				cout<<"\nVolunteer "<<v<<"\n";
				for (int dc = 0; dc < region_volunteer_list[i][v].detection_capability_map.size(); dc++)
				{
					cout<<"Channel "<<dc<<":"<<region_volunteer_list[i][v].detection_capability_map[dc]<<"\n";
				} */
				/* cout<<"\nTESTIN TESTING#################\n#################\n********************\n((((((((((((\n";
				cout <<"\nRegion "<<i<<"\n";
				cout<<"\nVolunteer "<<v<<"\n";
				for (int dc = 0; dc < region_volunteer_list[i][v].vol_det_cap.size(); dc++)
				{
					int channel = region_volunteer_list[i][v].vol_det_cap[dc].id;
					float det_cap = region_volunteer_list[i][v].vol_det_cap[dc].detection_capability;
					cout << "\nChannel "<<channel<<":  "<<det_cap<<"\n";
				} */
				
				
				
				
				
				//Residence Probability Update
				float sum_residence = 0.0;
				
				
				
				for (int rr = 1; rr <= region_list.size(); rr++)
				{
					sum_residence += region_volunteer_list[rr][v].residence_time;
				}
				if (sum_residence > 0.0)
					region_volunteer_list[i][v].residence_probability = (float)region_volunteer_list[i][v].residence_time/(float)sum_residence;
				
				
				//region_volunteer_list[i][v].channel_index = 0;
				
				//cout <<"\n fill halt";

				//updating the current and previous region of every volunteer (for every region)
				for (int k = 1; k <= region_list.size(); k++)
				{
					if (isInsideRegion(v, k) == true)
					{
						region_volunteer_list[i][v].prev_region = region_volunteer_list[i][v].curr_region;
						region_volunteer_list[i][v].curr_region = k;
						break;
					}
				}
				
			}
		}
		float alpha2 = 0.01;
		//Calculating the sojourn time and return time of all volunteers for every region
		for (int i = 1; i <= region_list.size(); i++)
		{
			for (int v = 1; v <= volunteer_list.size(); v++)
			{
				if (region_volunteer_list[i][v].prev_region!= 0 && region_volunteer_list[i][v].curr_region!= 0)
				{
					//Change in region
					if (region_volunteer_list[i][v].prev_region != region_volunteer_list[i][v].curr_region) 
					{
						//If current region is my region
						if (region_volunteer_list[i][v].curr_region == i)
						{
							
							region_volunteer_list[i][v].ret_time += AUI;
							region_volunteer_list[i][v].p_r = (float)region_volunteer_list[i][v].soj_time/(float)region_volunteer_list[i][v].ret_time;
							
							if(region_volunteer_list[i][v].p_r_avg  == 1.0f)
							{
								region_volunteer_list[i][v].p_r_avg = region_volunteer_list[i][v].p_r;
								
							}
							else
							{
								region_volunteer_list[i][v].p_r_avg = residence_param(region_volunteer_list[i][v].p_r, region_volunteer_list[i][v].p_r_avg);
	
							}
							region_volunteer_list[i][v].ret_time = 0.0f;
							region_volunteer_list[i][v].soj_time = 0.0f;
							
							
						}
						//if current region is not my region but previous region was mine
						else if (region_volunteer_list[i][v].curr_region != i && region_volunteer_list[i][v].prev_region == i)
						{
							region_volunteer_list[i][v].soj_time += AUI;
							region_volunteer_list[i][v].ret_time += AUI;
							
							if(region_volunteer_list[i][v].sojourn_avg == 0.0f)
								region_volunteer_list[i][v].sojourn_avg = (float)region_volunteer_list[i][v].soj_time;
							else
							{
								
								float c = 0.03;
								float Err = region_volunteer_list[i][v].sojourn_avg - region_volunteer_list[i][v].soj_time;
								region_volunteer_list[i][v].sigma = c * (Err * Err) + (1 - c)*region_volunteer_list[i][v].sigma;
								if (region_volunteer_list[i][v].sigma > 0.0)
								{
									region_volunteer_list[i][v].alpha10 = c * (Err * Err)/ region_volunteer_list[i][v].sigma;
								}
								
								region_volunteer_list[i][v].sojourn_avg = region_volunteer_list[i][v].alpha10*(float)region_volunteer_list[i][v].soj_time + (1 - region_volunteer_list[i][v].alpha10) * (float)region_volunteer_list[i][v].sojourn_avg;
							}
						}
						//if both current and previous regions are not my region
						else if (region_volunteer_list[i][v].curr_region != i && region_volunteer_list[i][v].prev_region != i)
						{
							if (region_volunteer_list[i][v].soj_time!=0.0f)
								region_volunteer_list[i][v].ret_time += AUI;
						}
					}
					//No change in region
					if (region_volunteer_list[i][v].prev_region == region_volunteer_list[i][v].curr_region) 
					{
						//if current region is my region
						if (region_volunteer_list[i][v].curr_region == i)
						{
							region_volunteer_list[i][v].soj_time += AUI;
							region_volunteer_list[i][v].ret_time += AUI;
						}
						//if current region is not my region
						if (region_volunteer_list[i][v].curr_region != i)
						{
							if (region_volunteer_list[i][v].soj_time!=0.0f)
								region_volunteer_list[i][v].ret_time += AUI;
						}
					}
				}
			}
		}
		for (int i = 1; i <= region_list.size(); i++)
		{
			for (int v = 1; v <= volunteer_list.size(); v++)
			{
				if(region_volunteer_list[i][v].p_r_avg == 0.0f && region_volunteer_list[i][v].p_r == 0.0f && region_volunteer_list[i][v].soj_time == 0.0f && region_volunteer_list[i][v].ret_time == 0.0f)
				{
					if (region_volunteer_list[i][v].curr_region == i)
						region_volunteer_list[i][v].p_r_avg = 1.0f;
				}
			}
		}
		
		iter++;
		
		//increase MI counter after burnout
		if (iter > burnout)
			MI_count++;
		
		
		cout<<"\n\n ----------------- Iteration:  -------------------- "<<iter<<"\n\n";
		//printf("\n\nSelecting volunteers\n");
		//print_vol_list();
		
		//print_region_vol_list();
		
		//--cnt;
		
		//Before the 1st MI
		
		/* if (no_of_MI == 0)
		{
			cout<<"\n Before the "<<no_of_MI<<"MI\n";
			
			for (int i = 1; i <= region_list.size(); i++)
			{
				
				cout <<"\nRegion "<<i<<endl;
				for (int v = 1; v <= volunteer_list.size(); v++)
				{
			
					cout<<"\nVolunteer"<<v<<" TP:"<< region_volunteer_list[i][v].TP<<endl;
					cout<<"\nVolunteer"<<v<<" FP:"<< region_volunteer_list[i][v].FP<<endl;
					cout<<"\nVolunteer"<<v<<" TN:"<<region_volunteer_list[i][v].TN<<endl;
					cout<<"\nVolunteer"<<v<<" FN:"<< region_volunteer_list[i][v].FN<<endl;
				}
			}
		} */
		
		//cout<<"\nwhat happens MI count"<<MI_count;
		//cout<<"\nhere\n";
		//Check if it is a MONITORING INTERVAL, Select Volunteers now
		if (MI_count == 5)
		{
			
			
			//cout<<"\nhere2\n";
			MI_count = 0;
			no_of_MI++;
			printf("\n\n +++++++++++ This is time for MI ++++++++++++\n\n");
			//Calculate trust parameters for the last bunch of selection
			
			for (int i = 1; i <= region_list.size(); i++)
			{
				//cout<<"\nhere3\n";
				if (TP_round[i] + FP_round[i] >0.0)
					accuracy_round[i] = (TP_round[i] /(TP_round[i] + FP_round[i]));
				
			}
			for (int c = 0; c < channel_count_per_region; c++)
			{
				
				if (channel_TP_round[c] + channel_FP_round[c] >0.0)
				{
					channel_accuracy_round[c] = channel_TP_round[c]/(channel_TP_round[c] + channel_FP_round[c]);
				}
				
			}
			
			//push the trust values in a list
			
			for (int i = 1; i <= region_list.size(); i++)
			{
				if (no_of_MI >1)
				{
					accuracy_list[i].push_back(accuracy_round[i]);
					
				}
			}
			
			//Push values in channel accuracy list
			for (int c = 0; c < channel_count_per_region; c++)
			{
				if (no_of_MI >1)
				{
					channel_accuracy_list[c].push_back(channel_accuracy_round[c]);
					
				}
			}
			
			
			//cout<<"\nhere3\n";
			/* for(int i = 1; i <=region_list.size(); i++)
			{
				cout<<"\nRegion "<<i<<"accuracy list\n";
				for (int j = 0; j< accuracy_list[i].size(); j++)
				{
					cout<<accuracy_list[i][j]<<",";
				}
			} */
			
			//Reset the trust parameters
			
			for (int i = 1; i <= region_list.size(); i++)
			{
				TP_round[i] = 0.0;
				TN_round[i] = 0.0;
				FP_round[i] = 0.0;
				FN_round[i] = 0.0;
				
			}
			
			for(int c = 0; c< channel_count_per_region; c++)
			{
				channel_TP_round[c] = 0.0;
				channel_FP_round[c] = 0.0;
			}
			
			//---------------------------------------------
			
			//cout<<"\nhere4\n";
			//
			//1. Calculate Separation factor for all volunteers of all regions
			for (int i = 1; i <= region_list.size(); i++)
			{
				for (int v = 1; v <= volunteer_list.size(); v++)
				{ 
					region_volunteer_list[i][v].sep_factor = separation_factor(i, region_volunteer_list[i][v].x, region_volunteer_list[i][v].y);
				}
			}
			
			for (int i = 1; i <= region_list.size(); i++)
			{
				for (int v = 1; v <= volunteer_list.size(); v++)
				{ 
					region_volunteer_list[i][v].shortest_time = shortest_time_calc(i, region_volunteer_list[i][v].x, region_volunteer_list[i][v].y, volunteer_list[v].average_speed);
					/* if (v == 3)
					{
						cout<<"\n??????????????????? SHORTEST TIME ??????????????????????????????\n";
						cout<<"VOLUNTEER :"<<v<<"\n";
						cout<<"WRT Region:"<<i<<"\n";
						cout<<"\nX and Y :"<<region_volunteer_list[i][v].x<<","<<region_volunteer_list[i][v].y<<"\n";
						cout<<"Average Speed: "<<volunteer_list[v].average_speed<<"\n";
						cout<<"Shortest time: "<<region_volunteer_list[i][v].shortest_time<<"\n";
					} */
				}
			}
			
			//2. Calculate mean reputation (named 'accuracy' for convenience) of all volunteers
			//For Secretary Algorithm
			for (int i = 1; i <= region_list.size(); i++)
			{
				for (int v = 1; v <= volunteer_list.size(); v++)
				{ 
					float sum =0.0;
					
					for (int j = 0; j<region_volunteer_list[i][v].reputation.size(); j++)
					{
						sum += region_volunteer_list[i][v].reputation[j];
					}
					region_volunteer_list[i][v].accuracy = (float)sum/(float)region_volunteer_list[i][v].reputation.size() ;
					
				}
			}
			//cout<<"\nhere5\n";
/* 			for (int i = 1; i <= region_list.size(); i++)
			{
				cout << "\nRegion "<<i<<endl;
				for (int v = 1; v <= volunteer_list.size(); v++)
				{ 
					cout<<"Volunteer "<<v<<"acuracy: "<<region_volunteer_list[i][v].accuracy<<",";
				}
				cout<<"\n";
			}
			 */
			//3. Normalize Accuracy and Pr_avg by Z score
			
			/* for (int i = 1; i <= region_list.size(); i++)
			{
				float sum_accuracy = 0.0;
				float sum_pravg = 0.0;
				for (int v = 1; v <= volunteer_list.size(); v++)
				{ 
					sum_accuracy += region_volunteer_list[i][v].accuracy;
					sum_pravg += region_volunteer_list[i][v].p_r_avg * region_volunteer_list[i][v].sep_factor;
				}
				float mean_accuracy = sum_accuracy/(float)volunteer_list.size();
				float mean_pravg = sum_pravg/(float)volunteer_list.size();
				
				float stdev_accuracy = 0.0;
				float stdev_pravg = 0.0;
				
				for (int v = 1; v <= volunteer_list.size(); v++)
				{ 
					stdev_accuracy += pow(((region_volunteer_list[i][v].accuracy) - mean_accuracy), 2);
					stdev_pravg += pow(((region_volunteer_list[i][v].p_r_avg * region_volunteer_list[i][v].sep_factor) - mean_pravg), 2);
				}
				stdev_accuracy = sqrt(stdev_accuracy/(float)volunteer_list.size());
				stdev_pravg += sqrt(stdev_pravg/(float)volunteer_list.size());
				
				for (int v = 1; v <= volunteer_list.size(); v++)
				{ 
					region_volunteer_list[i][v].normal_accuracy = abs(region_volunteer_list[i][v].accuracy - mean_accuracy)/(float)stdev_accuracy;
					region_volunteer_list[i][v].normal_pravg = abs((region_volunteer_list[i][v].p_r_avg * region_volunteer_list[i][v].sep_factor) - mean_pravg)/(float)stdev_pravg;
				}
				
			} */
			
			//3. Normalize Accuracy and Pr_avg by scaling #########
			
			for (int i = 1; i <= region_list.size(); i++)
			{
				float max_accuracy = region_volunteer_list[i][1].accuracy;
				float min_accuracy = region_volunteer_list[i][1].accuracy;
				
				for (int v = 1; v <= volunteer_list.size(); v++)
				{ 
					if(region_volunteer_list[i][v].accuracy > max_accuracy)
						max_accuracy = region_volunteer_list[i][v].accuracy;
					
					else if(region_volunteer_list[i][v].accuracy < min_accuracy)
						min_accuracy = region_volunteer_list[i][v].accuracy;
				}
				
				float max_pravg = region_volunteer_list[i][1].p_r_avg * region_volunteer_list[i][1].sep_factor;
				float min_pravg = region_volunteer_list[i][1].p_r_avg * region_volunteer_list[i][1].sep_factor;
				
				for (int v = 1; v <= volunteer_list.size(); v++)
				{ 
					if((region_volunteer_list[i][v].p_r_avg * region_volunteer_list[i][v].sep_factor) > max_pravg)
						max_pravg = region_volunteer_list[i][v].p_r_avg * region_volunteer_list[i][v].sep_factor;
					
					else if((region_volunteer_list[i][v].p_r_avg * region_volunteer_list[i][v].sep_factor) < min_pravg)
						min_pravg = region_volunteer_list[i][v].p_r_avg * region_volunteer_list[i][v].sep_factor;
				}
				for (int v = 1; v <= volunteer_list.size(); v++)
				{ 
					
					region_volunteer_list[i][v].normal_accuracy = abs(region_volunteer_list[i][v].accuracy - min_accuracy)/(float)(max_accuracy - min_accuracy);
					region_volunteer_list[i][v].normal_pravg = abs((region_volunteer_list[i][v].p_r_avg * region_volunteer_list[i][v].sep_factor) - min_pravg)/(float)(max_pravg - min_pravg);
				}
				
				
			}
			//cout<<"\nhere6\n";
			
			//Normalize reputations
			//1. Find Max
			std::map <int, float> max_reputation;
			std::map <int, float> min_reputation;
			
			for(int i = 1; i <= region_list.size(); i++)
			{
				max_reputation[i] = region_volunteer_list[i][1].reputation[0];
				min_reputation[i] = region_volunteer_list[i][1].reputation[0];
				for(int v = 1; v <= volunteer_list.size();v++)
				{
					
					for (int c = 0; c< channel_count_per_region; c++)
					{
						if(region_volunteer_list[i][v].reputation[c] > max_reputation[i])
							max_reputation[i] = region_volunteer_list[i][v].reputation[c];
						
						else if(region_volunteer_list[i][v].reputation[c] < min_reputation[i])
							min_reputation[i] = region_volunteer_list[i][v].reputation[c];
					}
				}
			}
			
			//2. Normalize
			for(int i = 1; i <= region_list.size(); i++)
			{
				
				for(int v = 1; v <= volunteer_list.size();v++)
				{					
					for (int c = 0; c< channel_count_per_region; c++)
					{
						region_volunteer_list[i][v].normal_reputation[c] = (region_volunteer_list[i][v].reputation[c] - min_reputation[i])/(max_reputation[i] - min_reputation[i]);
					}
				}
			}
			
			//Normalize SOJOURN AVERAGE ######
			float max_sojourn_avg;
			float min_sojourn_avg;
			for(int i = 1; i <= region_list.size(); i++)
			{
				max_sojourn_avg = region_volunteer_list[i][1].sojourn_avg;
				min_sojourn_avg = region_volunteer_list[i][1].sojourn_avg;
				for (int v = 1; v <= volunteer_list.size();v++)
				{
					if (region_volunteer_list[i][v].sojourn_avg > max_sojourn_avg)
					{
						max_sojourn_avg = region_volunteer_list[i][v].sojourn_avg;
					}
					else if (region_volunteer_list[i][v].sojourn_avg < min_sojourn_avg)
					{
						min_sojourn_avg = region_volunteer_list[i][v].sojourn_avg;
					}
				}
							
			}
			for(int i = 1; i <= region_list.size(); i++)
			{
				for(int v = 1; v <= volunteer_list.size();v++)
				{
					region_volunteer_list[i][v].normal_sojourn_avg = (region_volunteer_list[i][v].sojourn_avg - min_sojourn_avg)/(max_sojourn_avg - min_sojourn_avg);
				}
			}
			
			//NORMALIZE SHORTEST TIME
			float max_shortest_time;
			float min_shortest_time;
			for(int i = 1; i <= region_list.size(); i++)
			{
				max_shortest_time = region_volunteer_list[i][1].shortest_time;
				min_shortest_time = region_volunteer_list[i][1].shortest_time;
				
				for (int v = 1; v <= volunteer_list.size();v++)
				{
					if (region_volunteer_list[i][v].shortest_time > max_shortest_time)
					{
						max_shortest_time = region_volunteer_list[i][v].shortest_time;
					}
					else if (region_volunteer_list[i][v].shortest_time < min_shortest_time)
					{
						min_shortest_time = region_volunteer_list[i][v].shortest_time;
					}
				}
			}
			for(int i = 1; i <= region_list.size(); i++)
			{
				for(int v = 1; v <= volunteer_list.size();v++)
				{
					region_volunteer_list[i][v].normal_shortest_time = ((float)region_volunteer_list[i][v].shortest_time - min_shortest_time)/((float)max_shortest_time - min_shortest_time);
				}
			}
			
			/* cout<<"\n############$$$$$$$$ NORMAL PARAMETERS $$$$$$$$$#################\n";
			cout<<"\nNormal Shortest Time: "<<region_volunteer_list[1][2].normal_shortest_time<<"\n";
			cout<<"\nNormal Sojourn Average: "<<region_volunteer_list[1][10].sojourn_avg<<"\n";
			cout<<"\nNormal Reputation: "<<region_volunteer_list[1][2].normal_accuracy<<"\n"; */
			//sasasa
			
			 /* for(int i = 1; i <= region_list.size(); i++)
			{
				for(int v = 1; v <= volunteer_list.size();v++)
				{
					cout<<"\nSojourn Average: "<<region_volunteer_list[i][v].sojourn_avg<<"\n";
					cout<<"NORMAL Sojourn Average: "<<region_volunteer_list[i][v].normal_sojourn_avg<<"\n";
				}
			}  */
			//
			/////////////%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
			///////									QUALIFICATION								//////////////
			/////////////%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
			//4. Calculate Qualification of all volunteers of all region
			for (int i = 1; i <= region_list.size(); i++)
			{
				for (int v = 1; v <= volunteer_list.size(); v++)
				{ 
					//region_volunteer_list[i][v].qualification = region_volunteer_list[i][v].accuracy;
					//region_volunteer_list[i][v].qualification = (region_volunteer_list[i][v].p_r_avg * region_volunteer_list[i][v].sep_factor) ;
					
				//	region_volunteer_list[i][v].qualification = (region_volunteer_list[i][v].p_r_avg * region_volunteer_list[i][v].sep_factor) * region_volunteer_list[i][v].accuracy;
					
					//region_volunteer_list[i][v].qualification = (weight1*region_volunteer_list[i][v].normal_pravg) + (weight2*region_volunteer_list[i][v].normal_accuracy);
					
					float rep = region_volunteer_list[i][v].normal_accuracy;
					float soj = region_volunteer_list[i][v].normal_sojourn_avg;
					float tim = region_volunteer_list[i][v].normal_shortest_time;
					
					float pr = region_volunteer_list[i][v].residence_probability;
					//
					//1. Multiplicative
					//region_volunteer_list[i][v].qualification = exp(rep)*pr*((1 - tim)*log(2 - tim))*(soj * log(1 + soj));
					//
					//1.1 Multiplicative
					//region_volunteer_list[i][v].qualification = exp(rep)*pr*(log(2 - tim))*(log(1 + soj));
					
					//2. Additive
					//region_volunteer_list[i][v].qualification = exp(rep)*pr*(((1 - tim)*log(2 - tim)) + (soj * log(1 + soj)));
					
					//2.1 Additive
					//region_volunteer_list[i][v].qualification = exp(rep)*pr*((log(2 - tim))+(log(1 + soj)));
					
					//Additive
					//3. w1>w2
					region_volunteer_list[i][v].qualification = exp(rep)*pr*((0.75*(1-tim)) + (0.25 * soj));
					
					//4. w1 = w2
					//region_volunteer_list[i][v].qualification = exp(rep)*pr*((0.5*(1-tim)) + (0.5 * soj));
					
					//5. w1 < w2
					//region_volunteer_list[i][v].qualification = exp(rep)*pr*((0.25*(1-tim)) + (0.75 * soj));
					
					//Multiplicative
					//6. w1>w2
					//region_volunteer_list[i][v].qualification = exp(rep)*pr*((0.75*(1-tim)) * (0.25 * soj));
					
					//7. w1 = w2
					//region_volunteer_list[i][v].qualification = exp(rep)*pr*((0.5*(1-tim)) * (0.5 * soj));
					
					//8. w1 < w2
					//region_volunteer_list[i][v].qualification = exp(rep)*pr*((0.25*(1-tim)) * (0.75 * soj));
					
					//region_volunteer_list[i][v].qualification = rep;
					
					
					qualification_list[i][v] = region_volunteer_list[i][v].qualification;
				}
			}
			//
			 /*for (int i = 1; i <= region_list.size(); i++)
			{
				printf("\n\nQualifications for region %d: \n", i);
				for (int v = 1; v <= volunteer_list.size(); v++)
				{ 
					printf("\nVolunteer %d: %f\n", v, qualification_list[i][v]);
				}
			} 
			*/
			////-------------- Removing volunteers whose batteries have ended ------------------- ////////////////
			for (int i = 1; i <= region_list.size(); i++)
			{
				printf("\n\nQualifications for region %d: \n", i);
				for (int v = 1; v <= volunteer_list.size(); v++)
				{ 
					if(battery_list[v] <= 0)
					{
						qualification_list[i].erase(v);
					}
				}
			}
			 
			//cout<<"\nhere7\n";
			//Clear Stuff
					
			VS_regionList.clear(); //Clear the current set of selected volunters
			
			//clear selected values of all volunteers 
			for (int i = 1; i <= region_list.size(); i++)
			{
				for (int v = 1; v <= volunteer_list.size(); v++)
				{ 
					region_volunteer_list[i][v].selected = false;
				}
			}
			//
			//Select k s
			//s
			
			//############################################
			//		SELECT K
			//############################################
			
			srand(iter);
			int percentage = 75 + rand()%(25);//////////
			printf("\nPercentage :%d:", percentage);
			
			int curent_max_vol = qualification_list[1].size();
			
			cout<<"\n\n******* Current number of ACTIVE VOLUNTEERS:"<<curent_max_vol<<"\n\n";
			if (curent_max_vol <=1)
				break;
			
			int k = floor(((float)percentage/(float)100)* curent_max_vol);
			global_k = k;
			
			printf("\n value of k: %d", k);
			//safety
			//update mean reputations
			//update_mean_reputation();
			
			//Call the given algorithms
			init_current_channel_index(); //Clear the current channel index before calling the secretary algorithm
			
			
			
			
			
		//	std::map< int, std::map <int, vector <struct vol_ID_Qual> > region_channel_preference; 
			
			//Delete the current channel preference list
			region_channel_preference.clear();
 
			//Determine Channel Preference List

			for(int i = 1; i <= region_list.size(); i++)
			{
				for (int c = 0; c< channel_count_per_region; c++)
				{
					for(int v = 1; v <= volunteer_list.size();v++)
					{
						
						//***** CALCULATING QUALIFICATION
						
						float qualification2 = region_volunteer_list[i][v].normal_reputation[c] * (region_volunteer_list[i][v].normal_pravg);
						
						struct vol_ID_Qual vIDq;
						vIDq.vol_id = v;
						vIDq.qualification = qualification2;
						
						region_channel_preference[i][c].push_back(vIDq);
					}
				}
				
			}
			
			// sort the preferences
			for(int i = 1; i <= region_list.size(); i++)
			{
				for (int c = 0; c< channel_count_per_region; c++)
				{
					std::sort(region_channel_preference[i][c].begin(), region_channel_preference[i][c].end(), compareVolunteer_Qual);
				}
			}
			
			/*
			//Print the channel preference list
			cout <<"\n\n%%%%%%%%% CHANNEL PREFERENCE LIST %%%%%%%%%%%%%\n";
			
			for(int i = 1; i <= region_list.size(); i++)
			{
				for (int c = 0; c< channel_count_per_region; c++)
				{
					cout <<"Channel "<<c<<"==\n";
					for(int v = 0; v < region_channel_preference[i][c].size();v++)
					{
						cout<<"Volunteer "<<v<<" -> ID: "<<region_channel_preference[i][c][v].vol_id<<", Qualification: "<<region_channel_preference[i][c][v].qualification<<"\n\n";
					}
				}
			}
			
			*/
			int k_prime = (float)k*1.5;
			if(k_prime > volunteer_list.size())
			{
				k_prime = volunteer_list.size();
			}
			cout<<"\nVALUE OF K:::::::"<<k<<"\n";
			cout<<"\nVALUE OF K_PRIME:::::::"<<k_prime<<"\n";
			
			
			//Happiness Score parameter
			std::map< int, float > sum_happiness; //Region -> Happiness score
			for (int rr = 1;  rr <= region_list.size(); rr++)
			{
				sum_happiness[rr] = 0.0;
			}
			
			//################################# 
			//VANILLA ALGORITHMS
			//#################################
			/* 
			if(k>=1)
			{
				if (reverse1 == 0)
				{
					//cout <<"\n\nReverse is"<<reverse1<<"\n\n";
					for (int i = 1; i <= region_list.size(); i++)
					{
						std::map< int, std:: map <int, float> >qualification_list_2;
						
						selected_volunteers.clear();
						
						int k_temp = ceil ((float)k/(float)region_list.size());
						
						
						
						for (int ij = 1; ij <= region_list.size(); ij++)
						{
							for (int v = 1; v <= volunteer_list.size(); v++)
							{
								if(region_volunteer_list[ij][v].selected != true)
								{
									qualification_list_2[ij][v] = qualification_list[ij][v];
								}
							}
						}
						
						//Algorithm 1
						////Secretary2(qualification_list_2[i], k_temp, i);
						
						//Secretary2(qualification_list[i], k_temp, i);
						//random_algo2(qualification_list[i], k, i);
						
						////random_algo2(qualification_list_2[i], k_temp, i);
						
						//Secretary(qualification_list[i], k, i);
						//random_algo(qualification_list[i], k, i);
						//random_algo2(qualification_list[i], k, i);
						//CHANGE POINT
						
						////assign_channels(i, k_temp, sum_happiness); //
						////assign_channels_round_robin(i, k_temp, sum_happiness);
						
						
					}
					
				}
				else
				{
					//cout <<"\n\nReverse is"<<reverse1<<"\n\n";
					for (int i = region_list.size(); i >= 1; i--)
					{
						std::map< int, std:: map <int, float> >qualification_list_2;
						
						selected_volunteers.clear();
						//StableMarriage2(qualification_list[i], k, i);
						int k_temp = ceil ((float)k/(float)region_list.size());
						
						
						
						for (int ij = 1; ij <= region_list.size(); ij++)
						{
							for (int v = 1; v <= volunteer_list.size(); v++)
							{
								if(region_volunteer_list[ij][v].selected != true)
								{
									qualification_list_2[ij][v] = qualification_list[ij][v];
								}
							}
						}
						
						//Algorithm 1
						////Secretary2(qualification_list_2[i], k_temp, i);
						
						//Secretary2(qualification_list[i], k_temp, i);
						//random_algo2(qualification_list[i], k, i);
						
						////random_algo2(qualification_list_2[i], k_temp, i);
						
						//Secretary(qualification_list[i], k, i);
						//random_algo(qualification_list[i], k, i);
						//random_algo2(qualification_list[i], k, i);
						//CHANGE POINT
						
						////assign_channels(i, k_temp, sum_happiness); //
						////assign_channels_round_robin(i, k_temp, sum_happiness);
						
						
					}
				}
				//Deferred_Acceptance2(qualification_list, k);//
				Deferred_Acceptance_channelPropose(qualification_list, k);
				
			}
			if(reverse1 == 0)
				reverse1 = 1;
			else if (reverse1 == 1)
				reverse1 = 0;
			
			 */
			/* if(k>=1)
			//if(k_prime>=1)
			{
				for (int i = 1; i <= region_list.size(); i++)
				{
					selected_volunteers.clear();
					//StableMarriage2(qualification_list[i], k, i);
					
					//Algorithm 1
					//Secretary2(qualification_list[i], k, i);
					//random_algo2(qualification_list[i], k, i);
					
					//Secretary(qualification_list[i], k, i);
					//random_algo(qualification_list[i], k, i);
					//random_algo2(qualification_list[i], k, i);
					//CHANGE POINT
					//assign_channels(i, k, sum_happiness); 
					//assign_channels_round_robin(i, k, sum_happiness);
					
					
				}
				//Deferred_Acceptance2(qualification_list, k);
				Deferred_Acceptance_channelPropose(qualification_list, k);
				
				
			} */
			
			//########################## 
			//END of vanilla algorithms 
			//##########################
			
			
			//#####################
			//HYBRID Algorithms 
			//#####################
			
			
			
			if(k>=1)
			{
				if (reverse1 == 0)
				{
					cout <<"\n\nReverse is"<<reverse1<<"\n\n";
					for (int i = 1; i <= region_list.size(); i++)
					{
						std::map< int, std:: map <int, float> >qualification_list_2;
						
						selected_volunteers.clear();
						
						int k_temp = ceil ((float)k/(float)region_list.size());
						
						
						
						for (int ij = 1; ij <= region_list.size(); ij++)
						{
							for (int v = 1; v <= volunteer_list.size(); v++)
							{
								if(region_volunteer_list[ij][v].selected != true)
								{
									qualification_list_2[ij][v] = qualification_list[ij][v];
								}
							}
						}
						
						//Algorithm 1
						Secretary2(qualification_list_2[i], k_temp, i);
						//Secretary2(qualification_list[i], k_temp, i);
						//random_algo2(qualification_list[i], k, i);
						
						//Secretary(qualification_list[i], k, i);
						//random_algo(qualification_list[i], k, i);
						//random_algo2(qualification_list[i], k, i);
						//CHANGE POINT
						//assign_channels(i, k_temp, sum_happiness); //
						//assign_channels_round_robin(i, k_temp, sum_happiness);
						
						
					}
					
				}
				else
				{
					cout <<"\n\nReverse is"<<reverse1<<"\n\n";
					for (int i = region_list.size(); i >= 1; i--)
					{
						std::map< int, std:: map <int, float> >qualification_list_2;
						
						selected_volunteers.clear();
						//StableMarriage2(qualification_list[i], k, i);
						int k_temp = ceil ((float)k/(float)region_list.size());
						
						
						
						for (int ij = 1; ij <= region_list.size(); ij++)
						{
							for (int v = 1; v <= volunteer_list.size(); v++)
							{
								if(region_volunteer_list[ij][v].selected != true)
								{
									qualification_list_2[ij][v] = qualification_list[ij][v];
								}
							}
						}
						
						//Algorithm 1
						Secretary2(qualification_list_2[i], k_temp, i);
						//Secretary2(qualification_list[i], k_temp, i);
						//random_algo2(qualification_list[i], k, i);
						
						//Secretary(qualification_list[i], k, i);
						//random_algo(qualification_list[i], k, i);
						//random_algo2(qualification_list[i], k, i);
						//CHANGE POINT
						//assign_channels(i, k_temp, sum_happiness); //
						//assign_channels_round_robin(i, k_temp, sum_happiness);
						
						
					}
				}
				//Deferred_Acceptance2(qualification_list, k);
				
				
			}
			if(reverse1 == 0)
				reverse1 = 1;
			else if (reverse1 == 1)
				reverse1 = 0;
			
			
			//Experiment = 1st step secretary, then matching algorithm
			
			std::map< int, std:: map <int, float> >qualification_list_2;
			
			for (int i = 1; i <= region_list.size(); i++)
			{
				for (int v = 1; v <= volunteer_list.size(); v++)
				{
					if(region_volunteer_list[i][v].selected == true)
					{
						qualification_list_2[i][v] = qualification_list[i][v];
					}
				}
			}
			for (int i = 1; i <= region_list.size(); i++)
			{
				for (int v = 1; v <= volunteer_list.size(); v++)
				{ 
					region_volunteer_list[i][v].selected = false;
				}
			}
			
			//Algorithm 2
			//Deferred_Acceptance2(qualification_list_2, k);///
			Deferred_Acceptance_channelPropose(qualification_list_2, k);//

			
			
			//########################## 
			//END of hybrid algorithm 
			//##########################
			
			
			//Experiment = 1st step secretary, then matching algorithm
			
			/* std::map< int, std:: map <int, float> >qualification_list_2;
			
			for (int i = 1; i <= region_list.size(); i++)
			{
				for (int v = 1; v <= volunteer_list.size(); v++)
				{
					if(region_volunteer_list[i][v].selected == true)
					{
						qualification_list_2[i][v] = qualification_list[i][v];
					}
				}
			}
			for (int i = 1; i <= region_list.size(); i++)
			{
				for (int v = 1; v <= volunteer_list.size(); v++)
				{ 
					region_volunteer_list[i][v].selected = false;
				}
			}
			
			//Algorithm 2
			Deferred_Acceptance2(qualification_list_2, k);
 */
			
			
			//
			//5. Print the List of selected volunteers
			/* printf("\n#####################$$$$$$$$$$$$$$$$$$+++++++++++++++++++@@@@@@@@@@@@@@@@@\n");
			printf("			Selected Volunteers 						");
			printf("\n#####################$$$$$$$$$$$$$$$$$$+++++++++++++++++++@@@@@@@@@@@@@@@@@\n");
			
			 for (int i = 1; i <= VS_regionList.size(); i++)
			{
				printf("\n Region %d\n\n", i);
				
				for (std::map<int,float>::iterator it = VS_regionList[i].begin(); it!=VS_regionList[i].end(); ++it)
				{
					printf("\nVolunteer id:%d , Qualification: %f\n", it->first, it->second);
				}
			}
			*/
			cout<<"\n No. of volunteers selected: "<<selected_volunteers.size()<<endl;
			
			
			
			
			
			
		}
		
		//cout<<"\nhere8\n";
		/* printf("\n Selected values of all volunteers\n");
		for (int i = 1; i <= region_list.size(); i++)
		{
			printf("\nRegion %d", i);
			for (int v = 1; v <= volunteer_list.size(); v++)
			{ 
				printf("\nVolunteer ID: %d, Selected Value: %d\n", v, region_volunteer_list[i][v].selected);
			}
		} */
		
		cout<<"\nNo. of MI:"<<no_of_MI;
		
		//Calculating Hits and Misses for the selected volunteers
		
		std::map< int, int > no_of_selected_volunteers; //maps the number of selected volunteers to region
		for (int i = 1; i <= region_list.size(); i++)
		{
			no_of_selected_volunteers[i] = 0;
		}
		cout<<"\n\n #############\n&&&&&&&&&&&&&\nSELECTED VOLUNTEER LIST\n\n";
		
		if (no_of_MI >=1 && selected_volunteers.size()>0)
		{
			//cout<<"\nhere9\n";
			//Calculate the hit ratio
			for (int i = 1; i <= region_list.size(); i++)
			{
				printf("\nRegion %d\n", i);
				float hit = 0.0;
				float miss = 0.0;
				for (int v = 1; v <= volunteer_list.size(); v++)
				{ 
					if (region_volunteer_list[i][v].selected == true)
					{
						no_of_selected_volunteers[i]++;
						
						//Check if it is in the region
						//cout<<"Volunteer id: "<<v;
						if (isInsideRegion(v, i) == true)
						{
							//printf("\nIts a hit for volunteer %d \n (%d, %d)", v, region_volunteer_list[i][v].x, region_volunteer_list[i][v].y);
							hit += 1.0;
							//cout<<",Hit Score ="<<1<<"\n";
						}
						else
						{
							//printf("\nIts a miss for volunteer %d \n (%d, %d)", v, region_volunteer_list[i][v].x, region_volunteer_list[i][v].y);
							miss += 1.0;
							//cout<<",Hit Score="<<0<<"\n";
						}
					}
				}
				if ((hit + miss) >0.0)
				{
					float hit_ratio = hit/(hit+miss);
					//printf("\n So, hit ratio = %f\n", hit_ratio);
					hit_ratio_list[i].push_back(hit_ratio);
				}
					
			}
		}
		
		volunteer_hit_map.clear();
		
		volunteer_hit_map_overall.clear();
		
		//Calculate the hit ratio of the top k volunteers in the application_list
		//std::map< int, vector<struct vol_hit> > volunteer_hit_map; //region -> [(v, hit)]
		//store the entries from application list to volunteer_hit_map
		
		//application_list[region][channel].push_back(v);
		//std::map< int, std::map< int, vector<int> > > application_list;
		
		//1. push the hit scores of all the applied volunteers per region
		
		//map for keeping the volunteer ids unique
		/* std::map<int, std::map<int, int> > vol_unique;
		
		for (std::map< int, std::map< int, vector<int> > >::iterator it=application_list.begin(); it != application_list.end(); ++it)//region
        {
			int region = it->first;
			for (std::map<int, vector<int> >::iterator kt = application_list[region].begin(); kt != application_list[region].end(); ++kt)
			{
				int channel = kt->first;
				
				for (int zz = 0; zz < application_list[region][channel].size(); zz++)
				{
					int vol_id = application_list[region][channel][zz];
					vol_unique[region][channel][vol_id] = 1;
				}
			}
		} */
		//Fill in volunteer_hit_map_overall
		
		for(int region = 1; region <= region_list.size(); region++)
		{
			for (int vol = 1; vol <= volunteer_list.size(); vol++)
			{
				struct vol_hit vh;
				if (isInsideRegion(vol, region) == true) //i.e. volunteer vol is inside region
				{
					vh.v = vol;
					vh.hit = 1;
					volunteer_hit_map_overall[region].push_back(vh);
				}
			}
		}
		////sort the volunteer_hit_map_overall by the hit score of volunteers
		
		for (std::map< int, vector<struct vol_hit> >::iterator it = volunteer_hit_map_overall.begin(); it != volunteer_hit_map_overall.end(); ++it)
		{
			int region = it->first;
			std::sort(volunteer_hit_map_overall[region].begin(), volunteer_hit_map_overall[region].end(), compareByVolHitCount);
		}
		
				//calculate hit ratio of the optimal algorithm
		float K_R = ceil ((float)global_k/(float)region_list.size());
		for (std::map< int, vector<struct vol_hit> >::iterator it = volunteer_hit_map_overall.begin(); it != volunteer_hit_map_overall.end(); ++it)
		{
			int region = it->first;
			float total_hit = 0.0;
			float total = 0.0;
			for(int zz = 0; zz < volunteer_hit_map_overall[region].size(); zz++)
			{
				if(zz >= K_R)
				{
					break;
				}
				float current_hit = (float)volunteer_hit_map_overall[region][zz].hit;
				total_hit += current_hit;
				total += 1.0;
				
				
			}
			//insert in optimal hit ratio list
			if (total > 0.0)
			{
				//float hit_ratio = (float)total_hit/(float)no_of_selected_volunteers[region];
				
				float hit_ratio = (float)total_hit/(float)K_R;
				hit_ratio_list_optimal_overall[region].push_back(hit_ratio);
			}
			
		}
		
		//Fill in volunteer_hit_map
		
		for (std::map< int, std::map< int, vector<int> > >::iterator it=application_list.begin(); it != application_list.end(); ++it)//region
        {
			int region = it->first;
			std::vector <int> volunteer_id_list;
			for (std::map<int, vector<int> >::iterator kt = application_list[region].begin(); kt != application_list[region].end(); ++kt)
			{
				int channel = kt->first;
				
				for (int zz = 0; zz < application_list[region][channel].size(); zz++)
				{
					struct vol_hit vh;
					
					int vol_id = application_list[region][channel][zz];
					
					//check if vol_id is present in volunteer_id_list
					if(std::find(volunteer_id_list.begin(), volunteer_id_list.end(), vol_id) != volunteer_id_list.end()) 
					{
						/* v contains x */
					} 
					else 
					{
						/* v does not contain x */
						int hit_score = 0;
					
						if (isInsideRegion(vol_id, region) == true)
						{
							hit_score = 1;
						}
						vh.v = vol_id;
						vh.hit = hit_score;
						volunteer_hit_map[region].push_back(vh);
						
						volunteer_id_list.push_back(vol_id);
					}
				}
			}
		}
		
		//sort the volunteer_hit_map by the hit score of volunteers
		for (std::map< int, vector<struct vol_hit> >::iterator it = volunteer_hit_map.begin(); it != volunteer_hit_map.end(); ++it)
		{
			int region = it->first;
			std::sort(volunteer_hit_map[region].begin(), volunteer_hit_map[region].end(), compareByVolHitCount);
		}

		//print volunteer hit map per region
		
		cout<<"\n\n #############\n&&&&&&&&&&&&&\nSORTED VOLUNTEER HIT MAP\n\n";
		for (std::map< int, vector<struct vol_hit> >::iterator it = volunteer_hit_map.begin(); it != volunteer_hit_map.end(); ++it)
		{
			int region = it->first;
			cout<<"\nRegion: "<<region<<"\n\n";
			cout<<"Size: "<<volunteer_hit_map[region].size()<<"\n\n";
			/* for(int zz = 0; zz < volunteer_hit_map[region].size(); zz++)
			{
				cout<<"Volunteer id: "<<volunteer_hit_map[region][zz].v;
				cout<<", Hit Score: "<<volunteer_hit_map[region][zz].hit<<"\n\n";
			} */
		}
		
		
		//calculate hit ratio of the optimal algorithm
		for (std::map< int, vector<struct vol_hit> >::iterator it = volunteer_hit_map.begin(); it != volunteer_hit_map.end(); ++it)
		{
			int region = it->first;
			float total_hit = 0.0;
			float total = 0.0;
			for(int zz = 0; zz < volunteer_hit_map[region].size(); zz++)
			{
				if(zz >= no_of_selected_volunteers[region])
				{
					break;
				}
				float current_hit = (float)volunteer_hit_map[region][zz].hit;
				total_hit += current_hit;
				total += 1.0;
				
				
			}
			//insert in optimal hit ratio list
			if (total > 0.0)
			{
				float hit_ratio = (float)total_hit/(float)total;
				hit_ratio_list_optimal[region].push_back(hit_ratio);
			}
			
		}
		
		//Calculating accuracy of the selected volunteers
		
		//1. Select random interval for the sentinel to verify
		
		if (no_of_MI == 0)
		{
			//cout<<"\nhere10\n";
			//Burnout phase
			cout<<"\nThis is a burnout phase\n";
			
			for (int i = 1; i <= region_list.size(); i++)
			{
				rand_val(iter*i);
				double sentinel_on = ((double)binomial(0.5, 100)/100.0);
				if(sentinel_on > 0.4)
				{
					/* int n = 100;
					double p = 0.5;
					int seed = clock1 + iter;
					rand_val(seed);
					int bin_sentinel = binomial(p, n); //sentinel result */
				
					//now look for results of all volunteers
					for (int v = 1; v <= volunteer_list.size(); v++)
					{ 
						//cout<<"\nchecking for volunteer 1\n";
						//Find current region of volunteer
						if (isInsideRegion(v,i) == true)
						{
							float v_TP;
							float v_FP;
							int accurate_flag = 0;
							float detection_acc = region_volunteer_list[i][v].bin_value * region_volunteer_list[i][v].detection_capability_map[region_volunteer_list[i][v].channel_index];
							//if (region_volunteer_list[i][v].bin_value == 1) //True Positive
							if (detection_acc > 0.6)
							{
								/* cout <<"\nValue of v:"<<v;
								cout<<"\nValue of i:"<<i;
								cout<<"\nchannel index:"<<region_volunteer_list[i][v].channel_index<<"\n";
								cout<<"\nTP before: "<<region_volunteer_list[i][v].TP[region_volunteer_list[i][v].channel_index]<<"\n";
								
								cout<<"\n Channel index:"<<region_volunteer_list[i][v].channel_index<<"\n"; */
								region_volunteer_list[i][v].TP[region_volunteer_list[i][v].channel_index] += 1.0;	
								//cout<<"\nTP after: "<<region_volunteer_list[i][v].TP[region_volunteer_list[i][v].channel_index]<<"\n";
								v_TP = region_volunteer_list[i][v].TP[region_volunteer_list[i][v].channel_index];
								accurate_flag = 1;
							}
							else 
							{
								region_volunteer_list[i][v].FP[region_volunteer_list[i][v].channel_index] += 1.0;
								v_FP = region_volunteer_list[i][v].FP[region_volunteer_list[i][v].channel_index];
								accurate_flag = 0;
							}
							
							//Calculate Trust
							region_volunteer_list[i][v].trust[region_volunteer_list[i][v].channel_index] = (float)v_TP/(float)(v_TP + v_FP);
							float trust = region_volunteer_list[i][v].trust[region_volunteer_list[i][v].channel_index];
							
							//Calculate reputation (our approach)
							
							if (accurate_flag == 1)
							{
								region_volunteer_list[i][v].reputation[region_volunteer_list[i][v].channel_index]+= beta1*trust;
							}
							else
							{
								if (region_volunteer_list[i][v].reputation[region_volunteer_list[i][v].channel_index] < rep_threshold)
									region_volunteer_list[i][v].reputation[region_volunteer_list[i][v].channel_index] -= exp(beta2*(1-trust)); //if reputation is less than a given threshold, decrease more
								else
									region_volunteer_list[i][v].reputation[region_volunteer_list[i][v].channel_index] -= exp(beta3*(1-trust)); 
							} 
							
							//reputation (our approach)
							//REPUTATION_TEST
							//Reputation = trust
							
							//region_volunteer_list[i][v].reputation[region_volunteer_list[i][v].channel_index] = trust;
							
							
							// Linear increase/decrease
							
							/* if (accurate_flag == 1)
							{
								region_volunteer_list[i][v].reputation[region_volunteer_list[i][v].channel_index]+= 1.0;
							}
							else
							{
								region_volunteer_list[i][v].reputation[region_volunteer_list[i][v].channel_index]-= 1.0;
							} */
							
							
							//
							
							
							
							/* else
							{
								region_volunteer_list[i][v].reputation[region_volunteer_list[i][v].channel_index] -= exp(beta2*(1-trust));
							} */
							//Increment volunteer_channel_assignment_counter
							
							//volunteer_channel_assignment_counter[v][region_volunteer_list[i][v].channel_index]++;
							
							//Increment channel index
							
							if (v == 1)
							{
								cout <<"\n Region:"<<i;
								cout<<"\nCurrent channel index :"<<region_volunteer_list[i][v].channel_index;
								cout<<"\n Reputation:"<<region_volunteer_list[i][v].reputation[region_volunteer_list[i][v].channel_index];
								
							}
							region_volunteer_list[i][v].channel_index = region_volunteer_list[i][v].channel_index + 1;
							//cout<<"\nCurrent channel index :"<<region_volunteer_list[i][v].channel_index;
							//cout<<"\nno. of channels:"<<channel_list[i].size();
							if (region_volunteer_list[i][v].channel_index == channel_count_per_region)
							{
								region_volunteer_list[i][v].channel_index  = 0;
							}
							//cout<<"\nCurrent channel index after:"<<region_volunteer_list[i][v].channel_index;
							
						}	
					}
						
				}
			}
			
		}
		
		else if (no_of_MI >=1 && selected_volunteers.size()>0)
		{
			for (int i = 1; i <= region_list.size(); i++)
			{
				//cout<<"\nhere11\n";
				rand_val(iter*i);
				double sentinel_on = ((double)binomial(0.5, 100)/100.0);
				if(sentinel_on > 0.4) //sentinel will monitor
				{
					//cout<<"\nhere12\n";
					//cout<<"\nSentinel monitors region"<<i<<"\n";
					
					//Sentinel result
					/* int      n = 100;                   // Number of trials
					double   p = 0.5;                   // Probability of success
					int seed = clock1 + iter;
					rand_val(seed);
					int bin_sentinel = binomial(p, n); //sentinel result */
					
					//now look for selected volunteer results
					//cout<<"No. of volunteers selected:"<< volunteer_list.size();
					for (int v = 1; v <= volunteer_list.size(); v++)
					{
						//cout<<"\nhere13 vol loop\n";
				
						//only if volunteer is the region it is selected for, we see the readings of the volunteer
						
						/* cout<<"Volunteer x and y:"<<region_volunteer_list[i][v].x<<","<<region_volunteer_list[i][v].y<<"\n";
						cout<<"\nisInsideRegion(v,i)\n";
						cout<<isInsideRegion(v,i)<<"\n";
						cout<<"\nv:"<<v<<"\n";
						cout<<region_volunteer_list[i][v].selected;
						cout<<"\nselected or not\n"; */
						if (region_volunteer_list[i][v].selected == true && isInsideRegion(v,i) == true)
						{
							
							/* int seed_vol = (v+ iter + clock1)*i;
							rand_val(seed_vol);
							int bin_volunteer = binomial(p, n); //volunteer result */
							//cout<<"\ninside if\n";
														
							float v_TP;
							float v_FP;
							int accurate_flag = 0;
							
							//cout<<"\n***Volunteer "<<v<<"assigned channel "<<region_volunteer_list[i][v].channel_index<<"\n"; 
							float detection_acc = region_volunteer_list[i][v].bin_value * region_volunteer_list[i][v].detection_capability_map[region_volunteer_list[i][v].channel_index];
							
							if (detection_acc > 0.6)
							//if (region_volunteer_list[i][v].bin_value == 1) //True Positive
							{
								//cout<<"\ninside if2\n";
								TP_round[i] += 1.0;//For calculation of accuracy in this round
								channel_TP_round[region_volunteer_list[i][v].channel_index] += 1.0;
								region_volunteer_list[i][v].TP[region_volunteer_list[i][v].channel_index] += 1.0;	
								v_TP = region_volunteer_list[i][v].TP[region_volunteer_list[i][v].channel_index];
								accurate_flag = 1;
							}
							else 
							{
								//cout<<"\ninside if3\n";
								FP_round[i] += 1.0;//For calculation of accuracy in this round
								channel_FP_round[region_volunteer_list[i][v].channel_index] += 1.0;
								region_volunteer_list[i][v].FP[region_volunteer_list[i][v].channel_index] += 1.0;
								v_FP = region_volunteer_list[i][v].FP[region_volunteer_list[i][v].channel_index];
								accurate_flag = 0;
							}
							
							//Calculate Trust
							region_volunteer_list[i][v].trust[region_volunteer_list[i][v].channel_index] = v_TP/(v_TP + v_FP);
							float trust = region_volunteer_list[i][v].trust[region_volunteer_list[i][v].channel_index];
							
							//cout<<"\n here after calculating trust";
							//Increment volunteer_channel_assignment_counter
							
							//volunteer_channel_assignment_counter[v][region_volunteer_list[i][v].channel_index]++;
							
							//Calculate reputation
							if (accurate_flag == 1)
							{
								region_volunteer_list[i][v].reputation[region_volunteer_list[i][v].channel_index]+= beta1*trust;
							}
							else
							{
								if (region_volunteer_list[i][v].reputation[region_volunteer_list[i][v].channel_index] < rep_threshold)
									region_volunteer_list[i][v].reputation[region_volunteer_list[i][v].channel_index] -= exp(beta2*(1-trust)); //if reputation is less than a given threshold, decrease more
								else
									region_volunteer_list[i][v].reputation[region_volunteer_list[i][v].channel_index] -= exp(beta3*(1-trust)); 
							}
							
							//cout<<"\n here after calculating reputation";
							
						}
						//cout<<"\n inside volunteer for loop";
						
						
					}
					//cout<<"\n outside volunteer for loop";
				}
				//cout<<"\ninside sentinel decision";
			}
			//cout<<"\n outside region for loop";
		}
			
		//cout<<"\noutside the if statement";
		
		
	}
	
	
	/* printf("\n \n *****Binomial test:\n");
	std::random_device rd;
	std::mt19937 gen(rd());
	std::binomial_distribution<> d(100, 0.5);
	printf("== %d", d(gen)); */
	
	printf("\nNo. of iterations: %d\n", iter);
	/*
	printf("\n#### Test is Inside Polygon###\n");
	
	Point polygon1[] = {{0, 0}, {10, 0}, {10, 10}, {0, 10}}; 
    int n = sizeof(polygon1)/sizeof(polygon1[0]); 
    Point p = {10, 10}; 
    isInsidePolygon(polygon1, n, p)? printf("Yes \n"): printf( "No \n"); */
	
	//Print the hit ratio list 
	/* for (int i = 1; i <= region_list.size(); i++)
	{
		printf("\nRegion %d Hit Ratio list", i);
				
		for (int j = 0; j< hit_ratio_list[i].size() ; j++)
		{
			printf ("\nHit Ratio: %f \n", hit_ratio_list[i][j]);
		}
	} */
	
	for(int i = 1; i <=region_list.size(); i++)
	{
		cout<<"\nRegion "<<i<<"accuracy list\n";
		for (int j = 0; j< accuracy_list[i].size(); j++)
		{
			cout<<accuracy_list[i][j]<<",";
		}
	} 
	
	cout <<"\n=======================\n\n";
	//print channel accuracy lists
	
	for (int c = 0; c< channel_count_per_region; c++)
	{
		cout<<"\nChannel "<<c<<"accuracy list\n";
		for(int j = 0; j< channel_accuracy_list[c].size();j++)
		{
			cout<<channel_accuracy_list[c][j]<<",";
		}
	}
	
	cout <<"\n=======================\n\n";
	
	for(int i = 1; i <=region_list.size(); i++)
	{
		cout<<"\n\nRegion "<<i<<"hit ratio list\n";
		for (int j = 0; j<  hit_ratio_list[i].size(); j++)
		{
			cout<< hit_ratio_list[i][j]<<",";
		}
	}
	
	//print mean hit ratio list
	for(int i = 1; i <=region_list.size(); i++)
	{
		cout<<"\n\nRegion "<<i<<"OPTIMAL hit ratio list\n";
		for (int j = 0; j<  hit_ratio_list_optimal[i].size(); j++)
		{
			cout<< hit_ratio_list_optimal[i][j]<<",";
		}
	}
	
	//print mean hit ratio list overall
	for(int i = 1; i <=region_list.size(); i++)
	{
		cout<<"\n\nRegion "<<i<<"OPTIMAL OVERALL hit ratio list\n";
		for (int j = 0; j<  hit_ratio_list_optimal_overall[i].size(); j++)
		{
			cout<< hit_ratio_list_optimal_overall[i][j]<<",";
		}
	}
	
	//Calculate the mean hit ratio and its standard deviation
						
	for (int i = 1; i <= region_list.size(); i++)
	{
		printf("\n--------------- Region %d\n Hit Ratio ---------------", i);
		float sum = 0.0;
		for (int j = 0; j< hit_ratio_list[i].size(); j++)
		{
			sum += hit_ratio_list[i][j];	
		}
		float mean = sum/(float)hit_ratio_list[i].size();
		float standardDeviation = 0.0;
		for (int j = 0; j< hit_ratio_list[i].size(); j++)
		{
			standardDeviation += pow((hit_ratio_list[i][j] - mean), 2);
		}
		standardDeviation = sqrt(standardDeviation/(float)hit_ratio_list[i].size());
		printf("\nMean: %f\n", mean);
		printf("\nStandard Deviation: %f\n", standardDeviation);
	}
	
	
	//Calculate the mean happiness score and its standard deviation
	for (int i = 1; i <= region_list.size(); i++)
	{
		printf("\n--------------- Region %d\n Happiness Score ---------------", i);
		float sum = 0.0;
		for (int j = 0; j< happiness_score_list[i].size(); j++)
		{
			sum += (float)happiness_score_list[i][j];	
		}
		float mean = sum/(float)happiness_score_list[i].size();
		float standardDeviation = 0.0;
		for (int j = 0; j< happiness_score_list[i].size(); j++)
		{
			standardDeviation += pow((happiness_score_list[i][j] - mean), 2);
		}
		standardDeviation = sqrt(standardDeviation/(float)happiness_score_list[i].size());
		printf("\nMean: %f\n", mean);
		printf("\nStandard Deviation: %f\n", standardDeviation);
	}
	
	
	//Calculate the optimal mean hit ratio and its standard deviation
	for (int i = 1; i <= region_list.size(); i++)
	{
		printf("\n--------------- Region %d\n OPTIMAL Hit Ratio ---------------", i);
		float sum = 0.0;
		for (int j = 0; j< hit_ratio_list_optimal[i].size(); j++)
		{
			sum += hit_ratio_list_optimal[i][j];	
		}
		float mean = sum/(float)hit_ratio_list_optimal[i].size();
		float standardDeviation = 0.0;
		for (int j = 0; j< hit_ratio_list_optimal[i].size(); j++)
		{
			standardDeviation += pow((hit_ratio_list_optimal[i][j] - mean), 2);
		}
		standardDeviation = sqrt(standardDeviation/(float)hit_ratio_list_optimal[i].size());
		printf("\nMean: %f\n", mean);
		printf("\nStandard Deviation: %f\n", standardDeviation);
	}
	
	//Calculate the optimal mean hit ratio overall and its standard deviation
	for (int i = 1; i <= region_list.size(); i++)
	{
		printf("\n--------------- Region %d\n OPTIMAL OVERALL Hit Ratio ---------------", i);
		float sum = 0.0;
		for (int j = 0; j< hit_ratio_list_optimal_overall[i].size(); j++)
		{
			sum += hit_ratio_list_optimal_overall[i][j];	
		}
		float mean = sum/(float)hit_ratio_list_optimal_overall[i].size();
		float standardDeviation = 0.0;
		for (int j = 0; j< hit_ratio_list_optimal_overall[i].size(); j++)
		{
			standardDeviation += pow((hit_ratio_list_optimal_overall[i][j] - mean), 2);
		}
		standardDeviation = sqrt(standardDeviation/(float)hit_ratio_list_optimal_overall[i].size());
		printf("\nMean: %f\n", mean);
		printf("\nStandard Deviation: %f\n", standardDeviation);
	}
	
	//Calculate the mean trust, sensitivity, specificity, and standard deviation
	for (int i = 1; i <= region_list.size(); i++)
	{
		printf("\n--------------- Region %d\n Accuracy ---------------", i);
		float sum = 0.0;
		float sensitivity_sum = 0.0;
		float specificity_sum = 0.0;
		for (int j = 0; j< accuracy_list[i].size(); j++)
		{
			sum += accuracy_list[i][j];	
			//sensitivity_sum += sensitivity_list[i][j];
			//specificity_sum += specificity_list[i][j];
		}
		float mean = sum/(float)accuracy_list[i].size();
		//float sensitivity_mean = sensitivity_sum/(float)sensitivity_list[i].size();
		//float specificity_mean = specificity_sum/(float)specificity_list[i].size();
		
		float standardDeviation = 0.0;
		//float sensitivity_stdev = 0.0;
		//float specificity_stdev = 0.0;
		
		for (int j = 0; j< accuracy_list[i].size(); j++)
		{
			standardDeviation += pow(((accuracy_list[i][j]) - mean), 2);
			//sensitivity_stdev += pow(((sensitivity_list[i][j]) - sensitivity_mean), 2);
			//specificity_stdev += pow(((specificity_list[i][j]) - specificity_mean), 2);
		}
		standardDeviation = sqrt(standardDeviation/(float)accuracy_list[i].size());
		//sensitivity_stdev = sqrt(sensitivity_stdev/(float)sensitivity_list[i].size());
		//specificity_stdev = sqrt(specificity_stdev/(float)specificity_list[i].size()); 
		printf("\nMean Accuracy: %f\n", mean);
		printf("\nStandard Deviation: %f\n", standardDeviation);
		
		/* cout<<"\nMean Sensitivity:"<<sensitivity_mean<<endl;
		cout<<"\nSensitivity standard dev:"<<sensitivity_stdev<<endl;
				
		cout<<"\nMean Specificty: "<<specificity_mean<<endl;
		cout<<"\nSpecificty Stdev:"<<specificity_stdev<<endl; */
		
	}
	
	for (int i = 0; i < channel_count_per_region; i++)
	{
		printf("\n--------------- Channel %d\n Accuracy ---------------", i);
		float sum = 0.0;
		float sensitivity_sum = 0.0;
		float specificity_sum = 0.0;
		for (int j = 0; j< channel_accuracy_list[i].size(); j++)
		{
			sum += channel_accuracy_list[i][j];	
			//sensitivity_sum += sensitivity_list[i][j];
			//specificity_sum += specificity_list[i][j];
		}
		float mean = sum/(float)channel_accuracy_list[i].size();
		//float sensitivity_mean = sensitivity_sum/(float)sensitivity_list[i].size();
		//float specificity_mean = specificity_sum/(float)specificity_list[i].size();
		
		float standardDeviation = 0.0;
		//float sensitivity_stdev = 0.0;
		//float specificity_stdev = 0.0;
		
		for (int j = 0; j< channel_accuracy_list[i].size(); j++)
		{
			standardDeviation += pow(((channel_accuracy_list[i][j]) - mean), 2);
			//sensitivity_stdev += pow(((sensitivity_list[i][j]) - sensitivity_mean), 2);
			//specificity_stdev += pow(((specificity_list[i][j]) - specificity_mean), 2);
		}
		standardDeviation = sqrt(standardDeviation/(float)channel_accuracy_list[i].size());
		//sensitivity_stdev = sqrt(sensitivity_stdev/(float)sensitivity_list[i].size());
		//specificity_stdev = sqrt(specificity_stdev/(float)specificity_list[i].size()); 
		printf("\nMean Accuracy: %f\n", mean);
		printf("\nStandard Deviation: %f\n", standardDeviation);
		
		/* cout<<"\nMean Sensitivity:"<<sensitivity_mean<<endl;
		cout<<"\nSensitivity standard dev:"<<sensitivity_stdev<<endl;
				
		cout<<"\nMean Specificty: "<<specificity_mean<<endl;
		cout<<"\nSpecificty Stdev:"<<specificity_stdev<<endl; */
		
	}
	
	cout<<"\n\nTotal Number of volunteers::"<<total_volunteers<<endl;
	//if (cnt == 0)
	if (event_over() == true)
		init.set();
	
	/* printf ("\n\n Volunteer map:: \n\n");
	printf ("\n\n Volunteer map size :: %d\n\n", volunteer_list.size());
	for (int i = 1; i<= volunteer_list.size(); i++)
	{
		printf("\nVolunteer ID:%d",i);
		printf("(%d,%d)\n", volunteer_list[i].x, volunteer_list[i].y);
	}	 */
		
	
	report();			// model report
	//theory();
	mdlstat();			// model statistics
	
	for(int i = 1; i<=50; i++)
			printf("\n====================================================================================\n");
	
	
	
	/* std::time_t now = std::time(0);
	static boost::random::mt19937 gen{static_cast<std::uint32_t>(now)};
	static boost::random::bernoulli_distribution<> dist;
	std::cout << dist(gen) << '\n'; */
	
	/* mt19937 eng(time(NULL));
	int n = 100;
	int p = 0.5;
	cout<<"hello"<<endl;
	binomial_distribution<int> binomial(n, p);
	cout<<"hell0 2";
	for(int i=0; i < 10; i++)
	{
		cout<<"Hello there"<<i<<endl;
		
		cout << "[Binomial distribution]:" << binomial(eng) << endl;
	}
	cout << endl; */
	
	 /* std::default_random_engine generator;
	  std::binomial_distribution<int> distribution(9,0.5);

	  int p[10]={};
		printf("5\n");
	  for (int i=0; i<10; ++i) {
		int number = distribution(generator);
		std:: cout<<number<<std::endl;
		
	  } */
		
	
	printf("5\n");
	//halt
	
}
//######################################### SELECTION - ALGORITHMS ##########################################
//############## Stable Marriage ####################
struct vol_select_reject
{
	float qual;
	int select;
	int reject;
};
struct vol_channel_qual
{
	int id;
	float qual;
};
bool compareByQualification(const struct vol_channel_qual &a, const struct vol_channel_qual &b)
{
    return a.qual > b.qual;
}
bool compareBydetection_capability(const struct channel_vol_detection &a, const struct channel_vol_detection &b)
{
    return a.detection_capability> b.detection_capability;
}
struct volunteer_match_status
{
	int status;
	int region;
	int channel;
};
struct channel_score
{
	int channel_id;
	int score;
};

void Deferred_Acceptance_channelPropose(std:: map< int, std::map <int, float> > &A, int k)
{
	
	//Clear the application List map
	application_list.clear();
	
	selected_volunteers.clear();
	//Determine the number of volunteers for each channel
	int P;
	if(k < (channel_count_per_region * region_list.size()) && A[1].size()>=channel_count_per_region && A[2].size()>=channel_count_per_region)
	{
		P = 1;
	}
	else if (k >= (channel_count_per_region * region_list.size()) && A[1].size()>=channel_count_per_region && A[2].size()>=channel_count_per_region)
	{
		P = floor((float)k/(float)(channel_count_per_region * region_list.size()));
	}
	else
	{
		P = 0;
	}
	
	//Create channel preference list C_prime = Region -> channel -> List[(vol_id, qual)]
	std::map< int, std::map<int, vector <struct vol_channel_qual> > > C_prime; //region ->channel -> List[(vol_id, qual)]
	
		
	for (int region = 1; region <= region_list.size(); region++)
	{
		for (int channel = 0; channel < channel_count_per_region; channel++)
		{
			for (std::map<int,float>::iterator it=A[region].begin(); it != A[region].end(); ++it)
			{
				int v = it->first;
				
				struct vol_channel_qual vcq;
				vcq.id = v;
				
				//calculate the channel_specific qual
						
				float rep = region_volunteer_list[region][v].normal_reputation[channel];
				float soj = region_volunteer_list[region][v].normal_sojourn_avg;
				float tim = region_volunteer_list[region][v].normal_shortest_time;
				float pr = region_volunteer_list[region][v].residence_probability;
						
				vcq.qual = exp(rep)*pr*((0.75*(1-tim)) + (0.25 * soj));
				
				//Fill in Application_list
											
				//std::map< int, std::map< int, vector<int> > > application_list //region - > channel -> [list of volunteers]
				application_list[region][channel].push_back(v);
						
				C_prime[region][channel].push_back(vcq); //Filled C_prime
				
				
			}
		}
	}
	
	
	//Sort the entries of C_prime
	for (int region = 1; region <= region_list.size(); region++)
	{
		for (int channel = 0; channel < channel_count_per_region; channel++)
		{
			std::sort(C_prime[region][channel].begin(), C_prime[region][channel].end(), compareByQualification);
		}
	}
	
	//define volunteer_match list
	std::map< int, struct volunteer_match_status > volunteer_match_map; //volunteer_id -> {status, region, channel}
	
	//Initialize the volunteer_match list
	
	for (int region = 1; region <= region_list.size(); region++)
	{
		for (std::map<int,float>::iterator it=A[region].begin(); it != A[region].end(); ++it)
		{
			int v = it->first;
			struct volunteer_match_status vms;
			vms.status = 0;
			vms.region = -1;
			vms.channel = -1;
			volunteer_match_map[v] = vms;
		}
	}
	
	//Define channel match list region -> channel -> list[volunteer_id, qual]
	std::map< int, std::map< int,  vector <struct vol_channel_qual> > > channel_match_map;
	
	//Define volunteer preference list
	//Copy the channel detection capabilities of volunteers
	std::map< int, std::map<int, vector <struct channel_vol_detection> > > vol_det_cap_prime; //region->volunteer->List[channel_detection_capabilities]
	for (int region = 1; region <= region_list.size(); region++)
	{
		for (std::map<int,float>::iterator it=A[region].begin(); it != A[region].end(); ++it)
        {
			int v = it->first;
			vol_det_cap_prime[region][v] = region_volunteer_list[region][v].vol_det_cap;
		}
	}
	
	
	cout<<"\n&&&&&&&&&&&Volunteer Detection Capability list:%%%%%%%%%%%%%%%%%%%\n";
	for (int region = 1; region <= region_list.size(); region++)
	{
		cout<<"\nRegion: "<<region<<"\n";
		for (int i = 0; i < vol_det_cap_prime[region][10].size(); i++)
		{
			cout<<"\nChannel : "<<vol_det_cap_prime[region][10][i].id;
			cout<<"\nDetection Capability: "<<vol_det_cap_prime[region][10][i].detection_capability;
			cout<<"\n";
			
		}
	}
	
/* 	struct channel_score
	{
		int channel_id;
		int score;
	};
 */
 
 /*
	struct channel_vol_detection
	{
		int id;
		float detection_capability;
	};
*/
	//Define volunteer rank of channels list. volunteer-> region -> (channel, score)
	std::map< int, std::map< int, vector< struct channel_score > > > vol_channel_rank_list;
	for (int region = 1; region <= region_list.size(); region++)
    {
		
		for (std::map<int,float>::iterator it=A[region].begin(); it != A[region].end(); ++it)
		{
			int v = it->first;
			int region_flag = 0;
			if (isInsideRegion(v,region) == true)
			{
				region_flag = 1;
			}
			
			for (int ff = 0; ff < vol_det_cap_prime[region][v].size(); ff++)
			{
				struct channel_score cs;
				cs.channel_id = vol_det_cap_prime[region][v][ff].id;
				cs.score = ff + 1;
				if (region_flag != 1)
					cs.score += channel_count_per_region; //Increase the rank of channels in regions that v does not reside in
				
				vol_channel_rank_list[region][v].push_back(cs);
			}
		}
		
	}
	
	cout<<"\n&&&&&&&&&&&Volunteer Channel rank list:%%%%%%%%%%%%%%%%%%%\n";
	for (int region = 1; region <= region_list.size(); region++)
    {
		cout <<"\nRegion "<<region<<"\n";
		for (int ff = 0; ff < vol_det_cap_prime[region][10].size(); ff++)
		{
			cout<<"\nChannel:";
			cout<<vol_channel_rank_list[region][10][ff].channel_id;
			cout<<", Score: "<<vol_channel_rank_list[region][10][ff].score<<"\n";
		}
	}
	while(true)
	{
		int count = 0;
		for (int region = 1; region <= region_list.size(); region++)
		{
			
			for (int channel = 0; channel < channel_count_per_region; channel++)
			{
				if(channel_match_map[region][channel].size() < P && C_prime[region][channel].size() > 0) //If number of matches is less than P and size of preference list is greater than 0
				{
					
					
					int v = C_prime[region][channel][0].id;
					struct vol_channel_qual vcq;
					vcq.id = v;
					vcq.qual = C_prime[region][channel][0].qual;
					
					
					
					//Delete the first entry from C_prime
					C_prime[region][channel].erase(C_prime[region][channel].begin()+0);
					
					//check the match status of the chosen voluteer
					if(volunteer_match_map[v].status == 0)//if the volunteer has not been matched yet
					{
						//Fill the volunteer_match_map
						volunteer_match_map[v].status = 1;
						volunteer_match_map[v].channel = channel;
						volunteer_match_map[v].region = region;
						
						//Fill the channel_match_map
						channel_match_map[region][channel].push_back(vcq);
						
					}
					else if(volunteer_match_map[v].status == 1)//there is already a matched channel
					{
						int current_matched_channel = volunteer_match_map[v].channel;
						int current_matched_region = volunteer_match_map[v].region;
						
						//Find the rank of the currently matched channel
						int current_channel_rank;
						for(int rl = 0; rl < vol_channel_rank_list[current_matched_region][v].size(); rl++)
						{
							if(vol_channel_rank_list[current_matched_region][v][rl].channel_id == current_matched_channel)
							{
								current_channel_rank = vol_channel_rank_list[current_matched_region][v][rl].score;
							}
						}
						
						//Find rank of the new_incoming channel
						int new_channel_rank;
						for(int rl = 0; rl < vol_channel_rank_list[region][v].size(); rl++)
						{
							if(vol_channel_rank_list[region][v][rl].channel_id == channel)
							{
								new_channel_rank = vol_channel_rank_list[region][v][rl].score;
							}
						}
						if(new_channel_rank < current_channel_rank)
						{
							/* cout<<"\n\n %%%%%%%%%%%############$$$$$$$$ YES CHANGEEEEEEEEEEEEE!***********%%%%%%%%%%%"<<",Volunteer: "<<v<<", Region: "<<region<<", Current channel:"<<current_matched_channel<<", new channel: "<<channel<<"\n\n";
							
							cout<<"\n&&&&&&&&&&&Volunteer Channel rank list:%%%%%%%%%%%%%%%%%%%\n";
							for (int region1 = 1; region1 <= region_list.size(); region1++)
							{
								cout <<"\nRegion "<<region1<<"\n";
								for (int ff = 0; ff < vol_det_cap_prime[region1][v].size(); ff++)
								{
									cout<<"\nChannel:";
									cout<<vol_channel_rank_list[region1][v][ff].channel_id;
									cout<<", Score: "<<vol_channel_rank_list[region1][v][ff].score<<"\n";
								}
							} */
							
							//Change the volunteer_match_map
							volunteer_match_map[v].status = 1;
							volunteer_match_map[v].channel = channel;
							volunteer_match_map[v].region = region;
							
							//Fill the channel_match_map
							channel_match_map[region][channel].push_back(vcq);
							
							//Delete the entry from the previous matched channel
							for (int cml = 0; cml < channel_match_map[current_matched_region][current_matched_channel].size();cml++)
							{
								if (channel_match_map[current_matched_region][current_matched_channel][cml].id == v)
								{
									channel_match_map[current_matched_region][current_matched_channel].erase(channel_match_map[current_matched_region][current_matched_channel].begin() +  cml);
									
								}
							}
						}
						
						
					}
					
				}
			}
		}
		int flag = 0;
		for (int region = 1; region <= region_list.size(); region++)
		{
			for (int channel = 0; channel < channel_count_per_region; channel++)
			{
				if(channel_match_map[region][channel].size() < P && C_prime[region][channel].size() > 0)
				{
					flag = 1;
				}
			}
		}
		if(flag == 0)
			break;
			
		
		
		//Channel Match map
/* 		cout<<"\n#####################Channel Match Map####################\n";
		for (int region = 1; region <= region_list.size(); region++)
		{
			cout<<"\n&&&&& REGION: "<<region<<"\n";
			for (int channel = 0; channel < channel_count_per_region; channel++)
			{
				cout<<"\nChannel :"<<channel;
				for(int i = 0; i < channel_match_map[region][channel].size(); i++)
				{
					cout<<"\nVolunteer id: "<<channel_match_map[region][channel][i].id;
					cout<<", Volunteer Qual: "<<channel_match_map[region][channel][i].qual <<"\n";
				}
			}
			
		} */
		
		/* //Volunteer Match map
		//std::map< int, struct volunteer_match_status > volunteer_match_map;
		cout<<"\n#####################Volunteer Match Map####################\n";
		for (std::map< int, struct volunteer_match_status >::iterator it=volunteer_match_map.begin(); it != volunteer_match_map.end(); ++it)
		{
			int v = it->first;
			struct volunteer_match_status vms = it->second;
			cout<<"Volunteer: "<<v<<", status: "<<vms.status<<", channel: "<<vms.channel<<", Region: "<<vms.region<<"\n";
		} */
		
		
		
	}
	//Fill the list of selected volunteers
	for (int region = 1; region <= region_list.size(); region++)
	{
		//for (std::map<int,struct vol_channel_qual>::iterator it=C_prime[region].begin(); it != C_prime[region].end(); ++it)
		for (int channel = 0; channel < channel_count_per_region; channel++)
		{
			//int channel = it->first;

			for(int dc = 0; dc < channel_match_map[region][channel].size(); dc++)
			{
				int vol_id = channel_match_map[region][channel][dc].id;
				float vol_q = channel_match_map[region][channel][dc].qual;
				struct vol_qual vq;
				vq.id = vol_id;
				vq.qual = vol_q;
				selected_volunteers.push_back(vq);
				
				//setting selected flag
				region_volunteer_list[region][vol_id].selected = true;
				
				//setting channel_index
				region_volunteer_list[region][vol_id].channel_index = channel;
			}
			
		}
	}
	
	std::map< int, float > happiness_score; //
	
	//Happiness scores of volunteers
	for (int region = 1; region <= region_list.size(); region++)
	{
		//for (std::map<int,struct vol_channel_qual>::iterator it=C_prime[region].begin(); it != C_prime[region].end(); ++it)
		float region_score = 0.0;
		for (int channel = 0; channel < channel_count_per_region; channel++)
		{
			//int channel = it->first;

			for(int dc = 0; dc < channel_match_map[region][channel].size(); dc++)
			{
				int vol_id = channel_match_map[region][channel][dc].id;
				//Calculate the rank of the matched channel
				for (int ff = 0; ff < vol_channel_rank_list[region][vol_id].size(); ff++)
				{
					if(vol_channel_rank_list[region][vol_id][ff].channel_id == channel)
					{
						region_score = region_score + (float)vol_channel_rank_list[region][vol_id][ff].score;
					}
				}
				
				
			}
		}
		happiness_score_list[region].push_back(region_score);
		
	}
	
	
	
	
}

/* void Deferred_Acceptance2(std:: map< int, std::map <int, float> > &A, int k)
{
	
	//Clear the application List map
	application_list.clear();
	
	//Clear the selected_volunteers list
	selected_volunteers.clear();

	//Determine the number of volunteers for each channel
	int P;
	if(k < (channel_count_per_region * region_list.size()) && A[1].size()>=channel_count_per_region)
	{
		P = 1;
	}
	else if (k >= (channel_count_per_region * region_list.size()) && A[1].size()>=channel_count_per_region)
	{
		P = floor((float)k/(float)channel_count_per_region);
	}
	else
	{
		P = 0;
	}

	//Create the map A_prime for storing volunteer criterias 
	std::map< int, std::map<int, struct vol_select_reject> > A_prime; //region->volunteer_id -> (qual, select, reject)

	for (int region = 1; region <= region_list.size(); region++)
	{
		for (std::map<int,float>::iterator it=A[region].begin(); it != A[region].end(); ++it)
        {
        	struct vol_select_reject v_sc;
			v_sc.select = 0;
			v_sc.reject = 0;
			v_sc.qual = it->second;
			A_prime[region][it->first] = v_sc;
		}
	}

	//Create a map C_prime for the channels
	std::map< int, std::map<int, vector <struct vol_channel_qual> > > C_prime; //region ->channel -> List[(vol_id, qual)]

	//Copy the channel detection capabilities of volunteers
	std::map< int, std::map<int, vector <struct channel_vol_detection> > > vol_det_cap_prime; //region->volunteer->List[channel_detection_capabilities]
	for (int region = 1; region <= region_list.size(); region++)
	{
		for (std::map<int,float>::iterator it=A[region].begin(); it != A[region].end(); ++it)
        {
			int v = it->first;
			vol_det_cap_prime[region][v] = region_volunteer_list[region][v].vol_det_cap;
		}
	}		

	while(true) //break if count == 0
	{
		int count = 0;

		//prefer channels in the local region
	
		
		
		for (int region = 1; region <= region_list.size(); region++) //Iterate through all the regions
		{
			//prefer channels in the local region
			
			for (std::map<int,float>::iterator it=A[region].begin(); it != A[region].end(); ++it)
        	{
        		int v = it->first;
        		int flag = 0;
				
				//prefer channels in the local region
				if(isInsideRegion(v,region) == true)
				{
					//if there is still opportunity in this region
					if(vol_det_cap_prime[region][v].size() > 0 && A_prime[region][v].select == 0) //if v hasn't yet tried all channels in the region
					{
						flag = 1;
					}
					
					if (flag == 1) //if not yet selected and not rejected by all channels TODO
					{
						count ++;

						//Apply to top channel
						int channel = vol_det_cap_prime[region][v][0].id;
						float detection_capability = vol_det_cap_prime[region][v][0].detection_capability;
						
						//Insert to application list
						//std::map< int, std::map< int, vector<int> > > application_list //region - > channel -> [list of volunteers]
						application_list[region][channel].push_back(v);
									
						//Remove the channel from the list to prevent future applications to the same channel
						vol_det_cap_prime[region][v].erase(vol_det_cap_prime[region][v].begin()+0);
									
											
						
						
						
						
						//vcq.qual = A[region][v]; //Volunteer's qualification
						//vcq.qual = region_volunteer_list[region][v].reputation[channel];
						
						//Fill map C_prime
						struct vol_channel_qual vcq;
						vcq.id = v;
						
						//calculate the channel_specific qual
						
						float rep = region_volunteer_list[region][v].normal_reputation[channel];
						float soj = region_volunteer_list[region][v].normal_sojourn_avg;
						float tim = region_volunteer_list[region][v].normal_shortest_time;
						float pr = region_volunteer_list[region][v].residence_probability;
						
						vcq.qual = exp(rep)*pr*((0.75*(1-tim)) + (0.25 * soj));
						
						C_prime[region][channel].push_back(vcq); //Filled C_prime

					}
					else if (vol_det_cap_prime[region][v].size() == 0 && A_prime[region][v].select == 0)
					{
						//go through other regions
						for (int region1 = 1; region1 <= region_list.size() && region1 != region; region1++) //Iterate through all the regions
						{
							if(vol_det_cap_prime[region1][v].size() > 0 && A_prime[region1][v].select == 0)
							{
								count ++;

								//Apply to top channel
								int channel = vol_det_cap_prime[region1][v][0].id;
								float detection_capability = vol_det_cap_prime[region1][v][0].detection_capability;
								
								//Insert to application list
								//std::map< int, std::map< int, vector<int> > > application_list //region - > channel -> [list of volunteers]
								application_list[region1][channel].push_back(v);
											
								//Remove the channel from the list to prevent future applications to the same channel
								vol_det_cap_prime[region1][v].erase(vol_det_cap_prime[region1][v].begin()+0);
											
													
								
								
								
								
								//vcq.qual = A[region1][v]; //Volunteer's qualification
								//vcq.qual = region1_volunteer_list[region1][v].reputation[channel];
								
								//Fill map C_prime
								struct vol_channel_qual vcq;
								vcq.id = v;
								
								//calculate the channel_specific qual
								
								float rep = region_volunteer_list[region1][v].normal_reputation[channel];
								float soj = region_volunteer_list[region1][v].normal_sojourn_avg;
								float tim = region_volunteer_list[region1][v].normal_shortest_time;
								float pr = region_volunteer_list[region1][v].residence_probability;
								
								vcq.qual = exp(rep)*pr*((0.75*(1-tim)) + (0.25 * soj));
								
								C_prime[region1][channel].push_back(vcq); //Filled C_prime

							}
						}
					}

				}

        		

				
        	}

		}
		//For every channel in C_prime select the top P volunteers. Reject the rest
		for (int region = 1; region <= region_list.size(); region++)
		{
			//for (std::map<int, struct vol_channel_qual>::iterator it=C_prime[region].begin(); it != C_prime[region].end(); ++it)
			for (int channel = 0; channel < channel_count_per_region; channel++)
			{
				//int channel = it->first;
				if (C_prime[region][channel].size() > 0)
				{
					//sort the vector of all the volunteers
					std::sort(C_prime[region][channel].begin(), C_prime[region][channel].end(), compareByQualification);	//Descending sort 

					for(int dc = 0; dc < C_prime[region][channel].size(); dc++)
					{
						int vol_id = C_prime[region][channel][dc].id; //get the volunteer id
						if (dc < P) //Change the select flag of the selected volunteers
						{
							A_prime[region][vol_id].select = 1;
						}
						if(dc >=P)
						{
							//Start rejecting
							
							A_prime[region][vol_id].reject ++; //increase rejection count
							A_prime[region][vol_id].select = 0; //make select flag = 0
							
							//reject all volunteers after P
							//C_prime[region][channel].erase (C_prime[region][channel].begin()+dc);
						}
					}
				}
			}
		}
		
		//C_prime[region][channel].resize(P);
		
		for (int region = 1; region <= region_list.size(); region++)
		{
			//for (std::map<int, struct vol_channel_qual>::iterator it=C_prime[region].begin(); it != C_prime[region].end(); ++it)
			for (int channel = 0; channel < channel_count_per_region; channel++)
			{
				std::sort(C_prime[region][channel].begin(), C_prime[region][channel].end(), compareByQualification);
				//C_prime[region][channel].erase(C_prime[region][channel].begin()+P,C_prime[region][channel].end());
				C_prime[region][channel].resize(P);
			}
			
		}


		if(count == 0)
			break;

	}

	//Fill the list of selected volunteers
	for (int region = 1; region <= region_list.size(); region++)
	{
		//for (std::map<int,struct vol_channel_qual>::iterator it=C_prime[region].begin(); it != C_prime[region].end(); ++it)
		for (int channel = 0; channel < channel_count_per_region; channel++)
		{
			//int channel = it->first;

			for(int dc = 0; dc < C_prime[region][channel].size(); dc++)
			{
				//filling selected_volunteers list
				int vol_id = C_prime[region][channel][dc].id;
				float vol_q = C_prime[region][channel][dc].qual;
				struct vol_qual vq;
				vq.id = vol_id;
				vq.qual = vol_q;
				selected_volunteers.push_back(vq);
				
				//setting selected flag
				region_volunteer_list[region][vol_id].selected = true;
				
				//setting channel_index
				region_volunteer_list[region][vol_id].channel_index = channel;
				
			}			

		}
	}


} */

void Deferred_Acceptance2(std:: map< int, std::map <int, float> > &A, int k)
{
	
	//Clear the application List map
	application_list.clear();
	
	//Clear the selected_volunteers list
	selected_volunteers.clear();

	//Determine the number of volunteers for each channel
	int P;
	if(k < (channel_count_per_region * region_list.size()) && A[1].size()>=channel_count_per_region && A[2].size()>=channel_count_per_region )
	{
		P = 1;
	}
	else if (k >= (channel_count_per_region * region_list.size()) && A[1].size()>=channel_count_per_region && A[2].size()>=channel_count_per_region)
	{
		P = floor((float)k/((float)channel_count_per_region * region_list.size()));
	}
	else
	{
		P = 0;
	}
	cout<<"\n#### Vlaue of P:"<<P<<"\n\n";
	//Create the map A_prime for storing volunteer criterias 
	std::map< int, std::map<int, struct vol_select_reject> > A_prime; //region->volunteer_id -> (qual, select, reject)

	for (int region = 1; region <= region_list.size(); region++)
	{
		for (std::map<int,float>::iterator it=A[region].begin(); it != A[region].end(); ++it)
        {
        	struct vol_select_reject v_sc;
			v_sc.select = 0;
			v_sc.reject = 0;
			v_sc.qual = it->second;
			A_prime[region][it->first] = v_sc;
		}
	}

	//Create a map C_prime for the channels
	std::map< int, std::map<int, vector <struct vol_channel_qual> > > C_prime; //region ->channel -> List[(vol_id, qual)]

	//Copy the channel detection capabilities of volunteers
	std::map< int, std::map<int, vector <struct channel_vol_detection> > > vol_det_cap_prime; //region->volunteer->List[channel_detection_capabilities]
	for (int region = 1; region <= region_list.size(); region++)
	{
		for (std::map<int,float>::iterator it=A[region].begin(); it != A[region].end(); ++it)
        {
			int v = it->first;
			vol_det_cap_prime[region][v] = region_volunteer_list[region][v].vol_det_cap;
		}
	}	
	
	//Define volunteer rank of channels list. volunteer-> region -> (channel, score)
	std::map< int, std::map< int, vector< struct channel_score > > > vol_channel_rank_list;
	for (int region = 1; region <= region_list.size(); region++)
    {
		
		for (std::map<int,float>::iterator it=A[region].begin(); it != A[region].end(); ++it)
		{
			int v = it->first;
			int region_flag = 0;
			if (isInsideRegion(v,region) == true)
			{
				region_flag = 1;
			}
			
			for (int ff = 0; ff < vol_det_cap_prime[region][v].size(); ff++)
			{
				struct channel_score cs;
				cs.channel_id = vol_det_cap_prime[region][v][ff].id;
				cs.score = ff + 1;
				if (region_flag != 1)
					cs.score += channel_count_per_region; //Increase the rank of channels in regions that v does not reside in
				
				vol_channel_rank_list[region][v].push_back(cs);
			}
		}
		
	}

	while(true) //break if count == 0
	{
		int count = 0;

		//prefer channels in the local region
	
		
		
		for (int region = 1; region <= region_list.size(); region++) //Iterate through all the regions
		{
			//prefer channels in the local region
			
			for (std::map<int,float>::iterator it=A[region].begin(); it != A[region].end(); ++it)
        	{
        		int v = it->first;
        		int flag = 0;
				
				//prefer channels in the local region
				if(isInsideRegion(v,region) == true)
				{
					//cout<<"\n\n&&&&&&&&&&&Volunteer"<<v<<" ***Inside region***: "<<region<<"\n";
					//if there is still opportunity in this region
					if(vol_det_cap_prime[region][v].size() > 0 && A_prime[region][v].select == 0 ) //if v hasn't yet tried all channels in the region
					{
						flag = 1;
					}
					
					if (flag == 1) //if not yet selected and not rejected by all channels TODO
					{
						count ++;

						//Apply to top channel
						int channel = vol_det_cap_prime[region][v][0].id;
						float detection_capability = vol_det_cap_prime[region][v][0].detection_capability;
						
						//Insert to application list
						//std::map< int, std::map< int, vector<int> > > application_list //region - > channel -> [list of volunteers]
						application_list[region][channel].push_back(v);
						
						//cout<<"\n\n&&&&&&&&&&&Volunteer"<<v<<" got "<<region<<"\n";
									
						//Remove the channel from the list to prevent future applications to the same channel
						vol_det_cap_prime[region][v].erase(vol_det_cap_prime[region][v].begin()+0);
									
											
						
						
						
						
						//vcq.qual = A[region][v]; //Volunteer's qualification
						//vcq.qual = region_volunteer_list[region][v].reputation[channel];
						
						//Fill map C_prime
						struct vol_channel_qual vcq;
						vcq.id = v;
						
						//calculate the channel_specific qual
						
						float rep = region_volunteer_list[region][v].normal_reputation[channel];
						float soj = region_volunteer_list[region][v].normal_sojourn_avg;
						float tim = region_volunteer_list[region][v].normal_shortest_time;
						float pr = region_volunteer_list[region][v].residence_probability;
						
						vcq.qual = exp(rep)*pr*((0.75*(1-tim)) + (0.25 * soj));
						
						C_prime[region][channel].push_back(vcq); //Filled C_prime

					}
					else if (vol_det_cap_prime[region][v].size() == 0 && A_prime[region][v].select == 0 )
					{
						//go through other regions
						for (int region1 = 1; region1 <= region_list.size() && region1 != region; region1++) //Iterate through all the regions
						{
							if(vol_det_cap_prime[region1][v].size() > 0 && A_prime[region1][v].select == 0)
							{
								count ++;

								//Apply to top channel
								int channel = vol_det_cap_prime[region1][v][0].id;
								float detection_capability = vol_det_cap_prime[region1][v][0].detection_capability;
								
								//Insert to application list
								//std::map< int, std::map< int, vector<int> > > application_list //region - > channel -> [list of volunteers]
								application_list[region1][channel].push_back(v);
								//cout<<"\n\n&&&&&&&&&&&Volunteer"<<v<<" got "<<region1<<"\n";			
								//Remove the channel from the list to prevent future applications to the same channel
								vol_det_cap_prime[region1][v].erase(vol_det_cap_prime[region1][v].begin()+0);
											
													
								
								
								
								
								//vcq.qual = A[region1][v]; //Volunteer's qualification
								//vcq.qual = region1_volunteer_list[region1][v].reputation[channel];
								
								//Fill map C_prime
								struct vol_channel_qual vcq;
								vcq.id = v;
								
								//calculate the channel_specific qual
								
								float rep = region_volunteer_list[region1][v].normal_reputation[channel];
								float soj = region_volunteer_list[region1][v].normal_sojourn_avg;
								float tim = region_volunteer_list[region1][v].normal_shortest_time;
								float pr = region_volunteer_list[region1][v].residence_probability;
								
								vcq.qual = exp(rep)*pr*((0.75*(1-tim)) + (0.25 * soj));
								
								C_prime[region1][channel].push_back(vcq); //Filled C_prime

							}
						}
					}

				}

        		

				
        	}

		}
		//For every channel in C_prime select the top P volunteers. Reject the rest
		for (int region = 1; region <= region_list.size(); region++)
		{
			//for (std::map<int, struct vol_channel_qual>::iterator it=C_prime[region].begin(); it != C_prime[region].end(); ++it)
			for (int channel = 0; channel < channel_count_per_region; channel++)
			{
				//int channel = it->first;
				if (C_prime[region][channel].size() > 0)
				{
					//sort the vector of all the volunteers
					std::sort(C_prime[region][channel].begin(), C_prime[region][channel].end(), compareByQualification);	//Descending sort 

					for(int dc = 0; dc < C_prime[region][channel].size(); dc++)
					{
						int vol_id = C_prime[region][channel][dc].id; //get the volunteer id
						if (dc < P) //Change the select flag of the selected volunteers
						{
							A_prime[region][vol_id].select = 1;
						}
						if(dc >=P)
						{
							//Start rejecting
							
							A_prime[region][vol_id].reject ++; //increase rejection count
							A_prime[region][vol_id].select = 0; //make select flag = 0
							
							//reject all volunteers after P
							//C_prime[region][channel].erase (C_prime[region][channel].begin()+dc);
						}
					}
				}
			}
		}
		
		//C_prime[region][channel].resize(P);
		
		for (int region = 1; region <= region_list.size(); region++)
		{
			//for (std::map<int, struct vol_channel_qual>::iterator it=C_prime[region].begin(); it != C_prime[region].end(); ++it)
			for (int channel = 0; channel < channel_count_per_region; channel++)
			{
				std::sort(C_prime[region][channel].begin(), C_prime[region][channel].end(), compareByQualification);
				//C_prime[region][channel].erase(C_prime[region][channel].begin()+P,C_prime[region][channel].end());
				C_prime[region][channel].resize(P);
			}
			
		}


		if(count == 0)
			break;

	}

	std::map <int, int> count_per_region;
	//Fill the list of selected volunteers
	for (int region = 1; region <= region_list.size(); region++)
	{
		//for (std::map<int,struct vol_channel_qual>::iterator it=C_prime[region].begin(); it != C_prime[region].end(); ++it)
		for (int channel = 0; channel < channel_count_per_region; channel++)
		{
			//int channel = it->first;

			for(int dc = 0; dc < C_prime[region][channel].size(); dc++)
			{
				count_per_region[region]++;
				//filling selected_volunteers list
				int vol_id = C_prime[region][channel][dc].id;
				float vol_q = C_prime[region][channel][dc].qual;
				struct vol_qual vq;
				vq.id = vol_id;
				vq.qual = vol_q;
				selected_volunteers.push_back(vq);
				
				//setting selected flag
				region_volunteer_list[region][vol_id].selected = true;
				
				//setting channel_index
				region_volunteer_list[region][vol_id].channel_index = channel;
				
			}			

		}
	}
	
	cout<<"\n\n&&&& Region 1 COUNT: "<<count_per_region[1]<<", Region 2 COUNT: "<<count_per_region[2]<<"\n\n";
	
	//
	for (int region = 1; region <= region_list.size(); region++)
	{
		float region_score = 0.0;
		for (int channel = 0; channel < channel_count_per_region; channel++)
		{
			for(int dc = 0; dc < C_prime[region][channel].size(); dc++)
			{
				int vol_id = C_prime[region][channel][dc].id;
				//Calculate the rank of the matched channel
				for (int ff = 0; ff < vol_channel_rank_list[region][vol_id].size(); ff++)
				{
					if(vol_channel_rank_list[region][vol_id][ff].channel_id == channel)
					{
						region_score = region_score + (float)vol_channel_rank_list[region][vol_id][ff].score;
					}
				}
				
			}
			
		}
		
		happiness_score_list[region].push_back(region_score);
	}


}
void Deferred_Acceptance(std:: map< int, std::map <int, float> > &A, int k)
{
	selected_volunteers.clear();


	//Determine the number of volunteers for each channel
	int P;
	if(k < (channel_count_per_region * region_list.size()) && A[1].size()>=channel_count_per_region)
	{
		P = 1;
	}
	else if (k >= (channel_count_per_region * region_list.size()) && A[1].size()>=channel_count_per_region)
	{
		P = floor((float)k/(float)channel_count_per_region);
	}
	else
	{
		P = 0;
	}
	
	cout<<"****\n)))))\n&&&&& \nValue of P: "<<P;
	
	//Create the map A_prime for storing volunteer criterias 
	std::map< int, std::map<int, struct vol_select_reject> > A_prime; //region->volunteer_id -> (qual, select, reject)
	for (int region = 1; region <= region_list.size(); region++)
	{
		for (std::map<int,float>::iterator it=A[region].begin(); it != A[region].end(); ++it)
        {
			struct vol_select_reject v_sc;
			v_sc.select = 0;
			v_sc.reject = 0;
			v_sc.qual = it->second;
			A_prime[region][it->first] = v_sc;
		}
	}
	//Create a map C_prime for the channels
	std::map< int, std::map<int, vector <struct vol_channel_qual> > > C_prime; //region ->channel -> List[(vol_id, qual)]
	
	
	std::map< int, std::map<int, vector <struct channel_vol_detection> > > vol_det_cap_prime; //region->volunteer->List[channel_detection_capabilities]
	for (int region = 1; region <= region_list.size(); region++)
	{
		for (std::map<int,float>::iterator it=A[region].begin(); it != A[region].end(); ++it)
        {
			int v = it->first;
			vol_det_cap_prime[region][v] = region_volunteer_list[region][v].vol_det_cap;
		}
	}

	
	
	while(true) //break if count == 0
	{
		//cout<<"\nInside";
		int count = 0;
		//cout<<"\ncount_____________1 ="<<count;
		for (std::map<int,float>::iterator it=A[1].begin(); it != A[1].end(); ++it) //Take request of all volunteers
        {
			int v = it->first;
			//cout<<"\ncount ="<<count;
			int flag = 0;
			for (int region = 1; region <= region_list.size(); region++)
			{
				//if (A_prime[region][v].select == 0 && A_prime[region][v].reject < (channel_count_per_region * region_list.size()))//i.e. not yet not exhausted
				//cout<<"\n++++++++ Size: "<<vol_det_cap_prime[region][v].size();
				if(vol_det_cap_prime[region][v].size() > 0)
				{
					
					flag = 1;
				}
			}
			int region_list_empty = 0;
			if (flag == 1) //if not yet selected and not rejected by all channels
			{
				count ++;
				for (int region = 1; region <= region_list.size(); region++)
				{
					
					if(isInsideRegion(v,region) == true)//apply to local channels first
					{
						if(vol_det_cap_prime[region][v].size() > 0) //if there are still channels to apply
						{
							//Apply to top channel
							int channel = vol_det_cap_prime[region][v][0].id;
							float detection_capability = vol_det_cap_prime[region][v][0].detection_capability;
							
							//Remove the channel from the list to prevent future applications to the same channel
							vol_det_cap_prime[region][v].erase(vol_det_cap_prime[region][v].begin()+0);
							
							//Fill map C_prime
							struct vol_channel_qual vcq;
							vcq.id = v;
							vcq.qual = A[region][v]; //Volunteer's qualification
							C_prime[region][channel].push_back(vcq); //Filled C_prime
							break;
						}
						else
						{
							region_list_empty = 1;
						}
							
					}
					if (region_list_empty == 1)
					{
						for(int region1 = 1; region1 <= region_list.size(); region1++)
						{
							if(vol_det_cap_prime[region1][v].size() > 0) //if there are still channels to apply
							{
								//Apply to top channel
								int channel = vol_det_cap_prime[region1][v][0].id;
								float detection_capability = vol_det_cap_prime[region1][v][0].detection_capability;
								
								//Remove the channel from the list to prevent future applications to the same channel
								vol_det_cap_prime[region1][v].erase(vol_det_cap_prime[region1][v].begin()+0);
								
								//Fill map C_prime
								struct vol_channel_qual vcq;
								vcq.id = v;
								vcq.qual = A[region1][v]; //Volunteer's qualification
								C_prime[region1][channel].push_back(vcq); //Filled C_prime
								break;
							}
						}
					}
					
				}
				
			}
			
		}
		//For every channel in C_prime select the top P volunteers. Reject the rest
		for (int region = 1; region <= region_list.size(); region++)
		{
			for (int channel = 0; channel < channel_count_per_region; channel++)
			{
				if (C_prime[region][channel].size() > P)
				{
					//sort the vector of all the volunteers
					std::sort(C_prime[region][channel].begin(), C_prime[region][channel].end(), compareByQualification);	//Descending sort 
				
					
				}
				for(int dc = 0; dc < C_prime[region][channel].size(); dc++)
				{
					int vol_id = C_prime[region][channel][dc].id; //get the volunteer id
					if (dc < P) //Change the select flag of the selected volunteers
					{
						A_prime[region][vol_id].select = 1;
					}
					if(dc >=P)
					{
						//Start rejecting
						
						A_prime[region][vol_id].reject ++; //increase rejection count
						A_prime[region][vol_id].select = 0; //make select flag = 0
						
						//reject all volunteers after P
						C_prime[region][channel].erase (C_prime[region][channel].begin()+dc);
					}
				}
			}
		}
		if(count == 0)
			break;
	}
	
	//Fill the list of selected volunteers
	for (int region = 1; region <= region_list.size(); region++)
	{
		for (int channel = 0; channel < channel_count_per_region; channel++)
		{
			for(int dc = 0; dc < C_prime[region][channel].size(); dc++)
			{
				//filling selected_volunteers list
				int vol_id = C_prime[region][channel][dc].id;
				float vol_q = C_prime[region][channel][dc].qual;
				struct vol_qual vq;
				vq.id = vol_id;
				vq.qual = vol_q;
				selected_volunteers.push_back(vq);
				
				//setting selected flag
				region_volunteer_list[region][vol_id].selected = true;
				
				//setting channel_index
				region_volunteer_list[region][vol_id].channel_index = channel;
				
			}			
		}
	}
	
}




void StableMarriage(std:: map <int, float> &A, int k, int region)
{
	
	selected_volunteers.clear();
	std::map <int, bool> channel_booking_status;
	std::map <int, bool> volunteer_booking_status;
	
	std::map <int, int> channel_mapper;
	
	for(int i = 0; i < channel_count_per_region; i++)
	{
		channel_booking_status[i] = false;
	}
	for (std::map<int,float>::iterator it=A.begin(); it != A.end(); ++it)
    {
		volunteer_booking_status[it->first] = false;
	}
	int channel_count = channel_count_per_region;
	
	double no_of_iterations = floor((double)k/(double)channel_count_per_region);
	
	if (no_of_iterations == 0.0)
	{
		no_of_iterations = 1.0;
	}
	int channel_iter = 0;
	//while (no_of_iterations > 0.0)
	//{
		while (channel_count > 0)
		{
			for (channel_iter = 0; channel_iter < channel_count_per_region; channel_iter++)
			{
				if (channel_booking_status[channel_iter] == false)
				{
					//Book the 1st available and preferred volunteer
					for(int v = 0; v < region_channel_preference[region][channel_iter].size();v++)
					{
						int id = region_channel_preference[region][channel_iter][v].vol_id;
						//If preferred channel is free
						if (volunteer_booking_status[id] == false)
						{
							//assign this channel to the volunteer id
							region_volunteer_list[region][id].channel_index = channel_iter;
							
							//set channel booking status to true
							channel_booking_status[channel_iter] = true;
							
							//set volunteer booking status to true
							volunteer_booking_status[id] = true;
							
							channel_mapper[channel_iter] = id;
							
							//reduce channel count by 1
							channel_count--;
							break;
						}
						//If preferred channel is not free
						else if(volunteer_booking_status[id] == true)
						{
							int previous_channel = region_volunteer_list[region][id].channel_index;
							//check if the volunteer prefers this channel to the already assigned channel
							for (int cp = 0; cp < region_volunteer_list[region][id].channel_preference_vector.size(); cp++)
							{
								//if current channel appears before the already matched channel
								if (region_volunteer_list[region][id].channel_preference_vector[cp] == channel_iter)
								{
									
									
									//set previous channel booking status to false
									channel_booking_status[previous_channel] = false;
									
									//assign this channel to the volunteer id
									region_volunteer_list[region][id].channel_index = channel_iter;
									
									//set channel booking status to true
									channel_booking_status[channel_iter] = true;
									
									//set volunteer booking status to true
									volunteer_booking_status[id] = true;
									
									channel_mapper[channel_iter] = id;
									
									break;
									
								}
								else if (region_volunteer_list[region][id].channel_preference_vector[cp] == previous_channel)
								{
									break;
								}
							}
							
						}
						if (channel_booking_status[channel_iter] == true)
						{
							break;
						}
					}
				}
			}
			
			
			//recount channel_count
			int count = 0;
			for(int z = 0; z < channel_booking_status.size(); z++)
			{
				if (channel_booking_status[z] == false)
				{
					count++;
				}
			}
			channel_count = count;
		}
		
		no_of_iterations--;
		struct vol_qual vq;
		for (int z = 0; z < channel_count_per_region; z++)
		{
			cout<<"Channel : "<<z<<", Volunteer: "<<channel_mapper[z]<<"\n";
			int id = channel_mapper[z];
			float qualification = A[id];
			
			vq.id = id;
			vq.qual = qualification;
			selected_volunteers.push_back(vq);
			
			
			//Set the selection values to true in the volunteer_region_list
			region_volunteer_list[region][id].selected = true;
			
			//remove the selected volunteers from the all the preference lists
			
			
		}
		
	//}
	
	//cout<<"\n %%%%%$$$$$$$$$$$$$$$$$$$$$$$ MATCHES MATCHES MATCHES ##############################%%%%%%%%%%%%\n";
	//cout<<"\n %%%%%%%%%%%%%%%%%%%%%%%%%%%%% Stable Marriage Problem %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%############\n";
	//print the matches 
	//Fill the selected_volunteers list
	//struct vol_qual vq;
	
	for (int z = 0; z < channel_count_per_region; z++)
	{
		cout<<"Channel : "<<z<<", Volunteer: "<<channel_mapper[z]<<"\n";
		int id = channel_mapper[z];
		float qualification = A[id];
		
		vq.id = id;
		vq.qual = qualification;
		selected_volunteers.push_back(vq);
		
		
		//Set the selection values to true in the volunteer_region_list
		region_volunteer_list[region][id].selected = true;
		
		
	}
	//m2.insert(m1.begin(), m1.end());
	
}



struct mapToVector
{
	int id;
	float qualification;
};
bool compareBymapToVector(const struct mapToVector &a, const struct mapToVector &b)
{
    return a.qualification > b.qualification;
}

void StableMarriage2(std:: map <int, float> &A, int k, int region)
{
	selected_volunteers.clear();
	float no_of_iterations = floor((float)k/(float)channel_count_per_region);
	
	//Copy contents of map A to a temporary map B
	std::map <int, float> B;
	for (std::map<int,float>::iterator it=A.begin(); it != A.end(); ++it)
	{
		B[it->first] = it->second;
	}
	
	//sort B based on the qualification value
	//hard to sort a map by value because maps are sorted by keys
	
	//store the contents of B in a vector
	std::vector <struct mapToVector> B_temp;
	
	for (std::map<int,float>::iterator it=B.begin(); it != B.end(); ++it)
	{
		struct mapToVector mv;
		mv.id = it->first;
		mv.qualification = it->second;
		
		B_temp.push_back(mv);
	}
	//sort the vector B_temp in descending order of qualification
	std::sort(B_temp.begin(), B_temp.end(), compareBymapToVector);
	
	
	
		
	if (no_of_iterations == 0.0)
		no_of_iterations = 1.0;
	
	while (no_of_iterations > 0.0)
	{
		//get top ||C||volunteers and store it in temp based on their qualification
		std::map <int, float> temp;
		int it_count = 0;
		//cout<<"\n Values of B_temp"<<"\n";
		for (int i = 0; i < B_temp.size(); i++)
		{
			//cout<<"\nID:"<<B_temp[i].id<<", qual:"<<B_temp[i].qualification;
			if (it_count < channel_count_per_region && B_temp[i].id!= 0)
			{
				temp[B_temp[i].id] = B_temp[i].qualification;
				//Remove the entry from the vector
				B_temp.erase(B_temp.begin() + i);
			}
			
			it_count ++;
		}
		/* for (std::map<int,float>::iterator it = B.begin(); it != B.end(); ++it)
		{
			if (it_count < channel_count_per_region)
			{
				temp[it->first] = it->second;
				//Remove these entries from B
				B.erase(it->first);
			}
			it_count ++;
		} */
		//Set booking status of the volunteers in temp and of all the channels
		std::map <int, bool> channel_booking_status;
		std::map <int, bool> volunteer_booking_status;
		
		for(int i = 0; i < channel_count_per_region; i++)
		{
			channel_booking_status[i] = false;
		}
		for (std::map<int,float>::iterator it=temp.begin(); it != temp.end(); ++it)
		{
			volunteer_booking_status[it->first] = false;
		}
		
		
		//Delete the current set of channel preferences
		region_channel_preference.clear();
		
		
		//Set priority list of the channels
		//cout<<"\nContents of temp:\n";
		for (std::map<int,float>::iterator it=temp.begin(); it != temp.end(); ++it)
		{
			//cout<<"Key==>> "<<it->first<<", Value==>> "<<it->second<<"\n";
		}
		for (int c = 0; c < channel_count_per_region; c++)
		{
			for (std::map<int,float>::iterator it=temp.begin(); it != temp.end(); ++it)
			{
				int v = it->first;
				//***** CALCULATING QUALIFICATION*************
				
				/* cout<<"\nValue of v:"<<v;
				cout<<"\nValue of c:"<<c;
				cout<<"\nValue of region:"<<region<<"\n"; */
				
				
				
				//float qualification2 = (weight2* region_volunteer_list[region][v].normal_reputation[c]) + (weight1*region_volunteer_list[region][v].normal_pravg);
				float qualification2 = region_volunteer_list[region][v].normal_reputation[c];
				//float qualification2 = region_volunteer_list[region][v].normal_reputation[c];
				
				struct vol_ID_Qual vIDq;
				vIDq.vol_id = v;
				vIDq.qualification = qualification2;
						
				region_channel_preference[region][c].push_back(vIDq);
			}
		}
		//Sort the priority list of channels
		// sort the preferences
		
		for (int c = 0; c< channel_count_per_region; c++)
		{
			std::sort(region_channel_preference[region][c].begin(), region_channel_preference[region][c].end(), compareVolunteer_Qual);
		}
		
		//Print the channel preference list
		//cout <<"\n\n%%%%%%%%% CHANNEL PREFERENCE LIST %%%%%%%%%%%%%\n";
			
		
		for (int c = 0; c< channel_count_per_region; c++)
		{
			//cout <<"Channel "<<c<<"==\n";
			for(int v = 0; v < region_channel_preference[region][c].size();v++)
			{
				//cout<<"Volunteer "<<v<<" -> ID: "<<region_channel_preference[region][c][v].vol_id<<", Qualification: "<<region_channel_preference[region][c][v].qualification<<"\n\n";
			}
		}
		
		
		//Now match the channels to the volunteers
		int channel_count = channel_count_per_region;
		int channel_iter = 0;
		
		//Stores the channel matches
		std::map <int, int> channel_mapper; //channel_id -> volunteer_id
		while(channel_count > 0 && temp.size()==channel_count_per_region)
		{
			int size = temp.size();
			int difference = abs(channel_count_per_region - size);
			
			
			//cout<<"\ninner1\n";
			for (channel_iter = 0; channel_iter < channel_count_per_region; channel_iter++)
			{
				//cout<<"\ninner2\n";
				if (channel_booking_status[channel_iter] == false)
				{
					//Book the 1st available and preferred volunteer
					for(int v = 0; v < region_channel_preference[region][channel_iter].size();v++)
					{
						//cout<<"\ninner3\n";
						int id = region_channel_preference[region][channel_iter][v].vol_id;
						//If preferred channel is free
						if (volunteer_booking_status[id] == false)
						{
							//assign this channel to the volunteer id
							region_volunteer_list[region][id].channel_index = channel_iter;
							
							//set channel booking status to true
							channel_booking_status[channel_iter] = true;
							
							//set volunteer booking status to true
							volunteer_booking_status[id] = true;
							
							channel_mapper[channel_iter] = id;
							
							//reduce channel count by 1
							channel_count--;
							break;
						}
						//If preferred channel is not free
						else if(volunteer_booking_status[id] == true)
						{
							int previous_channel = region_volunteer_list[region][id].channel_index;
							//check if the volunteer prefers this channel to the already assigned channel
							for (int cp = 0; cp < region_volunteer_list[region][id].channel_preference_vector.size(); cp++)
							{
								//cout<<"\ninner4\n";
								//if current channel appears before the already matched channel
								if (region_volunteer_list[region][id].channel_preference_vector[cp] == channel_iter)
								{
									
									
									//set previous channel booking status to false
									channel_booking_status[previous_channel] = false;
									
									//assign this channel to the volunteer id
									region_volunteer_list[region][id].channel_index = channel_iter;
									
									//set channel booking status to true
									channel_booking_status[channel_iter] = true;
									
									//set volunteer booking status to true
									volunteer_booking_status[id] = true;
									
									channel_mapper[channel_iter] = id;
									
									break;
									
								}
								else if (region_volunteer_list[region][id].channel_preference_vector[cp] == previous_channel)
								{
									break;
								}
							}
							
						}
						if (channel_booking_status[channel_iter] == true)
						{
							break;
						}
					}
				}
			}
			
			
			//recount channel_count
			int count = 0;
			int size2 = channel_booking_status.size();
			for(int z = 0; z < channel_booking_status.size(); z++)
			{
				if (channel_booking_status[z] == false)
				{
					count++;
				}
			}
			channel_count = count;
			count = 0; 
			for(int z = 0; z < volunteer_booking_status.size(); z++)
			{
				if (volunteer_booking_status[z] == false)
				{
					count++;
				}
			}
			if (count == 0)
				channel_count = 0;
			
		}
		
		//Add the selected volunteers to the list of selected volunteers
		//cout<<"\n %%%%%$$$$$$$$$$$$$$$$$$$$$$$ MATCHES MATCHES MATCHES ##############################%%%%%%%%%%%%\n";
		//cout<<"\n %%%%%%%%%%%%%%%%%%%%%%%%%%%%% Stable Marriage Problem %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%############\n";
		
		//cout<<"\n *********** ROUND"<< no_of_iterations<<"*****************\n";
		
		struct vol_qual vq;
	
		for (int z = 0; z < channel_count_per_region; z++)
		{
			//cout<<"Channel : "<<z<<", Volunteer: "<<channel_mapper[z]<<"\n";
			int id = channel_mapper[z];
			float qualification = A[id];
			
			vq.id = id;
			vq.qual = qualification;
			selected_volunteers.push_back(vq);
			
			
			//Set the selection values to true in the volunteer_region_list
			region_volunteer_list[region][id].selected = true;
			
			
		}
		for (int i = 0; i< selected_volunteers.size();i++)
    	{
    		int id = selected_volunteers[i].id;
    		float qual = selected_volunteers[i].qual;
    		VS_regionList[region][id] = qual; 
    		
    		//Set the selection values to true in the volunteer_region_list
    		region_volunteer_list[region][id].selected = true;
    	}
		no_of_iterations -= 1.0;	
		
	}
}

//################################################ ------------ Secretary Algorithm ------------ #################################
void classic_secretary(std:: map <int, float> &A, int k, int region)
{
    float threshold;
    struct vol_qual v;
    for (std::map<int,float>::iterator it=A.begin(); it != A.end(); ++it)
    {
        v.id = it->first;
        v.qual = it->second;
        threshold = it->second;
        break;
    }
    
    
    int n = A.size();
    
    
    int breaker = 0;
    for (std::map<int,float>::iterator it=A.begin(); it != A.end(); ++it)
    {
        if(breaker>= floor(A.size()/2.7))
            break;
        if (it->second > threshold)
        {
            v.id = it->first;
            v.qual = it->second;
            threshold = it->second;
            cout<<"\n New Threshold: "<<threshold;
        }
        
        breaker++;
    }
    cout<<"\nSelected threhold:"<<threshold<<endl;
    int j =0;
    for (std::map<int,float>::iterator it=A.begin(); it != A.end(); ++it)
    {
        if (j>=breaker)
        {

            if (it->second >threshold)
            {
                v.id = it->first;
                v.qual = it->second;
            }
        }
        j++;
    }
    
    selected_volunteers.push_back(v);
    
    
}
bool compareByQual(const struct vol_qual &a, const struct vol_qual &b)
{
    return a.qual > b.qual;
}
void Secretary(std:: map <int, float> &A, int k, int region)
{
	selected_volunteers.clear();
	float t1 = clock1;
	//srand(t1);
	rand_val(t1);
	
	int n = A.size();
	
	int m = binomial(0.5, n);
	//int m = 1 + rand() % (n - 1);
	int l = 0;
	if (m> floor((float)k/2))
	{
		l = floor((float)k/2);
	}
	else
		l = m;
	
	std::vector <struct vol_qual> B;
	
	//Copy the qualification values of top m volunteers for the given region in a temporary list (vector) B
	struct vol_qual vq;
	for (int i = 1; i <= m; i++)
	{
		vq.id = i;
		vq.qual = A[i];
		B.push_back(vq);
	}
	
	std::sort(B.begin(), B.end(), compareByQual);	//Descending sort B
	
	/* printf("\n Descending Sort\n");
	for (int i = 0; i< B.size(); i++)
        printf("\nID: %d, Qualification: %f:", B[i].id, B[i].qual); */
	
	//Select 1st l volunteers
	for(int i = 0; i<l; i++)
	{
		vq.id = B[i].id;
		vq.qual = B[i].qual;
		selected_volunteers.push_back(vq);
	}
		
	float threshold = B[l].qual;	//Set threshold
	
	//Select the rest of the volunteers
	
	for (int i = m+1; i<= A.size(); i++)
	{
		if (A[i] >threshold && selected_volunteers.size() < k)
		{
			vq.id = i;
			vq.qual = A[i];
			selected_volunteers.push_back(vq);
		}
		
	}
	
	//now push the selected_volunteers list to a region-specific list of selected volunteers
	
	
	
	for (int i = 0; i< selected_volunteers.size();i++)
	{
		int id = selected_volunteers[i].id;
		float qual = selected_volunteers[i].qual;
		VS_regionList[region][id] = qual; 
		
		//Set the selection values to true in the volunteer_region_list
		region_volunteer_list[region][id].selected = true;
		
		//VS_regionList[region].push_back(vq);	
	}
	
	//Set the selection values to true in the volunteer_region_list
	
	
	//B.insert(A.begin(), A.end());
	//Sort B in descending order by value
		
}	

void Secretary2(std:: map <int, float> &A, int k, int region)
{
	//selected_volunteers.clear();
	
	//cout<<"\nValue of k:"<<k<<endl;
	
	if (k == 1)
	{
	    classic_secretary(A, k, region);
	}
	else if (k > 0)
	{
    	//srand(clock1);
    	//float t1 = 1 + rand() % (20- 1);
    	//srand(t1);
		float t1 = clock1;
    	rand_val(t1);
    	
    	int n = A.size();
    	
    	int m = binomial(0.5, n);
		
    	//cout<<"\n Value of n:"<<n<<endl;
    	//cout<<"\n Value of m:"<<m<<endl;
    	
    	int l = floor((float)k/2);
    	if (m>= floor((float)k/2))
    	{
    		l = floor((float)k/2);
    	}
    	else
    		l = m;
    		
    	std::map<int, float> temp_A;
		
		//change 1
		int A_pos = 0;
    	for (std::map<int,float>::iterator it=A.begin(); it != A.end(); ++it)
        {
            if(A_pos <m)
            {
                temp_A[it->first] = it->second;
            }
            else if (A_pos >= m)
                break;
            A_pos ++;
        }		
		//change 1
		
		/*
    	for (int i = 1; i<=m; i++)
    	{
    	    temp_A[i] = A[i];
    	}
		*/
		
    	Secretary2(temp_A, l, region);
    	std::vector <struct vol_qual> B;
    	
    	//Copy the qualification values of top m volunteers for the given region in a temporary list (vector) B
    	struct vol_qual vq;
		
		//change 2
    	
    	A_pos = 0;
    	for (std::map<int,float>::iterator it=A.begin(); it != A.end(); ++it)
        {
            if(A_pos <m)
            {
                vq.id = it->first;
    		    vq.qual = it->second;
    		    B.push_back(vq);
            }
            else if (A_pos >= m)
                break;
            A_pos ++;
        }
        
        //change 2
		
		/*
    	for (int i = 1; i <= m; i++)
    	{
    		vq.id = i;
    		vq.qual = A[i];
    		B.push_back(vq);
    	}
    	*/
		
    	std::sort(B.begin(), B.end(), compareByQual);	//Descending sort B
    	
    	/*printf("\n Descending Sort\n");
    	for (int i = 0; i< B.size(); i++)
            printf("\nID: %d, Qualification: %f:", B[i].id, B[i].qual); 
    	*/
    	//Select 1st l volunteers
    	/*for(int i = 0; i<l; i++)
    	{
    		vq.id = B[i].id;
    		vq.qual = B[i].qual;
    		selected_volunteers.push_back(vq);
    	}*/
    		
    	float threshold = B[l].qual;	//Set threshold
    	
    	//cout<<"\nthreshold for k = "<<k<<":"<<threshold<<endl;
    	
    	//Select the rest of the volunteers
		
		//change 3
    	
    	A_pos = 0;
    	for (std::map<int,float>::iterator it=A.begin(); it != A.end(); ++it)
        {
            if(A_pos >=m)
            {
                if (it->second >= threshold && selected_volunteers.size() < k)
    		    {
    		        vq.id = it->first;
    		        vq.qual = it->second;
    		        selected_volunteers.push_back(vq);
    		    }
            }
            A_pos++;
        }
    	
    	//change 3
    	
		/*
    	for (int i = m+1; i<= A.size(); i++)
    	{
    		if (A[i] >= threshold && selected_volunteers.size() < k)
    		{
    			vq.id = i;
    			vq.qual = A[i];
    			selected_volunteers.push_back(vq);
    		}
    		
    	}
    	*/
    	//now push the selected_volunteers list to a region-specific list of selected volunteers
    	
		
    	
    	
    	for (int i = 0; i< selected_volunteers.size();i++)
    	{
    		int id = selected_volunteers[i].id;
    		float qual = selected_volunteers[i].qual;
    		VS_regionList[region][id] = qual; 
    		
    		//Set the selection values to true in the volunteer_region_list
    		region_volunteer_list[region][id].selected = true;
			
			//Assign a channel to the volunteer
			/* region_volunteer_list[region][id].channel_index = channel_counter_region[region];
			channel_counter_region[region] ++;
			if (channel_counter_region[region] == channel_list[region].size())
			{
				channel_counter_region[region] = 0;
			} */
    			
    	}
    	
	}	
	
	//Put the volunteers in the channel-volunteer list == channel --> <volunteer qualification list>
	
	
	/* cout<<"\n*********Selected Volunteers:\n";
	for(int i = 0; i <selected_volunteers.size() ; i++)
    {
		cout<<"V iD: "<<selected_volunteers[i].id;
        cout<<"Qualification:"<<selected_volunteers[i].qual<<",";
		
		cout<<"\n";
    } */
    //cout <<"\n";
}	

bool compareByVolRep(const struct volunteer_reputation &a, const struct volunteer_reputation &b)
{
    return a.reputation > b.reputation;
}

void assign_channels(int region, int k, std::map< int, float > sum_happiness)
{
	
	//store the values in channel --> volunteer map
	cout<<"\n Assigning channels: \n";
	for (int c = 0; c < channel_count_per_region; c++)
	{
		vector<struct volunteer_reputation> vec;
		for (int j = 0; j< selected_volunteers.size(); j++)
		{
			struct volunteer_reputation vr;
			vr.id = selected_volunteers[j].id;
			//cout<<"\nSelected volunteer's reputation:"<<region_volunteer_list[region][selected_volunteers[j].id].reputation[c]<<"\n";
			vr.reputation = region_volunteer_list[region][selected_volunteers[j].id].reputation[c];
			vec.push_back(vr);
			//cout<<"\nVector pushed";
		}
		channel_volunteer_list[c] = vec;
		
	}
	
	
	
	//Sort the vectors in descending order
	for (int c = 0; c < channel_volunteer_list.size(); c++)
	{
		std::sort(channel_volunteer_list[c].begin(), channel_volunteer_list[c].end(), compareByVolRep);
	}
	
	//Assign the channels in round robin manner
	
	vector <int> assigned_ids;
	int assigned_counter = 0;
	int channel_counter = 0;
	while(assigned_counter <selected_volunteers.size())
	{
		int id;
		/* cout<<"\n assigned counter:"<<assigned_counter<<"\n";
		cout<<"\nchannel counter:"<<channel_counter<<"\n";
		cout<<"\nChannel volunteer list size:"<<channel_volunteer_list[channel_counter].size(); */
		
		//print channel volunteer list
		/* for (int c = 0; c< channel_volunteer_list.size(); c++)
		{
			cout<<"\nChannel:"<<c<<":";
			for (int i = 0; i< channel_volunteer_list[c].size(); i++)
			{
				cout<<"volunteeer id : "<<channel_volunteer_list[c][i].id<<" reputation: ";
				
				cout<<channel_volunteer_list[c][i].reputation<<",";
				
				cout<<"\n";
			}
		}
		 */
		
		//erase the selected one from the vector
		if(channel_volunteer_list[channel_counter].size()>0)
		{
			id = channel_volunteer_list[channel_counter][0].id;
			region_volunteer_list[region][id].channel_index = channel_counter;
			/* if (region == 1)
			{
				cout<<"\n\n Volunteer "<<id<<" GETS channel "<<region_volunteer_list[region][id].channel_index<<"\n\n";
			} */
			
			channel_volunteer_list[channel_counter].erase (channel_volunteer_list[channel_counter].begin()+0);
			//cout<<"\ninside access here 2\n";
			for (int c = 0; c < channel_count_per_region; c++)
			{
				for (int j = 0; j < channel_volunteer_list[c].size(); j++)
				{
					if (channel_volunteer_list[c][j].id == id)
					{
						channel_volunteer_list[c].erase(channel_volunteer_list[c].begin()+j);
					}
				}
			}
			//just sort them in descending order again, for safety.
			for (int c = 0; c < channel_volunteer_list.size(); c++)
			{
				std::sort(channel_volunteer_list[c].begin(), channel_volunteer_list[c].end(), compareByVolRep);
			}
			assigned_counter++;
		}
		
		//erase the selected one from the vectors of other regions
		
		
		
		channel_counter++;
		
		if (channel_counter == channel_count_per_region)
		{
			channel_counter = 0;
		}
		
		
	}
	
	
	//##############################
	//Calculate Happiness Score
	//##############################
	
	//Define volunteer preference list
	//Copy the channel detection capabilities of volunteers
	std::map< int, std::map<int, vector <struct channel_vol_detection> > > vol_det_cap_prime; //region->volunteer->List[channel_detection_capabilities]
	for (int rr = 1; rr <= region_list.size(); rr++)
	{
		for (int j = 0; j< selected_volunteers.size(); j++)
		{
			int v = selected_volunteers[j].id;
			vol_det_cap_prime[rr][v] = region_volunteer_list[rr][v].vol_det_cap;
		}
	}
	
	//Define volunteer rank of channels list. volunteer-> region -> (channel, score)
	std::map< int, std::map< int, vector< struct channel_score > > > vol_channel_rank_list;
	for (int rr = 1; rr <= region_list.size(); rr++)
    {
		
		for (int j = 0; j< selected_volunteers.size(); j++)
		{
			int v = selected_volunteers[j].id;
			int region_flag = 0;
			if (isInsideRegion(v,rr) == true)
			{
				region_flag = 1;
			}
			
			for (int ff = 0; ff < vol_det_cap_prime[rr][v].size(); ff++)
			{
				struct channel_score cs;
				cs.channel_id = vol_det_cap_prime[rr][v][ff].id;
				cs.score = ff + 1;
				if (region_flag != 1)
					cs.score += channel_count_per_region; //Increase the rank of channels in regions that v does not reside in
				
				vol_channel_rank_list[rr][v].push_back(cs);
			}
		}
		
	}
	
	//Calculate happiness score
	for (int j = 0; j< selected_volunteers.size(); j++)
	{
		int vol_id = selected_volunteers[j].id;
		int channel = region_volunteer_list[region][vol_id].channel_index;
			for (int ff = 0; ff < vol_channel_rank_list[region][vol_id].size(); ff++)
			{
				if(vol_channel_rank_list[region][vol_id][ff].channel_id == channel)
				{
					sum_happiness[region] += (float)vol_channel_rank_list[region][vol_id][ff].score;
				}
			}
		
		
	}
	//Push to happiness score list
	happiness_score_list[region].push_back(sum_happiness[region]);
	
	
	
	
	
	
}

void random_algo(std:: map <int, float> &A, int k, int region) //Selected random k volunteers
{
	float t1 = clock1+(float)region;
	//srand(t1);
	srand(t1);
	selected_volunteers.clear();
	struct vol_qual vq;
	for (int i = 1; i<= A.size(); i++)
	{
		
		if (selected_volunteers.size() ==k)
			break;
		else
		{
			int n = A.size();
			int id = 1 + rand() % (n - 1);
			if (region_volunteer_list[region][id].selected !=true)
			{
				vq.id = id;
				vq.qual = A[id];
				selected_volunteers.push_back(vq);
				region_volunteer_list[region][id].selected = true;
				VS_regionList[region][id] = A[id]; 
				
				region_volunteer_list[region][id].channel_index = channel_counter_region[region];
				channel_counter_region[region] ++;
				if (channel_counter_region[region] == channel_count_per_region)
				{
					channel_counter_region[region] = 0;
				}
			}
			
		}
	}
	
	
}
void random_algo2(std:: map <int, float> &A, int k, int region)
{
	
	selected_volunteers.clear();
	struct vol_qual vq;
	int z = 0;
	for (std::map<int,float>::iterator it=A.begin(); it != A.end(); ++it)
    {
		int t1 = iter+region+z;
		rand_val(t1);
		
		if (selected_volunteers.size() ==k)
			break;
		else
		{
			int bin_value = binomial(0.5, 100);
			//cout<<"\nProbability: "<<bin_value<<endl;
			
			if (bin_value >= 50)
			{
				//cout<<"\nSelected\n";
				vq.id = it->first;
				vq.qual = it->second;
				selected_volunteers.push_back(vq);
				region_volunteer_list[region][it->first].selected = true;
				VS_regionList[region][it->first] = it->second;
			}
		}
		z++;
	}
	
}

void assign_channels_round_robin(int region, int k, std::map< int, float > sum_happiness)
{
	int channel = 0;
	for (int v = 1; v <= volunteer_list.size(); v++)
	{
		if(channel == channel_count_per_region)
		{
			channel = 0;
		}
		if (region_volunteer_list[region][v].selected == true)
		{
			region_volunteer_list[region][v].channel_index = channel;
			channel++;
		}
	}
	//##############################
	//Calculate Happiness Score
	//##############################
	
	//Define volunteer preference list
	//Copy the channel detection capabilities of volunteers
	std::map< int, std::map<int, vector <struct channel_vol_detection> > > vol_det_cap_prime; //region->volunteer->List[channel_detection_capabilities]
	for (int rr = 1; rr <= region_list.size(); rr++)
	{
		for (int j = 0; j< selected_volunteers.size(); j++)
		{
			int v = selected_volunteers[j].id;
			vol_det_cap_prime[rr][v] = region_volunteer_list[rr][v].vol_det_cap;
		}
	}
	
	//Define volunteer rank of channels list. volunteer-> region -> (channel, score)
	std::map< int, std::map< int, vector< struct channel_score > > > vol_channel_rank_list;
	for (int rr = 1; rr <= region_list.size(); rr++)
    {
		
		for (int j = 0; j< selected_volunteers.size(); j++)
		{
			int v = selected_volunteers[j].id;
			int region_flag = 0;
			if (isInsideRegion(v,rr) == true)
			{
				region_flag = 1;
			}
			
			for (int ff = 0; ff < vol_det_cap_prime[rr][v].size(); ff++)
			{
				struct channel_score cs;
				cs.channel_id = vol_det_cap_prime[rr][v][ff].id;
				cs.score = ff + 1;
				if (region_flag != 1)
					cs.score += channel_count_per_region; //Increase the rank of channels in regions that v does not reside in
				
				vol_channel_rank_list[rr][v].push_back(cs);
			}
		}
		
	}
	
	//Calculate happiness score
	
	for (int j = 0; j< selected_volunteers.size(); j++)
	{
		int vol_id = selected_volunteers[j].id;
		int channel = region_volunteer_list[region][vol_id].channel_index;
			for (int ff = 0; ff < vol_channel_rank_list[region][vol_id].size(); ff++)
			{
				if(vol_channel_rank_list[region][vol_id][ff].channel_id == channel)
				{
					sum_happiness[region] += (float)vol_channel_rank_list[region][vol_id][ff].score;
				}
			}
		
		
	}
	//Push to happiness score list
	happiness_score_list[region].push_back(sum_happiness[region]);
}



// ####################################################---------- Function Related to processes ------------------------ #####################################


void print_channel_list()
{
	for (std::map<int, vector <struct channel> >::iterator it=channel_list.begin(); it !=channel_list.end(); ++it)
    {
		cout<<"\n\n Region: "<<it->first<<"\n";
		vector <struct channel> temp = it->second;
		
		for (int i = 0; i < temp.size(); i++)
		{
			cout<<"Channel "<<i<<": Permission= "<<temp[i].permission<<", Access = ";
			for (int j = 0; j <temp[i].access.size(); j++)
			{
				cout<<temp[i].access[j]<<",";
				
			}
			cout<<"\n";
			
		}
		//cout<<"iter";
	}
}

void malicious_user(int MU)
{
	float t1;
	create("malicious_user");
	
	//printf("\nInitial positions of malicious users\n");
	
	t1 = clock1;			// record start time
	
	//Set initial position of volunteers 
	int x = 0 + rand() % (bdry_X - 0);
	int y = 0 + rand() % (bdry_Y - 0);
	
	//printf("\n\n Malicious User %d => x:%d, y: %d",MU, x, y);
	
	MU_access(MU);
	
	while (event_over() == false)
	{
		int time_in_same_dir = 2;	
		hold (time_in_same_dir);
		
		//int bin_value  = binomial(p, 100);
		
		t1 = clock1;
		srand(t1);
		
		//Update the position of a volunteer
		float speed = 0 + rand() % (70 - 0);
		int direction = 0 + rand() % (180 - 0);
		
		float cosine = cos(direction * PI/180.0);
		float sine = sin(direction * PI/180.0);
		
		x = x+ (speed *time_in_same_dir* (float)cosine);
		y = y+(speed *time_in_same_dir*(float)sine); 
		
		//Wrap Up the positions when they go out of boundary
		if (x < 0)
			x = bdry_X - (abs(x)%bdry_X);
		else if (x > bdry_X)
			x = 0 + (x%bdry_X);
	
		if (y < 0)
			y = bdry_Y - (abs(y)%bdry_Y);
		else if (y > bdry_Y)
			y = 0 + (y%bdry_Y);
		
		struct Malicious_U m = {x, y};
		MU_list [MU] = m;
	
	}
		

}


void MU_access(int MU)
{
	
	create("MU_access");
	
	float t1 = clock1;
	int access_status = 0;
	
	int current_region;
	int previous_region = -1;
	int current_channel;
	
	std::string out_string;
	std::stringstream ss;
	ss << MU;
	out_string = ss.str();
	std::string id = "m"+out_string;
	
	while (event_over() == false)
	{
		//1. Check current region of an authorized user
		for (int i = 1; i<= region_count; i++)
		{
			if (isInsideRegion_AU_MU(MU_list[MU].x, MU_list[MU].y, i) == true)
			{	
				current_region = i;
				break;
			}
		}
		
		if (previous_region != current_region && previous_region != -1) //if the MU moved to a new region, access a free channel there and delete all accesses in the previous region.
		{
			
			//Delete access of channel in previous region, if any
			vector <struct channel> temp = channel_list[previous_region];
			
			
			if (current_channel != -1) //If the AU was accessing a channel
			{
				
				for (int k = 0; k< temp[current_channel].access.size(); k++)
				{
					if (temp[current_channel].access[k] == id)
					{
						temp[current_channel].access.erase(temp[current_channel].access.begin()+k);
					}
				}
				channel_list[previous_region] = temp;
				
				
				
			}
			access_status = 0;
			current_channel = -1;
		}
		previous_region = current_region;
		//2. check if it is already accessing a channel, if not look for a free channel to access
		if (access_status == 0)
		{
			
			//3. Check channels in the current region and access the first available channel
			vector <struct channel> temp = channel_list[current_region];
			//choose a channel to access randomly 
			
			t1 = clock1;
			srand(t1+MU);
			
			int channel_index = 0 + rand()%(channel_count_per_region);
			
			temp[channel_index].access.push_back(id);
			channel_list[current_region] = temp;
			access_status = 1;
			current_channel = channel_index;
			
			//cout<<"\n\nMalicious user "<<MU<<" accessed channel " <<current_channel<<"in region "<<current_region<<"\n";
			
		}
		else if (access_status == 1) //Randomly decide if to keep accessing or not 
		{
			t1 = clock1;
			srand(t1+MU);
			float choice = 0 + rand()%10;
			
			if (choice >5.0) //Disconnect
			{
				vector <struct channel> temp = channel_list[current_region];
				
				
				
				for (int k = 0; k< temp[current_channel].access.size(); k++)
				{
					if (temp[current_channel].access[k] == id)
					{
						temp[current_channel].access.erase(temp[current_channel].access.begin()+k);
					}
				}
				/* if (temp[current_channel].access == id)
				{
					temp[current_channel].access = "";
				} */
				
				//cout<<"\n\nMalicious user "<<MU<<" left channel " <<current_channel<<"in region "<<current_region<<"\n";
				channel_list[current_region] = temp;
				current_channel = -1;
				access_status = 0;
			}
		
		
		}
		
		hold(expntl(access_time));
	}
}

void authorized_user(int AU)
{
	float t1;
	create("authorized_user");
	
	//printf("\nInitial positions of authorized users\n");
	
	t1 = clock1;			// record start time
	
	//Set initial position of volunteers 
	int x = 0 + rand() % (bdry_X - 0);
	int y = 0 + rand() % (bdry_Y - 0);
	
	//printf("\n\n Authorized User %d => x:%d, y: %d",AU, x, y);
	
	AU_access(AU);
	
	while (event_over() == false)
	{
		int time_in_same_dir = 2;	
		hold (time_in_same_dir);
		
		//int bin_value  = binomial(p, 100);
		
		t1 = clock1;
		srand(t1);
		
		//Update the position of a volunteer
		float speed = 0 + rand() % (70 - 0);
		int direction = 0 + rand() % (180 - 0);
		
		float cosine = cos(direction * PI/180.0);
		float sine = sin(direction * PI/180.0);
		
		x = x+ (speed *time_in_same_dir* (float)cosine);
		y = y+(speed *time_in_same_dir*(float)sine); 
		
		//Wrap Up the positions when they go out of boundary
		if (x < 0)
			x = bdry_X - (abs(x)%bdry_X);
		else if (x > bdry_X)
			x = 0 + (x%bdry_X);
	
		if (y < 0)
			y = bdry_Y - (abs(y)%bdry_Y);
		else if (y > bdry_Y)
			y = 0 + (y%bdry_Y);
		

		struct Authorized_U a = {x, y};
		AU_list [AU] = a;
		
				
		
	}
	
	
	
}

void AU_access(int AU)
{
	
	create("AU_access");
	
	float t1 = clock1;
	int access_status = 0;
	//Access a channel
	
	int current_region;
	int previous_region = -1;
	int current_channel;
	
	std::string out_string;
	std::stringstream ss;
	ss << AU;
	out_string = ss.str();
	std::string id = "a"+out_string;
	
	while (event_over() == false)
	{
		//1. Check current region of an authorized user
		for (int i = 1; i<= region_count; i++)
		{
			if (isInsideRegion_AU_MU(AU_list[AU].x, AU_list[AU].y, i) == true)
			{	
				current_region = i;
				break;
			}
		}
		/* if (previous_region == -1)
		{
			access_status == 0;
		} */
		if (previous_region != current_region && previous_region != -1) //if the AU moved to a new region, access a free channel there and delete all accesses in the previous region.
		{
			
			//Delete access of channel in previous region, if any
			vector <struct channel> temp = channel_list[previous_region];
			
			
			if (current_channel != -1) //If the AU was accessing a channel
			{
				if (temp[current_channel].permission == id)
				{
					temp[current_channel].permission = "";
				}
				for (int k = 0; k< temp[current_channel].access.size(); k++)
				{
					if (temp[current_channel].access[k] == id)
					{
						temp[current_channel].access.erase(temp[current_channel].access.begin()+k);
					}
				}
				
				channel_list[previous_region] = temp;
				
			}
			access_status = 0;
			current_channel = -1;
		}
		previous_region = current_region;
		//2. check if it is already accessing a channel, if not look for a free channel to access
		if (access_status == 0)
		{
			
			//3. Check channels in the current region and access the first available channel
			vector <struct channel> temp = channel_list[current_region];
			for (int j = 0; j < temp.size(); j++)
			{
				if (temp[j].permission == "")
				{
					temp[j].permission = id;
					//temp[j].access = id;
					temp[j].access.push_back (id);
					channel_list[current_region] = temp;
					access_status = 1;
					current_channel = j;
					
					//cout<<"\n\nAuthorized user "<<AU<<" accessed channel " <<j<<"in region "<<current_region<<"\n";
					break;
				}
				else
				{
					access_status = 0;
					current_channel = -1;
				}
			}
		}
		else if (access_status == 1) //Randomly decide if to keep accessing or not 
		{
			t1 = clock1;
			srand(t1+AU);
			float choice = 0 + rand()%10;
			if (choice >5.0) //Disconnect
			{
				vector <struct channel> temp = channel_list[current_region];
				
				if (temp[current_channel].permission == id)
				{
					temp[current_channel].permission = "";
				}
				
				for (int k = 0; k< temp[current_channel].access.size(); k++)
				{
					if (temp[current_channel].access[k] == id)
					{
						temp[current_channel].access.erase(temp[current_channel].access.begin()+k);
					}
				}
				/* if (temp[current_channel].access == id)
				{
					temp[current_channel].access = "";
				} */
				
				//cout<<"\n\nAuthorized user "<<AU<<" left channel " <<current_channel<<"in region "<<current_region<<"\n";
				channel_list[current_region] = temp;
				current_channel = -1;
				access_status = 0;
			}
				
				
		}
		hold(expntl(access_time));
	}
	
}

/* bool compareBydetection_capability(const struct channel_vol_detection &a, const struct channel_vol_detection &b)
{
    return a.detection_capability> b.detection_capability;
} */

void volunteer(int i)				// arriving customer
{
	float t1;
	
	
	//cout<<"tick_tock";
	create("volunteer");
	srand(i);
	//int corrupt = 0;
	//Decide if a volunteer is corrupt
	float decide = (float) rand() / (RAND_MAX);
	
	int corrupt;
	
	//Steps:
	//1. Set preferences of volunteers
	//2. Set preferences of channels, based on accuracy
	//3. Run SMP alorithm
	
	if (decide > corrupt_percent)
	{
		corrupt = 0;
		cout<<"\n********************I am NOT CORRUPT: "<<decide;
	}
	else
	{
		corrupt = 1;
		cout<<"\n********************I am CORRUPT: "<<decide;
	}
	
	
	//Toss a coin to decide if a volunteer is corrupt
	
	
	/* if (corrupt == 1)
	{
		cout<<"\n********************I am CORRUPT";
	}
	else
	{
		cout<<"\n********************I am NOT corrupt**\n";
	} */
	/* if (corrupt_volunteer_count < total_corrupt_volunteers)
	{
		corrupt = 1;
		corrupt_volunteer_count++;	
	}*/
	//printf("\nInitial positions of volunteers\n");
	
	t1 = clock1;			// record start time
	
	//Set initial position of volunteers 
	float x = 0 + rand() % (bdry_X - 0);
	float y = 0 + rand() % (bdry_Y - 0);
	
	//printf("\n\n Volunteer %d => x:%d, y: %d",i, x, y);
	
	
	int seed = i;
	rand_val(seed);
	//float p = 0.5;
	//float p = (((float) rand() / RAND_MAX) * 0.5) ;
	
	
	//float p = (((float) rand() / RAND_MAX)) ;
	
	//cout<<"\nVolunteer "<<i<<"has probability of success = "<<p<<endl;
	
	//call a process to update battery
	hold(2);
	battery(i);
	
	//float p = uniform(0.0, 0.5);
	float p = 0.5;
	/* srand(t1+i);
	p = ((float) rand() / (RAND_MAX)); */
	
	//while (cnt > 0)
	
	//decide Channel preference.
	
	
	int travelling_status = 0;
	float time_to_destination = 0.0;
	float speed = 0.0;
	float angle;
	float average_speed;
	float sum_speed = 0.0;
	int no_of_decisions = 0;
	float x_next;
	float y_next;
	float distance_to_destination;
	
	std::map< int, vector <struct channel_vol_detection> > vol_channel_detection1; //region -> channel detection probability
		
	for(int region = 1; region <= region_list.size(); region++)
	{
		
		
		for (int c = 0; c< channel_count_per_region; c++)
		{
			struct channel_vol_detection cvd;
			cvd.id = c;
			cvd.detection_capability = uniform(0.5, 1.0);
			vol_channel_detection1[region].push_back(cvd);
		}
		//sort the vector in descending order by the detection capabilities
		std::sort(vol_channel_detection1[region].begin(), vol_channel_detection1[region].end(), compareBydetection_capability);
	}
	
	while (event_over() == false)
	{
		//cout<<"\nmy "<<i<<"probability:"<<p;
		float time_in_same_dir = 2.0;
		
		//hold(time_in_same_dir);
		//float p = uniform(0.3, 0.7);
		t1 = clock1;
		int bin_value;
		if (corrupt == 1)
		{
			bin_value = bernoulli(p); //tell truth with a given probability
		}
		else if (corrupt == 0)
		{
			bin_value = 1;
		}
		 
		//Find probability of correctly monitoring a channel
		

		
		
		
		
		t1 = clock1;
		srand(t1);
		
		//Implement Random Waypoint Mobility Model
		//1. If a volunteer is not travelling 
		
		if (travelling_status == 0)
		{
			float wait_time = 1 + rand() % (20);//10.0;//TODO make it random
			hold(wait_time);
			
			
			
			//choose a random next destination
			
			//int x_next = uniform(0, bdry_X);
			//int y_next = uniform(0, bdry_Y);
			
			x_next = 0 + rand() % (bdry_X - 0);
			y_next = 0 + rand() % (bdry_Y - 0);
			
			
			//Calculate the angle of movement
			float y_diff = (y_next - y);
			float x_diff = (x_next - x);
			
			float slope = (float)y_diff/(float)x_diff;
			angle = atan2(y_diff, x_diff);
			
			
								
			//Generate random speed
			speed = uniform(1.4, 70.0);
			
			//Calculate average_speed
			sum_speed += speed;
			no_of_decisions++;
			average_speed = (float)sum_speed/(float)no_of_decisions;
			
			
			//Distance to destination
			distance_to_destination = sqrt(pow(x_next - x, 2) +  pow(y_next - y, 2) * 1.0);
			
			//Time to destination
			time_to_destination = (float)distance_to_destination/(float)speed;
			
			//Set travel status
			travelling_status = 1;
			
			//update the map
			struct vol v = {(int)x, (int)y, bin_value, corrupt, average_speed, vol_channel_detection1};
			volunteer_list[i] = v;
			
			//Test Print
			/* cout<< "###########\n#############\n############";
			cout<<"Volunteer "<<i<<"has channel detection as\n";
			
			for(int region = 1; region <= region_list.size(); region++)
			{
				
				vector < struct channel_vol_detection > temp_v;
				temp_v = volunteer_list[i].vol_channel_detection[region];
				cout<<"Region: "<<region<<"\n";
				for (int vec_id = 0; vec_id < temp_v.size(); vec_id ++)
				{
					int channel_id = temp_v[vec_id].id;
					float capability = temp_v[vec_id].detection_capability;
					cout<<"Channel: "<<channel_id<<", capability: "<<capability<<"\n";
				}
				
			}
			cout<< "###########\n#############\n############"; */
			
			//Update Residence time
			for (int region = 1; region<= region_list.size(); region++)
			{
				if (isInsideRegion(i, region) == true)
				{
					region_volunteer_list[region][i].residence_time += wait_time;
				}
				
			}
			
			
			/*if(i == 3)
			{
				cout<<"\n\n******%%******* VOLUNTEER "<<i<<" ********%%********\n";
				cout<<"Travel Status: "<<travelling_status<<"\n";
				cout<<"Current Position: ("<<x<<","<<y<<")\n";
				cout<<"Next Position: ("<<x_next<<","<<y_next<<")\n";
				cout<<"Current Speed: "<<speed;
				cout<<"\nDistance to destination:"<<distance_to_destination;
				cout<<"\nTime to destination:"<<time_to_destination;
				cout<<"\n==========================////////////////////////////\n\n";
			}
			 */
		}
		else if (travelling_status == 1) //If a volunteer is travelling
		{
			if (time_to_destination > time_in_same_dir) //If volunteer is far from the destination
			{
				hold(time_in_same_dir); //travel for a minimum amount of time
				
				//Update time_to_destination
				time_to_destination -= time_in_same_dir;
				
				/* //Calculate the angle of movement
				float y_diff = (y_next - y);
				float x_diff = (x_next - x);
				
				float slope = (float)y_diff/(float)x_diff;
				angle = atan2(y_diff, x_diff); */
				
				//Update x and y 
				float cosine = cos(angle);
				float sine = sin(angle);
				x = x + (speed * time_in_same_dir * (float)cosine);
				y = y +(speed * time_in_same_dir *(float)sine); 
				
				
				//Wrap Up the positions when they go out of boundary
				if (x < 0)
					x = bdry_X - (abs((int)x)%bdry_X);
				else if (x > bdry_X)
					x = 0 + ((int)x%bdry_X);
			
				if (y < 0)
					y = bdry_Y - (abs((int)y)%bdry_Y);
				else if (y > bdry_Y)
					y = 0 + ((int)y%bdry_Y);
				
				//update the map
				struct vol v = {(int)x, (int)y, bin_value, corrupt, average_speed, vol_channel_detection1};
				volunteer_list[i] = v;
				
				//Test Print
				/* cout<< "###########\n#############\n############";
				cout<<"Volunteer "<<i<<"has channel detection as\n";
				
				for(int region = 1; region <= region_list.size(); region++)
				{
					
					vector < struct channel_vol_detection > temp_v;
					temp_v = volunteer_list[i].vol_channel_detection[region];
					cout<<"Region: "<<region<<"\n";
					for (int vec_id = 0; vec_id < temp_v.size(); vec_id ++)
					{
						int channel_id = temp_v[vec_id].id;
						float capability = temp_v[vec_id].detection_capability;
						cout<<"Channel: "<<channel_id<<", capability: "<<capability<<"\n";
					}
					
				}
				cout<< "###########\n#############\n############"; */
				
				//Update Residence time
				for (int region = 1; region<= region_list.size(); region++)
				{
					if (isInsideRegion(i, region) == true)
					{
						region_volunteer_list[region][i].residence_time += time_in_same_dir;
					}
					
				}
			
				
				/* if(i == 3)
				{
					cout<<"\n\n******%%******* VOLUNTEER "<<i<<" ********%%********\n";
					cout<<"COS value: "<< cosine<<", SIN value: "<<sine;
					cout<<"\nCurrent Position: ("<<x<<","<<y<<")\n";
					cout<<"Next Position: ("<<x_next<<","<<y_next<<")\n";
					cout<<"Current Speed: "<<speed;
					cout<<"\nDistance to destination:"<<distance_to_destination;
					cout<<"\nTime to destination:"<<time_to_destination;
					cout<<"\n==========================////////////////////////////\n\n";
				} */
				
			}
			else if (time_to_destination <= time_in_same_dir) //If volunteer has almost reached the destination
			{
				if (time_to_destination == 0.0)
					hold(2.0);
				else
					hold(time_to_destination);
				//Update time_to_destination
				
				/* //Calculate the angle of movement
				float y_diff = (y_next - y);
				float x_diff = (x_next - x);
				
				float slope = (float)y_diff/(float)x_diff;
				angle = atan2(y_diff, x_diff); */
				
				//Update x and y 
				float cosine = cos(angle);
				float sine = sin(angle);
				
				x = x + (speed * time_to_destination * (float)cosine);
				y = y + (speed * time_to_destination *(float)sine); 
				
				
								
				//Wrap Up the positions when they go out of boundary
				if (x < 0)
					x = bdry_X - (abs((int)x)%bdry_X);
				else if (x > bdry_X)
					x = 0 + ((int)x%bdry_X);
			
				if (y < 0)
					y = bdry_Y - (abs((int)y)%bdry_Y);
				else if (y > bdry_Y)
					y = 0 + ((int)y%bdry_Y);
				
				
				//update the map
				struct vol v = {(int)x, (int)y, bin_value, corrupt, average_speed, vol_channel_detection1};
				volunteer_list[i] = v;
				
				//Test Print
				/* cout<< "###########\n#############\n############";
				cout<<"Volunteer "<<i<<"has channel detection as\n";
				
				for(int region = 1; region <= region_list.size(); region++)
				{
					
					vector < struct channel_vol_detection > temp_v;
					temp_v = volunteer_list[i].vol_channel_detection[region];
					cout<<"Region: "<<region<<"\n";
					for (int vec_id = 0; vec_id < temp_v.size(); vec_id ++)
					{
						int channel_id = temp_v[vec_id].id;
						float capability = temp_v[vec_id].detection_capability;
						cout<<"Channel: "<<channel_id<<", capability: "<<capability<<"\n";
					}
					
				}
				cout<< "###########\n#############\n############"; */
					
				//Update travel-status
				travelling_status = 0;
				
				//Update Residence time
				for (int region = 1; region<= region_list.size(); region++)
				{
					if (isInsideRegion(i, region) == true)
					{
						if(time_to_destination == 0.0)
							region_volunteer_list[region][i].residence_time += 2.0;
						else
							region_volunteer_list[region][i].residence_time += time_to_destination;
					}
					
				}
				
				time_to_destination = 0;
				
				
				/* if(i == 3)
				{
					cout<<"\n\n******%%******* VOLUNTEER "<<i<<" ********%%********\n";
					cout<<"COS value: "<< cosine<<", SIN value: "<<sine;
					cout<<"\nCurrent Position: ("<<x<<","<<y<<")\n";
					cout<<"Next Position: ("<<x_next<<","<<y_next<<")\n";
					cout<<"Current Speed: "<<speed;
					cout<<"\nDistance to destination:"<<distance_to_destination;
					cout<<"\nTime to destination:"<<time_to_destination;
					cout<<"\n==========================////////////////////////////\n\n";
				} */
				
			}
		}	
	}
	
	
	qtbl.note_entry();		// note arrival
	tbl.record(clock1 - t1);		// record response time
	qtbl.note_exit();		// note departure
}


// ########################----------------Functions related to BATTERY -------------------------###################################

void battery(int i)
{
	float t1;
	create("battery");
	t1 = clock1;
	srand (t1);
	printf("\nI am battery of volunteer %d\n", i);
	
	//Initialize the battery level of the volunteer
	
	int b = 0 + rand() % (max_battery_level - 0);
	//float battery = (float)b;
	float battery = max_battery_level;
	printf("\n Initial battery level of volunteer %d: %f", i, battery);
	
	battery_list[i] = battery;
	
	
	while (battery_list[i] >0) //while battery level 
	{

		//Hold for the computational time
		float compute_hold = expntl(batt_compute);
		hold(compute_hold);
		
		//Adjust battery level
		battery_list [i] -= compute_hold;

		if (battery_list[i] >0)
		{
			hold(expntl(batt_idle));
			//Hold for exponentially idle time
		}
		
		
		//print_battery_list();
		
	}
	
}

bool event_over()
{
	int flag = 0;
	
	for (int i = 1; i<= battery_list.size(); i++)
	{
		if (battery_list[i] > 0)
		{
			flag = 1;
			break;
		}
	}
	if (flag == 0 && battery_list.size()>0)
	{
		printf("\nBattery over");
		return true;
	}	
	else
		return false;
	
}

//################################----- Functions Related to REGIONS --------------###########################
void init_regions()
{
	struct Point c;
	
	
	//Defining region 1
	c.x = 0;
	c.y = 0;
	region_list[1].bdry1 = c;
	c.x = 500;
	c.y = 0;
	region_list[1].bdry2 = c;
	c.x = 500;
	c.y = 500;
	region_list[1].bdry3 = c;
	c.x = 0;
	c.y = 500;
	region_list[1].bdry4 = c;	
	
	//Defining Region 2
	c.x = 501;
	c.y = 0;
	region_list[2].bdry1 = c;
	c.x = 1000;
	c.y = 0;
	region_list[2].bdry2 = c;
	c.x = 1000;
	c.y = 500;
	region_list[2].bdry3 = c;
	c.x = 501;
	c.y = 500;
	region_list[2].bdry4 = c;	
	
	
	/*
	//Change
	//Defining region 1
	c.x = 0;
	c.y = 0;
	region_list[1].bdry1 = c;
	c.x = 500;
	c.y = 0;
	region_list[1].bdry2 = c;
	c.x = 500;
	c.y = 500;
	region_list[1].bdry3 = c;
	c.x = 0;
	c.y = 500;
	region_list[1].bdry4 = c;	
	
	//Defining Region 2
	c.x = 501;
	c.y = 0;
	region_list[2].bdry1 = c;
	c.x = 1000;
	c.y = 0;
	region_list[2].bdry2 = c;
	c.x = 1000;
	c.y = 500;
	region_list[2].bdry3 = c;
	c.x = 501;
	c.y = 500;
	region_list[2].bdry4 = c;	
	
	//Defining Region 3
	c.x = 0;
	c.y = 501;
	region_list[3].bdry1 = c;
	c.x = 500;
	c.y = 501;
	region_list[3].bdry2 = c;
	c.x = 500;
	c.y = 1000;
	region_list[3].bdry3 = c;
	c.x = 0;
	c.y = 1000;
	region_list[3].bdry4 = c;	
	
	//Defining Region 4
	c.x = 501;
	c.y = 501;
	region_list[4].bdry1 = c;
	c.x = 1000;
	c.y = 501;
	region_list[4].bdry2 = c;
	c.x = 1000;
	c.y = 1000;
	region_list[4].bdry3 = c;
	c.x = 501;
	c.y = 1000;
	region_list[4].bdry4 = c;	
	*/
	
	
}
//hello
float residence_param(float p_r, float p_r_avg)
{
	float alpha = 0.01;
	p_r_avg =(alpha * p_r) + ((1-alpha) * (p_r_avg));
	return p_r_avg;
}
float separation_factor(int region, int x, int y)
{
	float cent_x = ((float)region_list[region].bdry1.x + (float)region_list[region].bdry2.x + (float)region_list[region].bdry3.x + (float)region_list[region].bdry4.x)/4.0;
	float cent_y = ((float)region_list[region].bdry1.y + (float)region_list[region].bdry2.y + (float)region_list[region].bdry3.y + (float)region_list[region].bdry4.y)/4.0;
	//printf("\nCentroid of region %d: (%f, %f)\n", region, cent_x, cent_y);
	
	float dist_x = ((float)x - cent_x)*((float)x - cent_x);
	float dist_y = ((float)y - cent_y)*((float)y - cent_y);
	//printf("\nDistance x: %f\n", dist_x);
	//printf("\nDistance y: %f\n", dist_y);
	
	float dist = sqrt(dist_x + dist_y);
	float gamma1 = 1.0;
	float gamma2 = 0.01;
	float separation_parameter = gamma1 * exp(-gamma2 * dist);
	//printf ("\nSeparation parameter: %f\n",separation_parameter); 
	return separation_parameter;
	
}
float shortest_time_calc(int region, int x, int y, float average_speed)
{
	float cent_x = ((float)region_list[region].bdry1.x + (float)region_list[region].bdry2.x + (float)region_list[region].bdry3.x + (float)region_list[region].bdry4.x)/4.0;
	float cent_y = ((float)region_list[region].bdry1.y + (float)region_list[region].bdry2.y + (float)region_list[region].bdry3.y + (float)region_list[region].bdry4.y)/4.0;
	//printf("\nCentroid of region %d: (%f, %f)\n", region, cent_x, cent_y);
	
	float dist_x = ((float)x - cent_x)*((float)x - cent_x);
	float dist_y = ((float)y - cent_y)*((float)y - cent_y);
	//printf("\nDistance x: %f\n", dist_x);
	//printf("\nDistance y: %f\n", dist_y);
	
	float dist = sqrt(dist_x + dist_y);
	
	float shortest_time = (float)dist/(float)average_speed;
	return shortest_time;
}

//################# ----- Functions to check if a point lies inside a polygon -------- ################## Reference : https://www.geeksforgeeks.org/how-to-check-if-a-given-point-lies-inside-a-polygon/

bool onSegment(Point p, Point q, Point r) 
{ 
    if (q.x <= max(p.x, r.x) && q.x >= min(p.x, r.x) && 
            q.y <= max(p.y, r.y) && q.y >= min(p.y, r.y)) 
        return true; 
    return false; 
} 

int orientation(Point p, Point q, Point r) 
{ 
    int val = (q.y - p.y) * (r.x - q.x) - 
              (q.x - p.x) * (r.y - q.y); 
  
    if (val == 0) return 0;  // colinear 
    return (val > 0)? 1: 2; // clock or counterclock wise 
} 
bool doIntersect(Point p1, Point q1, Point p2, Point q2) 
{ 
    // Find the four orientations needed for general and 
    // special cases 
    int o1 = orientation(p1, q1, p2); 
    int o2 = orientation(p1, q1, q2); 
    int o3 = orientation(p2, q2, p1); 
    int o4 = orientation(p2, q2, q1); 
  
    // General case 
    if (o1 != o2 && o3 != o4) 
        return true; 
  
    // Special Cases 
    // p1, q1 and p2 are colinear and p2 lies on segment p1q1 
    if (o1 == 0 && onSegment(p1, p2, q1)) return true; 
  
    // p1, q1 and p2 are colinear and q2 lies on segment p1q1 
    if (o2 == 0 && onSegment(p1, q2, q1)) return true; 
  
    // p2, q2 and p1 are colinear and p1 lies on segment p2q2 
    if (o3 == 0 && onSegment(p2, p1, q2)) return true; 
  
     // p2, q2 and q1 are colinear and q1 lies on segment p2q2 
    if (o4 == 0 && onSegment(p2, q1, q2)) return true; 
  
    return false; // Doesn't fall in any of the above cases 
} 

bool isInsidePolygon(Point polygon[], int n, Point p) 
{ 
    // There must be at least 3 vertices in polygon[] 
    if (n < 3)  return false; 
  
    // Create a point for line segment from p to infinite 
    Point extreme = {INF, p.y}; 
  
    // Count intersections of the above line with sides of polygon 
    int count = 0, i = 0; 
    do
    { 
        int next = (i+1)%n; 
  
        // Check if the line segment from 'p' to 'extreme' intersects 
        // with the line segment from 'polygon[i]' to 'polygon[next]' 
        if (doIntersect(polygon[i], polygon[next], p, extreme)) 
        { 
            // If the point 'p' is colinear with line segment 'i-next', 
            // then check if it lies on segment. If it lies, return true, 
            // otherwise false 
            if (orientation(polygon[i], p, polygon[next]) == 0) 
               return onSegment(polygon[i], p, polygon[next]); 
  
            count++; 
        } 
        i = next; 
    } while (i != 0); 
  
    // Return true if count is odd, false otherwise 
    return count&1;  // Same as (count%2 == 1) 
} 

//Function for AUs and MUs
bool isInsideRegion_AU_MU(int x, int y, int region)
{
    Point p = {x, y};
	
	//test 
	//printf("\n\n Point of vol: (%d, %d):", p.x, p.y);
	
    Point polygon[] = {region_list[region].bdry1, region_list[region].bdry2, region_list[region].bdry3, region_list[region].bdry4};
	//printf("\n\n Points of polygon: (%d, %d), (%d, %d), (%d, %d), (%d, %d):", polygon[0].x, polygon[0].y, polygon[1].x, polygon[1].y, polygon[2].x, polygon[2].y, polygon[3].x, polygon[3].y);
    //printf("\n\n");
	int n = sizeof(polygon)/sizeof(polygon[0]); 
	if (isInsidePolygon(polygon, n, p) == true)
	{
		//printf("\nInside\n");
		return true;
	}
	else
	{
		//printf("\nOutside\n");
		return false;
	}

}


//Function to call
bool isInsideRegion(int vol,int region)
{
    Point p = {volunteer_list[vol].x, volunteer_list[vol].y};
	
	//test 
	//printf("\n\n Point of vol: (%d, %d):", p.x, p.y);
	
    Point polygon[] = {region_list[region].bdry1, region_list[region].bdry2, region_list[region].bdry3, region_list[region].bdry4};
	//printf("\n\n Points of polygon: (%d, %d), (%d, %d), (%d, %d), (%d, %d):", polygon[0].x, polygon[0].y, polygon[1].x, polygon[1].y, polygon[2].x, polygon[2].y, polygon[3].x, polygon[3].y);
    //printf("\n\n");
	int n = sizeof(polygon)/sizeof(polygon[0]); 
	if (isInsidePolygon(polygon, n, p) == true)
	{
		//printf("\nInside\n");
		return true;
	}
	else
	{
		//printf("\nOutside\n");
		return false;
	}

}

//#################################################### ---- Centroid of a Polygon ---- ####################################### Reference: https://stackoverflow.com/questions/2792443/finding-the-centroid-of-a-polygon

/* Point centroid(Point polygon[], int vertexCount)
{
	Point_d centroid = {0, 0};
    double signedArea = 0.0;
    double x0 = 0.0; // Current vertex X
    double y0 = 0.0; // Current vertex Y
    double x1 = 0.0; // Next vertex X
    double y1 = 0.0; // Next vertex Y
    double a = 0.0;  // Partial signed area

    // For all vertices except last
    int i=0;
    for (i=0; i<vertexCount-1; ++i)
    {
        x0 = vertices[i].x;
        y0 = vertices[i].y;
        x1 = vertices[i+1].x;
        y1 = vertices[i+1].y;
        a = x0*y1 - x1*y0;
        signedArea += a;
        centroid.x += (x0 + x1)*a;
        centroid.y += (y0 + y1)*a;
    }

    // Do last vertex separately to avoid performing an expensive
    // modulus operation in each iteration.
    x0 = vertices[i].x;
    y0 = vertices[i].y;
    x1 = vertices[0].x;
    y1 = vertices[0].y;
    a = x0*y1 - x1*y0;
    signedArea += a;
    centroid.x += (x0 + x1)*a;
    centroid.y += (y0 + y1)*a;

    signedArea *= 0.5;
    centroid.x /= (6.0*signedArea);
    centroid.y /= (6.0*signedArea);

    return centroid;

}

//#################################################### ------Print Functions-----------################################################
void print_vol_list()
{
	printf ("\n\n Volunteer map:: \n\n");
	for (int i = 1; i<= volunteer_list.size(); i++)
	{
		printf("\nVolunteer ID:%d",i);
		printf("(%d,%d)\n", volunteer_list[i].x, volunteer_list[i].y);
	}	
}
 */
void print_region_vol_list()
{
	for (int i = 1; i<= region_list.size()-1; i++)
	{
		printf("\n##### Region %d #####\n", i);
		printf("Volunteer parameters\n");
		for (int v = 1; v <= volunteer_list.size(); v++)
		{
			printf("\n\nVolunteer ID: %d\n", v);
			printf("Current Position: (%d, %d)\n", region_volunteer_list[i][v].x, region_volunteer_list[i][v].y);
			printf("Region in Current Iteration: %d\n", region_volunteer_list[i][v].curr_region);
			printf("Region in Previous Iteration: %d\n", region_volunteer_list[i][v].prev_region);
			printf("Sojourn Time: %f\n", region_volunteer_list[i][v].soj_time);
			printf("Return Time: %f\n", region_volunteer_list[i][v].ret_time);
			printf("Latest Probability of Return: %f\n", region_volunteer_list[i][v].p_r);
			printf("Average Probability of Return: %f\n", region_volunteer_list[i][v].p_r_avg);
		}
	}
				
}
void print_battery_list()
{
	printf("\n##############\n");
	printf("Battery List\n");
	printf("\n==============\n");
	for (int i = 1; i<= battery_list.size(); i++)
	{
		
		printf("\nVolunteer ID:%d",i);
		printf(": %f\n", battery_list[i]);
	}
		
}

//#############################################--- Binomial distribution ------------ ##################################################

int binomial(double p, int n)
{
  int    bin_value;             // Computed Poisson value to be returned
  int    i;                     // Loop counter

  // Generate a binomial random variate
  bin_value = 0;
  for (i=0; i<n; i++)
    if (rand_val(0) < p) bin_value++;

  return(bin_value);
}

double rand_val(int seed)
{
  const long  a =      16807;  // Multiplier
  const long  m = 2147483647;  // Modulus
  const long  q =     127773;  // m div a
  const long  r =       2836;  // m mod a
  static long x;               // Random int value
  long        x_div_q;         // x divided by q
  long        x_mod_q;         // x modulo q
  long        x_new;           // New x value

  // Set the seed if argument is non-zero and then return zero
  if (seed > 0)
  {
    x = seed;
    return(0.0);
  }

  // RNG using integer arithmetic
  x_div_q = x / q;
  x_mod_q = x % q;
  x_new = (a * x_mod_q) - (r * x_div_q);
  if (x_new > 0)
    x = x_new;
  else
    x = x_new + m;

  // Return a random value between 0.0 and 1.0
  return((double) x / m);
}



/*
0) Exit when everyone's battery gets depleted --- DONE
1) Define the regions --DONE
2) Check if volunteer lies inside a region function --- DONE
3) Update volunteer location likelihood for all regions 
--- Calculate P_R and P_R_avg --- DONE
--- Calculate separation factor
4) Select volunteers using Secretary's algorithm.
5) Calculate the hit/miss rate in terms of location for all volunteers.
6) Calculate trust
*/

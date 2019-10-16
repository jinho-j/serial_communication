#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define component_number_GLL 6
#define component_number_RMC 11
#define component_number_XTE 6
#define SUCCESS 1;
#define FAIL    0;


struct GPS_XTE {

   char Status_System;
   char Status_Data;
   char Magnitude_of_cross_track_error[4];
   char Direction_to_steer;
   char Units;
   char Mode_indicator;
};

struct GPS_GLL
{
   float Latitue;
   char North_South;
   float Longitude;
   char East_West;
   float UTC_position;
   char Status;
   
};

struct GPS_RMC {

   char UTC[6];
   char Status;
   char Latitude[7];
   char N_S;
   char Longitude[7];
   char E_W;
   char Speed_knots[4];
   //char Course_over_ground[4];
   int Date;
   char Magnetic_variation[4];
   char E_W2;
   char Mode_indicator;
   char Navigational_status; 

};

class GPS{
   
private :

   GPS_GLL *GLL;
   GPS_RMC *RMC;
   GPS_XTE *XTE;
   GPS * NEXT_GPS;
public :

   GPS();
   int Parse_GPS(const char* sentence);
   int Parse_GLL(const char* sentence);
   int Parse_RMC(const char* sentence);
   int Parse_XTE(const char* sentence);
   
};

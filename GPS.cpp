#include "GPS.h"

GPS::GPS(){

   GLL=0;
   RMC=0;
   XTE=0;
   NEXT_GPS=0;
   
}
int GPS::Parse_GPS(const char* sentence){

   if(strstr(sentence,"GLL") != NULL)

      return Parse_GLL(sentence);

   else if(strstr(sentence,"RMC")!= NULL)

      return Parse_RMC(sentence);

   else if(strstr(sentence,"XTE")!= NULL) 
      return Parse_XTE(sentence);

   else
   return FAIL;

}




int GPS::Parse_GLL(const char* sentence){


   char * sentence_pointer;

   char * next_stence_pointer;

   int comp_length;

   char temp[100];

   int token_n = 0;

   char buff[100];

   strcpy(buff,sentence);

   sentence_pointer = strstr(buff, ",");

   printf("parse gll");
   while (token_n != component_number_GLL) {

      GLL = new GPS_GLL;

      sentence_pointer = strstr(sentence_pointer, ",") + 1;

      if (*sentence_pointer != ',')//연속 , 무시
      {
         next_stence_pointer = strstr(sentence_pointer, ",") + 1;

         comp_length = next_stence_pointer - sentence_pointer;

         memset(temp, 0, 100);

         //input information to temp 
         if (comp_length > 0)

            memcpy(temp, sentence_pointer, comp_length - 1);


         else

            memcpy(temp, sentence_pointer, buff + strlen(buff) - sentence_pointer);

         // input information       
         if (token_n == 0) {
            GLL->Latitue = atof(temp);
            printf("Latitue :  %.3f\n", GLL->Latitue);
         }
         else if (token_n == 1) {
            
            GLL->North_South = *temp;
            printf("North or South :  %c\n", GLL->North_South);
         }
         else if (token_n == 2) {
            GLL->Longitude = atof(temp);
            printf("Longitude :  %.3f\n", GLL->Longitude);

         }
         else if (token_n == 3) {
            GLL->East_West = *temp;
            printf("East or West :  %c\n", GLL->East_West);

         }

         else if (token_n == 4) {
            GLL->UTC_position = atof(temp);
            printf("UTC of position :  %.2f\n",GLL->UTC_position);
         }
         else if (token_n == 5) {
            GLL->Status = *temp;
            printf("Status :  %c\n", GLL->Status);
         }

      }
      token_n++;

   }
   return SUCCESS;

}


int GPS::Parse_RMC(const char* sentence){

   

    char buff[100];

   strcpy(buff,sentence);
   
       
      char *sentence_pointer;
      char *next_stence_pointer;
      int comp_length;
      char temp[10];
      int token_n=0;
    
      sentence_pointer = strstr(buff, ",");
     
      
      while(token_n !=component_number_RMC){
          RMC =new GPS_RMC;
          sentence_pointer = strstr(sentence_pointer, ",")+1;

      if (*sentence_pointer != ',')//연속 , 무시
      {
         next_stence_pointer = strstr(sentence_pointer, ",")+1;
   
         comp_length = next_stence_pointer - sentence_pointer;

         memset(temp, 0, 10);

            //input information to temp 
         if (comp_length> 0)                         
            
            memcpy(temp, sentence_pointer, comp_length - 1);
         
         else

            memcpy(temp, sentence_pointer, buff + strlen(buff)- sentence_pointer);
 
       if(token_n==0){
           memcpy(RMC->UTC, temp, strlen(temp)+1 );
           printf("UTC :  %s\n", RMC->UTC);
         }
       else if (token_n == 1) {
            RMC->Status =*temp;
            printf("Status :  %c\n", RMC->Status);
         }
       else if (token_n == 2) {
           memcpy(RMC->Latitude,temp,strlen(temp)+1);
            printf("Latitude :  %s\n", RMC->Latitude);
         }
       else if (token_n == 3) {
            RMC->N_S =*temp;
            printf("N_S :  %c\n", RMC->N_S);
         }
       else if (token_n == 4) {
           memcpy(RMC->Longitude,temp,strlen(temp)+1);
            printf("Longitude :  %s\n", RMC->Longitude);
         }
        else if (token_n == 5) {
            RMC->E_W =*temp;
            printf("E_W :  %c\n", RMC->E_W);
         }
        else if (token_n == 6) {
           memcpy(RMC->Speed_knots,temp,strlen(temp)+1);
            printf("Speed_knots :  %s\n", RMC->Speed_knots);
         }
        /*else if (token_n == 7) {
           memcpy(RMC->Course_over_ground,temp,strlen(temp)+1);
            printf("Speed_knots :  %s\n", RMC->Course_over_ground);
         }*/
        
         else if (token_n == 7) {
           RMC->Date = atoi(temp);
            printf("Date :  %d\n", RMC->Date);
         }
         else if (token_n == 8) {
           memcpy(RMC->Magnetic_variation,temp,strlen(temp)+1);
            printf("Magnetic_variation :  %s\n", RMC->Magnetic_variation);
         }
         else if (token_n == 9) {
            RMC->E_W2 =*temp;
            printf("E_W2 :  %c\n", RMC->E_W2);
         }
          else if (token_n == 10) {
            RMC->Mode_indicator =*temp;
            printf("Mode_indicator :  %c\n", RMC->Mode_indicator);

         }
         else{
            RMC->Navigational_status =*temp;
            printf("Navigational_status :  %c\n", RMC->Navigational_status);

         }
   }
     token_n++;
      }
     

   return SUCCESS;




}





int GPS::Parse_XTE(const char* sentence){
    char buff[100];

   strcpy(buff,sentence);
   
   char * sentence_pointer;

   char * next_stence_pointer;
   int comp_length;

   char temp[100];

   int token_n=0;


   sentence_pointer = strstr(buff, ",");


   while (token_n != component_number_XTE) {

      XTE = new GPS_XTE;
      
      sentence_pointer = strstr(sentence_pointer, ",")+1;

      if (*sentence_pointer != ',')//연속 , 무시 
      {
         next_stence_pointer = strstr(sentence_pointer, ",") + 1;

         comp_length = next_stence_pointer - sentence_pointer;

         memset(temp, 0, 100);

         //input information to temp 
         if (comp_length > 0)

            memcpy(temp, sentence_pointer, comp_length - 1);


         else

            memcpy(temp, sentence_pointer, buff + strlen(buff)- sentence_pointer);
         
            // input information 
         if (token_n == 0) {

            XTE->Status_System = *temp;
            printf("Status_System :  %c\n", XTE->Status_System);
         }
         else if (token_n == 1) {
      
            XTE->Status_Data = *temp;
            printf("Status_Data :  %c\n", XTE->Status_Data);
         }
         else if (token_n == 2) {

            memcpy(XTE->Magnitude_of_cross_track_error, temp, strlen(temp) + 1);
            printf("Magnitude_of_cross_track_error :  %s\n", XTE->Magnitude_of_cross_track_error);

         }
         else if (token_n == 3) {

            XTE->Direction_to_steer = *temp;
            printf("Direction_to_steer :  %c\n", XTE->Direction_to_steer);

         }

         else if (token_n == 4) {

            XTE->Units = *temp;
            printf("Units :  %c\n", XTE->Units);

         }
         else {

            //XTE->Mode_indicator = atoi(temp);  // 숫자 외 다른 문자 만나기 전까지 정수로 변환 
            XTE->Mode_indicator = *temp;
            printf("Mode_indicator :  %c\n", XTE->Mode_indicator);
         }
         
      }

      token_n++;

   }

   
   return SUCCESS;



}

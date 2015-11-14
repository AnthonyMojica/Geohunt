/*
* MyLibrary.h
*
* Created: 11/14/2015 10:35:49 AM
*  Author: Anthony
*/

#include <string.h>
#include <stdio.h>
//#include <math.h>

#ifndef MYLIBRARY_H_
#define MYLIBRARY_H_

float DegLat, DegLong, COG;
char* NSInd;
char* EWInd;
char PrintInfo[256];

float CalculateDistance(float Long, float Lat, float EndLong, float EndLat)
{
	float result;
	result = pow(EndLong - Long,2)+ pow(EndLat - Lat,2);
	result = sqrt(result);
	
	result *= 100;
	result  *= 3280.8f;
	return result;
}


void ParseData(char* theData)
{
	char* parsed;
	const char s[2] = ",";
	
	//$GPRMC
	parsed = strtok(theData, s);
	//Date/Time
	parsed = strtok(NULL, s);
	//Valid data Indicator
	parsed = strtok(NULL, s);
	//Latitude
	parsed = strtok(NULL, s);
	float x = parsed[0] - '0';
	x *= 10;
	float y = parsed[1] - '0';
	x += y;
	
	
	float j = parsed[2] - '0';
	j *= 10;
	float k = parsed[3] - '0';
	j += k;
	
	float xx = parsed[5] - '0';
	xx *= 10;
	float yy = parsed[6] - '0';
	xx += yy;
	
	float jj = parsed[7] - '0';
	jj *= 10;
	float kk = parsed[8] - '0';
	jj += kk;
	
	xx *= 100; 
	xx += jj;
	xx *= .0001;
	DegLat = x + ((j+xx) / 60);
	
	//NorthSouth Indicator
	NSInd = strtok(NULL, s);
	
		if(EWInd[0] == 'S' || EWInd[0] == 's' )		
		DegLat *= -1;
		
	//Longitude
	parsed = strtok(NULL, s);
		float z = parsed[0] - '0';
		z *= 100;
		x = parsed[1] - '0';
		x *= 10;
		y = parsed[2] - '0';
		x += y;
		x += z;
		
		
		j = parsed[3] - '0';
		j *= 10;
		k = parsed[4] - '0';
		j += k;
		
		xx = parsed[6] - '0';
		xx *= 10;
		yy = parsed[7] - '0';
		xx += yy;
		
		jj = parsed[8] - '0';
		jj *= 10;
		kk = parsed[9] - '0';
		jj += kk;
		
		xx *= 100;
		xx += jj;
		
		xx *= .0001;
		
		DegLong = x + ((j+xx) / 60);
	
	
	//EastWest Indicator
	EWInd = strtok(NULL, s);
	
	
	if(EWInd[0] == 'W' || EWInd[0] == 'w' )
	DegLong *= -1;
	
		parsed = strtok(NULL, s);
		
		parsed = strtok(NULL, s);
		
		
		if(strlen(parsed) > 5)
		{
					x = parsed[0] - '0';
					x *= 100;
					y = parsed[1] - '0';
					y *= 10;
					j = parsed[2] - '0';
					k = parsed[4] - '0';
					k*= 0.1f;
					z = parsed[5] - '0';
					z*= 0.01f;
			
		}
		else if(strlen(parsed) > 4)
		{
			x = 0;
					y = parsed[0] - '0';
					y *= 10;
					j = parsed[1] - '0';
					k = parsed[3] - '0';
					k*= 0.1f;
					z = parsed[4] - '0';
					z*= 0.01f;
			
		}
		else
		{
						x = 0;
						y = 0;
						j = parsed[0] - '0';
						k = parsed[2] - '0';
						k*= 0.1f;
						z = parsed[3] - '0';
						z*= 0.01f;
		}

		
		x = x + y + j + k + z;
		
		COG = x;
		
		
	
}

float CalcBearing(float CurLong, float CurLat, float EndLong, float EndLat)
{
	//http://www.idevgames.com/forums/thread-3215.html
	float VecX = EndLong - CurLong;
	float VecY = EndLat	- CurLat;
	
	float angle = atan2(VecY, VecX);
	
	
}
#endif /* MYLIBRARY_H_ */
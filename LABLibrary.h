/*
 * LABLIBrary.h
 *
 * Created: 7/28/2015 4:01:27 PM
 *  Author: rarndt
 */ 

 /*
This header corresponds to functions contained within
the libLABLibrary.a object file.  You may add this
library object file as a library to your project and
test the results of your functions to the results
of these library functions. 

The libLABLibrary.a file must be removed from your
project before performing your GeoCache Hunt.
 */
#ifndef LABLIBRARY_H_
#define LABLIBRARY_H_

#include "Arduino.h"

/**************************************************
Convert degrees to radians

float deg2rad(float fdeg)

Input:
	fdeg	degrees
	
Return:
	radians
	
**************************************************/
float deg2rad(float fdeg);

/**************************************************
Convert Degrees Minutes (DDMM.MMMM) into Decimal Degrees (DDD.DDDD)

float degMin2DecDeg(char *cind, char *ccor)

Input:
	cind = string char pointer containing the GPRMC latitude(N/S) or longitude (E/W) indicator
	ccor = string char pointer containing the GPRMC latitude or longitude DDDMM.MMMM coordinate

Return:
	Decimal degrees coordinate.
	
**************************************************/
float degMin2DecDeg(char *cind, char *ccor);

/************************************************** 
Calculate Great Circle Distance between to coordinates

float getDistance(float flat1, float flon1, float flat2, float flon2)

EARTH_RADIUS_FEET = 3959.00 radius miles * 5280 feet per mile

Input:
	flat1, flon1 = first latitude and longitude coordinate in decimal degrees
	flat2, flon2 = second latitude and longitude coordinate in decimal degrees

Return:
	distance in feet (3959 earth radius in miles * 5280 feet per mile)
**************************************************/
float calcDistance(float flat1, float flon1, float flat2, float flon2);

/**************************************************
Calculate Great Circle Bearing between two coordinates

float getBearing(float flat1, float flon1, float flat2, float flon2)

Input:
	flat1, flon1 = first latitude and longitude coordinate in decimal degrees
	flat2, flon2 = second latitude and longitude coordinate in decimal degrees

Return:
	angle in degrees from magnetic north
**************************************************/
float calcBearing(float flat1, float flon1, float flat2, float flon2);

#endif /* LABLIBRARY_H_ */
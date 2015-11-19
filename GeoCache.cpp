/******************************************************************************

GeoCache Hunt Project (GeoCache.cpp)

This is skeleton code provided as a project development guideline only.  You
are not required to follow this coding structure.  You are free to implement
your project however you wish.

List Team Members Here:

1.
2.
3.
4.

NOTES:

You only have 32k of program space and 2k of data space.  You must
use your program and data space wisely and sparingly.  You must also be
very conscious to properly configure the digital pin usage of the boards,
else weird things will happen.

The Arduino GCC sprintf() does not support printing floats or doubles.  You should
consider using sprintf(), dtostrf(), strtok() and strtod() for message string
parsing and converting between floats and strings.

The GPS provides latitude and longitude in degrees minutes format (DDDMM.MMMM).
You will need convert it to Decimal Degrees format (DDD.DDDD).  The switch on the
GPS Shield must be set to the "Soft Serial" position, else you will not receive
any GPS messages.

INFO:

The AdaFruit Ultimate GPS Shield
https://learn.adafruit.com/downloads/pdf/adafruit-ultimate-gps-logger-shield.pdf

Links on coordinates and calculations
http://www.gpsinformation.org/dale/nav.htm
http://www.csgnetwork.com/gpscoordconv.html
http://www.movable-type.co.uk/scripts/latlong.html
http://www.rapidtables.com/convert/number/degrees-minutes-seconds-to-degrees.htm
http://www.rapidtables.com/convert/number/degrees-to-degrees-minutes-seconds.htm

*******************************************************************************

Following is the GPS Shield "GPRMC" Message Structure.  This message is received
once a second.  You must parse the message to obtain the parameters required for
the GeoCache project.  GPS provides coordinates in Degrees Minutes (DDDMM.MMMM).
The coordinates in the following GPRMC sample message, after converting to Decimal
Degrees format(DDD.DDDDDD) is latitude(23.118757) and longitude(120.274060).  By
the way, this coordinate is GlobaTop Technology in Tiawan, who designed and
manufactured the GPS Chip.

"$GPRMC,064951.000,A,2307.1256,N,12016.4438,E,0.03,165.48,260406,3.05,W,A*2C/r/n"

$GPRMC,         // GPRMC Message
064951.000,     // utc time hhmmss.sss
A,              // status A=data valid or V=data not valid
2307.1256,      // Latitude 2307.1256 (degrees minutes format dddmm.mmmm)
N,              // N/S Indicator N=north or S=south
12016.4438,     // Longitude 12016.4438 (degrees minutes format dddmm.mmmm)
E,              // E/W Indicator E=east or W=west
0.03,           // Speed over ground knots
165.48,         // Course over ground (decimal degrees format ddd.dd)
260406,         // date ddmmyy
3.05,           // Magnetic variation (decimal degrees format ddd.dd)
W,              // E=east or W=west
A               // Mode A=Autonomous D=differential E=Estimated
*2C             // checksum
/r/n            // return and newline

******************************************************************************/

// Required
#include "Arduino.h"
#include "MyLibrary.h"
/*
Configuration settings (optional usage).

You do not need to use these defines, but it makes it easier to enable
certain capabilities for debugging during the development cycle.  There
may not be sufficient room in the FLASH or DATA memory to enable all these
libraries at the same time.  You are only permitted to have NEO_ON, ONE_ON
and SDC_ON during the GeoCache Treasure Hunt.
*/
#define NEO_ON 1		// NeoPixelShield
#define TRM_ON 1		// SerialTerminal
#define ONE_ON 0		// 1Sheeld
#define SDC_ON 1		// SecureDigital
#define GPS_ON 1		// GPSShield (off = simulated)

// define pin usage
#define NEO_TX	6		// NEO transmit
#define GPS_TX	7		// GPS transmit
#define GPS_RX	8		// GPS receive

// GPS message buffer
#define GPS_RX_BUFSIZ	128
char cstr[GPS_RX_BUFSIZ];

#if GPS_ON
#include "SoftwareSerial.h"
SoftwareSerial gps(GPS_RX, GPS_TX);
#endif

// library includes
#if NEO_ON
#include "NeoPixel.h"
Adafruit_NeoPixel strip = Adafruit_NeoPixel(40, NEO_TX, NEO_GRB + NEO_KHZ800);
#endif

#if ONE_ON
#define CUSTOM_SETTINGS
#define INCLUDE_TERMINAL_SHIELD
#include "OneSheeld.h"
#endif

#if SDC_ON
#include "SecureDigital.h"
File dataFile;
#endif

/*
Below is a Decimal Degrees formatted waypoint for the large tree
in the parking lot just outside the front entrance of FS3B-116.
*/

// FS3B-116 large tree outside front door
#define GEOLAT0 28.594532
#define GEOLON0 -81.304437

#if GPS_ON
/*
These are GPS command messages (only a few are used).
*/
#define PMTK_AWAKE "$PMTK010,002*2D"
#define PMTK_STANDBY "$PMTK161,0*28"
#define PMTK_Q_RELEASE "$PMTK605*31"
#define PMTK_ENABLE_WAAS "$PMTK301,2*2E"
#define PMTK_ENABLE_SBAS "$PMTK313,1*2E"
#define PMTK_CMD_HOT_START "$PMTK101*32"
#define PMTK_CMD_WARM_START "$PMTK102*31"
#define PMTK_CMD_COLD_START "$PMTK103*30"
#define PMTK_CMD_FULL_COLD_START "$PMTK104*37"
#define PMTK_SET_BAUD_9600 "$PMTK251,9600*17"
#define PMTK_SET_BAUD_57600 "$PMTK251,57600*2C"
#define PMTK_SET_NMEA_UPDATE_1HZ  "$PMTK220,1000*1F"
#define PMTK_SET_NMEA_UPDATE_5HZ  "$PMTK220,200*2C"
#define PMTK_API_SET_FIX_CTL_1HZ  "$PMTK300,1000,0,0,0,0*1C"
#define PMTK_API_SET_FIX_CTL_5HZ  "$PMTK300,200,0,0,0,0*2F"
#define PMTK_SET_NMEA_OUTPUT_RMC "$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29"
#define PMTK_SET_NMEA_OUTPUT_GGA "$PMTK314,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29"
#define PMTK_SET_NMEA_OUTPUT_RMCGGA "$PMTK314,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28"
#define PMTK_SET_NMEA_OUTPUT_OFF "$PMTK314,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28"
#endif

float theDist;
float Hopefully0;

#if NEO_ON

bool Debounce(unsigned int thePin);


//uint32_t RED = strip.Color(255,0,0);
//uint32_t GREEN = strip.Color(0,255,0);
//uint32_t BLUE = strip.Color(0,0,255);
//uint32_t ORANGE = strip.Color(255,100,0);
//uint32_t YELLOW = strip.Color(255,255,0);
//uint32_t WHITE = strip.Color(255,255,255);
//uint32_t NOLIGHT = strip.Color(0,0,0);
/*
Sets target number, heading and distance on NeoPixel Display
*/
void setNeoPixel()
{
	strip.setPixelColor(0,strip.Color(255,255,255));
	strip.setPixelColor(7,strip.Color(255,255,255));
	strip.setPixelColor(1,strip.Color(255,255,255));
	strip.setPixelColor(2,strip.Color(255,0,0));
	strip.setPixelColor(3,strip.Color(255,0,0));
	strip.setPixelColor(4,strip.Color(255,0,0));
	strip.setPixelColor(5,strip.Color(255,0,0));
	strip.setPixelColor(6,strip.Color(255,255,255));

	strip.setPixelColor(currentCoord + 2, strip.Color(0,255,0));
	uint32_t CurrentColor = strip.Color(0,255,0);
	
	if(theDist > 255)
	{
	theDist *= 0.30f;
	CurrentColor = strip.Color(255,255,0);
	}
	if(theDist > 255)
	{
	theDist *= 0.01f;
	CurrentColor = strip.Color(50,0,255);
	}

	uint8_t BinDist = (uint8_t)theDist;

	for(int i = 0; i < 8; i++)
	{
		if(BinDist & (1<<i))
		strip.setPixelColor(i+8, CurrentColor);
		else
		strip.setPixelColor(i+8, strip.Color(0,0,0));
	}

	strip.setPixelColor(16,0);
	strip.setPixelColor(17,0);
	strip.setPixelColor(18,0);
	strip.setPixelColor(24,0);
	strip.setPixelColor(25,strip.Color(255,255,255));
	strip.setPixelColor(26,0);
	strip.setPixelColor(32,0);
	strip.setPixelColor(33,0);
	strip.setPixelColor(34,0);

	if(Hopefully0 > 293 && Hopefully0 < 338)
	strip.setPixelColor(16,strip.Color(0,0,255));
	else if((Hopefully0 > 338 && Hopefully0 < 360) || (Hopefully0 > 0 && Hopefully0 < 22))
	strip.setPixelColor(17,strip.Color(0,0,255));
	else if(Hopefully0 > 22 && Hopefully0 < 67)
	strip.setPixelColor(18,strip.Color(0,0,255));
	
	else if(Hopefully0 >248 && Hopefully0 < 292)
	strip.setPixelColor(24,strip.Color(0,0,255));
	
	else if(Hopefully0 >67 && Hopefully0 < 112)
	strip.setPixelColor(26,strip.Color(0,0,255));
	
	
	else if(Hopefully0 >203 && Hopefully0 < 248)
	strip.setPixelColor(32,strip.Color(0,0,255));
	else if(Hopefully0 >158 && Hopefully0 < 203)
	strip.setPixelColor(33,strip.Color(0,0,255));
	else if(Hopefully0 >112 && Hopefully0 < 158)
	strip.setPixelColor(34,strip.Color(0,0,255));

	strip.show();
}

#endif

#if GPS_ON
/*
Get valid GPS message. This function returns ONLY once a second.

void getGPSMessage(void)

Side affects:
Message is placed in global "cstr" string buffer.

Input:
none

Return:
none

*/
void getGPSMessage(void)
{
	uint8_t x=0, y=0, isum=0;

	memset(cstr, 0, sizeof(cstr));
	
	// get name string
	while (true)
	{
		if (gps.peek() != -1)
		{
			cstr[x] = gps.read();
			
			// if multiple inline messages, then restart
			if ((x != 0) && (cstr[x] == '$'))
			{
				x = 0;
				cstr[x] = '$';
			}
			
			// if complete message
			if ((cstr[0] == '$') && (cstr[x++] == '\n'))
			{
				// nul terminate string before /r/n
				cstr[x-2] = 0;

				// if checksum not found
				if (cstr[x-5] != '*')
				{
					x = 0;
					continue;
				}
				
				// convert hex checksum to binary
				isum = strtol(&cstr[x-4], NULL, 16);
				
				// reverse checksum
				for (y=1; y < (x-5); y++) isum ^= cstr[y];
				
				// if invalid checksum
				if (isum != 0)
				{
					x = 0;
					continue;
				}
				
				// else valid message
				break;
			}
		}
	}
}

#else
/*
Get simulated GPS message once a second.

This is the same message and coordinates as described at the top of this
file.  You could edit these coordinates to point to the tree out front (GEOLAT0,
GEOLON0) to test your distance and direction calculations.  Just note that the
tree coordinates are in Decimal Degrees format, and the message coordinates are
in Degrees Minutes format.

void getGPSMessage(void)

Side affects:
Static GPRMC message is placed in global "cstr" string buffer.

Input:
none

Return:
none

*/
void getGPSMessage(void)
{
	static unsigned long gpsTime = 0;
	
	// simulate waiting for message
	while (gpsTime > millis()) delay(100);
	
	// do this once a second
	gpsTime = millis() + 1000;
	
	memcpy(cstr, "$GPRMC,064951.000,A,2307.1256,N,12016.4438,E,0.03,165.48,260406,3.05,W,A*2C", sizeof(cstr));

	return;
}
#endif

/*
Main Program Entry

int main(void)

Input:
none

Return:
false

*/
int main(void)
{
	// variables
	init();
	initiateCoords();
	strip.begin();
	strip.setBrightness(12);
	strip.show();
	// init target button
	pinMode(4, INPUT_PULLUP);
	#if TRM_ON
	Serial.begin(115200);
	#endif
	
	#if ONE_ON
	OneSheeld.begin();
	#endif
	
	#if NEO_ON
	// init NeoPixel Shield
	#endif

	#if SDC_ON
	/*
	Initialize the SecureDigitalCard and open a numbered sequenced file
	name "MyMapNN.txt" for storing your coordinates, where NN is the
	sequential number of the file.  The filename can not be more than 8
	chars in length (excluding the ".txt").
	*/
	SD.begin();
	
	int count = 0;
	char filename[8];
	while(true)
	{
		sprintf(filename,"File%i", count );	
		if(SD.exists(filename))
			count++;
		else 
		break;
	}

		
	dataFile = SD.open(filename, FILE_WRITE);
	Serial.println(filename);

	#endif
	
	// enable GPS sending GPRMC message
	#if GPS_ON
	gps.begin(9600);
	gps.println(PMTK_SET_NMEA_UPDATE_1HZ);
	gps.println(PMTK_API_SET_FIX_CTL_1HZ);
	gps.println(PMTK_SET_NMEA_OUTPUT_RMC);
	#endif
	
	bool buttondown = false;

	
	while (true)
	{
		// returns with message once a second
		getGPSMessage();
		
		if(Debounce(4))
		{
			if(!buttondown) // Do Once
			{
				ChangeDest();
				buttondown	= true;
			}
		}
		else
		{
			buttondown = false;
		}
		
		
		// if GPRMC message (3rd letter = R)
		while (cstr[3] == 'R')
		{
			// parse message parameters
			
			// calculated destination heading
			
			// calculated destination distance
			
			#if SDC_ON
			// write current position to SecureDigital then flush
			#endif

			break;
		}
		
		// set NeoPixel target display
		#if NEO_ON
		#endif
		
		#if ONE_ON
		// print debug information to OneSheeld Terminal
		#endif

		#if 1
		// print debug information to Serial Terminal
		//Serial.println(cstr);
		ParseData(cstr);
		theDist = CalculateDistance(DegLong, DegLat, theCoords[currentCoord].Lon ,theCoords[currentCoord].Lat);
		
		
		Hopefully0 = CalcBearing(DegLong, DegLat,  theCoords[currentCoord].Lon ,theCoords[currentCoord].Lat);
		
		Hopefully0 -= COG;
		
		Hopefully0 = fmod(Hopefully0, 360);
		if(Hopefully0 < 0)
		Hopefully0 += 360;
		
		char lat[12];
		dtostrf(DegLat, 4,6,lat);
		char lon[12];
		dtostrf(DegLong, 4,6,lon);
		//char co[12];
		//dtostrf(COG, 5,2,co);
		char dis[24];
		dtostrf(theDist, 4,6,dis);
		//char bearit[12];
		//dtostrf(Hopefully0, 4,4,bearit);
		//sprintf(cstr, "Lat: %s%s Lon: %s%s COG: %s Dist: %s Bear: %s", lon, lat, dis);
		sprintf(cstr, "%s, %s, %s", lon, lat, dis);
		//Serial.println(PrintInfo);
		//Serial.println(theDist);
		
		//Terminal.println(PrintInfo);
		

		////Terminal.println(dis);

		
		////Terminal.println(bearit);
		#endif
		
		setNeoPixel();
		
		#if SDC_ON
		// make a string for assembling the data to log:
		dataFile.println(cstr);
		//	  Terminal.println(cstr);
		dataFile.flush();
		#endif


		// if button pressed, set new target

		if (serialEventRun) serialEventRun();
	}
	//dataFile.close();
	return(false);
}

bool Debounce(unsigned int thePin)
{
	
	for(int i = 0; i < 100; i++)
	{
		if(digitalRead(thePin) == HIGH)
		return false;
	}
	
	return true;
}

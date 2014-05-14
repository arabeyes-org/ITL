#ifndef CITIES_CC
#define CITIES_CC

#define NCITIES (sizeof Cities / sizeof(Cities[0]))

  typedef struct
  {
    char *CityName;
    char *DegreeLong;    /* Longitude in decimal degree. */
    char *DegreeLat;     /* Latitude in decimal degree. */
    double GMTdiff;       /* GMT difference. */
    double SeaLev;        /* Height above Sea level (in meters) */
    bool DST;           /* Daylight savings time switch ( false if not used ).
                            Setting this to true should add true hour to all
                             the calculated prayer times */
  } sCityLocation ;

    sCityLocation Cities[] = {
		{ _("Makkah"),"21.4200","39.8300",3,0,false }
		,{ _("Madena"),"31.4200","49.8300",5,5,true }
		,{ _("Quds"),"21.4200","39.8300",3,0,false }
		,{ _("Hofuf"),"21.4200","39.8300",3,0,false}
		};
	
#endif //CITIES_CC

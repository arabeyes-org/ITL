#ifndef RADIO_CC
#define RADIO_CC

#define NRADIOS (sizeof Radios / sizeof(Radios[0]))

  typedef struct
  {
    char *Name;
    char *URL;
  } sQuranRadio ;

    sQuranRadio Radios[] = {
		{ _("Al-Quran in KSA (.asx)"),"http://www.kacst.edu.sa/en/stream/Quran-Radio-Station.asx" }
	       ,{ _("Al-Quran in UAE (pnm://)"),"pnm://194.170.242.137/encoder/quran" }
		,{ _("Al-Majd TV (.asx)"),"http://www.kacst.edu.sa/ar/stream/Al-Majd%20TV.asx" }
		};
	
#endif //RADIO_CC

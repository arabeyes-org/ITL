##  Prayer time calculation methods

The "getMethod" library function auto-fills a Method structure given a
method index number (the number preceeding the method name in the list
below). In case a specific method of calculation or the desired Fiqh
school values are not available below, it is still possible to
override or explicitly set any or all the values of the Method
structure. For the complete set of options and variables provided by
the Method structure, see the file "prayer.h".

        
Usage Example: 

```        
getMethod (method_index_number, &method)
```
        
Method List: 
        
1. Egyptian General Authority of Survey
	* Fajr Angle: 20
	* Ishaa Angle: 18
	* Used in: Indonesia, Iraq, Jordan, Lebanon, Malaysia, Singapore, Syria, parts of Africa, parts of the United States

2. University of Islamic Sciences, Karachi (Shaf'i)
	* Fajr Angle: 18
	* Ishaa Angle: 18
	* Used in: Iran, Kuwait, parts of Europe                                           
                            
3. University of Islamic Sciences, Karachi (Hanafi)
	* Fajr Angle: 18
	* Ishaa Angle: 18
	* Used in: Afghanistan, Bangladesh, India

4. Islamic Society of North America
	* Fajr Angle: 15
	* Ishaa Angle: 15
	* Used in: Canada, parts of the UK, parts of the United States

5. Muslim World League (MWL)
	* Fajr Angle: 18
	* Ishaa Angle: 17
	* Used in: parts of Europe, the far East, parts of the United States

6. Om Al-Qurra University
	* Fajr Angle: 18
	* Ishaa Angle: 0 (not used)
	* Ishaa Interval: 90 minutes from Maghrib prayer
	* Used in: Saudi Arabia

7. Fixed Ishaa Angle Interval
	* Fajr Angle: 19.5
	* Ishaa Angle: 0 (not used)
	* Ishaa Interval: 90 minutes from Maghrib prayer
	* Used in: Bahrain, Oman, Qatar, United Arab Emirates

8. Egyptian General Authority of Survey (Egypt)
	* Fajr Angle: 19.5
	* Ishaa Angle: 17.5
	* Used in: Egypt

9. Om Al-Qurra University (Ramadan)
	* Fajr Angle: 19
	* Ishaa Angle: 0 (not used)
	* Ishaa Interval: 120 minutes from Maghrib prayer
	* Used in: Saudi Arabia (during Ramadan)

10. Moonsighting Committee Worldwide (MWC)
	* Fajr Angle: 18 (used for comparison)
	* Ishaa Angle: 18 (used for comparison)
	* Fajr and Isha calculated with different values based on the season
	* Used in: parts of the United States, parts of the UK

11. Morocco Awqaf Ministry
	* Fajr Angle: 19
	* Ishaa Angle: 17
	* Zuhr Offset: 5 min
	* Maghrib Offset: 5 min
	* Used in: Morocco


##  Notes on the Angle Table Used Above

The ITL uses the above organization names in a very "tentative"
manner, as no contacts have been made to obtain the correct (or
up-to-date) numbers as published by such organizations. Different
sources (like books on prayer times, manuals of Athan clocks, and
online prayer time calculators) often present conflicting numbers and
country/value pairs. To deal with this shortcoming and still preserve
user expectations, we have opted to use a combination of methods with
slightly qualified names. In the future, new efforts
need to be undertaken to contact such organizations, or find
well-referenced documents showing what their official stance is.

As a precautionary measure, the library allows for full customization
of angle values (see the description of the "Method* conf"
variable). Users in locations with no consensus on Fajr/Ishaa
angles, and no data based on actual observation that these angle
values intend to emulate, can pass custom values to the library based
on their own observations or local mosque recommendations. This
approach, at least from a developer's perspective, is intended to
sidestep this whole debate until more observation data is published.

Other suggestions or county-specific information are welcome.


## High Latitude Calculation

At certain locations and times of year, some prayer times do not occur
or otherwise are impossible to precisely calculate using conventional
means. The selected extreme method will be used if the normal calculations
return an invalid value (this generally happens at latitudes of 49 and above)
or if the latitude is above the "extreme latitude" value. By default 
"extreme latitude" is set to 55 degrees, but this value can be customized 
by setting the the "Method.extremeLat" variable.

Method Category Information:

* Nearest Latitude (Aqrab Al-Bilaad): Calculate a prayer time
 using a safe latitude value. The recommended latitude by
 many schools of Fiqh is 48.5 degrees, but you can customize
 this by setting the "Method.nearestLat" variable.

* Nearest Good Day (Aqrab Al-Ayyam): The library determines
 the closest previous or next day that the Fajr and Ishaa
 times occur and are both valid.

* An [amount] of Night and Day: Unlike the above mentioned
 methods, the multiple methods in this category have no proof
 in traditional Shari'a (Fiqh) resources. These methods were
 introduced by modern day Muslim scholars and scientists for
 practical reasons only.

* Minutes from Shurooq/Maghrib: Use an interval time to
 calculate Fajr and Ishaa. This will set the values of Fajr
 and Ishaa to the same as the computed Shurooq and Maghrib
 respectively, then add or subtract the amount of minutes
 found in the "Method.fajrInv" and "Method.ishaaInv"
 variables.

Method List:
      
0. None: If unable to calculate, leave only the invalid prayer
        time as 99:99.
          
1. Nearest Latitude: Apply to all prayer times always.

2. Nearest Latitude: Apply to Fajr and Ishaa times always.

3. Nearest Latitude: Apply to Fajr and Ishaa times but only if
                    the library has detected that the current
                    Fajr or Ishaa time is invalid.

4. Nearest Good Day: Apply to all prayer times always.

5. Nearest Good Day: Apply to Fajr and Ishaa times but only if
                    the library has detected that the current
                    Fajr or Ishaa time is invalid. This is the
                    default method. (**Default**)

6. 1/7th of Night: Apply to Fajr and Ishaa times always.

7. 1/7th of Night: Apply to Fajr and Ishaa times but only if
                    the library has detected that the current
                    Fajr or Ishaa time is invalid.

8. 1/7th of Day: Apply to Fajr and Ishaa times always.

9. 1/7th of Day: Apply to Fajr and Ishaa times but only if the
                    library has detected that the current Fajr
                    or Ishaa time is invalid.

10. Half of the Night: Apply to Fajr and Ishaa times always.

11. Half of the Night: Apply to Fajr and Ishaa times but only
                      if the library has detected that the
                      current Fajr or Ishaa time is
                      invalid.

12. Minutes From... : Apply to Fajr and Ishaa times always.

13. Minutes From... : Apply to Fajr and Ishaa times but only if
                     the library has detected that the
                     current Fajr or Ishaa time is invalid.
                     
14. Nearest Good Day: Apply to both Fajr and Ishaa times if
                     either Fajr or Ishaa time is invalid.

15. Angle Based: Portion of the night based on Fajr and Ishaa angles.
                      Applies to Fajr and Ishaa if either times are invalid.



## References On High and Extreme Calculations:

Tariq Muneer (n.d.), The Islamic Prayer Times a Computational
Philosophy with Particular Reference to the Lack of Twilight Cessation
at Higher Latitudes.

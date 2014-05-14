/* locations-xml.h 
 *
 * Copyright (C) 2004 Gareth Owen
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

struct _WeatherLocation {
    gchar *name;
    gchar *code;
    gchar *zone;
    gchar *radar;
    gboolean zone_valid;
    gchar *coordinates;
    gdouble  latitude;
    gdouble  longitude;
    gboolean latlon_valid;
};
typedef struct _WeatherLocation WeatherLocation;

WeatherLocation *	weather_location_new 	(const gchar *trans_name,
						 const gchar *code,
						 const gchar *zone,
						 const gchar *radar,
						 const gchar *coordinates);

gboolean		weather_location_equal	(const WeatherLocation *location1,
						 const WeatherLocation *location2);

WeatherLocation *	weather_location_clone	(const WeatherLocation *location);

enum
{
    GWEATHER_XML_COL_LOC = 0,
    GWEATHER_XML_COL_POINTER,
    GWEATHER_XML_NUM_COLUMNS
}; 

int gweather_xml_load_locations( GtkTreeView *tree, WeatherLocation *current );



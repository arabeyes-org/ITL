/* locations-xml.c - Locations.xml parsing code
 *
 * Copyright (C) 2005 Ryan Lortie, 2004 Gareth Owen
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

#include <string.h>
#include <math.h>
#include <ctype.h>
#include <locale.h>
#include <gtk/gtk.h>
#include <libxml/xmlreader.h>

#include "locations-xml.h"
#include "defines.h"

static gint
gweather_xml_location_sort_func( GtkTreeModel *model, GtkTreeIter *a,
                                 GtkTreeIter *b, gpointer user_data )
{
    gint res;
    gchar *name_a, *name_b;
    gchar *fold_a, *fold_b;
        
    gtk_tree_model_get (model, a, GWEATHER_XML_COL_LOC, &name_a, -1);
    gtk_tree_model_get (model, b, GWEATHER_XML_COL_LOC, &name_b, -1);
        
    fold_a = g_utf8_casefold(name_a, -1);
    fold_b = g_utf8_casefold(name_b, -1);
        
    res = g_utf8_collate(fold_a, fold_b);
    
    g_free(name_a);
    g_free(name_b);
    g_free(fold_a);
    g_free(fold_b);
    
    return res;
}
 
static char*
gweather_xml_get_value( xmlTextReaderPtr xml )
{
  char* value;

  /* check for null node */
  if ( xmlTextReaderIsEmptyElement( xml ) )
    return NULL;
    
  /* the next "node" is the text node containing the value we want to get */
  if( xmlTextReaderRead( xml ) != 1 )
    return NULL;

  value = (char *) xmlTextReaderValue( xml );

  /* move on to the end of this node */
  while( xmlTextReaderNodeType( xml ) != XML_READER_TYPE_END_ELEMENT )
    if( xmlTextReaderRead( xml ) != 1 )
    {
      xmlFree( value );
      return NULL;
    }

  /* consume the end element too */
  if( xmlTextReaderRead( xml ) != 1 )
  {
    xmlFree( value );
    return NULL;
  }
    
  return value;
}

static char *
gweather_xml_parse_name( xmlTextReaderPtr xml )
{
  const char * const *locales;
  const char *this_language;
  int best_match = INT_MAX;
  char *lang, *tagname;
  gboolean keep_going;
  char *name = NULL;
  int i;

  locales = g_get_language_names();

  do
  {
    /* First let's get the language */
    lang = (char *) xmlTextReaderXmlLang( xml );

    if( lang == NULL )
      this_language = "C";
    else
      this_language = lang;

    /* the next "node" is text node containing the actual name */
    if( xmlTextReaderRead( xml ) != 1 )
    {
      xmlFree( lang );
      return NULL;
    }

    for( i = 0; locales[i] && i < best_match; i++ )
      if( !strcmp( locales[i], this_language ) )
      {
        /* if we've already encounted a less accurate
           translation, then free it */
        xmlFree( name );

        name = (char *) xmlTextReaderValue( xml );
        best_match = i;

        break;
      }

    xmlFree( lang );

    while( xmlTextReaderNodeType( xml ) != XML_READER_TYPE_ELEMENT )
      if( xmlTextReaderRead( xml ) != 1 )
      {
        xmlFree( name );
        return NULL;
      }

    /* if the next tag is another <name> then keep going */
    tagname = (char *) xmlTextReaderName( xml );
    keep_going = !strcmp( tagname, "name" );
    xmlFree( tagname );

  } while( keep_going );

  return name;
}

static int
gweather_xml_parse_node (GtkTreeView *view, GtkTreeIter *parent,
                         xmlTextReaderPtr xml, WeatherLocation *current,
                         const char *dflt_radar, const char *dflt_zone,
                         const char *cityname)
{
  GtkTreeStore *store = GTK_TREE_STORE( gtk_tree_view_get_model( view ) );
  char *name, *code, *zone, *radar, *coordinates;
  char **city, *nocity = NULL;
  GtkTreeIter iter, *self;
  gboolean is_location;
  char *tagname;
  int ret = -1;
  int tagtype;

  if( (tagname = (char *) xmlTextReaderName( xml )) == NULL )
    return -1;

  if( !strcmp( tagname, "city" ) )
    city = &name;
  else
    city = &nocity;

  is_location = !strcmp( tagname, "location" );

  /* if we're processing the top-level, then don't create a new iter */
  if( !strcmp( tagname, "gweather" ) )
    self = NULL;
  else
  {
    self = &iter;
    /* insert this node into the tree */
    gtk_tree_store_append( store, self, parent );
  }

  xmlFree( tagname );

  coordinates = NULL;
  radar = NULL;
  zone = NULL;
  code = NULL;
  name = NULL;

  /* absorb the start tag */
  if( xmlTextReaderRead( xml ) != 1 )
    goto error_out;

  /* start parsing the actual contents of the node */
  while( (tagtype = xmlTextReaderNodeType( xml )) !=
         XML_READER_TYPE_END_ELEMENT )
  {

    /* skip non-element types */
    if( tagtype != XML_READER_TYPE_ELEMENT )
    {
      if( xmlTextReaderRead( xml ) != 1 )
        goto error_out;

      continue;
    }
    
    tagname = (char *) xmlTextReaderName( xml );

    if( !strcmp( tagname, "region" ) || !strcmp( tagname, "country" ) ||
        !strcmp( tagname, "state" ) || !strcmp( tagname, "city" ) ||
        !strcmp( tagname, "location" ) )
    {
      /* recursively handle sub-sections */
      if( gweather_xml_parse_node( view, self, xml, current,
                                   radar, zone, *city ) )
        goto error_out;
    }
    else if ( !strcmp( tagname, "name" ) )
    {
      xmlFree( name );
      if( (name = gweather_xml_parse_name( xml )) == NULL )
        goto error_out;
    }
    else if ( !strcmp( tagname, "code" ) )
    {
      xmlFree( code );
      if( (code = gweather_xml_get_value( xml )) == NULL )
        goto error_out;
    }
    else if ( !strcmp( tagname, "zone" ) )
    {
      xmlFree( zone );
      if( (zone = gweather_xml_get_value( xml )) == NULL )
        goto error_out;
    }
    else if ( !strcmp( tagname, "radar" ) )
    {
      xmlFree( radar );
      if( (radar = gweather_xml_get_value( xml )) == NULL )
        goto error_out;
    }
    else if ( !strcmp( tagname, "coordinates" ) )
    {
      xmlFree( coordinates );
      if( (coordinates = gweather_xml_get_value( xml )) == NULL )
        goto error_out;
    }
    else /* some strange tag */
    {
      /* skip past it */
      if( xmlTextReaderRead( xml ) != 1 )
        goto error_out;
    }

    xmlFree( tagname );
  }

  if( self )
    gtk_tree_store_set( store, self, GWEATHER_XML_COL_LOC, name, -1 );

  /* absorb the end tag.  in the case of processing a <gweather> then 'self'
     is NULL.  In this case, we let this fail since we might be at EOF */
  if( xmlTextReaderRead( xml ) != 1 && self )
    goto error_out;

  /* if this is an actual location, setup the WeatherLocation for it */
  if( is_location )
  {
    WeatherLocation *new_loc;

    if( cityname == NULL )
      cityname = name;

    if( radar != NULL )
      dflt_radar = radar;

    if( zone != NULL )
      dflt_zone = zone;

    new_loc =  weather_location_new( cityname, code, dflt_zone,
                                     dflt_radar, coordinates );

    gtk_tree_store_set( store, &iter, GWEATHER_XML_COL_POINTER, new_loc, -1 );

    /* If this location is actually the currently selected one, select it */
  /*
  if( current && weather_location_equal( new_loc, current ) )
    {
      GtkTreePath *path;

      path = gtk_tree_model_get_path( GTK_TREE_MODEL (store), &iter );
      gtk_tree_view_expand_to_path( view, path );
      gtk_tree_view_set_cursor( view, path, NULL, FALSE );
      gtk_tree_view_scroll_to_cell( view, path, NULL, TRUE, 0.5, 0.5 );
      gtk_tree_path_free( path );
    }*/
  }
  

  ret = 0;

error_out:
  xmlFree( name );
  xmlFree( code );
  xmlFree( zone );
  xmlFree( radar );
  xmlFree( coordinates );

  return ret;
}

/*****************************************************************************
 * Func:    gweather_xml_load_locations()
 * Desc:    Main entry point for loading the locations from the XML file
 * Parm:
 *      *tree:      tree to view locations
 *      *current:   currently selected location
 */
int
gweather_xml_load_locations( GtkTreeView *tree, WeatherLocation *current )
{
  char *tagname, *format;
  GtkTreeSortable *sortable;
  xmlTextReaderPtr xml;
  int keep_going;
  int ret = -1;

  /* Open the xml file containing the different locations */
  xml = xmlNewTextReaderFilename (g_build_filename(MINBAR_DATADIR,"Locations.xml",NULL));
  
  if( xml == NULL )
  {
    goto error_out;
  }

  /* fast forward to the first element */
  do
  {
    /* if we encounter a problem here, exit right away */
    if( xmlTextReaderRead( xml ) != 1 )
      goto error_out;
  } while( xmlTextReaderNodeType( xml ) != XML_READER_TYPE_ELEMENT );

  /* check the name and format */
  tagname = (char *) xmlTextReaderName( xml );
  keep_going = tagname && !strcmp( tagname, "gweather" );
  xmlFree( tagname );

  if( !keep_going )
    goto error_out;

  format = (char *) xmlTextReaderGetAttribute( xml, (xmlChar *) "format" );
  keep_going = format && !strcmp( format, "1.0" );
  xmlFree( format );

  if( !keep_going )
    goto error_out;

  ret = gweather_xml_parse_node( tree, NULL, xml, current, NULL, NULL, NULL );

  if( ret )
    goto error_out;

  /* Sort the tree */
  sortable = GTK_TREE_SORTABLE (gtk_tree_view_get_model( tree ));
  gtk_tree_sortable_set_default_sort_func( sortable,
                                           &gweather_xml_location_sort_func,
                                           NULL, NULL);
  gtk_tree_sortable_set_sort_column_id( sortable, 
                                     GTK_TREE_SORTABLE_DEFAULT_SORT_COLUMN_ID,
                                        GTK_SORT_ASCENDING );
error_out:
  xmlFreeTextReader( xml );

  return ret;
}

/*
 * Convert string of the form "DD-MM-SSH" to radians
 * DD:degrees (to 3 digits), MM:minutes, SS:seconds H:hemisphere (NESW)
 * Return value is positive for N,E; negative for S,W.
 */
static gdouble dmsh2rad (const gchar *latlon)
{
    char *p1, *p2;
    int deg, min, sec, dir;
    gdouble value;
    
    if (latlon == NULL)
	return DBL_MAX;
    p1 = strchr(latlon, '-');
    p2 = strrchr(latlon, '-');
    if (p1 == NULL || p1 == latlon) {
        return DBL_MAX;
    } else if (p1 == p2) {
	sscanf (latlon, "%d-%d", &deg, &min);
	sec = 0;
    } else if (p2 == 1 + p1) {
	return DBL_MAX;
    } else {
	sscanf (latlon, "%d-%d-%d", &deg, &min, &sec);
    }
    if (deg > 180 || min >= 60 || sec >= 60)
	return DBL_MAX;
    value = (gdouble)((deg * 60 + min) * 60 + sec) * M_PI / 648000.;

    dir = toupper(latlon[strlen(latlon) - 1]);
    if (dir == 'W' || dir == 'S')
	value = -value;
    else if (dir != 'E' && dir != 'N' && (value != 0.0 || dir != '0'))
	value = DBL_MAX;
    return value;
}

WeatherLocation *weather_location_new (const gchar *name, const gchar *code,
				       const gchar *zone, const gchar *radar,
                                       const gchar *coordinates)
{
    WeatherLocation *location;

    location = g_new(WeatherLocation, 1);

    /* name and metar code must be set */
    location->name = g_strdup(name);
    location->code = g_strdup(code);

    if (zone) {    
        location->zone = g_strdup(zone);
    } else {
        location->zone = g_strdup("------");
    }

    if (radar) {
        location->radar = g_strdup(radar);
    } else {
        location->radar = g_strdup("---");
    }

    if (location->zone[0] == '-') {
        location->zone_valid = FALSE;
    } else {
        location->zone_valid = TRUE;
    }

    location->coordinates = NULL;
    if (coordinates)
    {
	char **pieces;

	pieces = g_strsplit (coordinates, " ", -1);

	if (g_strv_length (pieces) == 2)
	{
            location->coordinates = g_strdup(coordinates);
            location->latitude = dmsh2rad (pieces[0]);
	    location->longitude = dmsh2rad (pieces[1]);
	}

	g_strfreev (pieces);
    }

    if (!location->coordinates)
    {
        location->coordinates = g_strdup("---");
        location->latitude = DBL_MAX;
        location->longitude = DBL_MAX;
    }

    location->latlon_valid = (location->latitude < DBL_MAX && location->longitude < DBL_MAX);
    
    return location;
}

gboolean weather_location_equal (const WeatherLocation *location1, const WeatherLocation *location2)
{
    if (!location1->code || !location2->code)
        return 1;
    return ( (strcmp(location1->code, location2->code) == 0) &&
             (strcmp(location1->name, location2->name) == 0) );    
}

WeatherLocation *weather_location_clone (const WeatherLocation *location)
{
    WeatherLocation *clone;

    clone = weather_location_new (location->name,
				  location->code, location->zone,
				  location->radar, location->coordinates);
    clone->latitude = location->latitude;
    clone->longitude = location->longitude;
    clone->latlon_valid = location->latlon_valid;
    return clone;
}


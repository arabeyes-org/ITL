#include "config.h"

static GSettings *city, *prefs, *athan;

static MinbarConfig* 
config_new(void) 
{
	MinbarConfig* config = g_malloc(sizeof(MinbarConfig));
	
	// TODO: initialize variables

	return config;
}

void 
config_init(void) 
{
	city = g_settings_new("org.gnome.minbar.city");
	prefs = g_settings_new("org.gnome.minbar.preferences");
	athan = g_settings_new("org.gnome.minbar.athan");
}

MinbarConfig* config_read(void) {
	MinbarConfig* config = config_new();
	config->city = g_settings_get_string(city, "name");
	config->latitude = g_settings_get_double(city, "latitude");
	config->longitude = g_settings_get_double(city, "longitude");
	config->correction = g_settings_get_double(city, "correction");

	config->method = g_settings_get_int(prefs, "method");
	config->start_hidden = g_settings_get_boolean(prefs, "start-hidden");
	config->close_closes = g_settings_get_boolean(prefs, "closes");

	config->notification = g_settings_get_boolean(prefs, "notification");
	config->notification_time = g_settings_get_int(prefs, "notification-time");

	config->athan_enabled = g_settings_get_boolean(athan, "play");
	config->athan_normal = g_settings_get_string(athan, "normal");
	config->athan_subh = g_settings_get_string(athan, "subh");

	return config;
}

void config_save(MinbarConfig* config) {
	g_settings_set_string(city, "name", config->city);
	g_settings_set_double(city, "latitude", config->latitude);
	g_settings_set_double(city, "longitude", config->longitude);
	g_settings_set_double(city, "correction", config->correction);

	g_settings_set_int(prefs, "method", config->method);
	g_settings_set_boolean(prefs, "start-hidden", config->start_hidden);
	g_settings_set_boolean(prefs, "closes", config->close_closes);
	g_settings_set_boolean(prefs, "notification", config->notification);
	g_settings_set_int(prefs, "notification-time", config->notification_time);

	g_settings_set_boolean(athan, "play", config->athan_enabled);
	g_settings_set_string(athan, "normal", config->athan_normal);
	g_settings_set_string(athan, "subh", config->athan_subh);
}

#include "myjson.h"
#include <unistd.h>
#include <time.h>

json_t *weather_json_struct = NULL;

const char *choices_array[18] = {"station_id", "current_time", "current_temp", "current_pressure", "current_humidity",
                                 "current_speed", "current_cloudiness", "current_rain", "current_sunrise", "current_sunset",
                                 "forecast3_time", "forecast3_temp", "forecast6_time", "forecast6_temp", "forecast9_time",
                                 "forecast9_temp", "forecast12_time", "forecast12_temp"
                                };


json_t *createJsonStruct(char *current, char *forecast) {
	json_t *root;
	json_error_t error;

	root = json_loads(current, 0, &error);

	if ( !root ) {

		fprintf( stderr, "error: on line %d: %s\n", error.line, error.text );
		return NULL;
	}

	if ( !json_is_object(root) ) {
		fprintf( stderr, "error:    root is not an array\n" );
		json_decref(root);
		return NULL;
	}

	json_t **JsonArray = malloc(sizeof(json_t *) * 18);


	JsonArray[0] =  json_object_get(root, "id");
	JsonArray[1] =  json_object_get(root, "dt");
	JsonArray[2] =  json_object_get(json_object_get(root, "main"), "temp");
	JsonArray[3] =  json_object_get(json_object_get(root, "main"), "pressure");
	JsonArray[4] =  json_object_get(json_object_get(root, "main"), "humidity");
	JsonArray[5] =  json_object_get(json_object_get(root, "wind"), "speed");
	JsonArray[6] =  json_object_get(json_object_get(root, "clouds"), "all");
	//JsonArray[7] =  json_object_get(json_object_get(root,"main"),"temp");
	JsonArray[8] =  json_object_get(json_object_get(root, "sys"), "sunrise");
	JsonArray[9] =  json_object_get(json_object_get(root, "sys"), "sunset");


	// printf("dt:%lld\nsunrise:%lld\nsunset:%lld\n",json_integer_value(JsonArray[1]),json_integer_value(JsonArray[8]),json_integer_value(JsonArray[9]) );
	// sleep(3000);
	//printf("%lld\n",json_integer_value(JsonArray[0]) );
	//printf("%.2f\n", json_real_value(JsonArray[2]) );

	//printf("%s\n",json_string_value(res) );

	// printf("JSON:\n%s\n", json_dumps(root, JSON_ENSURE_ASCII) );

	//json_t *json_integer(json_int_t value)

	json_t *array, *obj, *stateDescriptionObj;

	array = json_array();

	//station
	obj = json_object();

	json_object_set_new(obj, "link", json_string("http://myserver.org:8080/items/station_id"));

	char temp_state[100];
	sprintf(temp_state, "%lld", json_integer_value(JsonArray[0]));
	json_object_set_new(obj, "state", json_string(temp_state) );

	stateDescriptionObj = json_object();
	json_object_set_new(stateDescriptionObj, "pattern", json_string("%s"));
	json_object_set_new(stateDescriptionObj, "readonly", json_boolean(1));
	json_object_set_new(stateDescriptionObj, "options", json_array());

	json_object_set_new(obj, "stateDescription", stateDescriptionObj);

	json_object_set_new(obj, "editable", json_boolean(1));
	json_object_set_new(obj, "type", json_string("String"));
	json_object_set_new(obj, "name", json_string("WeatherAndForecast_Station_StationId"));
	json_object_set_new(obj, "label", json_string("Station Id"));
	json_object_set_new(obj, "tags", json_array());
	json_object_set_new(obj, "groupNames", json_array());


	json_array_append(array, obj);


	//Current Observation Time
	obj = json_object();

	json_object_set_new(obj, "link", json_string("http://myserver.org:8080/items/current_time"));


	time_t curtime = json_integer_value(JsonArray[1]);

	struct tm  ts;
	char time_buf[80];


	ts = *gmtime(&curtime);
	strftime(time_buf, sizeof(time_buf), "%Y-%m-%dT%H:%M:%S.000%z", &ts);



	json_object_set_new(obj, "state", json_string(time_buf) );

	stateDescriptionObj = json_object();
	json_object_set_new(stateDescriptionObj, "pattern", json_string("%1$tY-%1$tm-%1$td %1$tH:%1$tM:%1$tS"));
	json_object_set_new(stateDescriptionObj, "readonly", json_boolean(1));
	json_object_set_new(stateDescriptionObj, "options", json_array());

	json_object_set_new(obj, "stateDescription", stateDescriptionObj);

	json_object_set_new(obj, "editable", json_boolean(1));
	json_object_set_new(obj, "type", json_string("DateTime"));
	json_object_set_new(obj, "name", json_string("WeatherAndForecast_Current_ObservationTime"));
	json_object_set_new(obj, "label", json_string("Observation Time"));
	json_object_set_new(obj, "category", json_string("Time"));
	json_object_set_new(obj, "tags", json_array());
	json_object_set_new(obj, "groupNames", json_array());


	json_array_append(array, obj);

	//Current Temp
	obj = json_object();

	json_object_set_new(obj, "link", json_string("http://myserver.org:8080/items/current_temp"));

	bzero(temp_state, 100);

	sprintf(temp_state, "%.2f °C", json_real_value(JsonArray[2]));
	json_object_set_new(obj, "state", json_string(temp_state) );

	//json_object_set_new(obj,"state", JsonArray[2]);

	stateDescriptionObj = json_object();
	json_object_set_new(stateDescriptionObj, "pattern", json_string("%.1f %unit%"));
	json_object_set_new(stateDescriptionObj, "readonly", json_boolean(1));
	json_object_set_new(stateDescriptionObj, "options", json_array());

	json_object_set_new(obj, "stateDescription", stateDescriptionObj);

	json_object_set_new(obj, "editable", json_boolean(1));
	json_object_set_new(obj, "type", json_string("Number:Temperature"));
	json_object_set_new(obj, "name", json_string("WeatherAndForecast_Current_OutdoorTemperature"));
	json_object_set_new(obj, "label", json_string("Outdoor Temperature"));
	json_object_set_new(obj, "category", json_string("Temperature"));
	json_object_set_new(obj, "tags", json_array());
	json_object_set_new(obj, "groupNames", json_array());


	json_array_append(array, obj);

	//Current Pressure
	obj = json_object();

	json_object_set_new(obj, "link", json_string("http://myserver.org:8080/items/current_pressure"));
	bzero(temp_state, 100);

	sprintf(temp_state, "%.1f hPa", (float)json_integer_value(JsonArray[3]));
	json_object_set_new(obj, "state", json_string(temp_state) );

	stateDescriptionObj = json_object();
	json_object_set_new(stateDescriptionObj, "pattern", json_string("%.3f %unit%"));
	json_object_set_new(stateDescriptionObj, "readonly", json_boolean(1));
	json_object_set_new(stateDescriptionObj, "options", json_array());

	json_object_set_new(obj, "stateDescription", stateDescriptionObj);

	json_object_set_new(obj, "editable", json_boolean(1));
	json_object_set_new(obj, "type", json_string("Number:Pressure"));
	json_object_set_new(obj, "name", json_string("WeatherAndForecast_Current_BarometricPressure"));
	json_object_set_new(obj, "label", json_string("Barometric Pressure"));
	json_object_set_new(obj, "category", json_string("Pressure"));
	json_object_set_new(obj, "tags", json_array());
	json_object_set_new(obj, "groupNames", json_array());


	json_array_append(array, obj);

	//Current Humidity
	obj = json_object();

	json_object_set_new(obj, "link", json_string("http://myserver.org:8080/items/current_humidity"));

	bzero(temp_state, 100);

	sprintf(temp_state, "%lld %%", json_integer_value(JsonArray[4]));
	json_object_set_new(obj, "state", json_string(temp_state) );

	stateDescriptionObj = json_object();
	json_object_set_new(stateDescriptionObj, "pattern", json_string("%.0f %%"));
	json_object_set_new(stateDescriptionObj, "readonly", json_boolean(1));
	json_object_set_new(stateDescriptionObj, "options", json_array());

	json_object_set_new(obj, "stateDescription", stateDescriptionObj);

	json_object_set_new(obj, "editable", json_boolean(1));
	json_object_set_new(obj, "type", json_string("Number:Dimensionless"));
	json_object_set_new(obj, "name", json_string("WeatherAndForecast_Current_AtmosphericHumidity"));
	json_object_set_new(obj, "label", json_string("Atmospheric Humidity"));
	json_object_set_new(obj, "category", json_string("Humitidy"));
	json_object_set_new(obj, "tags", json_array());
	json_object_set_new(obj, "groupNames", json_array());


	json_array_append(array, obj);

	//Current Speed
	obj = json_object();

	json_object_set_new(obj, "link", json_string("http://myserver.org:8080/items/current_speed"));

	bzero(temp_state, 100);

	sprintf(temp_state, "%.1f m/s", json_real_value(JsonArray[5]));
	json_object_set_new(obj, "state", json_string(temp_state) );

	stateDescriptionObj = json_object();
	json_object_set_new(stateDescriptionObj, "pattern", json_string("%.1f %unit%"));
	json_object_set_new(stateDescriptionObj, "readonly", json_boolean(1));
	json_object_set_new(stateDescriptionObj, "options", json_array());

	json_object_set_new(obj, "stateDescription", stateDescriptionObj);

	json_object_set_new(obj, "editable", json_boolean(1));
	json_object_set_new(obj, "type", json_string("Number:Speed"));
	json_object_set_new(obj, "name", json_string("WeatherAndForecast_Current_WindSpeed"));
	json_object_set_new(obj, "label", json_string("Wind Speed"));
	json_object_set_new(obj, "category", json_string("Wind"));
	json_object_set_new(obj, "tags", json_array());
	json_object_set_new(obj, "groupNames", json_array());


	json_array_append(array, obj);

	//Current Cloud
	obj = json_object();

	json_object_set_new(obj, "link", json_string("http://myserver.org:8080/items/current_cloudiness"));

	bzero(temp_state, 100);

	sprintf(temp_state, "%lld %%", json_integer_value(JsonArray[6]));
	json_object_set_new(obj, "state", json_string(temp_state) );

	stateDescriptionObj = json_object();
	json_object_set_new(stateDescriptionObj, "minimum", json_integer(0));
	json_object_set_new(stateDescriptionObj, "maximum", json_integer(100));
	json_object_set_new(stateDescriptionObj, "pattern", json_string("%d %unit%"));
	json_object_set_new(stateDescriptionObj, "readonly", json_boolean(1));
	json_object_set_new(stateDescriptionObj, "options", json_array());

	json_object_set_new(obj, "stateDescription", stateDescriptionObj);

	json_object_set_new(obj, "editable", json_boolean(1));
	json_object_set_new(obj, "type", json_string("Number:Dimensionless"));
	json_object_set_new(obj, "name", json_string("WeatherAndForecast_Current_Cloudiness"));
	json_object_set_new(obj, "label", json_string("Cloudiness"));
	json_object_set_new(obj, "category", json_string("Clouds"));
	json_object_set_new(obj, "tags", json_array());
	json_object_set_new(obj, "groupNames", json_array());


	json_array_append(array, obj);


	//Current rain
	obj = json_object();

	json_object_set_new(obj, "link", json_string("http://myserver.org:8080/items/current_rain"));
	//json_object_set_new(obj,"state", JsonArray[7]);

	stateDescriptionObj = json_object();
	json_object_set_new(stateDescriptionObj, "pattern", json_string("%.2f %unit%"));
	json_object_set_new(stateDescriptionObj, "readonly", json_boolean(1));
	json_object_set_new(stateDescriptionObj, "options", json_array());

	json_object_set_new(obj, "stateDescription", stateDescriptionObj);

	json_object_set_new(obj, "editable", json_boolean(1));
	json_object_set_new(obj, "type", json_string("Number:Length"));
	json_object_set_new(obj, "name", json_string("WeatherAndForecast_Current_Rain"));
	json_object_set_new(obj, "label", json_string("Rain"));
	json_object_set_new(obj, "category", json_string("Rain"));
	json_object_set_new(obj, "tags", json_array());
	json_object_set_new(obj, "groupNames", json_array());


	json_array_append(array, obj);

	//Current SunRise
	obj = json_object();

	json_object_set_new(obj, "link", json_string("http://myserver.org:8080/items/current_sunrise"));

	time_t curtime_sunset = json_integer_value(JsonArray[8]);

	struct tm  ts_sunrise;

	char temp_buf_sunrise[80];

	ts_sunrise = *gmtime(&curtime_sunset);
	strftime(temp_buf_sunrise, sizeof(temp_buf_sunrise), "%Y-%m-%dT%H:%M:%S.000%z", &ts_sunrise);



	json_object_set_new(obj, "state", json_string(time_buf) );

	stateDescriptionObj = json_object();
	json_object_set_new(stateDescriptionObj, "pattern", json_string("%d"));
	json_object_set_new(stateDescriptionObj, "readonly", json_boolean(1));
	json_object_set_new(stateDescriptionObj, "options", json_array());

	json_object_set_new(obj, "stateDescription", stateDescriptionObj);

	json_object_set_new(obj, "editable", json_boolean(1));
	json_object_set_new(obj, "type", json_string("DateTime"));
	json_object_set_new(obj, "name", json_string("WeatherAndForecast_Current_SunRise_Time"));
	json_object_set_new(obj, "label", json_string("Sunrise Time"));
	json_object_set_new(obj, "category", json_string("Time"));
	json_object_set_new(obj, "tags", json_array());
	json_object_set_new(obj, "groupNames", json_array());


	json_array_append(array, obj);

	//Current SunSet
	obj = json_object();

	json_object_set_new(obj, "link", json_string("http://myserver.org:8080/items/current_sunset"));

	curtime = json_integer_value(JsonArray[9]);

	struct tm  ts_sunset;

	bzero(time_buf, 80);


	ts_sunset = *gmtime(&curtime);
	strftime(time_buf, sizeof(time_buf), "%Y-%m-%dT%H:%M:%S.000%z", &ts_sunset);

	json_object_set_new(obj, "state", json_string(time_buf) );

	stateDescriptionObj = json_object();
	json_object_set_new(stateDescriptionObj, "pattern", json_string("%d"));
	json_object_set_new(stateDescriptionObj, "readonly", json_boolean(1));
	json_object_set_new(stateDescriptionObj, "options", json_array());

	json_object_set_new(obj, "stateDescription", stateDescriptionObj);

	json_object_set_new(obj, "editable", json_boolean(1));
	json_object_set_new(obj, "type", json_string("DateTime"));
	json_object_set_new(obj, "name", json_string("WeatherAndForecast_Current_SunSet_Time"));
	json_object_set_new(obj, "label", json_string("Sunset Time"));
	json_object_set_new(obj, "category", json_string("Time"));
	json_object_set_new(obj, "tags", json_array());
	json_object_set_new(obj, "groupNames", json_array());


	json_array_append(array, obj);


	//printf("JSON:\n%s\n", json_dumps(array, JSON_ENSURE_ASCII) );


	// Forecast

	json_t *root2;
	json_error_t error2;

	root2 = json_loads(forecast, 0, &error2);

	if ( !root2 ) {

		fprintf( stderr, "error: on line %d: %s\n", error2.line, error2.text );
		return NULL;
	}

	if ( !json_is_object(root2) ) {
		fprintf( stderr, "error:    root is not an array\n" );
		json_decref(root2);
		return NULL;
	}

	JsonArray[10] =  json_object_get(json_array_get(json_object_get(root2, "list"), 1), "dt");
	JsonArray[11] =  json_object_get(json_object_get(json_array_get(json_object_get(root2, "list"), 1), "main"), "temp");
	JsonArray[12] =  json_object_get(json_array_get(json_object_get(root2, "list"), 2), "dt");
	JsonArray[13] =  json_object_get(json_object_get(json_array_get(json_object_get(root2, "list"), 2), "main"), "temp");
	JsonArray[14] =  json_object_get(json_array_get(json_object_get(root2, "list"), 3), "dt");
	JsonArray[15] =  json_object_get(json_object_get(json_array_get(json_object_get(root2, "list"), 3), "main"), "temp");
	JsonArray[16] =  json_object_get(json_array_get(json_object_get(root2, "list"), 4), "dt");
	JsonArray[17] =  json_object_get(json_object_get(json_array_get(json_object_get(root2, "list"), 4), "main"), "temp");

	//ForeCast Time 3

	obj = json_object();

	json_object_set_new(obj, "link", json_string("http://myserver.org:8080/items/forecast3_time"));

	curtime = json_integer_value(JsonArray[10]);

	struct tm  ts_3;

	bzero(time_buf, 80);


	ts_3 = *gmtime(&curtime);
	strftime(time_buf, sizeof(time_buf), "%Y-%m-%dT%H:%M:%S.000%z", &ts_3);


	json_object_set_new(obj, "state", json_string(time_buf));

	stateDescriptionObj = json_object();
	json_object_set_new(stateDescriptionObj, "pattern", json_string("%1$tY-%1$tm-%1$td %1$tH:%1$tM:%1$tS"));
	json_object_set_new(stateDescriptionObj, "readonly", json_boolean(1));
	json_object_set_new(stateDescriptionObj, "options", json_array());

	json_object_set_new(obj, "stateDescription", stateDescriptionObj);

	json_object_set_new(obj, "editable", json_boolean(1));
	json_object_set_new(obj, "type", json_string("DateTime"));
	json_object_set_new(obj, "name", json_string("WeatherAndForecast_ForecastHours03_ForecastTime"));
	json_object_set_new(obj, "label", json_string("Forecast Time"));
	json_object_set_new(obj, "category", json_string("Time"));
	json_object_set_new(obj, "tags", json_array());
	json_object_set_new(obj, "groupNames", json_array());


	json_array_append(array, obj);

	//ForeCast Temp 3

	obj = json_object();

	json_object_set_new(obj, "link", json_string("http://myserver.org:8080/items/forecast3_temp"));

	bzero(temp_state, 100);

	sprintf(temp_state, "%.2f °C", json_real_value(JsonArray[11]));
	json_object_set_new(obj, "state", json_string(temp_state) );

	stateDescriptionObj = json_object();
	json_object_set_new(stateDescriptionObj, "pattern", json_string("%.1f %unit%"));
	json_object_set_new(stateDescriptionObj, "readonly", json_boolean(1));
	json_object_set_new(stateDescriptionObj, "options", json_array());

	json_object_set_new(obj, "stateDescription", stateDescriptionObj);

	json_object_set_new(obj, "editable", json_boolean(1));
	json_object_set_new(obj, "type", json_string("Number:Temperature"));
	json_object_set_new(obj, "name", json_string("WeatherAndForecast_ForecastHours03_ForecastedTemperature"));
	json_object_set_new(obj, "label", json_string("Forecast Temperature"));
	json_object_set_new(obj, "category", json_string("Temperature"));
	json_object_set_new(obj, "tags", json_array());
	json_object_set_new(obj, "groupNames", json_array());


	json_array_append(array, obj);

	//ForeCast Time 6

	obj = json_object();

	json_object_set_new(obj, "link", json_string("http://myserver.org:8080/items/forecast6_time"));

	curtime = json_integer_value(JsonArray[12]);

	struct tm  ts_6;

	bzero(time_buf, 80);


	ts_6 = *gmtime(&curtime);
	strftime(time_buf, sizeof(time_buf), "%Y-%m-%dT%H:%M:%S.000%z", &ts_6);


	json_object_set_new(obj, "state", json_string(time_buf));

	stateDescriptionObj = json_object();
	json_object_set_new(stateDescriptionObj, "pattern", json_string("%1$tY-%1$tm-%1$td %1$tH:%1$tM:%1$tS"));
	json_object_set_new(stateDescriptionObj, "readonly", json_boolean(1));
	json_object_set_new(stateDescriptionObj, "options", json_array());

	json_object_set_new(obj, "stateDescription", stateDescriptionObj);

	json_object_set_new(obj, "editable", json_boolean(1));
	json_object_set_new(obj, "type", json_string("DateTime"));
	json_object_set_new(obj, "name", json_string("WeatherAndForecast_ForecastHours06_ForecastTime"));
	json_object_set_new(obj, "label", json_string("Forecast Time"));
	json_object_set_new(obj, "category", json_string("Time"));
	json_object_set_new(obj, "tags", json_array());
	json_object_set_new(obj, "groupNames", json_array());


	json_array_append(array, obj);

	//ForeCast Temp 6

	obj = json_object();

	json_object_set_new(obj, "link", json_string("http://myserver.org:8080/items/forecast6_temp"));

	bzero(temp_state, 100);

	sprintf(temp_state, "%.2f °C", json_real_value(JsonArray[13]));
	json_object_set_new(obj, "state", json_string(temp_state) );

	stateDescriptionObj = json_object();
	json_object_set_new(stateDescriptionObj, "pattern", json_string("%.1f %unit%"));
	json_object_set_new(stateDescriptionObj, "readonly", json_boolean(1));
	json_object_set_new(stateDescriptionObj, "options", json_array());

	json_object_set_new(obj, "stateDescription", stateDescriptionObj);

	json_object_set_new(obj, "editable", json_boolean(1));
	json_object_set_new(obj, "type", json_string("Number:Temperature"));
	json_object_set_new(obj, "name", json_string("WeatherAndForecast_ForecastHours06_ForecastedTemperature"));
	json_object_set_new(obj, "label", json_string("Forecast Temperature"));
	json_object_set_new(obj, "category", json_string("Temperature"));
	json_object_set_new(obj, "tags", json_array());
	json_object_set_new(obj, "groupNames", json_array());


	json_array_append(array, obj);

	//ForeCast Time 9

	obj = json_object();

	json_object_set_new(obj, "link", json_string("http://myserver.org:8080/items/forecast9_time"));


	curtime = json_integer_value(JsonArray[14]);

	struct tm  ts_9;

	bzero(time_buf, 80);


	ts_9 = *gmtime(&curtime);
	strftime(time_buf, sizeof(time_buf), "%Y-%m-%dT%H:%M:%S.000%z", &ts_9);


	json_object_set_new(obj, "state", json_string(time_buf));

	stateDescriptionObj = json_object();
	json_object_set_new(stateDescriptionObj, "pattern", json_string("%1$tY-%1$tm-%1$td %1$tH:%1$tM:%1$tS"));
	json_object_set_new(stateDescriptionObj, "readonly", json_boolean(1));
	json_object_set_new(stateDescriptionObj, "options", json_array());

	json_object_set_new(obj, "stateDescription", stateDescriptionObj);

	json_object_set_new(obj, "editable", json_boolean(1));
	json_object_set_new(obj, "type", json_string("DateTime"));
	json_object_set_new(obj, "name", json_string("WeatherAndForecast_ForecastHours09_ForecastTime"));
	json_object_set_new(obj, "label", json_string("Forecast Time"));
	json_object_set_new(obj, "category", json_string("Time"));
	json_object_set_new(obj, "tags", json_array());
	json_object_set_new(obj, "groupNames", json_array());


	json_array_append(array, obj);

	//ForeCast Temp 9

	obj = json_object();

	json_object_set_new(obj, "link", json_string("http://myserver.org:8080/items/forecast9_temp"));

	bzero(temp_state, 100);

	sprintf(temp_state, "%.2f °C", json_real_value(JsonArray[15]));
	json_object_set_new(obj, "state", json_string(temp_state) );

	stateDescriptionObj = json_object();
	json_object_set_new(stateDescriptionObj, "pattern", json_string("%.1f %unit%"));
	json_object_set_new(stateDescriptionObj, "readonly", json_boolean(1));
	json_object_set_new(stateDescriptionObj, "options", json_array());

	json_object_set_new(obj, "stateDescription", stateDescriptionObj);

	json_object_set_new(obj, "editable", json_boolean(1));
	json_object_set_new(obj, "type", json_string("Number:Temperature"));
	json_object_set_new(obj, "name", json_string("WeatherAndForecast_ForecastHours09_ForecastedTemperature"));
	json_object_set_new(obj, "label", json_string("Forecast Temperature"));
	json_object_set_new(obj, "category", json_string("Temperature"));
	json_object_set_new(obj, "tags", json_array());
	json_object_set_new(obj, "groupNames", json_array());


	json_array_append(array, obj);

	//ForeCast Time 12

	obj = json_object();

	json_object_set_new(obj, "link", json_string("http://myserver.org:8080/items/forecast12_time"));

	curtime = json_integer_value(JsonArray[16]);

	struct tm  ts_12;

	bzero(time_buf, 80);

	ts_12 = *gmtime(&curtime);
	strftime(time_buf, sizeof(time_buf), "%Y-%m-%dT%H:%M:%S.000%z", &ts_12);


	json_object_set_new(obj, "state", json_string(time_buf));

	stateDescriptionObj = json_object();
	json_object_set_new(stateDescriptionObj, "pattern", json_string("%1$tY-%1$tm-%1$td %1$tH:%1$tM:%1$tS"));
	json_object_set_new(stateDescriptionObj, "readonly", json_boolean(1));
	json_object_set_new(stateDescriptionObj, "options", json_array());

	json_object_set_new(obj, "stateDescription", stateDescriptionObj);

	json_object_set_new(obj, "editable", json_boolean(1));
	json_object_set_new(obj, "type", json_string("DateTime"));
	json_object_set_new(obj, "name", json_string("WeatherAndForecast_ForecastHours12_ForecastTime"));
	json_object_set_new(obj, "label", json_string("Forecast Time"));
	json_object_set_new(obj, "category", json_string("Time"));
	json_object_set_new(obj, "tags", json_array());
	json_object_set_new(obj, "groupNames", json_array());


	json_array_append(array, obj);

	//ForeCast Temp 12

	obj = json_object();

	json_object_set_new(obj, "link", json_string("http://myserver.org:8080/items/forecast12_temp"));

	bzero(temp_state, 100);

	sprintf(temp_state, "%.2f °C", json_real_value(JsonArray[17]));
	json_object_set_new(obj, "state", json_string(temp_state) );

	stateDescriptionObj = json_object();
	json_object_set_new(stateDescriptionObj, "pattern", json_string("%.1f %unit%"));
	json_object_set_new(stateDescriptionObj, "readonly", json_boolean(1));
	json_object_set_new(stateDescriptionObj, "options", json_array());

	json_object_set_new(obj, "stateDescription", stateDescriptionObj);

	json_object_set_new(obj, "editable", json_boolean(1));
	json_object_set_new(obj, "type", json_string("Number:Temperature"));
	json_object_set_new(obj, "name", json_string("WeatherAndForecast_ForecastHours12_ForecastedTemperature"));
	json_object_set_new(obj, "label", json_string("Forecast Temperature"));
	json_object_set_new(obj, "category", json_string("Temperature"));
	json_object_set_new(obj, "tags", json_array());
	json_object_set_new(obj, "groupNames", json_array());


	json_array_append(array, obj);


	return array;


}


char * JsonHeaderRemover(char * buff) {

	char token_buff[100000];
	char *token;
	char final_buff[100000];

	strcpy(token_buff, buff);
	strcpy(final_buff, buff);


	/* get the first token */
	token = strtok(token_buff, " ");

	/* walk through other tokens */
	token = strtok(NULL, " ");


	if (strcmp(token, "200") != 0) {
		printf("Error\n" );
		return NULL;
	}

	token = strtok(final_buff, "\r\n\r\n");

	while (1) {
		token = strtok(NULL, "\r\n\r\n");
		if (*token == '{') {

			break;
		}
	}


	char *json_string = malloc((strlen(token) + 1) * sizeof(char));
	strcpy(json_string, token);

	return json_string;

}



// int main() {



// 	char buff1[10000], buff2[100000];

// 	FILE *fp = fopen("./myjason_sample.json", "r");
// 	if(fp != NULL)
// 	{
// 	   fread(&buff1, sizeof(char), 10000, fp);
// 	   fclose(fp);
// 	}



// 	fp = fopen("./myjason_sample_forecast.json", "r");

// 	if(fp != NULL)
// 	{
// 	   fread(&buff2, sizeof(char), 100000, fp);
// 	   fclose(fp);
// 	}


// 	printf("JSON:\n%s\n", json_dumps(createJsonStruct(buff1,buff2), JSON_ENSURE_ASCII) );






// 	return 1;

// }
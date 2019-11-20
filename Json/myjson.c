#include <stdlib.h>
#include <string.h>
#include <jansson.h>
#include <stdio.h>


json_t *createJsonStruct(char *current, char *forecast){


	json_t *root;
	json_error_t error;

	root = json_loads(current, 0, &error);

	if ( !root ){

	    fprintf( stderr, "error: on line %d: %s\n", error.line, error.text );
	    return NULL;
	}

	if ( !json_is_object(root) ){
	    fprintf( stderr, "error:    root is not an array\n" );
	    json_decref(root);
	    return NULL;
	}

	json_t **JsonArray = malloc(sizeof(json_t *)*18);


	JsonArray[0] =  json_object_get(root,"id");
	JsonArray[1] =  json_object_get(root,"dt");
	JsonArray[2] =  json_object_get(json_object_get(root,"main"),"temp");
	JsonArray[3] =  json_object_get(json_object_get(root,"main"),"pressure");
	JsonArray[4] =  json_object_get(json_object_get(root,"main"),"humidity");
	JsonArray[5] =  json_object_get(json_object_get(root,"wind"),"currentSpeed");
	JsonArray[6] =  json_object_get(json_object_get(root,"clouds"),"all");
	//JsonArray[7] =  json_object_get(json_object_get(root,"main"),"temp");
	JsonArray[8] =  json_object_get(json_object_get(root,"sys"),"sunrise");
	JsonArray[9] =  json_object_get(json_object_get(root,"sys"),"sunset");




	//printf("%lld\n",json_integer_value(JsonArray[0]) );
	//printf("%.2f\n", json_real_value(JsonArray[2]) );
	
	//printf("%s\n",json_string_value(res) );

	// printf("JSON:\n%s\n", json_dumps(root, JSON_ENSURE_ASCII) );

	//json_t *json_integer(json_int_t value)

	json_t *array, *obj, *stateDescriptionObj;

	array = json_array();

	//station
	obj = json_object();
	
	json_object_set_new(obj,"link",json_string("http://myserver.org:8080/items/station_id"));
	json_object_set_new(obj,"state", JsonArray[0]);

	stateDescriptionObj = json_object();
	json_object_set_new(stateDescriptionObj,"pattern", json_string("%s"));
	json_object_set_new(stateDescriptionObj,"readonly", json_boolean(1));
	json_object_set_new(stateDescriptionObj,"options", json_array());

	json_object_set_new(obj,"stateDescription", stateDescriptionObj);

	json_object_set_new(obj,"editable", json_boolean(1));
	json_object_set_new(obj,"type", json_string("String"));
	json_object_set_new(obj,"name", json_string("WeatherAndForecast_Station_StationId"));
	json_object_set_new(obj,"label", json_string("Station Id"));
	json_object_set_new(obj,"tags", json_array());
	json_object_set_new(obj,"groupNames", json_array());


	json_array_append(array,obj);


	//Current Observation Time
	obj = json_object();
	
	json_object_set_new(obj,"link",json_string("http://myserver.org:8080/items/current_time"));
	json_object_set_new(obj,"state", JsonArray[1]);

	stateDescriptionObj = json_object();
	json_object_set_new(stateDescriptionObj,"pattern", json_string("%1$tY-%1$tm-%1$td %1$tH:%1$tM:%1$tS"));
	json_object_set_new(stateDescriptionObj,"readonly", json_boolean(1));
	json_object_set_new(stateDescriptionObj,"options", json_array());

	json_object_set_new(obj,"stateDescription", stateDescriptionObj);

	json_object_set_new(obj,"editable", json_boolean(1));
	json_object_set_new(obj,"type", json_string("DateTime"));
	json_object_set_new(obj,"name", json_string("WeatherAndForecast_Current_ObservationTime"));
	json_object_set_new(obj,"label", json_string("Observation Time"));
	json_object_set_new(obj,"category", json_string("Time"));
	json_object_set_new(obj,"tags", json_array());
	json_object_set_new(obj,"groupNames", json_array());


	json_array_append(array,obj);

	//Current Temp
	obj = json_object();
	
	json_object_set_new(obj,"link",json_string("http://myserver.org:8080/items/current_temp"));
	json_object_set_new(obj,"state", JsonArray[2]);

	stateDescriptionObj = json_object();
	json_object_set_new(stateDescriptionObj,"pattern", json_string("%.1f %unit%"));
	json_object_set_new(stateDescriptionObj,"readonly", json_boolean(1));
	json_object_set_new(stateDescriptionObj,"options", json_array());

	json_object_set_new(obj,"stateDescription", stateDescriptionObj);

	json_object_set_new(obj,"editable", json_boolean(1));
	json_object_set_new(obj,"type", json_string("Number:Temperature"));
	json_object_set_new(obj,"name", json_string("WeatherAndForecast_Current_OutdoorTemperature"));
	json_object_set_new(obj,"label", json_string("Outdoor Temperature"));
	json_object_set_new(obj,"category", json_string("Temperature"));
	json_object_set_new(obj,"tags", json_array());
	json_object_set_new(obj,"groupNames", json_array());


	json_array_append(array,obj);

	//Current Pressure
	obj = json_object();
	
	json_object_set_new(obj,"link",json_string("http://myserver.org:8080/items/current_pressure"));
	json_object_set_new(obj,"state", JsonArray[3]);

	stateDescriptionObj = json_object();
	json_object_set_new(stateDescriptionObj,"pattern", json_string("%.3f %unit%"));
	json_object_set_new(stateDescriptionObj,"readonly", json_boolean(1));
	json_object_set_new(stateDescriptionObj,"options", json_array());

	json_object_set_new(obj,"stateDescription", stateDescriptionObj);

	json_object_set_new(obj,"editable", json_boolean(1));
	json_object_set_new(obj,"type", json_string("Number:Pressure"));
	json_object_set_new(obj,"name", json_string("WeatherAndForecast_Current_BarometricPressure"));
	json_object_set_new(obj,"label", json_string("Barometric Pressure"));
	json_object_set_new(obj,"category", json_string("Pressure"));
	json_object_set_new(obj,"tags", json_array());
	json_object_set_new(obj,"groupNames", json_array());


	json_array_append(array,obj);

	//Current Humidity
	obj = json_object();
	
	json_object_set_new(obj,"link",json_string("http://myserver.org:8080/items/current_humidity"));
	json_object_set_new(obj,"state", JsonArray[4]);

	stateDescriptionObj = json_object();
	json_object_set_new(stateDescriptionObj,"pattern", json_string("%.0f %%"));
	json_object_set_new(stateDescriptionObj,"readonly", json_boolean(1));
	json_object_set_new(stateDescriptionObj,"options", json_array());

	json_object_set_new(obj,"stateDescription", stateDescriptionObj);

	json_object_set_new(obj,"editable", json_boolean(1));
	json_object_set_new(obj,"type", json_string("Number:Dimensionless"));
	json_object_set_new(obj,"name", json_string("WeatherAndForecast_Current_AtmosphericHumidity"));
	json_object_set_new(obj,"label", json_string("Atmospheric Humidity"));
	json_object_set_new(obj,"category", json_string("Humitidy"));
	json_object_set_new(obj,"tags", json_array());
	json_object_set_new(obj,"groupNames", json_array());


	json_array_append(array,obj);

	//Current Speed
	obj = json_object();
	
	json_object_set_new(obj,"link",json_string("http://myserver.org:8080/items/current_speed"));
	json_object_set_new(obj,"state", JsonArray[5]);

	stateDescriptionObj = json_object();
	json_object_set_new(stateDescriptionObj,"pattern", json_string("%.1f %unit%"));
	json_object_set_new(stateDescriptionObj,"readonly", json_boolean(1));
	json_object_set_new(stateDescriptionObj,"options", json_array());

	json_object_set_new(obj,"stateDescription", stateDescriptionObj);

	json_object_set_new(obj,"editable", json_boolean(1));
	json_object_set_new(obj,"type", json_string("Number:Speed"));
	json_object_set_new(obj,"name", json_string("WeatherAndForecast_Current_WindSpeed"));
	json_object_set_new(obj,"label", json_string("Wind Speed"));
	json_object_set_new(obj,"category", json_string("Wind"));
	json_object_set_new(obj,"tags", json_array());
	json_object_set_new(obj,"groupNames", json_array());


	json_array_append(array,obj);

	//Current Cloud
	obj = json_object();
	
	json_object_set_new(obj,"link",json_string("http://myserver.org:8080/items/current_cloudiness"));
	json_object_set_new(obj,"state", JsonArray[6]);

	stateDescriptionObj = json_object();
	json_object_set_new(stateDescriptionObj,"minimum", json_integer(0));
	json_object_set_new(stateDescriptionObj,"maximum", json_integer(100));
	json_object_set_new(stateDescriptionObj,"pattern", json_string("%d %unit%"));
	json_object_set_new(stateDescriptionObj,"readonly", json_boolean(1));
	json_object_set_new(stateDescriptionObj,"options", json_array());

	json_object_set_new(obj,"stateDescription", stateDescriptionObj);

	json_object_set_new(obj,"editable", json_boolean(1));
	json_object_set_new(obj,"type", json_string("Number:Dimensionless"));
	json_object_set_new(obj,"name", json_string("WeatherAndForecast_Current_Cloudiness"));
	json_object_set_new(obj,"label", json_string("Cloudiness"));
	json_object_set_new(obj,"category", json_string("Clouds"));
	json_object_set_new(obj,"tags", json_array());
	json_object_set_new(obj,"groupNames", json_array());


	json_array_append(array,obj);


	//Current rain
	obj = json_object();
	
	json_object_set_new(obj,"link",json_string("http://myserver.org:8080/items/current_rain"));
	//json_object_set_new(obj,"state", JsonArray[7]);

	stateDescriptionObj = json_object();
	json_object_set_new(stateDescriptionObj,"pattern", json_string("%.2f %unit%"));
	json_object_set_new(stateDescriptionObj,"readonly", json_boolean(1));
	json_object_set_new(stateDescriptionObj,"options", json_array());

	json_object_set_new(obj,"stateDescription", stateDescriptionObj);

	json_object_set_new(obj,"editable", json_boolean(1));
	json_object_set_new(obj,"type", json_string("Number:Length"));
	json_object_set_new(obj,"name", json_string("WeatherAndForecast_Current_Rain"));
	json_object_set_new(obj,"label", json_string("Rain"));
	json_object_set_new(obj,"category", json_string("Rain"));
	json_object_set_new(obj,"tags", json_array());
	json_object_set_new(obj,"groupNames", json_array());


	json_array_append(array,obj);

	//Current SunRise
	obj = json_object();
	
	json_object_set_new(obj,"link",json_string("http://myserver.org:8080/items/current_sunrise"));
	json_object_set_new(obj,"state", JsonArray[8]);

	stateDescriptionObj = json_object();
	json_object_set_new(stateDescriptionObj,"pattern", json_string("%d"));
	json_object_set_new(stateDescriptionObj,"readonly", json_boolean(1));
	json_object_set_new(stateDescriptionObj,"options", json_array());

	json_object_set_new(obj,"stateDescription", stateDescriptionObj);

	json_object_set_new(obj,"editable", json_boolean(1));
	json_object_set_new(obj,"type", json_string("DateTime"));
	json_object_set_new(obj,"name", json_string("WeatherAndForecast_Current_SunRise_Time"));
	json_object_set_new(obj,"label", json_string("Sunrise Time"));
	json_object_set_new(obj,"category", json_string("Time"));
	json_object_set_new(obj,"tags", json_array());
	json_object_set_new(obj,"groupNames", json_array());


	json_array_append(array,obj);

	//Current SunSet
	obj = json_object();
	
	json_object_set_new(obj,"link",json_string("http://myserver.org:8080/items/current_sunset"));
	json_object_set_new(obj,"state", JsonArray[9]);

	stateDescriptionObj = json_object();
	json_object_set_new(stateDescriptionObj,"pattern", json_string("%d"));
	json_object_set_new(stateDescriptionObj,"readonly", json_boolean(1));
	json_object_set_new(stateDescriptionObj,"options", json_array());

	json_object_set_new(obj,"stateDescription", stateDescriptionObj);

	json_object_set_new(obj,"editable", json_boolean(1));
	json_object_set_new(obj,"type", json_string("DateTime"));
	json_object_set_new(obj,"name", json_string("WeatherAndForecast_Current_SunSet_Time"));
	json_object_set_new(obj,"label", json_string("Sunset Time"));
	json_object_set_new(obj,"category", json_string("Time"));
	json_object_set_new(obj,"tags", json_array());
	json_object_set_new(obj,"groupNames", json_array());


	json_array_append(array,obj);


	//printf("JSON:\n%s\n", json_dumps(array, JSON_ENSURE_ASCII) );


	// Forecast

	json_t *root2;
	json_error_t error2;

	root2 = json_loads(forecast, 0, &error2);

	if ( !root2 ){

	    fprintf( stderr, "error: on line %d: %s\n", error2.line, error2.text );
	    return NULL;
	}

	if ( !json_is_object(root2) ){
	    fprintf( stderr, "error:    root is not an array\n" );
	    json_decref(root2);
	    return NULL;
	}

	JsonArray[10] =  json_object_get(json_array_get(json_object_get(root2,"list"),1),"dt");
	JsonArray[11] =  json_object_get(json_object_get(json_array_get(json_object_get(root2,"list"),1),"main"),"temp");
	JsonArray[12] =  json_object_get(json_array_get(json_object_get(root2,"list"),2),"dt");
	JsonArray[13] =  json_object_get(json_object_get(json_array_get(json_object_get(root2,"list"),2),"main"),"temp");
	JsonArray[14] =  json_object_get(json_array_get(json_object_get(root2,"list"),3),"dt");
	JsonArray[15] =  json_object_get(json_object_get(json_array_get(json_object_get(root2,"list"),3),"main"),"temp");
	JsonArray[16] =  json_object_get(json_array_get(json_object_get(root2,"list"),4),"dt");
	JsonArray[17] =  json_object_get(json_object_get(json_array_get(json_object_get(root2,"list"),4),"main"),"temp");

	//ForeCast Time 3

	obj = json_object();
	
	json_object_set_new(obj,"link",json_string("http://myserver.org:8080/items/forecast3_time"));
	json_object_set_new(obj,"state", JsonArray[10]);

	stateDescriptionObj = json_object();
	json_object_set_new(stateDescriptionObj,"pattern", json_string("%1$tY-%1$tm-%1$td %1$tH:%1$tM:%1$tS"));
	json_object_set_new(stateDescriptionObj,"readonly", json_boolean(1));
	json_object_set_new(stateDescriptionObj,"options", json_array());

	json_object_set_new(obj,"stateDescription", stateDescriptionObj);

	json_object_set_new(obj,"editable", json_boolean(1));
	json_object_set_new(obj,"type", json_string("DateTime"));
	json_object_set_new(obj,"name", json_string("WeatherAndForecast_ForecastHours03_ForecastTime"));
	json_object_set_new(obj,"label", json_string("Forecast Time"));
	json_object_set_new(obj,"category", json_string("Time"));
	json_object_set_new(obj,"tags", json_array());
	json_object_set_new(obj,"groupNames", json_array());


	json_array_append(array,obj);

	//ForeCast Temp 3

	obj = json_object();
	
	json_object_set_new(obj,"link",json_string("http://myserver.org:8080/items/forecast3_temp"));
	json_object_set_new(obj,"state", JsonArray[11]);

	stateDescriptionObj = json_object();
	json_object_set_new(stateDescriptionObj,"pattern", json_string("%.1f %unit%"));
	json_object_set_new(stateDescriptionObj,"readonly", json_boolean(1));
	json_object_set_new(stateDescriptionObj,"options", json_array());

	json_object_set_new(obj,"stateDescription", stateDescriptionObj);

	json_object_set_new(obj,"editable", json_boolean(1));
	json_object_set_new(obj,"type", json_string("Number:Temperature"));
	json_object_set_new(obj,"name", json_string("WeatherAndForecast_ForecastHours03_ForecastedTemperature"));
	json_object_set_new(obj,"label", json_string("Forecast Temperature"));
	json_object_set_new(obj,"category", json_string("Temperature"));
	json_object_set_new(obj,"tags", json_array());
	json_object_set_new(obj,"groupNames", json_array());


	json_array_append(array,obj);

	//ForeCast Time 6

	obj = json_object();
	
	json_object_set_new(obj,"link",json_string("http://myserver.org:8080/items/forecast6_time"));
	json_object_set_new(obj,"state", JsonArray[12]);

	stateDescriptionObj = json_object();
	json_object_set_new(stateDescriptionObj,"pattern", json_string("%1$tY-%1$tm-%1$td %1$tH:%1$tM:%1$tS"));
	json_object_set_new(stateDescriptionObj,"readonly", json_boolean(1));
	json_object_set_new(stateDescriptionObj,"options", json_array());

	json_object_set_new(obj,"stateDescription", stateDescriptionObj);

	json_object_set_new(obj,"editable", json_boolean(1));
	json_object_set_new(obj,"type", json_string("DateTime"));
	json_object_set_new(obj,"name", json_string("WeatherAndForecast_ForecastHours06_ForecastTime"));
	json_object_set_new(obj,"label", json_string("Forecast Time"));
	json_object_set_new(obj,"category", json_string("Time"));
	json_object_set_new(obj,"tags", json_array());
	json_object_set_new(obj,"groupNames", json_array());


	json_array_append(array,obj);

	//ForeCast Temp 6

	obj = json_object();
	
	json_object_set_new(obj,"link",json_string("http://myserver.org:8080/items/forecast6_temp"));
	json_object_set_new(obj,"state", JsonArray[13]);

	stateDescriptionObj = json_object();
	json_object_set_new(stateDescriptionObj,"pattern", json_string("%.1f %unit%"));
	json_object_set_new(stateDescriptionObj,"readonly", json_boolean(1));
	json_object_set_new(stateDescriptionObj,"options", json_array());

	json_object_set_new(obj,"stateDescription", stateDescriptionObj);

	json_object_set_new(obj,"editable", json_boolean(1));
	json_object_set_new(obj,"type", json_string("Number:Temperature"));
	json_object_set_new(obj,"name", json_string("WeatherAndForecast_ForecastHours06_ForecastedTemperature"));
	json_object_set_new(obj,"label", json_string("Forecast Temperature"));
	json_object_set_new(obj,"category", json_string("Temperature"));
	json_object_set_new(obj,"tags", json_array());
	json_object_set_new(obj,"groupNames", json_array());


	json_array_append(array,obj);

	//ForeCast Time 9

	obj = json_object();
	
	json_object_set_new(obj,"link",json_string("http://myserver.org:8080/items/forecast9_time"));
	json_object_set_new(obj,"state", JsonArray[14]);

	stateDescriptionObj = json_object();
	json_object_set_new(stateDescriptionObj,"pattern", json_string("%1$tY-%1$tm-%1$td %1$tH:%1$tM:%1$tS"));
	json_object_set_new(stateDescriptionObj,"readonly", json_boolean(1));
	json_object_set_new(stateDescriptionObj,"options", json_array());

	json_object_set_new(obj,"stateDescription", stateDescriptionObj);

	json_object_set_new(obj,"editable", json_boolean(1));
	json_object_set_new(obj,"type", json_string("DateTime"));
	json_object_set_new(obj,"name", json_string("WeatherAndForecast_ForecastHours09_ForecastTime"));
	json_object_set_new(obj,"label", json_string("Forecast Time"));
	json_object_set_new(obj,"category", json_string("Time"));
	json_object_set_new(obj,"tags", json_array());
	json_object_set_new(obj,"groupNames", json_array());


	json_array_append(array,obj);

	//ForeCast Temp 9

	obj = json_object();
	
	json_object_set_new(obj,"link",json_string("http://myserver.org:8080/items/forecast9_temp"));
	json_object_set_new(obj,"state", JsonArray[15]);

	stateDescriptionObj = json_object();
	json_object_set_new(stateDescriptionObj,"pattern", json_string("%.1f %unit%"));
	json_object_set_new(stateDescriptionObj,"readonly", json_boolean(1));
	json_object_set_new(stateDescriptionObj,"options", json_array());

	json_object_set_new(obj,"stateDescription", stateDescriptionObj);

	json_object_set_new(obj,"editable", json_boolean(1));
	json_object_set_new(obj,"type", json_string("Number:Temperature"));
	json_object_set_new(obj,"name", json_string("WeatherAndForecast_ForecastHours09_ForecastedTemperature"));
	json_object_set_new(obj,"label", json_string("Forecast Temperature"));
	json_object_set_new(obj,"category", json_string("Temperature"));
	json_object_set_new(obj,"tags", json_array());
	json_object_set_new(obj,"groupNames", json_array());


	json_array_append(array,obj);

	//ForeCast Time 12

	obj = json_object();
	
	json_object_set_new(obj,"link",json_string("http://myserver.org:8080/items/forecast12_time"));
	json_object_set_new(obj,"state", JsonArray[16]);

	stateDescriptionObj = json_object();
	json_object_set_new(stateDescriptionObj,"pattern", json_string("%1$tY-%1$tm-%1$td %1$tH:%1$tM:%1$tS"));
	json_object_set_new(stateDescriptionObj,"readonly", json_boolean(1));
	json_object_set_new(stateDescriptionObj,"options", json_array());

	json_object_set_new(obj,"stateDescription", stateDescriptionObj);

	json_object_set_new(obj,"editable", json_boolean(1));
	json_object_set_new(obj,"type", json_string("DateTime"));
	json_object_set_new(obj,"name", json_string("WeatherAndForecast_ForecastHours12_ForecastTime"));
	json_object_set_new(obj,"label", json_string("Forecast Time"));
	json_object_set_new(obj,"category", json_string("Time"));
	json_object_set_new(obj,"tags", json_array());
	json_object_set_new(obj,"groupNames", json_array());


	json_array_append(array,obj);

	//ForeCast Temp 12

	obj = json_object();
	
	json_object_set_new(obj,"link",json_string("http://myserver.org:8080/items/forecast12_temp"));
	json_object_set_new(obj,"state", JsonArray[17]);

	stateDescriptionObj = json_object();
	json_object_set_new(stateDescriptionObj,"pattern", json_string("%.1f %unit%"));
	json_object_set_new(stateDescriptionObj,"readonly", json_boolean(1));
	json_object_set_new(stateDescriptionObj,"options", json_array());

	json_object_set_new(obj,"stateDescription", stateDescriptionObj);

	json_object_set_new(obj,"editable", json_boolean(1));
	json_object_set_new(obj,"type", json_string("Number:Temperature"));
	json_object_set_new(obj,"name", json_string("WeatherAndForecast_ForecastHours12_ForecastedTemperature"));
	json_object_set_new(obj,"label", json_string("Forecast Temperature"));
	json_object_set_new(obj,"category", json_string("Temperature"));
	json_object_set_new(obj,"tags", json_array());
	json_object_set_new(obj,"groupNames", json_array());


	json_array_append(array,obj);


	return array;


}



int main() {

	char buff1[10000], buff2[100000];

	FILE *fp = fopen("./myjason_sample.json", "r");
	if(fp != NULL)
	{
	   fread(&buff1, sizeof(char), 10000, fp);
	   fclose(fp);
	}

	//printf("%s\n",buff1 );
	
	fp = fopen("./myjason_sample_forecast.json", "r");

	if(fp != NULL)
	{
	   fread(&buff2, sizeof(char), 100000, fp);
	   fclose(fp);
	}

	//printf("%s\n",buff2 );
	printf("JSON:\n%s\n", json_dumps(createJsonStruct(buff1,buff2), JSON_ENSURE_ASCII) );
	
	return 1;

}
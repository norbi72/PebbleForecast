var timerId = null,
    lastCoords = {latitude: 47.5, longitude: 19.02}; // Budapest

var xhrRequest = function (url, type, callback, error) {
  var xhr = new XMLHttpRequest();
  xhr.pebbleAsync = false;
  xhr.onload = function () {
    if (xhr.readyState == 4) {			
			if (xhr.status == 200) {
        console.log('onLoad');
        callback(this.responseText);
      }
    }
  };
  xhr.onerror = function (e) { 
    console.log('onError');
    console.log(JSON.stringify(e));
    error(xhr, xhr.status); 
  }; 
  xhr.open(type, url, true);
  xhr.send();
};
 
function locationSuccess(pos, retry_count) {
  retry_count = retry_count || 0;
  
  // Construct URL
  // DEBUG: Budapest "lon":19.02,"lat":47.5
  if (typeof pos == 'undefined' || null === pos || typeof pos.coords == 'undefined') {
    pos = {coords: lastCoords};
  } else {
    lastCoords = pos.coords;
  }
  // Kréta
  //pos.coords.latitude = 35.316456; 
  //pos.coords.longitude = 25.396850;
  // Budapest
  //pos.coords.latitude = 47.5; 
  //pos.coords.longitude = 19.02;
  
  // old: url = "https://api.forecast.io/forecast/f7247b8af3538d4f61b5c688140be497/" +
  var url = "https://api.darksky.net/forecast/f7247b8af3538d4f61b5c688140be497/" +
      pos.coords.latitude + "," + pos.coords.longitude;
  console.log(url);
  
  // Send request to OpenWeatherMap; xhrRequest function declaration see above
  xhrRequest(url, 'GET', 
    function(responseText) {
      // responseText contains a JSON object with weather info
      // {
      //  "coord":{"lon":19.02,"lat":47.5},
      //  "sys":{"type":1,"id":5724,"message":0.0122,"country":"HU","sunrise":1426826777,"sunset":1426870584},
      //  "weather":[{"id":800,"main":"Clear","description":"Sky is Clear","icon":"01d"}],
      //  "base":"cmc stations",
      //  "main":{"temp":279.15,"pressure":1027,"humidity":60,"temp_min":279.15,"temp_max":279.15},
      //  "wind":{"speed":4.1,"deg":90},
      //  "clouds":{"all":0},
      //  "dt":1426836600,"id":7284823,"name":"Budapest XII. kerulet","cod":200
      // }
      // {"latitude":47.504821,"longitude":19.0218375,"timezone":"Europe/Budapest","offset":2,"currently":{"time":1434099830,"summary":"Clear","icon":"clear-day","precipIntensity":0,"precipProbability":0,"temperature":82.61,"apparentTemperature":84.22,"dewPoint":64.44,"humidity":0.54,"windSpeed":2.09,"windBearing":120,"cloudCover":0,"pressure":1016.55,"ozone":337.23},"hourly":{"summary":"Partly cloudy starting this evening.","icon":"partly-cloudy-night","data":[{"time":1434099600,"summary":"Clear","icon":"clear-day","precipIntensity":0,"precipProbability":0,"temperature":82.43,"apparentTemperature":84.02,"dewPoint":64.4,"humidity":0.55,"windSpeed":1.86,"windBearing":113,"cloudCover":0,"pressure":1016.57,"ozone":337.31},{"time":1434103200,"summary":"Clear","icon":"clear-day","precipIntensity":0,"precipProbability":0,"temperature":85.33,"apparentTemperature":87.1,"dewPoint":64.95,"humidity":0.51,"windSpeed":6.49,"windBearing":151,"cloudCover":0,"pressure":1016.18,"ozone":336.09},{"time":1434106800,"summary":"Clear","icon":"clear-day","precipIntensity":0,"precipProbability":0,"temperature":87.55,"apparentTemperature":88.97,"dewPoint":64.11,"humidity":0.46,"windSpeed":8.07,"windBearing":161,"cloudCover":0,"pressure":1015.75,"ozone":333.97},{"time":1434110400,"summary":"Clear","icon":"clear-day","precipIntensity":0,"precipProbability":0,"temperature":88.84,"apparentTemperature":89.52,"dewPoint":62.49,"humidity":0.42,"windSpeed":9.11,"windBearing":168,"cloudCover":0.06,"pressure":1015.27,"ozone":332.41},{"time":1434114000,"summary":"Clear","icon":"clear-day","precipIntensity":0,"precipProbability":0,"temperature":89.15,"apparentTemperature":89.32,"dewPoint":61.29,"humidity":0.4,"windSpeed":9.49,"windBearing":171,"cloudCover":0.12,"pressure":1014.72,"ozone":332.2},{"time":1434117600,"summary":"Clear","icon":"clear-day","precipIntensity":0,"precipProbability":0,"temperature":88.58,"apparentTemperature":88.5,"dewPoint":60.68,"humidity":0.39,"windSpeed":9.38,"windBearing":174,"cloudCover":0.19,"pressure":1014.14,"ozone":332.54},{"time":1434121200,"summary":"Partly Cloudy","icon":"partly-cloudy-day","precipIntensity":0,"precipProbability":0,"temperature":87.36,"apparentTemperature":87.23,"dewPoint":60.45,"humidity":0.41,"windSpeed":9.34,"windBearing":176,"cloudCover":0.25,"pressure":1013.69,"ozone":332.57},{"time":1434124800,"summary":"Partly Cloudy","icon":"partly-cloudy-day","precipIntensity":0,"precipProbability":0,"temperature":85.35,"apparentTemperature":85.58,"dewPoint":61.1,"humidity":0.44,"windSpeed":8.51,"windBearing":176,"cloudCover":0.28,"pressure":1013.42,"ozone":331.84},{"time":1434128400,"summary":"Partly Cloudy","icon":"partly-cloudy-day","precipIntensity":0,"precipProbability":0,"temperature":83.12,"apparentTemperature":83.91,"dewPoint":62.09,"humidity":0.49,"windSpeed":7.53,"windBearing":176,"cloudCover":0.28,"pressure":1013.27,"ozone":330.79},{"time":1434132000,"summary":"Partly Cloudy","icon":"partly-cloudy-day","precipIntensity":0,"precipProbability":0,"temperature":81.01,"apparentTemperature":82.24,"dewPoint":62.74,"humidity":0.54,"windSpeed":6.71,"windBearing":176,"cloudCover":0.3,"pressure":1013.18,"ozone":329.78},{"time":1434135600,"summary":"Partly Cloudy","icon":"partly-cloudy-night","precipIntensity":0,"precipProbability":0,"temperature":78.74,"apparentTemperature":78.74,"dewPoint":62.74,"humidity":0.58,"windSpeed":6.26,"windBearing":180,"cloudCover":0.34,"pressure":1013.16,"ozone":328.73},{"time":1434139200,"summary":"Partly Cloudy","icon":"partly-cloudy-night","precipIntensity":0,"precipProbability":0,"temperature":76.11,"apparentTemperature":76.11,"dewPoint":62.22,"humidity":0.62,"windSpeed":6.12,"windBearing":186,"cloudCover":0.4,"pressure":1013.21,"ozone":327.71},{"time":1434142800,"summary":"Partly Cloudy","icon":"partly-cloudy-night","precipIntensity":0,"precipProbability":0,"temperature":73.54,"apparentTemperature":73.54,"dewPoint":61.43,"humidity":0.66,"windSpeed":6.07,"windBearing":189,"cloudCover":0.43,"pressure":1013.23,"ozone":327.3},{"time":1434146400,"summary":"Partly Cloudy","icon":"partly-cloudy-night","precipIntensity":0,"precipProbability":0,"temperature":71.37,"apparentTemperature":71.37,"dewPoint":60.7,"humidity":0.69,"windSpeed":6.08,"windBearing":190,"cloudCover":0.42,"pressure":1013.17,"ozone":328.06},{"time":1434150000,"summary":"Partly Cloudy","icon":"partly-cloudy-night","precipIntensity":0,"precipProbability":0,"temperature":69.71,"apparentTemperature":69.71,"dewPoint":60.13,"humidity":0.72,"windSpeed":6.19,"windBearing":190,"cloudCover":0.39,"pressure":1013.07,"ozone":329.42},{"time":1434153600,"summary":"Partly Cloudy","icon":"partly-cloudy-night","precipIntensity":0,"precipProbability":0,"temperature":68.59,"apparentTemperature":68.59,"dewPoint":59.72,"humidity":0.73,"windSpeed":6.35,"windBearing":190,"cloudCover":0.34,"pressure":1012.95,"ozone":330.27},{"time":1434157200,"summary":"Partly Cloudy","icon":"partly-cloudy-night","precipIntensity":0,"precipProbability":0,"temperature":67.83,"apparentTemperature":67.83,"dewPoint":59.45,"humidity":0.75,"windSpeed":6.33,"windBearing":189,"cloudCover":0.27,"pressure":1012.79,"ozone":330.26},{"time":1434160800,"summary":"Clear","icon":"clear-night","precipIntensity":0,"precipProbability":0,"temperature":67.49,"apparentTemperature":67.49,"dewPoint":59.32,"humidity":0.75,"windSpeed":6.28,"windBearing":185,"cloudCover":0.18,"pressure":1012.61,"ozone":329.74},{"time":1434164400,"summary":"Clear","icon":"clear-day","precipIntensity":0,"precipProbability":0,"temperature":68.23,"apparentTemperature":68.23,"dewPoint":59.59,"humidity":0.74,"windSpeed":6.38,"windBearing":185,"cloudCover":0.11,"pressure":1012.52,"ozone":328.63},{"time":1434168000,"summary":"Clear","icon":"clear-day","precipIntensity":0,"precipProbability":0,"temperature":70.07,"apparentTemperature":70.07,"dewPoint":59.9,"humidity":0.7,"windSpeed":6.79,"windBearing":190,"cloudCover":0.07,"pressure":1012.6,"ozone":326.43},{"time":1434171600,"summary":"Clear","icon":"clear-day","precipIntensity":0,"precipProbability":0,"temperature":72.67,"apparentTemperature":72.67,"dewPoint":60.12,"humidity":0.65,"windSpeed":7.64,"windBearing":196,"cloudCover":0.06,"pressure":1012.76,"ozone":323.63},{"time":1434175200,"summary":"Clear","icon":"clear-day","precipIntensity":0,"precipProbability":0,"temperature":75.14,"apparentTemperature":75.14,"dewPoint":60,"humidity":0.59,"windSpeed":8.75,"windBearing":202,"cloudCover":0.06,"pressure":1012.84,"ozone":321.65},{"time":1434178800,"summary":"Clear","icon":"clear-day","precipIntensity":0,"precipProbability":0,"temperature":77.67,"apparentTemperature":77.67,"dewPoint":59.72,"humidity":0.54,"windSpeed":10.22,"windBearing":205,"cloudCover":0.09,"pressure":1012.76,"ozone":321.27},{"time":1434182400,"summary":"Clear","icon":"clear-day","precipIntensity":0,"precipProbability":0,"temperature":80.66,"apparentTemperature":81.25,"dewPoint":59.44,"humidity":0.49,"windSpeed":12.03,"windBearing":208,"cloudCover":0.13,"pressure":1012.58,"ozone":321.72},{"time":1434186000,"summary":"Clear","icon":"clear-day","precipIntensity":0,"precipProbability":0,"temperature":83.8,"apparentTemperature":83.76,"dewPoint":59.6,"humidity":0.44,"windSpeed":13.76,"windBearing":211,"cloudCover":0.16,"pressure":1012.36,"ozone":322.07},{"time":1434189600,"summary":"Clear","icon":"clear-day","precipIntensity":0,"precipProbability":0,"temperature":86.72,"apparentTemperature":86.68,"dewPoint":60.62,"humidity":0.42,"windSpeed":15.15,"windBearing":214,"cloudCover":0.14,"pressure":1012.09,"ozone":321.82},{"time":1434193200,"summary":"Clear","icon":"clear-day","precipIntensity":0,"precipProbability":0,"temperature":89.27,"apparentTemperature":89.84,"dewPoint":62.22,"humidity":0.41,"windSpeed":16.13,"windBearing":217,"cloudCover":0.11,"pressure":1011.78,"ozone":321.48},{"time":1434196800,"summary":"Clear","icon":"clear-day","precipIntensity":0,"precipProbability":0,"temperature":91.09,"apparentTemperature":92.39,"dewPoint":63.48,"humidity":0.4,"windSpeed":16.54,"windBearing":219,"cloudCover":0.08,"pressure":1011.4,"ozone":321.63},{"time":1434200400,"summary":"Clear","icon":"clear-day","precipIntensity":0,"precipProbability":0,"temperature":91.97,"apparentTemperature":93.5,"dewPoint":63.75,"humidity":0.39,"windSpeed":16.23,"windBearing":220,"cloudCover":0.05,"pressure":1010.91,"ozone":322.57},{"time":1434204000,"summary":"Clear","icon":"clear-day","precipIntensity":0,"precipProbability":0,"temperature":91.56,"apparentTemperature":92.71,"dewPoint":63.13,"humidity":0.39,"windSpeed":15.33,"windBearing":220,"cloudCover":0.02,"pressure":1010.36,"ozone":324.01},{"time":1434207600,"summary":"Clear","icon":"clear-day","precipIntensity":0,"precipProbability":0,"temperature":90.33,"apparentTemperature":91.18,"dewPoint":62.7,"humidity":0.4,"windSpeed":14.07,"windBearing":220,"cloudCover":0,"pressure":1009.92,"ozone":325.67},{"time":1434211200,"summary":"Clear","icon":"clear-day","precipIntensity":0,"precipProbability":0,"temperature":88.14,"apparentTemperature":88.91,"dewPoint":62.72,"humidity":0.43,"windSpeed":12.66,"windBearing":220,"cloudCover":0,"pressure":1009.66,"ozone":327.93},{"time":1434214800,"summary":"Clear","icon":"clear-day","precipIntensity":0,"precipProbability":0,"temperature":85.47,"apparentTemperature":86.34,"dewPoint":62.87,"humidity":0.47,"windSpeed":11.01,"windBearing":219,"cloudCover":0,"pressure":1009.51,"ozone":330.42},{"time":1434218400,"summary":"Clear","icon":"clear-day","precipIntensity":0,"precipProbability":0,"temperature":83.05,"apparentTemperature":84.11,"dewPoint":62.94,"humidity":0.51,"windSpeed":9.82,"windBearing":217,"cloudCover":0,"pressure":1009.5,"ozone":331.8},{"time":1434222000,"summary":"Clear","icon":"clear-night","precipIntensity":0,"precipProbability":0,"temperature":80.74,"apparentTemperature":82.01,"dewPoint":62.79,"humidity":0.55,"windSpeed":9.14,"windBearing":215,"cloudCover":0,"pressure":1009.69,"ozone":331.11},{"time":1434225600,"summary":"Clear","icon":"clear-night","precipIntensity":0,"precipProbability":0,"temperature":78.54,"apparentTemperature":78.54,"dewPoint":62.62,"humidity":0.58,"windSpeed":8.64,"windBearing":213,"cloudCover":0,"pressure":1010.03,"ozone":329.32},{"time":1434229200,"summary":"Clear","icon":"clear-night","precipIntensity":0,"precipProbability":0,"temperature":76.47,"apparentTemperature":76.47,"dewPoint":62.18,"humidity":0.61,"windSpeed":8.35,"windBearing":213,"cloudCover":0,"pressure":1010.28,"ozone":327.94},{"time":1434232800,"summary":"Clear","icon":"clear-night","precipIntensity":0,"precipProbability":0,"temperature":74.48,"apparentTemperature":74.48,"dewPoint":61.16,"humidity":0.63,"windSpeed":8.31,"windBearing":217,"cloudCover":0.03,"pressure":1010.28,"ozone":327.37},{"time":1434236400,"summary":"Clear","icon":"clear-night","precipIntensity":0,"precipProbability":0,"temperature":72.65,"apparentTemperature":72.65,"dewPoint":59.93,"humidity":0.64,"windSpeed":8.31,"windBearing":221,"cloudCover":0.08,"pressure":1010.17,"ozone":327.22},{"time":1434240000,"summary":"Clear","icon":"clear-night","precipIntensity":0,"precipProbability":0,"temperature":70.98,"apparentTemperature":70.98,"dewPoint":58.97,"humidity":0.66,"windSpeed":8.08,"windBearing":224,"cloudCover":0.11,"pressure":1010.08,"ozone":327.78},{"time":1434243600,"summary":"Clear","icon":"clear-night","precipIntensity":0,"precipProbability":0,"temperature":70.3,"apparentTemperature":70.3,"dewPoint":59.56,"humidity":0.69,"windSpeed":7.61,"windBearing":229,"cloudCover":0.09,"pressure":1010.05,"ozone":329.66},{"time":1434247200,"summary":"Clear","icon":"clear-night","precipIntensity":0,"precipProbability":0,"temperature":68.39,"apparentTemperature":68.39,"dewPoint":59.08,"humidity":0.72,"windSpeed":6.68,"windBearing":233,"cloudCover":0.06,"pressure":1010.05,"ozone":332.25},{"time":1434250800,"summary":"Clear","icon":"clear-day","precipIntensity":0,"precipProbability":0,"temperature":67.59,"apparentTemperature":67.59,"dewPoint":58.88,"humidity":0.74,"windSpeed":5.89,"windBearing":238,"cloudCover":0.04,"pressure":1010.09,"ozone":334.06},{"time":1434254400,"summary":"Clear","icon":"clear-day","precipIntensity":0,"precipProbability":0,"temperature":69.2,"apparentTemperature":69.2,"dewPoint":59.73,"humidity":0.72,"windSpeed":5.65,"windBearing":246,"cloudCover":0.03,"pressure":1010.2,"ozone":334.23},{"time":1434258000,"summary":"Clear","icon":"clear-day","precipIntensity":0,"precipProbability":0,"temperature":72.69,"apparentTemperature":72.69,"dewPoint":61.55,"humidity":0.68,"windSpeed":6.05,"windBearing":259,"cloudCover":0.02,"pressure":1010.35,"ozone":333.62},{"time":1434261600,"summary":"Clear","icon":"clear-day","precipIntensity":0,"precipProbability":0,"temperature":75.56,"apparentTemperature":75.56,"dewPoint":62.55,"humidity":0.64,"windSpeed":6.51,"windBearing":261,"cloudCover":0.02,"pressure":1010.42,"ozone":333.25},{"time":1434265200,"summary":"Clear","icon":"clear-day","precipIntensity":0,"precipProbability":0,"temperature":77.95,"apparentTemperature":77.95,"dewPoint":63.11,"humidity":0.6,"windSpeed":7.01,"windBearing":258,"cloudCover":0.01,"pressure":1010.42,"ozone":333.68},{"time":1434268800,"summary":"Clear","icon":"clear-day","precipIntensity":0,"precipProbability":0,"temperature":80.41,"apparentTemperature":81.88,"dewPoint":63.44,"humidity":0.56,"windSpeed":7.74,"windBearing":251,"cloudCover":0.01,"pressure":1010.35,"ozone":334.36},{"time":1434272400,"summary":"Clear","icon":"clear-day","precipIntensity":0,"precipProbability":0,"temperature":82.97,"apparentTemperature":84.22,"dewPoint":63.48,"humidity":0.52,"windSpeed":8.69,"windBearing":247,"cloudCover":0,"pressure":1010.17,"ozone":334.67}]},"daily":{"summary":"Light rain on Monday and Tuesday, with temperatures peaking at 94°F on Sunday.","icon":"rain","data":[{"time":1434060000,"summary":"Partly cloudy starting in the evening.","icon":"partly-cloudy-night","sunriseTime":1434077269,"sunsetTime":1434134537,"moonPhase":0.85,"precipIntensity":0.0007,"precipIntensityMax":0.0027,"precipIntensityMaxTime":1434142800,"precipProbability":0.13,"precipType":"rain","temperatureMin":65.49,"temperatureMinTime":1434078000,"temperatureMax":89.15,"temperatureMaxTime":1434114000,"apparentTemperatureMin":65.49,"apparentTemperatureMinTime":1434078000,"apparentTemperatureMax":89.32,"apparentTemperatureMaxTime":1434114000,"dewPoint":61.8,"humidity":0.6,"windSpeed":3.71,"windBearing":152,"cloudCover":0.41,"pressure":1015.6,"ozone":331.58},{"time":1434146400,"summary":"Clear throughout the day.","icon":"clear-day","sunriseTime":1434163659,"sunsetTime":1434220971,"moonPhase":0.89,"precipIntensity":0,"precipIntensityMax":0,"precipProbability":0,"temperatureMin":67.49,"temperatureMinTime":1434160800,"temperatureMax":91.97,"temperatureMaxTime":1434200400,"apparentTemperatureMin":67.49,"apparentTemperatureMinTime":1434160800,"apparentTemperatureMax":93.5,"apparentTemperatureMaxTime":1434200400,"dewPoint":61.24,"humidity":0.56,"windSpeed":10.2,"windBearing":210,"cloudCover":0.11,"pressure":1011.59,"ozone":326.2},{"time":1434232800,"summary":"Light rain overnight.","icon":"rain","sunriseTime":1434250051,"sunsetTime":1434307403,"moonPhase":0.93,"precipIntensity":0.0003,"precipIntensityMax":0.0017,"precipIntensityMaxTime":1434315600,"precipProbability":0.01,"precipType":"rain","temperatureMin":67.59,"temperatureMinTime":1434250800,"temperatureMax":93.77,"temperatureMaxTime":1434290400,"apparentTemperatureMin":67.59,"apparentTemperatureMinTime":1434250800,"apparentTemperatureMax":94.68,"apparentTemperatureMaxTime":1434290400,"dewPoint":61.93,"humidity":0.55,"windSpeed":8.34,"windBearing":234,"cloudCover":0.02,"pressure":1009.07,"ozone":330.14},{"time":1434319200,"summary":"Light rain in the morning and overnight.","icon":"rain","sunriseTime":1434336445,"sunsetTime":1434393832,"moonPhase":0.96,"precipIntensity":0.006,"precipIntensityMax":0.0151,"precipIntensityMaxTime":1434337200,"precipProbability":0.83,"precipType":"rain","temperatureMin":66.25,"temperatureMinTime":1434402000,"temperatureMax":82.67,"temperatureMaxTime":1434369600,"apparentTemperatureMin":66.25,"apparentTemperatureMinTime":1434402000,"apparentTemperatureMax":84.25,"apparentTemperatureMaxTime":1434369600,"dewPoint":63.57,"humidity":0.68,"windSpeed":9.46,"windBearing":258,"cloudCover":0.47,"pressure":1008.38,"ozone":334.59},{"time":1434405600,"summary":"Light rain in the morning.","icon":"rain","sunriseTime":1434422842,"sunsetTime":1434480259,"moonPhase":0.99,"precipIntensity":0.0046,"precipIntensityMax":0.0143,"precipIntensityMaxTime":1434423600,"precipProbability":0.8,"precipType":"rain","temperatureMin":62.22,"temperatureMinTime":1434420000,"temperatureMax":75.95,"temperatureMaxTime":1434463200,"apparentTemperatureMin":62.22,"apparentTemperatureMinTime":1434420000,"apparentTemperatureMax":75.95,"apparentTemperatureMaxTime":1434463200,"dewPoint":54.35,"humidity":0.63,"windSpeed":5.85,"windBearing":327,"cloudCover":0.89,"pressure":1016.49,"ozone":329.61},{"time":1434492000,"summary":"Partly cloudy in the morning.","icon":"partly-cloudy-day","sunriseTime":1434509242,"sunsetTime":1434566684,"moonPhase":0.04,"precipIntensity":0,"precipIntensityMax":0,"precipProbability":0,"temperatureMin":59.04,"temperatureMinTime":1434506400,"temperatureMax":76.55,"temperatureMaxTime":1434549600,"apparentTemperatureMin":59.04,"apparentTemperatureMinTime":1434506400,"apparentTemperatureMax":76.55,"apparentTemperatureMaxTime":1434549600,"dewPoint":47.66,"humidity":0.49,"windSpeed":10.44,"windBearing":42,"cloudCover":0.18,"pressure":1019.3,"ozone":338.68},{"time":1434578400,"summary":"Mostly cloudy starting in the afternoon.","icon":"partly-cloudy-night","sunriseTime":1434595644,"sunsetTime":1434653106,"moonPhase":0.07,"precipIntensity":0,"precipIntensityMax":0,"precipProbability":0,"temperatureMin":58.39,"temperatureMinTime":1434596400,"temperatureMax":77.29,"temperatureMaxTime":1434636000,"apparentTemperatureMin":58.39,"apparentTemperatureMinTime":1434596400,"apparentTemperatureMax":77.29,"apparentTemperatureMaxTime":1434636000,"dewPoint":51.78,"humidity":0.54,"windSpeed":4.76,"windBearing":78,"cloudCover":0.24,"pressure":1017.7,"ozone":324.44},{"time":1434664800,"summary":"Partly cloudy in the morning.","icon":"partly-cloudy-day","sunriseTime":1434682049,"sunsetTime":1434739525,"moonPhase":0.1,"precipIntensity":0.0004,"precipIntensityMax":0.0015,"precipIntensityMaxTime":1434736800,"precipProbability":0.02,"precipType":"rain","temperatureMin":64.33,"temperatureMinTime":1434672000,"temperatureMax":80.87,"temperatureMaxTime":1434718800,"apparentTemperatureMin":64.33,"apparentTemperatureMinTime":1434672000,"apparentTemperatureMax":80.4,"apparentTemperatureMaxTime":1434718800,"dewPoint":55.23,"humidity":0.56,"windSpeed":6.88,"windBearing":292,"cloudCover":0.37,"pressure":1015.19,"ozone":328.7}]},"flags":{"sources":["isd","metno_ce","madis","fnmoc","cmc","gfs"],"isd-stations":["128305-99999","128380-99999","128390-99999","128400-99999","128430-99999"],"metno-license":"Based on data from the Norwegian Meteorological Institute. (http://api.met.no/)","madis-stations":["AT509","C5468","D3332","E3429","E4670","LHBP"],"units":"us"}}
      
      var json;
      try {
        json = JSON.parse(responseText);
      } catch(e) {
        console.log("Weather request from forecast.io failed:");
        console.log(e);
        if (retry_count < 3) {
          console.log('Retrying...');
          locationSuccess(pos, retry_count+1);
          return true;
        }
      }
 
      // Temperature in Kelvin requires adjustment
      var temperature = Math.round((json.currently.temperature-32) * 0.5556);
      console.log("Temperature is " + temperature);
 
      // Conditions
      var conditions = json.currently.summary;      
      console.log("Conditions are " + conditions);
      
      var icon = json.currently.icon;      
      console.log("Icon is " + icon);
      //"sys":{"type":1,"id":2856,"message":0.0228,"country":"US","sunrise":1426763649,"sunset":1426807263}
      var sunrise = json.daily.data[0].sunriseTime,
          sunset = json.daily.data[0].sunsetTime,
          sunriseDate = new Date(sunrise * 1000),
          sunrise_hour = sunriseDate.getHours(),
          sunrise_min = sunriseDate.getMinutes(),
          sunsetDate = new Date(sunset * 1000),
          sunset_hour = sunsetDate.getHours(),
          sunset_min = sunsetDate.getMinutes();
                    
      console.log("Sunrise, sunset: " + sunrise + " / " + sunset);
      console.log(sunrise_hour + ':' + sunrise_min + " / " + sunset_hour + ':' + sunset_min);
      /*
      xhrRequest("https://maps.googleapis.com/maps/api/timezone/json?location="+
                 pos.coords.latitude + "," + pos.coords.longitude + "&timestamp=" + sunrise, 'GET', 
                function(resp) {} );
      */
      var wind_speed = Math.round(json.currently.windSpeed /* m/s */ * 3.6), // km/h
          wind_direction = json.currently.windBearing;
      console.log("Wind speed, direction: " + wind_speed + " / " + wind_direction);
      
      var pressure = Math.round(json.currently.pressure);
      console.log("Pressure: " + pressure);

      var humidity = Math.round(json.currently.humidity*100);
      console.log("Humidity: " + humidity);
      
      var ozone = Math.round(json.currently.ozone);
      console.log("Ozone: " + ozone);

      var forecast = [],
          forecast_precip_intensity = [],
          forecast_precip_probability = [],
          forecast_first_time = 0,
          forecastHours = 0;
      
      json.hourly.data.forEach(function(entry) {
        if (forecastHours < 24) {
          var celsius = Math.round((entry.temperature-32) * 0.5556),
              precip_intensity_mm = Math.round(entry.precipIntensity * 254),  // tized mm-ben !
              precip_probability_percent = Math.round(entry.precipProbability * 100);
        
          //precip_intensity_mm = Math.floor(Math.random() * 20);
        
          forecast.push(celsius <= 0 ? celsius+255 : celsius);  // Nem lehet 0 a tömbben!
          forecast_precip_intensity.push(precip_intensity_mm);
          forecast_precip_probability.push(precip_probability_percent);
          console.log(entry.time + ': ' + celsius + ' C, precipIntensity: ' + precip_intensity_mm + ' mm, precipProbability: ' + precip_probability_percent + '%');
        
          if (forecastHours === 0) {
            var myDate = new Date(entry.time * 1000);
            forecast_first_time = myDate.getHours();
            console.log("Forecast first time: " + forecast_first_time);
          }
          forecastHours++;
        }
      });
      
      // Assemble dictionary using our keys
      var dictionary = {
        "KEY_READY": true,
        "KEY_TEMPERATURE": temperature,
        "KEY_CONDITIONS": conditions,
        "KEY_ICON": icon,
        "KEY_SUNRISE_HOUR": sunrise_hour,
        "KEY_SUNRISE_MIN": sunrise_min,
        "KEY_SUNSET_HOUR": sunset_hour,
        "KEY_SUNSET_MIN": sunset_min,
        "KEY_WIND_SPEED": wind_speed,
        "KEY_WIND_DIRECTION": wind_direction,
        "KEY_PRESSURE": pressure,
        "KEY_HUMIDITY": humidity,
        "KEY_OZONE": ozone,
        "KEY_FORECAST": forecast,
        "KEY_FORECAST_FIRST_TIME": forecast_first_time,
        "KEY_FORECAST_PRECIP_INTENSITY": forecast_precip_intensity,
        "KEY_FORECAST_PRECIP_PROBABILITY": forecast_precip_probability
      };
      
      console.log(JSON.stringify(dictionary));
 
      // Send to Pebble
      Pebble.sendAppMessage(dictionary,
        function(e) {
          console.log("Weather info sent to Pebble successfully!");
        },
        function(e) {
          console.log("Error sending weather info to Pebble!");
          console.log(JSON.stringify(e));
        }
      );
      
      return true;
    },
    function(xhr, status) {
      console.log('-- AJAX error');
      console.log('--  status:');
      console.log(status);
      console.log('--  xhr:');
      console.log(JSON.stringify(xhr));
    }
    
  );
}  // locationSuccess  

function locationError(err) {
  console.log("Error requesting location!");
  if (typeof err != 'undefined' && typeof err.code != 'undefined') {
    console.log("Error code: "+err.code+" Message: "+err.message);
    if (err.code == err.PERMISSION_DENIED) {
      console.log("Error: Access is denied!");
    } else if( err.code == 2) {
      console.log("Error: Position is unavailable!");
    }
  } else {
    console.log("No error details, because err is undefined or err.code is undefined");
    console.log(JSON.stringify(err));
  }
  locationSuccess(null, 0);
}
 
function getWeather() {
  //window.navigator.geolocation.watchPosition(
  navigator.geolocation.getCurrentPosition(
    locationSuccess,
    locationError,
    {enableHighAccuracy: true, timeout: 10000, maximumAge: 10000}
  );
}

function sendReadyMessage() {
  var dictionary = {
        "KEY_READY": true
      };
  
  Pebble.sendAppMessage(dictionary,
        function(e) {
          console.log("Ready sent to Pebble successfully!");
        },
        function(e) {
          console.log("Error sending ready to Pebble!");
          console.log(JSON.stringify(e));
        }
      );
}

// Listen for when the watchface is opened
Pebble.addEventListener('ready', 
  function(e) {
    console.log("PebbleKit JS ready!");
 
    // Get the initial weather
    //getWeather();
    ///sendReadyMessage();
    if (null === timerId) {
      timerId = setTimeout(function() {sendReadyMessage();}, 1000);
    } else {
      console.log("Not the first READY call, ignoring...");
    } 
  }
);
 
// Listen for when an AppMessage is received
Pebble.addEventListener('appmessage',
  function(e) {
    console.log('Received AppMessage in JS: ' + JSON.stringify(e.payload));
    return getWeather();
  }                     
);
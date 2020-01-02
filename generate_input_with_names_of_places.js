// utm converter to convert from lat-lng to x-y coordinates
var utmObj = require('utm-latlng');

// Google Maps API key with Geolocation and Distance Matrix APIs Enabled
var API_KEY = "";

var utm = new utmObj();

const googleMapsClient = require('@google/maps').createClient({
  key: API_KEY
});

var bus = 6;
var capacity = 32;
var kms = 92000;
var no_of_employees = 29;

// people count boaring at each place (ordered by places in origin)
var places_people_count = [0, 1, 5, 2, 5, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1];
var start_time = "6:15";
var end_time = "7:35";

var origin = ["Bosch Bidadi, Bengaluru",
"Devegowda Petrol Bunk, Bengaluru",
"Hoskeralli, Bengaluru",
"Channasandra RNSIT, Bengaluru",
"Kathriguppe, Bengaluru",
"Kamakya Theatre, Bengaluru",
"PESIT Collage, Bengaluru",
"Katherguppe Circle, Bengaluru",
"Bata Show Room, Bengaluru",
"Ittamadu, Bengaluru",
"Rajarajeshwari temple, Bengaluru",
"Jantha Bazar, Bengaluru",
"Rajarajeshwarinagar Double Road, Bengaluru",
"Kanthi Sweets RR Nagar, Bengaluru",
"Chowdeshwari Talkies, Bengaluru",
"Jayanagar, Bengaluru",
"Kadirenahallli, Bengaluru",
"Mantri Apartment, Bengaluru",
"Kodipalya (Uttarahalli main Road), Bengaluru",
"Uttarahalli road Kengeri, Bengaluru"
];


var places = new Map();
var len = origin.length;

for(var i = 0; i < len; ++i)
{
  googleMapsClient.geocode({
      address: origin[i]
    }, function(err, response) {
      if (!err) {
        // console.log(response.query.address);
        // console.log(JSON.stringify(response.json.results[0].geometry.location, null, "      "));

        var ans1 = utm.convertLatLngToUtm(response.json.results[0].geometry.location.lat
          , response.json.results[0].geometry.location.lng, 10);

        // process.stdout.write(String(ans1.Easting) + " ");
        // console.log(ans1.Northing);
        places.set(response.query.address, [ans1.Easting, ans1.Northing]);
      }
      else
      {
        console.log(err);
      }
    });
}
setTimeout(function() {
  process.stdout.write(String(places.get(origin[0])[0] + " "));
  process.stdout.write(String(places.get(origin[0])[1]) + "\n");

  process.stdout.write(String(len) + "\n");

  for (var i = 0; i < origin.length; i++) {
    process.stdout.write(String(origin[i]) + "\n");
    process.stdout.write(String(places.get(origin[i])[0]) + "  ");
    process.stdout.write(String(places.get(origin[i])[1]) + "\n");
  }
  console.log(bus);
  console.log(capacity);
  process.stdout.write(String(start_time) + " " + String(end_time));
  console.log();
  console.log(no_of_employees);

  for(var i = 0; i < places_people_count.length; ++i)
    process.stdout.write(String(places_people_count[i]) + " ");
  console.log();
  callMatrixApi();
  setTimeout(function(){
    process.stdout.write(String(kms));
  }, 5000)
}, 6000);

function callMatrixApi() {
  var ENTRIES_PER_QUERY = 4;
  for(var k = 0; k < origin.length; k = k + ENTRIES_PER_QUERY)
  {
    googleMapsClient.distanceMatrix({
        origins: origin.slice(k, k + ENTRIES_PER_QUERY),
        destinations: origin
    }, function(err, response) {
      if (!err) {
        // console.log(JSON.stringify(response.json.origin_addresses, null, "      "));
        // console.log(JSON.stringify(response.json.rows[0], null, "      "));

        for(var i = 0; i < ENTRIES_PER_QUERY; ++i) 
        {
          for(var j = 0; j < origin.length; ++j)
          {
            process.stdout.write(JSON.stringify(response.json.rows[i].elements[j].distance.value, null, "      ") + " ");
          }
          console.log();
        }
       }
      else
      {
        console.log(err);
      }
    });
  }
}


// utm converter to convert from lat-lng to x-y coordinates
var utmObj = require('utm-latlng');

// Google Maps API key with Geolocation and Distance Matrix APIs Enabled
var API_KEY = "";

var utm = new utmObj();

const googleMapsClient = require('@google/maps').createClient({
  key: API_KEY
});

var bus = 10;
var capacity = 50;
var kms = 49700;
var no_of_employees = 95;

// people count boaring at each place (ordered by places in origin)
var places_people_count = [0, 1, 22, 1, 1, 7, 1, 2, 1, 2, 1, 5, 1, 4, 4, 1, 2, 1, 2, 7, 1, 3, 2, 4, 6, 1, 1, 1, 2, 2, 1, 5 ];
var start_time = "04:15";
var end_time = "05:35";

var origin = [
"Bosch Bidadi, Bengaluru",
"Adugodi, Bengaluru",
"Adugodi Signal, Bengaluru",
"Anand Ashram, Bengaluru",
"Aneypalya, Bengaluru",
"Arakere BTS Layout, Bengaluru",
"Arakere Gate, Bengaluru",
"Ashram Bus Stop, Bengaluru",
"Ashram HDFC Bank, Bengaluru",
"Austin Town, Bengaluru",
"BPL Stop, Bengaluru",
"BTM, Bengaluru",
"BTM 2nd stage, Bengaluru",
"Bannerghatta Road, Bengaluru",
"Canara  Bank, Bengaluru",
"Dairy Circle, Bengaluru",
"Gottigere, Bengaluru",
"Hulimavu Gate, Bengaluru",
"Jayadeva Hospital Junction, Bengaluru",
"Koramangala, Bengaluru",
"Koramangala Depot, Bengaluru",
"Koramangala Police Station, Bengaluru",
"Lakkasandra Bus Stop, Bengaluru",
"Mico Layout, Bengaluru",
"Mico Signal, Bengaluru",
"Sagar Appolo Hospital, Bengaluru",
"Silk Board, Bengaluru",
"Spar Stop, Bengaluru",
"Thilaknagar, Bengaluru",
"Udupi Guarden, Bengaluru",
"Vijaya Bank Adugodi, Bengaluru",
"Viveknagar, Bengaluru",
];

var coordinates = 
[
  [12.7972,77.4239],
  [ 12.9435045,77.6075158 ],
  [ 12.9436745,77.6077108 ],
  [ 12.9268719,77.5990938 ],
  [ 12.9478156,77.6027586 ],
  [ 12.9229302,77.5663966 ],
  [ 12.889589,77.5976873 ],
  [ 12.9268719,77.5990938 ],
  [ 12.9485816,77.5797068 ],
  [ 12.9567638,77.6129863 ],
  [ 12.887181,77.5970994 ],
  [ 12.9164844,77.6016567 ],
  [ 12.9125291,77.5982493 ],
  [ 12.9089725,77.5979482 ],
  [ 12.9481897,77.6002658 ],
  [ 12.9389163,77.6008787 ],
  [ 12.8560296,77.5886844 ],
  [ 12.8881313,77.5914776 ],
  [ 12.9175363,77.5999589 ],
  [ 12.9350054,77.6115462 ],
  [ 12.9420488,77.6232414 ],
  [ 12.9408786,77.6198734 ],
  [ 12.9368414,77.6004492 ],
  [ 12.9164844,77.6016567 ],
  [ 12.9442415,77.6026388 ],
  [ 12.9268719,77.5990938 ],
  [ 12.9169078,77.6216554 ],
  [ 12.9640503,77.5711259 ],
  [ 12.9218711,77.5986135 ],
  [ 12.9176934,77.6089431 ],
  [ 12.9378759,77.5944627 ],
  [ 12.9521797,77.6188548 ],
];

var places = new Map();
var len = origin.length;

// ********* skipping this part as we already have latitudes and longitudes ***********s
// for(var i = 0; i < len; ++i)
// {
//   googleMapsClient.geocode({
//       address: origin[i]
//     }, function(err, response) {
//       if (!err) {
//         // console.log(response.query.address);
//         // console.log(JSON.stringify(response.json.results[0].geometry.location, null, "      "));

//         var ans1 = utm.convertLatLngToUtm(response.json.results[0].geometry.location.lat
//           , response.json.results[0].geometry.location.lng, 10);

//         // process.stdout.write(String(ans1.Easting) + " ");
//         // console.log(ans1.Northing);
//         places.set(response.query.address, [ans1.Easting, ans1.Northing]);
//       }
//       else
//       {
//         console.log(err);
//       }
//     });
// }

  for(var i = 0; i < origin.length; ++i)
  {
    // console.log(i);
     var ans1 = utm.convertLatLngToUtm(coordinates[i][0], coordinates[i][1], 10);

        // process.stdout.write(String(ans1.Easting) + " ");
        // console.log(ans1.Northing);
        places.set(origin[i], [ans1.Easting, ans1.Northing]);
  }

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
  }, 5000);

function callMatrixApi() {
  var ENTRIES_PER_QUERY = 1;
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


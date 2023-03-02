#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <TinyGPS++.h>
#include "thingProperties.h"
TinyGPSPlus gps;                     // The TinyGPS++ object
SoftwareSerial ss(4, 5);
const char *ssid = "8483";           // ssid of your wifi
const char *password = "8665485635"; // password of your wifi
double latitudes, longitudes;
int year, month, date, hour, minute, second;
String date_str, time_str, lat_str, lng_str;
WiFiServer server(80);

void setup() {
  Serial.begin(9600);
  // Initialize serial and wait for port to open:
  ss.begin(9600);
  // This delay gives the chance to wait for a Serial Monitor without blocking if none is found
  delay(1500); 

  // Defined in thingProperties.h
  initProperties();

  // Connect to Arduino IoT Cloud
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);
  
  /*
     The following function allows you to obtain more information
     related to the state of network and IoT Cloud connection and errors
     the higher number the more granular information you’ll get.
     The default is 0 (only errors).
     Maximum is 4
 */

  Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);           // connecting to wifi
    while (WiFi.status() != WL_CONNECTED) // while wifi not connected
    {
        delay(500);
        Serial.print("."); // print "...."
    }
    Serial.println("");
    Serial.println("WiFi connected");
    server.begin();
    Serial.println("Server started");
    Serial.println(WiFi.localIP()); // Print the IP address
  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();
}

void loop() {
  ArduinoCloud.update();
  // Your code here 
  while (ss.available() > 0)     // while data is available
        if (gps.encode(ss.read())) // read gps data
        {
            if (gps.location.isValid()) // check whether gps location is valid
            {
                latitudes = gps.location.lat();
                lat_str = String(latitudes, 9); // latitudes location is stored in a string
                longitudes = gps.location.lng();
                lng_str = String(longitudes, 9); 
                coordinates = {latitudes, longitudes};// longitudes location is stored in a string
                latitude = lat_str;
                longitude = lng_str;
            }
            if (gps.date.isValid()) // check whether gps date is valid
            {
                date_str = "";
                date = gps.date.day();
                month = gps.date.month();
                year = gps.date.year();
                if (date < 10)
                    date_str = '0';
                date_str += String(date); // values of date,month and year are stored in a string
                date_str += " / ";

                if (month < 10)
                    date_str += '0';
                date_str += String(month); // values of date,month and year are stored in a string
                date_str += " / ";
                if (year < 10)
                    date_str += '0';
                date_str += String(year); // values of date,month and year are stored in a string
            }
            if (gps.time.isValid()) // check whether gps time is valid
            {
                time_str = "";
                hour = gps.time.hour();
                minute = gps.time.minute();
                second = gps.time.second();
                minute = (minute + 60); // converting to IST
                if (minute > 59)
                {
                    minute = minute - 60;
                    hour = hour + 1;
                }
                hour = (hour + 6);
                if (hour > 23)
                    hour = hour - 24; // converting to IST
                if (hour < 10)
                    time_str = '0';
                time_str += String(hour); // values of hour,minute and time are stored in a string
                time_str += " : ";
                if (minute < 10)
                    time_str += '0';
                time_str += String(minute); // values of hour,minute and time are stored in a string
                time_str += " : ";
                if (second < 10)
                 time_str += String(second); // values of hour,minute and time are stored in a string
            }
            date_Time = date_str;
            date_Time += "\n";
            date_Time += time_str;
            
        }
    WiFiClient client = server.available(); // Check if a client has connected
    if (!client)
    {
        return;
    }
    // Response
    String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n <!DOCTYPE html>";
    s += "<html lang='en'>";

    s += "<head>";
    s += "<meta charset='UTF-8'>";
    s += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
    s += "<meta http-equiv='X-UA-Compatible' content='ie=edge'>";
    s += "<title>NEO-6M GPS Readings</title>";
    s += "<style>";
    s += "#map {";
    s += "height: 700px;";
    s += "width: 100%;";
    s += "}";
    s += "</style>";
    s += "</head>";

    s += "<body>";
    s += "<h1 style; font-size:300%;; ALIGN=CENTER>NEO-6M GPS Readings</h1>";
    s += "<p ALIGN=CENTER style=font-size:150%;;> <b>Location Details</b></p>";
    s += "<table align='center' cellspacing='0' cellpadding='5' border='1' bgcolor='#000000'>";

    s += "<tr bgcolor='#ffffff'>";
    s += "<th>latitudes</th>";
    s += "<td ALIGN=CENTER>";
    s += lat_str;
    s += "</td>";
    s += "</tr>";
    s += "<tr bgcolor='#ffffff'>";
    s += "<th>longitudes</th>";
    s += "<td ALIGN=CENTER>";
    s += lng_str;
    s += "</td>";
    s += "</tr>";
    s += "<tr bgcolor='#ffffff'>";
    s += "<th>Date</th>";
    s += "<td ALIGN=CENTER>";
    s += date_str;
    s += "</td>";
    s += "</tr>";
    s += "<tr bgcolor='#ffffff'>";
    s += "<th>Time</th>";
    s += "<td ALIGN=CENTER>";
    s += time_str;
    s += "</td>";
    s += "</tr>";
    s += "</table>";
    s += "<p ALIGN=CENTER style=font-size:150%;> <b>Map</b></p>";
    s += "<div id='map'></div>";
    s += "<script>";

    s += "var map;";
    s += "var marker;";
    s += "var longitudes = ";
    s += lng_str;
    s += ";";
    s += "var latitudes = ";
    s += lat_str;
    s += ";";
    s += "var INTERVAL = 5000;";

    s += "function initMap() {";
    s += "var options = {";
    s += "zoom: 16,";
    s += "center: { lat: ";
    s += lat_str;
    s += ", lng: ";
    s += lng_str;
    s += "},";
    s += "mapTypeId: google.maps.MapTypeId.HYBRID,";
    s += "};";
    s += "map = new google.maps.Map(document.getElementById('map'), options);";
    s += "}";

    s += "function getMarkers() {";
    s += "var newLatLng = new google.maps.LatLng(";
    s += lat_str;
    s += ",";
    s += lng_str;
    s += ");";
    s += "marker = new google.maps.Marker({";
    s += "position: newLatLng,";
    s += "map: map";
    s += "});";
    s += "}";

    s += "window.setInterval(getMarkers, INTERVAL);";

    s += "</script>";
    s += "<script async defer";
    s += " src='https://maps.googleapis.com/maps/api/js?key=AIzaSyDZuWS9zjT9GnNPK3mqjBjb_wllLfVMEk0&callback=initMap'>";
    s += "</script>";

    if (gps.location.isValid())
    {

        s += "<p align=center><a style="
             "color:RED;font-size:125%;"
             " href="
             "http://maps.google.com/maps?&z=15&mrt=yp&t=k&q=";
        s += lat_str;
        s += "+";
        s += lng_str;
        s += ""
             " target="
             "_top"
             ">Click here</a> to open the location in Google Maps.</p>";
    }

    s += "</body> </html> \n";

    client.print(s); // all the values are send to the webpage
    delay(100);
    
}
/*
  Since GPS is READ_WRITE variable, onGPSChange() is
  executed every time a new value is received from IoT Cloud.
*/
void onGPSChange()  {
}
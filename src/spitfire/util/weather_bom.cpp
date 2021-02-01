#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>

#include <spitfire/spitfire.h>
#include <spitfire/communication/http.h>
#include <spitfire/storage/file.h>
#include <spitfire/storage/json.h>
#include <spitfire/util/weather_bom.h>

namespace spitfire {

namespace util {

namespace weather {

namespace bom {

  class text_content_listener : public spitfire::network::http::cRequestListener {
  public:
    virtual void _OnTextContentReceived(const std::string& sContent) override {
      content = sContent;
    }

    std::string content;
  };

  const std::string API_LOCATIONS = "https://api.weather.bom.gov.au/v1/locations";

  bool ParseSearchLocationsJSONResponse(const std::string& response_json, std::vector<locations_response_entry>& locations)
  {
    locations.clear();

    spitfire::json::cDocument document;
    spitfire::json::reader reader;
    if (!reader.ReadFromStringUTF8(document, response_json)) {
      return false;
    }

    const spitfire::json::cNode* data = document.GetChild("data");
    if ((data == nullptr) || !data->IsTypeArray()) {
      return false;
    }

    const std::vector<spitfire::json::cNode*> data_children = data->GetValueObjectOrArray();
    for (auto&& child : data_children) {
      locations_response_entry item;
      child->GetAttribute("geohash", item.geohash);
      child->GetAttribute("id", item.id);
      child->GetAttribute("name", item.name);
      child->GetAttribute("postcode", item.postcode);
      child->GetAttribute("state", item.state);

      locations.push_back(item);
    }

    return true;
  }

  bool ParseGetWeatherJSONResponse(const std::string& weather_json, weather_reading& reading)
  {
    spitfire::json::cDocument document;
    spitfire::json::reader reader;
    if (!reader.ReadFromStringUTF8(document, weather_json)) {
      return false;
    }

    const spitfire::json::cNode* data = document.GetChild("data");
    if ((data == nullptr) || !data->IsTypeObject()) {
      return false;
    }
  
    // At least the temperature should be present
    data->GetAttribute("temp", reading.temp_celcius);
    data->GetAttribute("temp_feels_like", reading.temp_feels_like_celcius);

    const spitfire::json::cNode* wind = data->GetChild("wind");
    if ((wind != nullptr) && wind->IsTypeObject()) {
      wind->GetAttribute("speed_kilometre", reading.wind.speed_kilometres);
      wind->GetAttribute("direction", reading.wind.direction);
    }

    const spitfire::json::cNode* gust = data->GetChild("gust");
    if ((gust != nullptr) && gust->IsTypeObject()) {
      gust->GetAttribute("speed_kilometre", reading.gust.speed_kilometres);
    }

    data->GetAttribute("rain_since_9am", reading.rain_since_9am_millimetres);
    data->GetAttribute("humidity", reading.humidity);

    return true;
  }

  // Example API calls:
  // https://api.weather.bom.gov.au/v1/locations?search=2615
  // https://api.weather.bom.gov.au/v1/locations?search=Hawker+ACT

  bool SearchLocations(const std::string& search, std::vector<locations_response_entry>& locations)
  {
    locations.clear();

    const std::string url = API_LOCATIONS + "?search=" + search;

    //spitfire::network::http::cHTTP http;
    text_content_listener listener;
    //http.Download(url, spitfire::network::http::METHOD::GET, listener);

    spitfire::network::http::DownloadHTTPS(url, listener);

    const std::string response_json = listener.content;
    std::cout<<"Response JSON: "<<response_json<<std::endl;

    return ParseSearchLocationsJSONResponse(response_json, locations);
  }

  // Example API calls:
  // https://api.weather.bom.gov.au/v1/locations/r3dp2m/observations
  bool GetWeatherReading(const std::string& geohash, weather_reading& reading)
  {
    // Trim the geohash to just 6 characters for this request
    const std::string geohash_trimmed = geohash.substr(0, 6);

    const std::string url = API_LOCATIONS + "/" + geohash_trimmed + "/observations";
  
    //spitfire::network::http::cHTTP http;
    text_content_listener listener;
    //http.Download(url, spitfire::network::http::METHOD::GET, listener);

    spitfire::network::http::DownloadHTTPS(url, listener);

    const std::string response_json = listener.content;
    std::cout<<"Response JSON: "<<response_json<<std::endl;

    return ParseGetWeatherJSONResponse(response_json, reading);
  }
}

}

}

}

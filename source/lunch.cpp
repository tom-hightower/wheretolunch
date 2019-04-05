#include <iostream>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include "curl/curl.h"
#include "json/json.h"

using namespace std;
namespace
{
    size_t callback(
        const char* in,
        size_t size,
        size_t num,
        string* out)
    {
        const size_t totalBytes(size * num);
        out->append(in, totalBytes);
        return totalBytes;
    }
}

static Json::Value* get_curl_json(string url) {
    CURL* curl = curl_easy_init();

    // Set remote URL.
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    // Response information.
    int httpCode(0);
    unique_ptr<string> httpData(new string());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, httpData.get());

    // Run our HTTP GET command, capture the HTTP response code, and clean up.
    curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
    curl_easy_cleanup(curl);
    Json::Value *jsonData = new Json::Value;

    if (httpCode == 200) {
        cout << "\nGot successful response from " << url << endl;

        Json::Reader jsonReader;

        if (jsonReader.parse(*httpData.get(), *jsonData)) {
            return jsonData;
        } else {
            cout << "Could not parse HTTP data as JSON" << endl;
            cout << "HTTP data was:\n" << *httpData.get() << endl;
            return jsonData;
        }
    } else {
        cout << "Couldn't GET from " << url << " - exiting" << endl;
        return jsonData;
    }
}

enum StringValue {
    evNotDefined,
    evDistance,
    evLocation,
    evHelp,
    evMaxPrice,
    evMinPrice,
    evZipcode
};

static std::map<std::string, StringValue> s_mapStringValues;
static char szInput[_MAX_PATH];
static void Initialize();

string url_builder(string api, vector<string> args) {
    string url = "http://localhost:5000/" + api;
    if (!args.empty()) {
        url += "?";
        for (int i = 0; i < args.size() - 1; i+=2) {
            if (!args[i + 1].empty()) {
                url += args[i];
                url += "=" + args[i+1] + "&";
            }
        }
    }
    return url;
}

static void show_usage(string name) {
    cerr << "Usage: " << name << " <option(s)>\n"
	    "If no location is specified through -l or -z, the application defaults to the address found via your IP address info\n\n"
        "Options:\n"
	    "\t-d,--dist=DISTANCE\t\tSpecify maximum distance radius (default 4km)\n"
	    "\t-l,--location=COORD\t\tSpecify location coordinates in the form 00.000,00.000\n"
        "\t-h,--help\t\tShow this help message\n"
	    "\t-m,--maxprice=PRICE\t\tSpecify the max price range on a scale of 0-4 (cheapest to most expensive)\n"
	    "\t-n,--minprice=PRICE\t\tSpecify the min price range on a scale of 0-4 (cheapest to most expensive)\n"
	    "\t-z,--zip=ZIPCODE\t\tSpecify location via zipcode"
        << endl;
}

int main(int argc, char* argv[]) {
    bool help = false;

    Initialize();
    string distance, location, maxprice, minprice, zipcode;
    for (int i = 1; i < argc + 1; i++) {
        if (argv[i]) {
            switch (s_mapStringValues[string(argv[i])]) {
            case evDistance:
                if (argv[i++]) distance = argv[i++];
                break;
            case evLocation:
                if (argv[i++]) location = argv[i++];
                break;
            case evMaxPrice:
                if (argv[i++]) maxprice = argv[i++];
                break;
            case evHelp:
                help = true;
                break;
            case evMinPrice:
                if (argv[i++]) minprice = argv[i++];
                break;
            case evZipcode:
                if (argv[i++]) zipcode = argv[i++];
                break;
            }
        }
    }

    cout << distance << location << maxprice << minprice << zipcode;
    if (help) {
        show_usage(argv[0]);
        return 1;
    }
    const string ip_url = url_builder("ipinfo", {});
    Json::Value *ip_data = get_curl_json(ip_url);
    if (!(*ip_data)) {
        cout << "Failed to get IP Data" << endl;
        return 1;
    }
    string ipcoord = (*ip_data)["loc"].asString();

    vector<string> google_args = {
        "dist", distance,
        "coord", location,
        "maxprice", maxprice,
        "minprice", minprice,
        "zip", zipcode,
        "ipcoord", ipcoord
    };

    const string google_url = url_builder("google", google_args);
    Json::Value *google_data = get_curl_json(google_url);
    cout << (*google_data)["results"] << endl;
    
    //const string ipUrl = string("http://localhost:5000/ipinfo");
    //const string placeUrl = string("http://localhost:5000/google?zip=30332");
    //auto thing = get_curl_json(ipUrl);
    //auto thing2 = get_curl_json(placeUrl);

    return 0;
}

void Initialize() {
    s_mapStringValues["-d"] = evDistance;
    s_mapStringValues["-l"] = evLocation;
    s_mapStringValues["-h"] = evHelp;
    s_mapStringValues["-m"] = evMaxPrice;
    s_mapStringValues["-n"] = evMinPrice;
    s_mapStringValues["-z"] = evZipcode;
}
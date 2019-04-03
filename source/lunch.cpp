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

static int get_curl_json(string url) {
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

    if (httpCode == 200)
    {
        cout << "\nGot successful response from " << url << endl;

        Json::Value jsonData;
        Json::Reader jsonReader;

        if (jsonReader.parse(*httpData.get(), jsonData))
        {
            cout << "Successfully parsed JSON data" << endl;
            cout << "\nJSON data received:" << endl;
            cout << jsonData.toStyledString() << endl;

            const string dateString(jsonData["date"].asString());
            const size_t unixTimeMs(
                jsonData["milliseconds_since_epoch"].asUInt64());
            const string timeString(jsonData["time"].asString());

            cout << "Natively parsed:" << endl;
            cout << "\tDate string: " << dateString << endl;
            cout << "\tUnix timeMs: " << unixTimeMs << endl;
            cout << "\tTime string: " << timeString << endl;
            cout << endl;
        }
        else
        {
            cout << "Could not parse HTTP data as JSON" << endl;
            cout << "HTTP data was:\n" << *httpData.get() << endl;
            return 1;
        }
    }
    else
    {
        cout << "Couldn't GET from " << url << " - exiting" << endl;
        return 1;
    }
}

static void show_usage(string name) {
    cerr << "Usage: " << name << " <option(s)>"
        "Options:\n"
        "\t-h,--help\t\tShow this help message\n"
        << endl;
}

int main(int argc, char* argv[]) {
    if (argc > 2) {
        show_usage(argv[0]);
        return 1;
    }
    const string ipUrl = string("http://localhost:5000/ipinfo");
    const string placeUrl = string("http://localhost:5000/google");
    auto thing = get_curl_json(ipUrl);
    auto thing2 = get_curl_json(placeUrl);

    return 0;
}

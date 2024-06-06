#include <iostream>
#include <string>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <windows.h>

using json = nlohmann::json;

const std::string API_KEY = getenv("WEATHER_API_KEY");
const std::string BASE_URL = "https://api.weatherapi.com/v1/current.json?key=" + API_KEY + "&aqi=yes&lang=pl&q=";

size_t WriteCallback(void *contents, const size_t size, const size_t nmemb, std::string *s) {
    const size_t totalSize = size * nmemb;
    s->append(static_cast<char *>(contents), totalSize);
    return totalSize;
}

std::string fetchWeatherData(const std::string &location) {
    std::string readBuffer;
    if (CURL *curl = curl_easy_init()) {
        if (char *output = curl_easy_escape(curl, location.c_str(), location.length())) {
            const std::string url = BASE_URL + output;
            curl_free(output);

            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
            const CURLcode res = curl_easy_perform(curl);
            curl_easy_cleanup(curl);

            if (res != CURLE_OK) {
                std::cerr << "Error: " << curl_easy_strerror(res) << "\n";
            }
        }
    }
    return readBuffer;
}

void printWeather(const json &weatherData) {
    if (weatherData.contains("error")) {
        std::cout << "Error: " << weatherData["error"]["message"] << "\n";
        return;
    }

    std::cout << "╔═══════════════════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║ Pogoda w 📌 " << weatherData["location"]["name"].get<std::string>() << ", " << weatherData["location"]["country"].get<std::string>() << "\n";
    std::cout << "╠═══════════════════════════════════════════════════════════════════════════════╣\n";
    std::cout << "║\n";
    std::cout << "║  ℹ️  " << weatherData["current"]["condition"]["text"].get<std::string>() << "\n";
    std::cout << "║\n";
    std::cout << "║  🌡️  Temperatura: " << weatherData["current"]["temp_c"] << "°C (odczuwalna " << weatherData["current"]["feelslike_c"] << "°C)\n";
    std::cout << "║  💦  Wilgotność: " << weatherData["current"]["humidity"] << "%\n";
    std::cout << "║  🅿️  Ciśnienie: " << weatherData["current"]["pressure_mb"] << " hPa\n";
    std::cout << "║  ☀️  Indeks UV: " << weatherData["current"]["uv"] << "\n";
    std::cout << "║  🍃  Prędkość wiatru: " << weatherData["current"]["wind_kph"] << " km/h\n";
    const std::string aqi[] = {"Bardzo dobra", "Dobra", "Niezdrowa dla osób wrażliwych", "Niezdrowa", "Bardzo niezdrowa", "Chuj*wa"};
    std::cout << "║  🚬  Jakość powietrza: " << aqi[weatherData["current"]["air_quality"]["us-epa-index"].get<int>() - 1] << "\n";
    std::cout << "║\n";
    std::cout << "║  🔁  Ostatnio zaktualizowano: " << weatherData["current"]["last_updated"].get<std::string>() << "\n";
    std::cout << "║\n";
    std::cout << "╚═══════════════════════════════════════════════════════════════════════════════╝\n";
}


int main() {
    SetConsoleOutputCP(CP_UTF8);

    std::cout << "╔═══════════════════════════╗\n";
    std::cout << "║  🌤️  Prognoza pogody  🌤️  ║\n";
    std::cout << "╚═══════════════════════════╝\n";

    std::string location;
    while (true) {
        std::cout << "\n📍 Wpisz nazwę miasta: ";
        std::getline(std::cin, location);
        if (location.empty()) {
            std::cout << "Lokalizacja nie może być pusta.\n";
        } else {
            break;
        }
    }

    system("cls");

    std::string response = fetchWeatherData(location);
    const auto weatherData = json::parse(response);

    printWeather(weatherData);

    system("pause");

    return 0;
}

#include <iostream>
#include <unordered_map>
#include <map>
#include <format>
#include <tuple>

template<typename T>
struct hash;

struct CityRecord
{
    std::string Name;
    u_int64_t Population;
    double Latitude, Longtitude;
    
    bool operator==(const CityRecord& other) const
    {
        return (Population == other.Population);
    }
};

struct compareCity
{
    bool operator() (const CityRecord& c1, const CityRecord& c2) const
    {
        return c1.Name < c2.Name;
    }
};

namespace std
{
    template<>
    struct hash<CityRecord>
    {
        size_t operator() (const CityRecord& key) const
        {
            return hash<std::string>()(key.Name);
        }
    };

//     template<>
//     struct less<CityRecord>
//     {
//         bool operator() (const CityRecord& c1, const CityRecord& c2) const
//         {
//             return c1.Name < c2.Name;
//         }
//     };
// }
}
int main(int, char**) {

    std::map<std::string, CityRecord> cityMap;
    cityMap["Melbourne"] = CityRecord{ "Melbourne", 5'000'000, 2.4, 9.4 };
    cityMap["Lol-town"] = CityRecord{ "Lol-town", 7'000'000, 2.7, 9.4 };
    cityMap["Berlin"] = CityRecord{ "Berlin", 8'800'000, 5.4, 7.4 };
    cityMap["Paris"] = CityRecord{ "Paris", 7'600'000, 8.4, 3.4 };
    cityMap["London"] = CityRecord{ "London", 4'500'000, 10.4, 1.4 };

    CityRecord& berlinData = cityMap["Berlin"];

    auto [name, po, lati, longti] = berlinData;

    std::cout << std::format("{}, {}, {}, {}.\n", name, po, lati, longti);

    std::cout << "--------------------------------\n";

    for (auto &&elem : cityMap)
    {
        std::cout << elem.first << ": " << elem.second.Population << "\n";
    }
    std::cout << "--------------------------------\n";

    std::unordered_map<std::string, CityRecord> ucityMap;

    ucityMap["Melbourne"] = CityRecord{ "Melbourne", 5'000'000, 2.4, 9.4 };
    ucityMap["Lol-town"] = CityRecord{ "Lol-town", 7'000'000, 2.7, 9.4 };
    ucityMap["Berlin"] = CityRecord{ "Berlin", 8'800'000, 5.4, 7.4 };
    ucityMap["Paris"] = CityRecord{ "Paris", 7'600'000, 8.4, 3.4 };
    ucityMap["London"] = CityRecord{ "London", 4'500'000, 10.4, 1.4 };

    for (auto &&elem : ucityMap)
    {
        std::cout << elem.first << ": " << elem.second.Population << "\n";
    }

    // std::cout << ucityMap.size() << "\n";

    CityRecord& foundCity = ucityMap["Paris"];

    foundCity.Name = "Paris2";

    // std::cout << ucityMap.at("Paris").Name << "\n";

    CityRecord& parisData = ucityMap.at("Paris");
    
    auto [name2, po2, lati2, longti2] = parisData;
    // std::cout << std::format("{}, {}, {}, {}.\n", name2, po2, lati2, longti2);
    // std::cout << "--------------------------------\n";

    std::map<CityRecord, u_int32_t, compareCity> foundedMap;

    foundedMap[{"Hanoi", 1'000'000, 1.4, 8.8}] = 10101900;

    CityRecord hanoi{ "Hanoi", 1'000'000, 1.4, 8.8 };

    // std::cout << foundedMap[hanoi] << "\n";

    return 0;
}

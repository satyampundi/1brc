#include <bits/stdc++.h>
#include <string>
#include <cstdint>
#include <chrono>

using namespace std;

string dataPath = "data/measurements.txt";
int main()
{
    auto start = std::chrono::high_resolution_clock::now();

    map<string, vector<double>> cityData; // 0->min 1->max 2->sum 3->count

    ifstream data(dataPath);
    if (!data.is_open())
    {
        std::cerr << "Error opening file!" << std::endl;
        return 1; // Indicate an error
    }
    int cnt = 0;
    // double max = INT_MAX, min = INT_MIN, count = 0;
    // double sum = 0;

    string line;
    while (getline(data, line))
    {
        istringstream iss(line);
        string city;
        double value;
        if (cnt == 100000)
        {
            break;
        }

        if (std::getline(iss, city, ';') && iss >> value)
        {
            if (cityData.count(city))
            {
                vector<double> t = cityData[city];
                t[2] += value;
                if (t[0] > value)
                {
                    t[0] = value;
                }
                if (t[1] < value)
                {
                    t[1] = value;
                }
                t[3]++;
                cityData[city] = t;
            }
            else
            {
                vector<double> t(4, 0);
                t[3] = 1;
                t[2] = value;
                t[1] = value;
                t[0] = value;
                cityData[city] = t;
            }
            cnt++;
        }
    }

    for (auto x : cityData)
    {
        cout << x.first << " min: " << x.second[0] << " max: " << x.second[1] << " mean: " << x.second[2] / x.second[3] << endl;
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    cout << "Execution Time: " << duration.count() << " ms" << endl;

    return 0;
}
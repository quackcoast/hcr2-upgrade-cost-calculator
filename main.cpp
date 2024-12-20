#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <fstream>
#include <sstream>

using namespace std;

struct Upgrade {
    string carName;
    int level;
    int cost;
};

int getUserBudget() {
    int budget = 0;
    cout << "Enter your available budget: ";
    cin >> budget;
    return budget;
}

vector<char> carUnlocked() {
    vector<char> hasCar;
    vector<string> carList = {
        "Hill Climber", "Hill Climber Mk. 2", "Sports Car", "Dune Buggy", "Super Diesel",
        "Superbike", "Supercar", "Rally Car", "Lowrider", "Beast", "Hot Rod",
        "Racing Truck", "Formula", "Muscle Car", "Monster Truck", "Rotator",
        "Chopper", "Tank", "CC-EV", "Bus", "Rock Bouncer", "Hoverbike",
        "Raider", "Moonlander", "Monowheel", "Snowmobile", "Motocross",
        "Tractor", "Scooter", "Glider"
    };

    for (int i = 0; i < carList.size(); i++) {
        char response;
        cout << "Do you have the " << carList[i] << "? (y/n)" << endl;
        cin >> response;

        if (response == 'y' || response == 'n') {
            hasCar.push_back(response);
        } else {
            cout << "Invalid input. Please enter 'y' or 'n'." << endl;
            i--;
        }
    }

    return hasCar;
}

// Function to get four values for each unlocked car
vector<vector<int>> getCarAttributes(const vector<char>& hasCar, const vector<string>& carList) {
    vector<vector<int>> carAttributes;

    for (int i = 0; i < hasCar.size(); i++) {
        if (hasCar[i] == 'y') {
            cout << "Enter 4 values (0-20) for the " << carList[i] << ":" << endl;
            vector<int> attributes(4);
            for (int j = 0; j < 4; j++) {
                int value;
                cout << "Value " << j + 1 << ": ";
                cin >> value;

                // Validate input range
                while (value < 0 || value > 20) {
                    cout << "Invalid input. Enter a number between 0 and 20: ";
                    cin >> value;
                }
                attributes[j] = value;
            }
            carAttributes.push_back(attributes);
        }
    }

    return carAttributes;
}

// Table for feature level values by category and level (30K to 230K)
const unordered_map<int, vector<int>> featureLevels = {
    {30, {0, 800, 1500, 2200, 2900, 3700, 4500, 5400, 6400, 7400, 8600, 10000, 11000, 13000, 15000, 17000, 20000, 23000, 26000, 30000}},
    {41, {0, 1200, 1900, 2700, 3600, 4500, 5500, 6600, 7800, 9200, 11000, 12000, 14000, 17000, 19000, 22000, 26000, 30000, 35000, 41000}},
    {52, {0, 1600, 2400, 3300, 4300, 5300, 6500, 7800, 9200, 11000, 13000, 15000, 17000, 20000, 24000, 28000, 32000, 38000, 44000, 52000}},
    {62, {0, 2000, 2900, 3900, 5000, 6100, 7500, 9000, 11000, 13000, 15000, 17000, 20000, 24000, 28000, 33000, 38000, 45000, 53000, 62000}},
    {120, {0, 2000, 3000, 4100, 5400, 6900, 8600, 11000, 13000, 16000, 19000, 24000, 29000, 35000, 43000, 52000, 64000, 79000, 97000, 120000}},
    {160, {0, 2000, 3000, 4200, 5600, 7200, 9100, 11000, 14000, 18000, 22000, 27000, 33000, 41000, 51000, 64000, 80000, 100000, 120000, 160000}},
    {230, {0, 2000, 3100, 4400, 5900, 7700, 9900, 13000, 16000, 20000, 26000, 33000, 41000, 53000, 67000, 86000, 110000, 140000, 180000, 230000}}
};

// Car-to-feature level mapping
const unordered_map<string, int> carFeatureMap = {
    {"Hill Climber", 30}, {"Hill Climber Mk. 2", 41}, {"Sports Car", 120}, {"Dune Buggy", 52},
    {"Super Diesel", 52}, {"Superbike", 230}, {"Supercar", 230}, {"Rally Car", 160},
    {"Lowrider", 120}, {"Beast", 160}, {"Hot Rod", 230}, {"Racing Truck", 230},
    {"Formula", 230}, {"Muscle Car", 230}, {"Monster Truck", 62}, {"Rotator", 120},
    {"Chopper", 52}, {"Tank", 62}, {"CC-EV", 230}, {"Bus", 41},
    {"Rock Bouncer", 160}, {"Hoverbike", 230}, {"Raider", 230}, {"Moonlander", 62},
    {"Monowheel", 52}, {"Snowmobile", 62}, {"Motocross", 52}, {"Tractor", 41}, {"Scooter", 30}, {"Glider", 52}
};

// Function to calculate total feature level value
int calculateFeatureValue(const vector<char>& hasCar, const vector<string>& carList, const vector<vector<int>>& carAttributes) {
    int totalValue = 0;
    int index = 0;

    for (int i = 0; i < hasCar.size(); i++) {
        if (hasCar[i] == 'y') {
            int featureCategory = carFeatureMap.at(carList[i]);
            const vector<int>& levelValues = featureLevels.at(featureCategory);
            int carTotal = 0;

            for (int level : carAttributes[index]) {
                int cumulativeCost = 0;
                for (int j = 0; j < level; j++) {
                    cumulativeCost += levelValues[j];  // Sum up to the current level
                }
                carTotal += cumulativeCost;  // Add this part's cumulative cost
            }

            totalValue += carTotal;  // Add the car's total to the final value
            index++;
        }
    }
    return totalValue;
}

void findCheapestUpgrades(int budget, const vector<char>& hasCar, const vector<string>& carList, const vector<vector<int>>& carAttributes) {
    vector<Upgrade> affordableUpgrades;
    int index = 0; // Track the index for the carAttributes vector

    for (int i = 0; i < carList.size(); ++i) {
        if (hasCar[i] != 'y') continue; // Skip if the car is not unlocked

        // Iterate over each attribute for the current car
        for (int part = 0; part < carAttributes[index].size(); ++part) {
            int currentLevel = carAttributes[index][part];

            // Ensure valid indexing for featureLevels lookup
            int featureCategory = carFeatureMap.at(carList[i]);
            const vector<int>& levelValues = featureLevels.at(featureCategory);

            // Check future levels to see if they are cheaper upgrades
            for (int futureLevel = currentLevel + 1; futureLevel <= min(20, currentLevel + 3); ++futureLevel) {
                int upgradeCost = levelValues[futureLevel - 1]; // Cost for upgrading to this level

                if (upgradeCost <= budget) {
                    affordableUpgrades.push_back({
                        carList[i],
                        futureLevel,
                        upgradeCost
                    });
                }
            }
        }
        ++index; // Move to the next car in carAttributes
    }

    // Sort by cost
    sort(affordableUpgrades.begin(), affordableUpgrades.end(), [](const Upgrade& a, const Upgrade& b) {
        return a.cost < b.cost;
    });

    // Display top 50 affordable upgrades
    cout << "\nTop 50 Cheapest Upgrades within Budget:\n";
    for (int i = 0; i < min(50, (int)affordableUpgrades.size()); ++i) {
        const auto& upgrade = affordableUpgrades[i];
        cout << i + 1 << ". " << upgrade.carName
             << " - Upgrade Part " << (i % 4 + 1)  // Show part being upgraded
             << " to Level " << upgrade.level
             << " for " << upgrade.cost << " coins\n";
    }
}

// Function to save data to a file
void saveDataToFile(const vector<char>& hasCar, const vector<vector<int>>& carAttributes) {
    ofstream outFile("carData.txt");
    if (!outFile) {
        cout << "Error opening file for saving data.\n";
        return;
    }
    
    // Save car ownership
    for (char c : hasCar) {
        outFile << c << ' ';
    }
    outFile << '\n';

    // Save car attributes
    for (const auto& attributes : carAttributes) {
        for (int attr : attributes) {
            outFile << attr << ' ';
        }
        outFile << '\n';
    }
    outFile.close();
    cout << "Data saved successfully.\n";
}

// Function to load data from a file
bool loadDataFromFile(vector<char>& hasCar, vector<vector<int>>& carAttributes) {
    ifstream inFile("carData.txt");
    if (!inFile) {
        cout << "No saved data found.\n";
        return false;
    }

    hasCar.clear();
    carAttributes.clear();

    string line;
    
    // Load car ownership
    getline(inFile, line);
    istringstream carStream(line);
    char response;
    while (carStream >> response) {
        hasCar.push_back(response);
    }

    // Load car attributes
    while (getline(inFile, line)) {
        istringstream attrStream(line);
        vector<int> attributes;
        int attr;
        while (attrStream >> attr) {
            attributes.push_back(attr);
        }
        carAttributes.push_back(attributes);
    }
    inFile.close();
    cout << "Data loaded successfully.\n";
    return true;
}

int main() {
    vector<char> hasCar;
    vector<vector<int>> carAttributes;
    vector<string> carList = {
        "Hill Climber", "Hill Climber Mk. 2", "Sports Car", "Dune Buggy", "Super Diesel", 
        "Superbike", "Supercar", "Rally Car", "Lowrider", "Beast", "Hot Rod", 
        "Racing Truck", "Formula", "Muscle Car", "Monster Truck", "Rotator", 
        "Chopper", "Tank", "CC-EV", "Bus", "Rock Bouncer", "Hoverbike", 
        "Raider", "Moonlander", "Monowheel", "Snowmobile", "Motocross", 
        "Tractor", "Scooter", "Glider"
    };

    char useSavedData;
    cout << "Do you have a saved data code? (y/n): ";
    cin >> useSavedData;

    if (useSavedData == 'y' && loadDataFromFile(hasCar, carAttributes)) {
        cout << "Loaded data will be used.\n";
    } else {
        // Get user car ownership information
        hasCar = carUnlocked();

        // Collect car attributes levels
        carAttributes = getCarAttributes(hasCar, carList);

        // Save the data for future use
        saveDataToFile(hasCar, carAttributes);
    }

    // Get the user's available budget
    int budget = getUserBudget();

    // Find and display the cheapest upgrades within budget
    findCheapestUpgrades(budget, hasCar, carList, carAttributes);

    return 0;
}

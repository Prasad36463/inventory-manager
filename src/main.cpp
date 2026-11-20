// Project B: Inventory Manager
// Concepts: inheritance + polymorphism, STL map/unique_ptr, exceptions, CSV file I/O
#include "common.h"
#include <fstream>
#include <sstream>
#include <map>
#include <memory>
#include <iomanip>
using namespace std;

class InventoryException : public runtime_error { public: using runtime_error::runtime_error; };

class Item {
protected:
    int id; string name; int qty; double price;
public:
    Item(int i, string n, int q, double p) : id(i), name(std::move(n)), qty(q), price(p) {
        if (q < 0) throw InventoryException("Quantity cannot be negative");
        if (p < 0) throw InventoryException("Price cannot be negative");
    }
    virtual ~Item() {}
    int getId() const { return id; }
    const string& getName() const { return name; }
    void stockIn(int n)  { if (n <= 0) throw InventoryException("Stock-in quantity must be positive"); qty += n; }
    void stockOut(int n) {
        if (n <= 0) throw InventoryException("Stock-out quantity must be positive");
        if (n > qty) throw InventoryException("Only " + to_string(qty) + " in stock, cannot remove " + to_string(n));
        qty -= n;
    }
    void setPrice(double p) { if (p < 0) throw InventoryException("Price cannot be negative"); price = p; }
    virtual string type() const { return "Normal"; }
    virtual string extra() const { return ""; }
    string toCSV() const {
        ostringstream o; o << type() << "," << id << "," << name << "," << qty << "," << price << "," << extra(); return o.str();
    }
    virtual void show() const {
        cout << "  " << setw(3) << id << " | " << left << setw(14) << name << right << " | qty " << setw(4) << qty
             << " | Rs " << fixed << setprecision(2) << setw(8) << price << " | " << type();
        if (!extra().empty()) cout << " (exp " << extra() << ")";
        cout << "\n";
    }
};

class PerishableItem : public Item {
    string expiry;
public:
    PerishableItem(int i, string n, int q, double p, string e) : Item(i, n, q, p), expiry(std::move(e)) {}
    string type() const override { return "Perishable"; }
    string extra() const override { return expiry; }
};

class Inventory {
    map<int, unique_ptr<Item>> items;
    string file;
public:
    explicit Inventory(string f) : file(std::move(f)) {}
    void load() {
        ifstream in(file);
        if (!in) { cout << "No data file found, starting with empty inventory.\n"; return; }
        string line; int n = 0;
        while (getline(in, line)) {
            stringstream ss(line); string t, id, nm, q, p, ex;
            if (!getline(ss, t, ',') || !getline(ss, id, ',') || !getline(ss, nm, ',') || !getline(ss, q, ',') || !getline(ss, p, ',')) continue;
            getline(ss, ex);
            try {
                if (t == "Perishable") items[stoi(id)] = make_unique<PerishableItem>(stoi(id), nm, stoi(q), stod(p), ex);
                else items[stoi(id)] = make_unique<Item>(stoi(id), nm, stoi(q), stod(p));
                n++;
            } catch (...) {}
        }
        cout << "Loaded " << n << " item(s) from " << file << "\n";
    }
    void save() const {
        ofstream out(file);
        if (!out) throw InventoryException("Cannot write to " + file);
        for (const auto& p : items) out << p.second->toCSV() << "\n";
        cout << "Saved " << items.size() << " item(s) to " << file << "\n";
    }
    void add(unique_ptr<Item> it) {
        int id = it->getId();
        if (items.count(id)) throw InventoryException("Item ID " + to_string(id) + " already exists");
        items[id] = std::move(it);
    }
    Item& find(int id) {
        auto it = items.find(id);
        if (it == items.end()) throw InventoryException("Item ID " + to_string(id) + " not found");
        return *it->second;
    }
    void list() const { if (items.empty()) cout << "  (empty)\n"; for (const auto& p : items) p.second->show(); }
    void search(const string& key) const {
        bool f = false;
        for (const auto& p : items) if (p.second->getName().find(key) != string::npos) { p.second->show(); f = true; }
        if (!f) cout << "  No item matches '" << key << "'\n";
    }
};

int main() {
    Inventory inv("inventory_data.csv");
    inv.load();
    while (true) {
        cout << "\n== INVENTORY MENU: 1 Add | 2 List | 3 Search | 4 Stock in | 5 Stock out | 6 Price | 0 Save & exit ==\n";
        try {
            int ch = readInt("Choice: ");
            if (ch == 0) { inv.save(); cout << "Goodbye!\n"; break; }
            switch (ch) {
                case 1: { int id = readInt("Item ID: "); string n = readLine("Name: "); int q = readInt("Quantity: ");
                          double p = readDouble("Price: "); string perish = readLine("Perishable? (y/n): ");
                          if (id <= 0 || n.empty()) throw InventoryException("ID must be positive and name cannot be empty");
                          if (perish == "y") inv.add(make_unique<PerishableItem>(id, n, q, p, readLine("Expiry (YYYY-MM-DD): ")));
                          else inv.add(make_unique<Item>(id, n, q, p));
                          cout << "Item added.\n"; break; }
                case 2: inv.list(); break;
                case 3: inv.search(readLine("Name contains: ")); break;
                case 4: { int id = readInt("Item ID: "); inv.find(id).stockIn(readInt("Quantity to add: ")); cout << "Stock updated.\n"; break; }
                case 5: { int id = readInt("Item ID: "); inv.find(id).stockOut(readInt("Quantity to remove: ")); cout << "Stock updated.\n"; break; }
                case 6: { int id = readInt("Item ID: "); inv.find(id).setPrice(readDouble("New price: ")); cout << "Price updated.\n"; break; }
                default: cout << "Invalid menu choice.\n";
            }
        } catch (const InventoryException& e) { cout << "Inventory error: " << e.what() << "\n"; }
          catch (const invalid_argument& e)   { cout << "Input error: " << e.what() << "\n"; }
          catch (const runtime_error& e)      { cout << "Stopped: " << e.what() << "\n"; inv.save(); break; }
    }
    return 0;
}

#include "crow_all.h"
#include <fstream>
#include <string>
#include <vector>

using namespace std;

struct Contact {
    string name;
    string phone;
    string email;
};

vector<Contact> loadContacts(const string& filename) {
    vector<Contact> contacts;
    ifstream inFile(filename);
    if (inFile.is_open()) {
        string line;
        while (getline(inFile, line)) {
            size_t pos1 = line.find(',');
            size_t pos2 = line.find(',', pos1 + 1);
            Contact contact;
            contact.name = line.substr(0, pos1);
            contact.phone = line.substr(pos1 + 1, pos2 - pos1 - 1);
            contact.email = line.substr(pos2 + 1);
            contacts.push_back(contact);
        }
        inFile.close();
    }
    return contacts;
}

void saveContact(const string& filename, const Contact& contact) {
    ofstream outFile(filename, ios::app);
    if (outFile.is_open()) {
        outFile << contact.name << "," << contact.phone << "," << contact.email << endl;
        outFile.close();
    }
}

int main() {
    crow::SimpleApp app;
    const string filename = "contacts.txt";

    CROW_ROUTE(app, "/")([](){
        return "Welcome to the Contact Manager!";
    });

    CROW_ROUTE(app, "/contacts")
    ([&filename](){
        auto contacts = loadContacts(filename);
        crow::json::wvalue x;
        for (const auto& contact : contacts) {
            x["contacts"].push_back(crow::json::wvalue{{"name", contact.name}, {"phone", contact.phone}, {"email", contact.email}});
        }
        return x;
    });

    CROW_ROUTE(app, "/add_contact").methods("POST"_method)
    ([&filename](const crow::request& req){
        auto x = crow::json::load(req.body);
        if (!x) {
            return crow::response(400);
        }
        Contact contact;
        contact.name = x["name"].s();
        contact.phone = x["phone"].s();
        contact.email = x["email"].s();
        saveContact(filename, contact);
        return crow::response(200);
    });

    app.port(8080).multithreaded().run();
}

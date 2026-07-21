#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>

using namespace std;

struct Student { string id, name, batch; };
struct Record { int qNum, marks; string sid, status, batch; };

class VUTracker {
    vector<Student> students;
    vector<Record> records;
    const string path = "C:/Users/user/Desktop/";

    void load() {
        ifstream sf(path + "students.txt"), rf(path + "quiz_records.txt");
        string id, name, batch, q, sid, m, st, rb;
        while (getline(sf, id, ',') && getline(sf, name, ',') && getline(sf, batch)) students.push_back({id, name, batch});
        while (getline(rf, q, ',') && getline(rf, sid, ',') && getline(rf, m, ',') && getline(rf, st, ',') && getline(rf, rb)) 
            records.push_back({stoi(q), stoi(m), sid, st, rb});
    }

    void save() {
        ofstream sf(path + "students.txt"), rf(path + "quiz_records.txt");
        for (auto &s : students) sf << s.id << "," << s.name << "," << s.batch << "\n";
        for (auto &r : records) rf << r.qNum << "," << r.sid << "," << r.marks << "," << r.status << "," << r.batch << "\n";
    }

    Record* findR(int q, string id, string b) {
        for (auto &r : records) if (r.qNum == q && r.sid == id && r.batch == b) return &r;
        return nullptr;
    }

public:
    VUTracker() { load(); }

    void registerBatch() {
        string bYear, id, nm; int n;
        cout << "\nEnter Batch Year & Total Students: "; cin >> bYear >> n;
        for (int i = 0; i < n; i++) {
            cout << "\nEnter ID: "; cin >> id;
            if (find_if(students.begin(), students.end(), [&](Student &s){ return s.id == id && s.batch == bYear; }) != students.end()) {
                cout << "[Alert] Already Exists!\n"; i--; continue;
            }
            cin.ignore();
            cout << "Enter Name: "; getline(cin, nm);
            students.push_back({id, nm, bYear});
        }
        save();
    }

    void addSingleStudent() {
        string bYear, id, nm;
        cout << "\nEnter Batch Year for New Student: "; cin >> bYear;
        cout << "Enter Student ID: "; cin >> id;
        
        if (find_if(students.begin(), students.end(), [&](Student &s){ return s.id == id && s.batch == bYear; }) != students.end()) {
            cout << "[Alert] Student with this ID already exists in this batch!\n";
            return;
        }
        cin.ignore();
        cout << "Enter Student Name: "; getline(cin, nm);
        
        students.push_back({id, nm, bYear});
        save();
        cout << ">>> New Student Added Successfully! <<<\n";
    }

    void manageMarks(bool batchMode) {
        string bYear, id; int q, m;
        cout << "\nEnter Batch & Quiz Num: "; cin >> bYear >> q;
        if (!batchMode) { cout << "Student ID: "; cin >> id; }
        
        bool found = false;
        for (auto &s : students) {
            if (s.batch != bYear || (!batchMode && s.id != id)) continue;
            found = true;
            cout << "Marks for " << s.name << " (0=Missed): "; cin >> m;
            if (Record *r = findR(q, s.id, bYear)) { r->marks = m; r->status = m > 0 ? "Attempted" : "Missed"; }
            else records.push_back({q, m, s.id, m > 0 ? "Attempted" : "Missed", bYear});
            if (!batchMode) break;
        }
        if (!found && !batchMode) cout << "[Error] Student not found!\n";
        save();
    }

    void report() {
        string bYear; int q;
        cout << "\nEnter Batch & Quiz Num: "; cin >> bYear >> q;
        cout << "\nID\t\tName\t\tMarks\t\tStatus\n-----------------------------------------------------\n";
        for (auto &r : records) {
            if (r.qNum == q && r.batch == bYear) {
                string nm = "Unknown";
                for (auto &s : students) if (s.id == r.sid && s.batch == bYear) { nm = s.name; break; }
                cout << r.sid << "\t\t" << nm << "\t\t" << r.marks << "\t\t" << r.status << "\n";
            }
        }
    }

    void searchStudent() {
        string bYear, query; cout << "\nEnter Batch & Search (ID/Name): "; cin >> bYear; cin.ignore(); getline(cin, query);
        for (auto &s : students) {
            if (s.batch == bYear && (s.id == query || s.name.find(query) != string::npos)) {
                cout << "\nID: " << s.id << " | Name: " << s.name << "\nQuiz\t\tMarks\t\tStatus\n-----------------------------------\n";
                for (auto &r : records) if (r.sid == s.id && r.batch == bYear) cout << r.qNum << "\t\t" << r.marks << "\t\t" << r.status << "\n";
            }
        }
    }

    void clearRecords() {
        int opt; cout << "\n1. Clear Batch\n2. Reset All\nChoice: "; cin >> opt;
        if (opt == 1) {
            string b; cout << "Enter Batch: "; cin >> b;
            students.erase(remove_if(students.begin(), students.end(), [&](Student &s){ return s.batch == b; }), students.end());
            records.erase(remove_if(records.begin(), records.end(), [&](Record &r){ return r.batch == b; }), records.end());
            save();
        } else if (opt == 2) {
            students.clear(); records.clear(); save();
        }
    }
};

int main() {
    VUTracker vt; int c;
    while (true) {
        cout << "\n1. Register batch\n2. Batch Marks\n3. Individual Marks\n4. Quiz report\n5. Search individually\n6. Reset & delete\n7. Add Single Student (Late Entry)\n8. Exit\n\nChoice: ";
        if (!(cin >> c) || c == 8) break;
        if (c == 1) vt.registerBatch();
        else if (c == 2) vt.manageMarks(true);
        else if (c == 3) vt.manageMarks(false);
        else if (c == 4) vt.report();
        else if (c == 5) vt.searchStudent();
        else if (c == 6) vt.clearRecords();
        else if (c == 7) vt.addSingleStudent();
    }
}

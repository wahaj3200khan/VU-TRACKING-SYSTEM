#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <cctype>

using namespace std;

struct Student { string id, name, batch; };
struct Record { int qNum, marks; string sid, status, batch; };

class VUTracker {
    vector<Student> students;
    vector<Record> records;
    
    const string studentFile = "C:/Users/user/Desktop/students.txt";
    const string recordFile = "C:/Users/user/Desktop/quiz_records.txt";

    void load() {
        ifstream sf(studentFile);
        if (sf.is_open()) {
            string id, name, batch;
            while (getline(sf, id, ',') && getline(sf, name, ',') && getline(sf, batch)) {
                students.push_back({id, name, batch});
            }
            sf.close();
        }

        ifstream rf(recordFile);
        if (rf.is_open()) {
            string q, sid, m, st, rBatch;
            while (getline(rf, q, ',') && getline(rf, sid, ',') && getline(rf, m, ',') && getline(rf, st, ',') && getline(rf, rBatch)) {
                records.push_back({stoi(q), stoi(m), sid, st, rBatch});
            }
            rf.close();
        }
    }

    void save() {
        ofstream sf(studentFile);
        for (auto &s : students) sf << s.id << "," << s.name << "," << s.batch << "\n";
        sf.close();

        ofstream rf(recordFile);
        for (auto &r : records) rf << r.qNum << "," << r.sid << "," << r.marks << "," << r.status << "," << r.batch << "\n";
        rf.close();
    }

    Record* findR(int q, string id, string batch) {
        for (auto &r : records) if (r.qNum == q && r.sid == id && r.batch == batch) return &r;
        return nullptr;
    }

    bool isValidName(const string &str) {
        if (str.empty()) return false;
        for (char c : str) {
            if (!isalpha(c) && !isspace(c)) return false;
        }
        return true;
    }

public:
    VUTracker() { load(); }

    void registerBatch() {
        string bYear; 
        cout << "\n===================================\n";
        cout << "Enter Batch Year (e.g., 2026): "; 
        cin >> bYear;
        
        int n; 
        cout << "Enter Number of Students to Register: "; 
        cin >> n;
        
        for (int i = 0; i < n; i++) {
            string id, nm;
            cout << "\n--- Student " << i + 1 << " ---\n";
            cout << "Enter ID: "; 
            cin >> id;
            
            auto it = find_if(students.begin(), students.end(), [&](Student &s){ return s.id == id && s.batch == bYear; });
            if (it != students.end()) {
                cout << "\n[Alert] ID: " << it->id << " | Name: " << it->name << " - Already Exists in Batch " << bYear << "!\n";
                i--;
                continue;
            }
            
            cin.ignore();
            while (true) {
                cout << "Enter Name (Alphabets only): ";
                getline(cin, nm);
                if (isValidName(nm)) break;
                cout << "Invalid Name! Numbers or special characters are not allowed.\n";
            }
            
            students.push_back({id, nm, bYear});
        }
        save();
        cout << "\n>>> Batch Registered Successfully! <<<\n";
    }

    void manageMarks(bool batchMode) {
        string bYear; 
        cout << "\n===================================\n";
        cout << "Enter Batch Year: "; cin >> bYear;
        int q, m; string id;
        cout << "Quiz Num: "; cin >> q;
        if (!batchMode) { cout << "Student ID: "; cin >> id; }
        
        bool studentFound = false;
        cout << "\n--- Entering Marks ---\n";
        for (auto &s : students) {
            if (s.batch != bYear) continue;
            if (!batchMode && s.id != id) continue;
            
            studentFound = true;
            cout << "Marks for " << s.name << " (0=Missed): "; cin >> m;
            Record *r = findR(q, s.id, bYear);
            if (r) { r->marks = m; r->status = m > 0 ? "Attempted" : "Missed"; }
            else records.push_back({q, m, s.id, m > 0 ? "Attempted" : "Missed", bYear});
            if (!batchMode) break;
        }
        if (!studentFound && !batchMode) cout << "\n[Error] No student found with ID " << id << " in Batch " << bYear << "\n";
        save();
        cout << "\n>>> Marks Updated Successfully! <<<\n";
    }

    void report() {
        string bYear; 
        cout << "\n===================================\n";
        cout << "Enter Batch Year: "; cin >> bYear;
        int q; cout << "Quiz Num: "; cin >> q;
        
        cout << "\n-----------------------------------------------------\n";
        cout << "ID\t\tName\t\tMarks\t\tStatus\n";
        cout << "-----------------------------------------------------\n";
        for (auto &r : records) {
            if (r.qNum == q && r.batch == bYear) {
                string sName = "Unknown";
                for (auto &s : students) {
                    if (s.id == r.sid && s.batch == bYear) {
                        sName = s.name;
                        break;
                    }
                }
                cout << r.sid << "\t\t" << sName << "\t\t" << r.marks << "\t\t" << r.status << "\n";
            }
        }
        cout << "-----------------------------------------------------\n";
    }

    void searchStudent() {
        string bYear; 
        cout << "\n===================================\n";
        cout << "Enter Batch Year: "; cin >> bYear;
        cin.ignore();
        string query; cout << "Enter Student ID or Name to search: "; getline(cin, query);
        bool found = false;
        
        for (auto &s : students) {
            if (s.batch == bYear && (s.id == query || s.name.find(query) != string::npos)) {
                found = true;
                cout << "\n===================================\n";
                cout << "Student Found:\n";
                cout << "ID: " << s.id << " | Name: " << s.name << " | Batch: " << s.batch << "\n";
                cout << "-----------------------------------\n";
                cout << "Quiz\t\tMarks\t\tStatus\n";
                cout << "-----------------------------------\n";
                for (auto &r : records) {
                    if (r.sid == s.id && r.batch == bYear) {
                        cout << r.qNum << "\t\t" << r.marks << "\t\t" << r.status << "\n";
                    }
                }
                cout << "-----------------------------------\n";
            }
        }
        if (!found) cout << "\n[Result] No student found matching inside Batch " << bYear << "\n";
    }

    void clearRecords() {
        cout << "\n===================================\n";
        cout << "1. Clear Specific Batch\n";
        cout << "2. All Clear (Reset Whole System)\n";
        cout << "Choice: ";
        int option; cin >> option;

        if (option == 1) {
            string bYear; cout << "Enter Batch Year to clear: "; cin >> bYear;
            
            students.erase(remove_if(students.begin(), students.end(), [&](Student &s){ return s.batch == bYear; }), students.end());
            records.erase(remove_if(records.begin(), records.end(), [&](Record &r){ return r.batch == bYear; }), records.end());
            
            save();
            cout << "\n>>> Data for Batch " << bYear << " cleared successfully! <<<\n";
        } 
        else if (option == 2) {
            string confirm;
            cout << "Are you sure you want to delete ALL data? (yes/no): "; cin >> confirm;
            if (confirm == "yes" || confirm == "YES") {
                students.clear();
                records.clear();
                save();
                cout << "\n>>> Entire system database reset successfully! <<<\n";
            } else {
                cout << "\n>>> Operation Cancelled. <<<\n";
            }
        }
    }
};

int main() {
    VUTracker vt;
    int c;
    while (true) {
        cout << "\n===================================\n"
             << "          VUTRACKER MENU           \n"
             << "===================================\n"
             << "1. Register Batch\n"
             << "2. Add Quiz Marks (Batch Mode)\n"
             << "3. Update Marks Individually\n"
             << "4. Quiz Report\n"
             << "5. Search Student (by ID/Name)\n"
             << "6. Clear / Reset Records (Semester End)\n"
             << "7. Exit\n"
             << "-----------------------------------\n"
             << "Choice: ";
        if (!(cin >> c) || c == 7) break;
        if (c == 1) vt.registerBatch();
        else if (c == 2) vt.manageMarks(true);
        else if (c == 3) vt.manageMarks(false);
        else if (c == 4) vt.report();
        else if (c == 5) vt.searchStudent();
        else if (c == 6) vt.clearRecords();
    }
}
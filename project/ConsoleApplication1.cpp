#include <algorithm>
#include <cctype>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>
#include <filesystem>

using namespace std;

struct Task {
    int id{};
    string title;
    string category;
    string dueDate;   // YYYY-MM-DD
    int priority{};   // 1 = low, 5 = high
    bool completed{};
    string notes;
};

string trim(const string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == string::npos) return "";
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

string toLower(string s) {
    transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return static_cast<char>(tolower(c)); });
    return s;
}

bool isValidDateFormat(const string& date) {
    if (date.size() != 10) return false;
    for (int i : {0, 1, 2, 3, 5, 6, 8, 9}) {
        if (!isdigit(static_cast<unsigned char>(date[i]))) return false;
    }
    return date[4] == '-' && date[7] == '-';
}

bool parseDate(const string& date, tm& out) {
    if (!isValidDateFormat(date)) return false;
    int y = stoi(date.substr(0, 4));
    int m = stoi(date.substr(5, 2));
    int d = stoi(date.substr(8, 2));
    if (y < 1900 || m < 1 || m > 12 || d < 1 || d > 31) return false;

    tm t{};
    t.tm_year = y - 1900;
    t.tm_mon = m - 1;
    t.tm_mday = d;
    t.tm_hour = 12;
    t.tm_isdst = -1;
    time_t tt = mktime(&t);
    if (tt == -1) return false;

    tm normalized{};
    localtime_s(&normalized, &tt);
    if (normalized.tm_year != t.tm_year || normalized.tm_mon != t.tm_mon || normalized.tm_mday != t.tm_mday) {
        return false;
    }

    out = t;
    return true;
}

string currentDate() {
    time_t now = time(nullptr);
    tm local{};
    localtime_s(&local, &now);
    ostringstream oss;
    oss << setfill('0') << setw(4) << (local.tm_year + 1900) << '-'
        << setw(2) << (local.tm_mon + 1) << '-'
        << setw(2) << local.tm_mday;
    return oss.str();
}

int daysBetween(const string& fromDate, const string& toDate) {
    tm from{}, to{};
    if (!parseDate(fromDate, from) || !parseDate(toDate, to)) return 0;
    time_t a = mktime(&from);
    time_t b = mktime(&to);
    double diff = difftime(b, a) / 86400.0;
    return static_cast<int>(diff >= 0 ? diff + 0.5 : diff - 0.5);
}

int promptInt(const string& message, int minValue, int maxValue) {
    while (true) {
        cout << message;
        string line;
        getline(cin, line);
        stringstream ss(line);
        int value;
        char extra;
        if (ss >> value && !(ss >> extra) && value >= minValue && value <= maxValue) {
            return value;
        }
        cout << "Invalid input. Enter a number from " << minValue << " to " << maxValue << ".\n";
    }
}

string promptLine(const string& message, bool allowEmpty = false) {
    while (true) {
        cout << message;
        string line;
        getline(cin, line);
        line = trim(line);
        if (allowEmpty || !line.empty()) return line;
        cout << "Input cannot be empty. Try again.\n";
    }
}

bool promptYesNo(const string& message) {
    while (true) {
        cout << message;
        string line;
        getline(cin, line);
        line = toLower(trim(line));
        if (line == "y" || line == "yes") return true;
        if (line == "n" || line == "no") return false;
        cout << "Type y or n.\n";
    }
}

class TaskManager {
private:
    vector<Task> tasks;
    string filePath = "data/tasks.txt";

    static string boolToText(bool value) {
        return value ? "Done" : "Open";
    }

    static bool textToBool(const string& text) {
        string v = toLower(trim(text));
        return (v == "1" || v == "true" || v == "done" || v == "yes");
    }

public:
    TaskManager() {
        filesystem::create_directories("data");
    }

    void load() {
        tasks.clear();
        ifstream in(filePath);
        if (!in.is_open()) return;

        string line;
        while (getline(in, line)) {
            if (line.empty()) continue;
            stringstream ss(line);
            string part;
            Task t;

            if (!getline(ss, part, '|')) continue;
            t.id = stoi(part);
            if (!getline(ss, t.title, '|')) continue;
            if (!getline(ss, t.category, '|')) continue;
            if (!getline(ss, t.dueDate, '|')) continue;
            if (!getline(ss, part, '|')) continue;
            t.priority = stoi(part);
            if (!getline(ss, part, '|')) continue;
            t.completed = textToBool(part);
            if (!getline(ss, t.notes)) t.notes = "";

            tasks.push_back(t);
        }
    }

    void save() const {
        ofstream out(filePath, ios::trunc);
        for (const auto& t : tasks) {
            out << t.id << '|' << t.title << '|' << t.category << '|' << t.dueDate << '|'
                << t.priority << '|' << (t.completed ? 1 : 0) << '|' << t.notes << '\n';
        }
    }

    int nextId() const {
        int highest = 0;
        for (const auto& t : tasks) highest = max(highest, t.id);
        return highest + 1;
    }

    void addTask() {
        cout << "\n--- Add New Task ---\n";
        Task t;
        t.id = nextId();
        t.title = promptLine("Task title: ");
        t.category = promptLine("Category (school, work, personal, etc.): ");

        while (true) {
            t.dueDate = promptLine("Due date (YYYY-MM-DD): ");
            tm dummy{};
            if (parseDate(t.dueDate, dummy)) break;
            cout << "Please enter a real date in YYYY-MM-DD format.\n";
        }

        t.priority = promptInt("Priority (1 low - 5 high): ", 1, 5);
        t.completed = promptYesNo("Is it completed now? (y/n): ");
        t.notes = promptLine("Notes (optional, press Enter for none): ", true);

        tasks.push_back(t);
        save();
        cout << "Task saved with ID #" << t.id << ".\n";
    }

    void printHeader() const {
        cout << left
            << setw(5) << "ID"
            << setw(24) << "Title"
            << setw(16) << "Category"
            << setw(14) << "Due Date"
            << setw(10) << "Priority"
            << setw(12) << "Status"
            << "Notes" << '\n';
        cout << string(85, '-') << '\n';
    }

    void printTask(const Task& t) const {
        cout << left
            << setw(5) << t.id
            << setw(24) << t.title.substr(0, 23)
            << setw(16) << t.category.substr(0, 15)
            << setw(14) << t.dueDate
            << setw(10) << t.priority
            << setw(12) << boolToText(t.completed)
            << t.notes.substr(0, 40) << '\n';
    }

    void viewTasks() const {
        cout << "\n--- Saved Tasks ---\n";
        if (tasks.empty()) {
            cout << "No tasks saved yet.\n";
            return;
        }
        printHeader();
        for (const auto& t : tasks) printTask(t);
    }

    int findIndexById(int id) const {
        for (size_t i = 0; i < tasks.size(); ++i) {
            if (tasks[i].id == id) return static_cast<int>(i);
        }
        return -1;
    }

    void updateTask() {
        cout << "\n--- Update Task ---\n";
        if (tasks.empty()) {
            cout << "No tasks available.\n";
            return;
        }

        int id = promptInt("Enter task ID to update: ", 1, numeric_limits<int>::max());
        int index = findIndexById(id);
        if (index == -1) {
            cout << "Task not found.\n";
            return;
        }

        Task& t = tasks[index];
        cout << "Current title: " << t.title << '\n';
        string input = promptLine("New title (press Enter to keep): ", true);
        if (!input.empty()) t.title = input;

        cout << "Current category: " << t.category << '\n';
        input = promptLine("New category (press Enter to keep): ", true);
        if (!input.empty()) t.category = input;

        cout << "Current due date: " << t.dueDate << '\n';
        input = promptLine("New due date (press Enter to keep): ", true);
        if (!input.empty()) {
            tm dummy{};
            while (!parseDate(input, dummy)) {
                cout << "Invalid date. Try again.\n";
                input = promptLine("New due date (YYYY-MM-DD): ", true);
            }
            t.dueDate = input;
        }

        cout << "Current priority: " << t.priority << '\n';
        input = promptLine("New priority 1-5 (press Enter to keep): ", true);
        if (!input.empty()) {
            stringstream ss(input);
            int p;
            if (ss >> p && p >= 1 && p <= 5) t.priority = p;
            else cout << "Priority kept unchanged.\n";
        }

        if (promptYesNo("Change completion status? (y/n): ")) {
            t.completed = promptYesNo("Mark as completed? (y/n): ");
        }

        cout << "Current notes: " << t.notes << '\n';
        input = promptLine("New notes (press Enter to keep): ", true);
        if (!input.empty()) t.notes = input;

        save();
        cout << "Task updated.\n";
    }

    void deleteTask() {
        cout << "\n--- Delete Task ---\n";
        if (tasks.empty()) {
            cout << "No tasks to delete.\n";
            return;
        }
        int id = promptInt("Enter task ID to delete: ", 1, numeric_limits<int>::max());
        int index = findIndexById(id);
        if (index == -1) {
            cout << "Task not found.\n";
            return;
        }

        printHeader();
        printTask(tasks[index]);
        if (promptYesNo("Delete this task? (y/n): ")) {
            tasks.erase(tasks.begin() + index);
            save();
            cout << "Task deleted.\n";
        }
        else {
            cout << "Delete canceled.\n";
        }
    }

    void searchTasks() const {
        cout << "\n--- Search Tasks ---\n";
        if (tasks.empty()) {
            cout << "No tasks available.\n";
            return;
        }

        string keyword = toLower(promptLine("Search by title, category, or notes: "));
        vector<Task> matches;
        for (const auto& t : tasks) {
            string haystack = toLower(t.title + " " + t.category + " " + t.notes);
            if (haystack.find(keyword) != string::npos) matches.push_back(t);
        }

        if (matches.empty()) {
            cout << "No matching tasks found.\n";
            return;
        }

        printHeader();
        for (const auto& t : matches) printTask(t);
    }

    void markCompleteToggle() {
        cout << "\n--- Mark Task Complete / Incomplete ---\n";
        if (tasks.empty()) {
            cout << "No tasks available.\n";
            return;
        }
        int id = promptInt("Enter task ID: ", 1, numeric_limits<int>::max());
        int index = findIndexById(id);
        if (index == -1) {
            cout << "Task not found.\n";
            return;
        }
        tasks[index].completed = !tasks[index].completed;
        save();
        cout << "Task #" << id << " is now " << (tasks[index].completed ? "completed" : "open") << ".\n";
    }

    void sortTasksMenu() {
        cout << "\n--- Sort Tasks ---\n";
        if (tasks.empty()) {
            cout << "No tasks to sort.\n";
            return;
        }
        cout << "1. Due date (soonest first)\n";
        cout << "2. Priority (highest first)\n";
        cout << "3. Status (open first)\n";
        cout << "4. Title (A-Z)\n";
        int choice = promptInt("Choose: ", 1, 4);

        switch (choice) {
        case 1:
            sort(tasks.begin(), tasks.end(), [](const Task& a, const Task& b) { return a.dueDate < b.dueDate; });
            break;
        case 2:
            sort(tasks.begin(), tasks.end(), [](const Task& a, const Task& b) {
                if (a.priority != b.priority) return a.priority > b.priority;
                return a.dueDate < b.dueDate;
                });
            break;
        case 3:
            sort(tasks.begin(), tasks.end(), [](const Task& a, const Task& b) {
                if (a.completed != b.completed) return !a.completed; // open first
                return a.dueDate < b.dueDate;
                });
            break;
        case 4:
            sort(tasks.begin(), tasks.end(), [](const Task& a, const Task& b) { return toLower(a.title) < toLower(b.title); });
            break;
        }
        save();
        cout << "Tasks sorted and saved.\n";
        viewTasks();
    }

    void smartCoach() const {
        cout << "\n--- Focus Coach ---\n";
        if (tasks.empty()) {
            cout << "No tasks saved yet. Add a few tasks first.\n";
            return;
        }

        string today = currentDate();
        vector<Task> openTasks;
        for (const auto& t : tasks) {
            if (!t.completed) openTasks.push_back(t);
        }

        if (openTasks.empty()) {
            cout << "All tasks are completed. Great job!\n";
            return;
        }

        sort(openTasks.begin(), openTasks.end(), [&](const Task& a, const Task& b) {
            int da = daysBetween(today, a.dueDate);
            int db = daysBetween(today, b.dueDate);
            if (da != db) return da < db;
            if (a.priority != b.priority) return a.priority > b.priority;
            return a.title < b.title;
            });

        int overdue = 0, dueSoon = 0;
        for (const auto& t : openTasks) {
            int daysLeft = daysBetween(today, t.dueDate);
            if (daysLeft < 0) overdue++;
            else if (daysLeft <= 3) dueSoon++;
        }

        cout << "Today: " << today << '\n';
        cout << "Open tasks: " << openTasks.size() << " | Overdue: " << overdue << " | Due soon (3 days): " << dueSoon << '\n';
        cout << "Top task to do first:\n";
        printHeader();
        printTask(openTasks.front());

        cout << "\nCoach advice:\n";
        if (overdue > 0) {
            cout << "- Finish overdue tasks first so they do not keep stacking up.\n";
        }
        if (dueSoon > 0) {
            cout << "- Use a 25-minute focus sprint on your next due-soon task.\n";
        }
        if (openTasks.front().priority >= 4) {
            cout << "- Your highest-priority item should be your main focus today.\n";
        }
        else {
            cout << "- You have room to balance school, work, and personal tasks.\n";
        }
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    TaskManager manager;
    manager.load();

    while (true) {
        cout << "\n====================================\n";
        cout << "   FocusForge - Student Task AI\n";
        cout << "====================================\n";
        cout << "1. Add task\n";
        cout << "2. View tasks\n";
        cout << "3. Update task\n";
        cout << "4. Delete task\n";
        cout << "5. Mark complete / incomplete\n";
        cout << "6. Search tasks\n";
        cout << "7. Sort tasks\n";
        cout << "8. Focus coach\n";
        cout << "9. Save and exit\n";

        int choice = promptInt("Choose an option: ", 1, 9);

        switch (choice) {
        case 1: manager.addTask(); break;
        case 2: manager.viewTasks(); break;
        case 3: manager.updateTask(); break;
        case 4: manager.deleteTask(); break;
        case 5: manager.markCompleteToggle(); break;
        case 6: manager.searchTasks(); break;
        case 7: manager.sortTasksMenu(); break;
        case 8: manager.smartCoach(); break;
        case 9:
            manager.save();
            cout << "Saved. Goodbye!\n";
            return 0;
        }
    }
}

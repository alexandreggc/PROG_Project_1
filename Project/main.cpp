#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>

using namespace std;

void invalidInput() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cout << "Please choose a valid option." << endl;
}

void rules() {
    int comeback = -1;
    cout << endl;
    cout << "Rules \n" << endl;
    cout << "Press 0 to return to the main menu. " << endl;

    while (true) {
        cin >> comeback;
        if (cin.peek() == '\n' && !cin.fail() && comeback == 0) {
            break;
        }
        invalidInput();
    }
}

void play() {
    int maze_number;
    string filename;
    cout << endl;
    cout << "Insert maze's number (1-99) or 0 to return to main menu: ";

    // choose maze number
    while (true) {
        cin >> maze_number;
        if (cin.peek() == '\n' && !cin.fail() && maze_number >= 0 && maze_number<100) {
            break;
        }
        invalidInput();
    }
    if (maze_number == 0) {
        return;
    }

    // create maze's file name
    stringstream s;
    filename = to_string(maze_number);
    s << setfill('0') << setw(2) << filename;

    filename = "maze_" + s.str() + ".txt";
    cout << filename << endl;
}

int main() {
    int opt;
    while (true) {
        // menu display
        cout << endl;
        cout << "1) Rules" << endl;
        cout << "2) Play" << endl;
        cout << "0) Exit" << endl;
        cout << endl;

        // player chooses the option
        while (true) {
            cin >> opt;
            if (cin.peek() == '\n' && !cin.fail() && (opt == 0 || opt == 1 || opt == 2)) {
                break;
            }
            invalidInput();
        }

        switch (opt) {
            case 1: // display rules
                rules();
                break;
            case 2: // start game
                play();
                break;
            case 0:
                exit(0);
            default:
                break;
        }
    }
}
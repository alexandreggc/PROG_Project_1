#include <iostream>

using namespace std;

int main() {
    int gamemode;
    //Menu display
    cout << endl;
    cout << "1) Rules" << endl;
    cout << "2) Play" << endl;
    cout << "0) Exit" << endl; 
    cout << endl;

    //Player chooses the gamemode
    while (true) {
        cin >> gamemode;
        if (cin.peek() == '\n' && !cin.fail() && (gamemode==0 || gamemode == 1 || gamemode == 2)) {
            break;
        }
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Please choose a valid gamemode." << endl;
    }

    //display rules
    if (gamemode == 1) {
        int comeback = 1;
        cout << endl;
        cout << "Rules \n" << endl; 
        cout << "Press 0 to return to the main menu. " << endl;

        while (true) {
            cin >> comeback;
            if (cin.peek() == '\n' && !cin.fail() && comeback == 0) {
                break;
            }
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Please insert a valid input." << endl;
        }

        if (comeback == 0) main(); //return to main menu
    }

    //start game
    else if (gamemode == 2) {
        cout << endl;
        cout << "game scene";
    }

    else if (gamemode == 0) return 0;
}
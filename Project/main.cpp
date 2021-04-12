#include <iostream>

using namespace std;

int main() {
    int gamemode;
    cout << "1) Rules" << endl;
    cout << "2) Play" << endl;
    cout << "0) Exit" << endl; //Menu display

    cin >> gamemode; //Player chooses the gamemode

    while ((gamemode != 1) && (gamemode != 2) && (gamemode != 0))
    {
        cout << "Please choose a valid gamemode." << endl;
        cin >> gamemode;
    }

    if (gamemode == 1) {
        int comeback = 1;
        cout << "Rules \n" << endl; //display rules
        cout << "Press 0 to return to the main menu. " << endl;

        while (comeback != 0)
            cin >> comeback;
        if (comeback == 0) main(); //return to main menu
    }

    else if (gamemode == 2)
        cout << "game scene"; //start game

    else if (gamemode == 0) return 0;
}
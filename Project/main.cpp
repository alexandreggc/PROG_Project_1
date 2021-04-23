#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>

using namespace std;

void invalidInput();
void rules();
void play();

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
                exit(0); // exit game
            default:
                break;
        }
    }
}

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

string choose_maze() {
    int maze_number;
    string filename;
    stringstream s;   
    cout << endl;
    cout << "Insert maze's number (1-99) or 0 to return to main menu: ";
    while (true) {
        cin >> maze_number;
        if (cin.peek() == '\n' && !cin.fail() && maze_number >= 0 && maze_number < 100) {
            break;
        }
        invalidInput();
    }
    // create file's name
    filename = to_string(maze_number);
    s << setfill('0') << setw(2) << filename;
    filename = "maze_" + s.str() + ".txt";
    return filename;
}

int load_mazefile(ifstream &f, string filename) {
    f.open(filename);
    if (!f.is_open()) {
        cerr << "Maze file not found!";
        return 0;
    }
    return 1;
}

void play() {
    string filename;
    ifstream f;
    while (true) {
        filename = choose_maze();
        if (load_mazefile(f, filename)) {
            char c;
            f.ignore(10000, '\n');
            while (f.get(c)) cout << c;
            f.close();
            break;
        }
    }
 
    // load maze in vectors
    // identificar cenas do maze
    // funcao relogio
    // funcao gameplay
      // check gameover
         // parar tempo
      // input do jogador
      // atualizar posicao do jogador
      // atualizar posicoes dos robots
      // check colisions dos robots
      // imprimir os maze
    // leaderboard 
      // carregar ficheiro 
      // ordenar os tempos
      //
}
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <vector>

using namespace std;

struct Robot {
    int id_num;
    int x, y;
};
struct Player {
    int x, y;
};

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

// function for invalid inputs that clears stream buffer
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

// funtion to choose the maze number and create the file name
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
    if (maze_number == 0)
        return "exit";

    // create the maze file name
    filename = to_string(maze_number);
    s << setfill('0') << setw(2) << filename;
    filename = "maze_" + s.str() + ".txt";
    return filename;
}

// function to load the maze file
int load_mazefile(ifstream &f) { 
    while (true) {
        string filename = choose_maze();
        if (filename == "exit")
            return 0;
        f.open(filename);
        if (f.is_open()) break;
        cerr << "Maze file not found!";
    }
    return 1;
}

// function to load the maze file into a 2D maze vector
void maze_to_vectors(ifstream &f, vector<vector<char>> &maze) {
    char c;
    f.ignore(10000, '\n');
    while (true) {
        vector<char> line;
        while (f.get(c) && c != '\n') {
            line.push_back(c);
        }
        maze.push_back(line);
        if (f.eof()) break;
    }
}

void identify_elements(vector<vector<char>> &maze, Player &pl, vector<Robot> &rb) {
    int count = 0;
    for (size_t i = 0; i < maze.size(); i++) {
        for (size_t j = 0; j < maze.at(i).size(); j++) {
            char c = maze.at(i).at(j);
            if (c == 'R') {
                rb.push_back({ count + 1, (int)j, (int)i });
                count += 1;
            }
            else if (c == 'H')
                pl = { (int)j, (int)i };
        }
    }
    /*cout << rb[0].x << endl << rb[1].x << endl;
    cout << pl.x << " " << pl.y << endl;*/
}

void play() {
    string filename;
    ifstream f;
    vector<vector<char>> maze;

    Player player;
    vector<Robot> robots;

    if (!load_mazefile(f))
        return;
    maze_to_vectors(f, maze);
    identify_elements(maze, player, robots);

    // display maze
    for (size_t i = 0; i < maze.size(); i++) {
        cout << endl;
        for (size_t j = 0; j < maze.at(i).size(); j++) {
            cout << maze.at(i).at(j);
        }
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
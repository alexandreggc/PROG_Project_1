#include <iostream>
#include <ios>
#include <limits>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <vector>

using namespace std;

struct Robot {
    bool alive;
    int id_num;
    int x, y;
};
struct Player {
    bool alive;
    int x, y;
};
struct NameAndTime {
    string name;
    int time;
};

void invalid_input(const string msg);
void rules();
void play();
double timer();

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
            else if (cin.fail() && cin.eof()) exit(0);
            invalid_input("Please enter a valid option. ");
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
void invalid_input(const string msg) {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cout << endl << msg << ' ';
}

// function that returns the player movement vector
vector<int> move_dir(const char dir) {
    if (dir == 'Q') return { -1,-1 };
    if (dir == 'W') return { 0,-1 };
    if (dir == 'E') return { 1,-1 };
    if (dir == 'A') return { -1,0 };
    if (dir == 'S') return { 0,0 };
    if (dir == 'D') return { 1,0 };
    if (dir == 'Z') return { -1,1 };
    if (dir == 'X') return { 0,1 };
    if (dir == 'C') return { 1,1 };
}

// fucntion to display the game rules
void rules() {
    int comeback = -1;
    cout << endl;
    cout << "Welcome to the Maze Game! In this game the player is placed in a maze made up of high-voltage fences and posts.\nThere are also some interceptor robots that will try to destroy the player. If the player touches the maze or any\nof these robots, that is the end of the game. The robots are also destroyed when they touch the fences/posts or\nwhen they collide with each other. Every time the player moves in any direction (horizontally, vertically, or\ndiagonally) to a contiguous cell, each robot moves one cell closer to the new player's location, in whichever\ndirection is the shortest path. The robots have no vision sensors but they have an accurate odor sensor that allows\nthem to follow the player! There is only one hope: make the robots hit the maze or each other. If all of them are\ndestroyed, the player wins. We wish you good luck... \n" << endl;
    cout << "Press 0 to return to the main menu. " << endl;

    while (true) {
        cin >> comeback;
        if (cin.peek() == '\n' && !cin.fail() && comeback == 0) {
            break;
        }
        else if (cin.fail() && cin.eof()) exit(0);
        invalid_input("Please enter a valid option. ");
    }
}

// chooses the maze number
int choose_maze() {
    int maze_number;
    cout << endl;
    cout << "Enter the maze number (1-99) or 0 to return to main menu: ";
    while (true) {
        cin >> maze_number;
        if (cin.peek() == '\n' && !cin.fail() && maze_number >= 0 && maze_number < 100) {
            break;
        }
        else if (cin.fail() && cin.eof()) exit(0);
        invalid_input("Please enter a valid maze number. ");
    }

    return maze_number;
}

// creates maze file name if file argument is 'm' and winners file name if it's 'w'
string file_str(const char file, const int maze_number) {
    string filename;
    stringstream file_number;
    if (maze_number == 0)
        return "exit";
    filename = to_string(maze_number);
    file_number << setfill('0') << setw(2) << filename;
    if (file == 'm')
        filename = "MAZE_" + file_number.str();
    else if(file == 'w')
        filename = "MAZE_" + file_number.str() + "_WINNERS";
    return filename + ".txt";
}

// loads the maze file
int load_mazefile(ifstream &f,const int maze_number) {
    while (true) {
        string maze_name = file_str('m', maze_number);
        if (maze_name == "exit")
            return 0;
        f.open(maze_name);
        if (f.is_open() && f.good()) break;
        cerr << "Maze file not found!";
    }
    return 1;
}

// loads the maze file into a 2D maze vector
void maze_to_vectors(ifstream &f, vector<vector<char>> &maze) {
    char c;
    f.ignore(numeric_limits<streamsize>::max(), '\n');
    while (true) {
        vector<char> line;
        while (f.get(c) && c != '\n') {
            line.push_back(c); // setting up x coordinates
        }
        maze.push_back(line); // setting up y coordinates
        if (f.eof()) break;
    }
}

// identifies the position of the player and the robots in the maze
void identify_elements(vector<vector<char>> &maze, Player &pl, vector<Robot> &rb) {
    int count = 0;
    for (size_t i = 0; i < maze.size(); i++) {
        for (size_t j = 0; j < maze.at(i).size(); j++) {
            char c = maze.at(i).at(j);
            if (c == 'R') {
                rb.push_back({true, count + 1, (int)j, (int)i }); // robots positions
                count += 1;
            }
            else if (c == 'H')
                pl = {true, (int)j, (int)i }; // player position
        }
    }
}

// returns the direction of the minimum path from robot to player
vector<int> min_path(Player& pl, Robot& rb) {
    if (rb.x > pl.x) {
        if (rb.y > pl.y)
            return { -1, -1 };
        if (rb.y < pl.y)
            return { -1, 1 };
        if (rb.y == pl.y)
            return { -1, 0 };
    }
    else if (rb.x < pl.x) {
        if (rb.y > pl.y)
            return { 1, -1 };
        if (rb.y < pl.y)
            return { 1, 1 };
        if (rb.y == pl.y)
            return { 1, 0 };
    }
    else if (rb.x == pl.x) {
        if (rb.y > pl.y)
            return { 0, -1 };
        if (rb.y < pl.y)
            return { 0, 1 };
    }
}

// function that displays the maze
void display_maze(vector<vector<char>>& maze) {
    for (size_t i = 0; i < maze.size(); i++) {
        cout << endl;
        for (size_t j = 0; j < maze.at(i).size(); j++) {
            cout << maze.at(i).at(j);
        }
    }
    cout << endl;
}

// function that checks if player was captured or died
int check_gameover(vector<vector<char>>& maze, Player &pl) {
    if (!pl.alive) {
        int zero = 0;
        display_maze(maze);
        cout << "You lose! Enter 0 to return to main menu: ";
        while (true) {
            cin >> zero;
            if (cin.peek() == '\n' && !cin.fail() && zero == 0) {
                break;
            }
            else if (cin.fail() && cin.eof()) exit(0);
            invalid_input("Please enter a valid option. ");
        }
        return 1;
    }
    else return 0;
}

// function that updates the player position
int update_player_pos(vector<vector<char>>& maze, Player& pl, const char dir) {
    int x, y;
    x = move_dir(dir).at(0) + pl.x;
    y = move_dir(dir).at(1) + pl.y;
    char maze_c = maze.at(y).at(x);
    if (maze_c == 'r')
        return 0;
    if (maze_c == ' ') {
        maze.at(pl.y).at(pl.x) = ' '; // previous position becomes empty
        maze.at(y).at(x) = 'H'; // player reaches new position
        pl.x = x;
        pl.y = y;
        return 1;
    }
    if (maze_c == 'R' || maze_c == '*') {
        maze.at(pl.y).at(pl.x) = ' '; // previous position becomes empty
        maze.at(y).at(x) = 'h'; // player reaches new position
        pl.x = x;
        pl.y = y;
        pl.alive = false;
        return 1;
    }
}

// function that returns the index of a robot given his position
int robot_ind(vector<Robot>& robots, int x, int y) {
    for (size_t i = 0; i < robots.size(); i++) {
        if (robots.at(i).x == x && robots.at(i).y == y)
            return (int)i;
    }
}

// functoin that updates robot's status
void update_robots_pos(vector<vector<char>>& maze, Player& pl, Robot& rb, vector<Robot> &robots) {
    vector<int> dir = min_path(pl, rb);
    int x, y;
    x = dir.at(0) + rb.x;
    y = dir.at(1) + rb.y;
    char maze_c = maze.at(y).at(x);

    if (maze_c == 'r' || maze_c == 'R' || maze_c == '*') {
        if (maze_c == 'R') robots.at(robot_ind(robots, x, y)).alive = false;
        rb.alive = false;
        maze.at(rb.y).at(rb.x) = ' ';
        maze.at(y).at(x) = 'r';
    }
    else if (maze_c == ' ') {
        maze.at(rb.y).at(rb.x) = ' '; // previous position becomes empty
        maze.at(y).at(x) = 'R'; // robot reaches new position
        rb.x = x;
        rb.y = y;
    }
    else if (maze_c == 'H') {
        maze.at(rb.y).at(rb.x) = ' '; // previous position becomes empty
        maze.at(y).at(x) = 'h'; // captured player position becames 'h'
        rb.x = x;
        rb.y = y;
        pl.alive = false;
    }
}

// function that returns a vector with the direction of the player's movement
void player_input(vector<vector<char>>& maze, Player &pl) {
    char dir;
    cout << endl;
    cout << "Enter the position you want to move to: ";
    while (true) {
        cin >> dir;
        dir = toupper(dir);
        if (cin.peek() == '\n' && !cin.fail() && (dir == 'Q' || dir == 'W' || dir == 'A' ||
            dir == 'S' || dir == 'Z' || dir == 'D' || dir == 'X' || dir == 'C' || dir == 'E'))
        {
            if (update_player_pos(maze, pl, dir))
                break;
        }
        else if (cin.fail() && cin.eof()) exit(0);
        invalid_input("Please enter a valid position. ");
    }
}

// function that count the time in seconds
double timer() {
    time_t current_time;
    double seconds;
    time(&current_time);
    seconds = difftime(current_time, 0);
    return seconds;
}

// creates the maze winners file
void winners_file(const string filename) {
    fstream file(filename.c_str());
    if (file.is_open() && file.good() == true) // checks if the file already exists
        return;
    else {
        file.close();
        ofstream f{filename};  // creates leaderboard file if it doesn't exist
        fstream file(filename.c_str());
        file << "Player          - Time\n";
        file << "----------------------\n";
        file.close();
    }
}

//returns the winner's name
string winner_name () {
    string name;
    cout << endl << "Congratulations, you won!" << endl;
    cout << "What's your name (15 characters maximum)? ";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    while (true) {
        cin.clear();
        getline(cin, name, '\n');
        if (!cin.fail() && name.size() <= 15){
            break;
        }
        else if (cin.fail() && cin.eof()) exit(0);
        cout << "Please enter a valid name. ";
    }
    return name;
}

//shows the leaderboard
void leaderboard (const double start_time, const int maze_number) {
    string filename = file_str('w', maze_number);
    double final_time = difftime(timer(), start_time);
    winners_file(filename);

    stringstream winner_s, final_time_s;
    string winner = winner_name();
    vector<NameAndTime> winner_vect;
    string board;

    winner_vect.push_back({ winner, (int)final_time });
    ofstream file(filename, fstream::app);
    winner_s << left << setfill(' ') << setw(15) << winner;
    final_time_s << setfill(' ') << setw(4) << to_string((int)final_time);
    file << winner_s.str() << " - " << final_time_s.str() << "\n";
}

bool any_robots_alive(vector<Robot> &robots) {
    for (size_t i = 0; i < robots.size(); i++) {
        if (robots.at(i).alive) {
            return 1;
        }
    }
    return 0;
}

void play() {
    string filename;
    ifstream f;
    vector<vector<char>> maze;

    Player player;
    vector<Robot> robots;
    vector<int> player_dir;

    // prepare data to initialize the game
    int maze_number = choose_maze();
    if (!load_mazefile(f, maze_number))
        return;
    maze_to_vectors(f, maze);
    identify_elements(maze, player, robots);

    bool gameplay = true;
    double start_time = timer(); // initializes timer

    // game loop
    while (gameplay) {
        display_maze(maze);
        player_input(maze, player);
        if (check_gameover(maze, player)) break;
        for (size_t i = 0; i < robots.size(); i++) {
            if (robots.at(i).alive) {
                update_robots_pos(maze, player, robots.at(i), robots);
                if (check_gameover(maze, player)) {
                    gameplay = false;
                    break;
                }
            }
        }
        if (!any_robots_alive(robots)) {
            display_maze(maze);
            leaderboard(start_time, maze_number);
            break;
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

// ler ficheiro e passar para vetores
// escrever ficheiro a partir dos vetores

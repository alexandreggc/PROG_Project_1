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
    int x, y;
};

void invalidInput();
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

vector<int> move_dir(char dir) {
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
        else if (cin.fail() && cin.eof()) exit(0);
        invalidInput();
    }
}

//chooses the maze number
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
        invalidInput();
    }

    return maze_number;
}

// creates the maze file name
string maze_name() {
    int maze_number = choose_maze();
    string filename;
    stringstream s;
    if (maze_number == 0)
        return "exit";
    filename = to_string(maze_number);
    s << setfill('0') << setw(2) << filename;
    filename = "MAZE_" + s.str() + ".txt";
    return filename;
}

// loads the maze file
int load_mazefile(ifstream &f) {
    while (true) {
        string filename = maze_name();
        if (filename == "exit")
            return 0;
        f.open(filename);
        if (f.is_open()) break;
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
                pl = {(int)j, (int)i }; // player position
        }
    }
    /*cout << rb[0].x << endl << rb[0].y << endl;
    cout << pl.x << " " << pl.y << endl;*/
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

//displays the maze
void display_maze(vector<vector<char>>& maze) {
    for (size_t i = 0; i < maze.size(); i++) {
        cout << endl;
        for (size_t j = 0; j < maze.at(i).size(); j++) {
            cout << maze.at(i).at(j);
        }
    }
}

void gameover() {
    exit(0);
}

// function that updates the player position
int update_player_pos(vector<vector<char>>& maze, Player& pl, char dir) {
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
    if (maze_c == 'R' || maze_c == '*')
        gameover();
}

int robot_ind(vector<Robot>& robots, int x, int y) {
    for (size_t i = 0; i < robots.size(); i++) {
        if (robots.at(i).x == x && robots.at(i).y == y)
            return (int)i;
    }
}

void update_robots_pos(vector<vector<char>>& maze, Player& pl, Robot& rb, vector<Robot> &robots) {
    vector<int> dir = min_path(pl, rb);
    int x, y;
    x = dir.at(0) + rb.x;
    y = dir.at(1) + rb.y;
    char maze_c = maze.at(y).at(x);
    cout << "player: " << pl.x << ' ' << pl.y << endl;
    cout << "robot: " << rb.x << ' ' << rb.y << endl;
    cout << "dir: " << dir.at(0) << ' ' << dir.at(1) << endl;
    cout << "robots size: " << robots.size() << endl;
    cout << "ind robot: " << robot_ind(robots, x, y) << endl;

    if (maze_c == 'r' || maze_c == 'R' || maze_c == '*') {
        cout << "passou 1" << endl << endl;
        if (maze_c == 'R') robots.at(robot_ind(robots, x, y)).alive = false;
        rb.alive = false;
        maze.at(rb.y).at(rb.x) = ' ';
        maze.at(y).at(x) = 'r'; 
    }
    else if (maze_c == ' ') {
        cout << "passou 2"<< endl << endl;
        maze.at(rb.y).at(rb.x) = ' '; // previous position becomes empty
        maze.at(y).at(x) = 'R'; // robot reaches new position
        rb.x = x;
        rb.y = y;
    }
    else gameover();
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
        invalidInput();
    }
}

double timer() {
    time_t current_time;
    double seconds;
    time(&current_time);
    seconds = difftime(current_time, 0);
    return seconds;
}

// checks if the file already exists
inline bool file_exists(const string& name) {
    ifstream f(name.c_str());
    return f.good();
}

// creates the maze winners filename
string winners_filename() {
    int maze_number = choose_maze();
    string filename;
    stringstream s;
    filename = to_string(maze_number);
    s << setfill('0') << setw(2) << filename;
    filename = "MAZE_" + s.str() + " WINNERS" + ".txt";
    return filename;
}

//creates the maze winners file
void winners_file() {
    string filename = winners_filename();
    if (file_exists(filename) == true)
        return ;
    else
        ofstream file {filename};
}

//returns the winner's name
char winner_name () {
    char name;
    cout << "Congratulations! What's your name? " << endl;
    cin >> name;
    return name;
}

//shows the leaderboard
void leaderboard (double start_time) {
    winners_file();
    char winner = winner_name();
    double final_time = difftime(timer(), start_time);
    struct NameAndTime {
        char name;
        int time;
    };
    vector<NameAndTime> winner_vect;
    winner_vect.push_back({winner, (int)final_time});
    fstream fs;
    fs.open (winners_filename(), fstream::in | fstream::out);
    fs << winner << final_time;
}

void play() {
    string filename;
    ifstream f;
    vector<vector<char>> maze;

    Player player;
    vector<Robot> robots;
    vector<int> player_dir;

    // prepare data to initialize the game
    if (!load_mazefile(f))
        return;
    maze_to_vectors(f, maze);
    identify_elements(maze, player, robots);
    double start_time = timer(); // initializes timer

    // game loop
    while (true) {
        display_maze(maze);
        player_input(maze, player);
        for (size_t i = 0; i < robots.size(); i++) {
            if (robots.at(i).alive)
                update_robots_pos(maze, player, robots.at(i), robots);
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

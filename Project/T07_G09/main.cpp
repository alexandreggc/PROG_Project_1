//T07_G09
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

//------------------------------INITIALIZE ALL FUNCTIONS---------------------------------
void invalid_input(const string msg);
void rules();
void play();

//-----------------------------DATA INITIALIZE FUNCTIONS---------------------------------
int choose_maze();
string file_str(const char file, const int maze_number);
int load_mazefile(ifstream& f, const int maze_number);
void maze_to_vectors(ifstream& f, vector<vector<char>>& maze);
void identify_elements(vector<vector<char>>& maze, Player& pl, vector<Robot>& rb);
double timer();

//-------------------------------GAMELOOP FUNCTIONS--------------------------------------
void display_maze(vector<vector<char>>& maze);
int check_gameover(vector<vector<char>>& maze, Player& pl);
bool any_robots_alive(vector<Robot>& robots);

//-------------------------------PLAYER FUNCTIONS----------------------------------------
void player_input(vector<vector<char>>& maze, Player& pl);
vector<int> move_dir(const char dir);
int update_player_pos(vector<vector<char>>& maze, Player& pl, const char dir);

//-------------------------------ROBOT FUNCTIONS-----------------------------------------
vector<int> min_path(Player& pl, Robot& rb);
int robot_ind(vector<Robot>& robots, int x, int y);
void update_robots_pos(vector<vector<char>>& maze, Player& pl, Robot& rb, vector<Robot>& robots);

//------------------------------LEADERBOARD FUNCTIONS------------------------------------
void leaderboard(double start_time, const int maze_number);
void winners_file(const string filename);
void new_winner(const double final_time, const string filename, vector<NameAndTime>& winners);
string winner_name();
void file_to_vectors(const string filename, vector<NameAndTime>& winners);
void sort_winners(vector<NameAndTime>& winners);
void del_duplicate(vector<NameAndTime>& winners);
void vectors_to_file(const string filename, vector<NameAndTime>& winners);

//-------------------------------SORTING SUBFUNCTIONS------------------------------------
string cmp_names(const string name1, const string name2);
void switch_pos(vector<NameAndTime>& winners, int i);
//---------------------------------------------------------------------------------------

// clears stream buffer
void invalid_input(const string msg) {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cout << endl << msg << ' ';
}

int main() {
    int opt;
    cout << "Welcome to Robots Game!" << endl;
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

// displays the game rules
void rules() {
    int comeback = -1;
    cout << endl;
    cout << "Welcome to the Maze Game! In this game the player is placed in a maze made up of high-voltage fences and posts.\nThere are also some interceptor robots that will try to destroy the player. If the player touches the maze or any\nof these robots, that is the end of the game. The robots are also destroyed when they touch the fences/posts or\nwhen they collide with each other. Every time the player moves in any direction (horizontally, vertically, or\ndiagonally) to a contiguous cell, each robot moves one cell closer to the new player's location, in whichever\ndirection is the shortest path. The robots have no vision sensors but they have an accurate odor sensor that allows\nthem to follow the player! There is only one hope: make the robots hit the maze or each other. If all of them are\ndestroyed, the player wins. We wish you good luck... \n" << endl;
    cout << "You can move the player character or stay in the same position by pressing the key correspoding to the direction \nyou want to go, according to the following scheme: " << endl << endl;
    cout << "Q [up & left]       W [up]         E [up & right]" << endl;
    cout << "A [left]            S [stay]       D [right]" << endl;
    cout << "Z [down & left]     X [down]       C [down & right]" << endl << endl;
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


//-----------------------------DATA INITIALIZE FUNCTIONS---------------------------------

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
string file_str(const char file, int maze_number) {
    string filename;
    stringstream file_number;
    if (maze_number == 0)
        return "exit";
    filename = to_string(maze_number);
    file_number << setfill('0') << setw(2) << filename;
    if (file == 'm')
        filename = "MAZE_" + file_number.str();
    else if (file == 'w')
        filename = "MAZE_" + file_number.str() + "_WINNERS";
    return filename + ".txt";
}

// loads the maze file
int load_mazefile(ifstream& f, int maze_number) {
    while (true) {
        string maze_name = file_str('m', maze_number);
        if (maze_name == "exit")
            return 0;
        f.open(maze_name);
        if (f.is_open() && f.good()) break;
        cerr << "Maze file not found!" << endl;
        return 0;
    }
    return 1;
}

// loads the maze file into a 2D maze vector
void maze_to_vectors(ifstream& f, vector<vector<char>>& maze) {
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
void identify_elements(vector<vector<char>>& maze, Player& pl, vector<Robot>& rb) {
    int id_num = 1;
    for (size_t i = 0; i < maze.size(); i++) {
        for (size_t j = 0; j < maze.at(i).size(); j++) {
            char c = maze.at(i).at(j);
            if (c == 'R') {
                rb.push_back({ true, id_num, (int)j, (int)i }); // robots positions
                id_num += 1;
            }
            else if (c == 'H')
                pl = { true, (int)j, (int)i }; // player position
        }
    }
}

// count the time in seconds
double timer() {
    time_t current_time;
    double seconds;
    time(&current_time);
    seconds = difftime(current_time, 0);
    return seconds;
}


//-------------------------------GAMELOOP FUNCTIONS--------------------------------------

// displays the maze in console
void display_maze(vector<vector<char>>& maze) {
    for (size_t i = 0; i < maze.size(); i++) {
        cout << endl;
        for (size_t j = 0; j < maze.at(i).size(); j++) {
            cout << maze.at(i).at(j);
        }
    }
    cout << endl;
}

// checks if player was captured or died
int check_gameover(vector<vector<char>>& maze, Player& pl) {
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

// checks if there are any robots alive
bool any_robots_alive(vector<Robot>& robots) {
    for (size_t i = 0; i < robots.size(); i++) {
        if (robots.at(i).alive) {
            return 1;
        }
    }
    return 0;
}


//-------------------------------PLAYER FUNCTIONS----------------------------------------

// returns a vector with the direction of the player's movement
void player_input(vector<vector<char>>& maze, Player& pl) {
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

// returns the player movement vector
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

// updates the player position
int update_player_pos(vector<vector<char>>& maze, Player& pl, const char dir) {
    int x, y;
    x = move_dir(dir).at(0) + pl.x;
    y = move_dir(dir).at(1) + pl.y;
    char maze_c = maze.at(y).at(x);
    if (maze_c == 'r')  // return impossible move
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


//-------------------------------ROBOT FUNCTIONS-----------------------------------------

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

// returns the index of a robot given his position
int robot_ind(vector<Robot>& robots, int x, int y) {
    for (size_t i = 0; i < robots.size(); i++) {
        if (robots.at(i).x == x && robots.at(i).y == y)
            return (int)i;
    }
}

// updates robot's status
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


//------------------------------LEADERBOARD FUNCTIONS------------------------------------

// leaderboard main function
void leaderboard(double start_time, const int maze_number) {
    vector<NameAndTime> winners;
    string filename = file_str('w', maze_number);
    double final_time = difftime(timer(), start_time);

    winners_file(filename);
    new_winner(final_time, filename, winners);
    file_to_vectors(filename, winners);
    sort_winners(winners);
    del_duplicate(winners);
    vectors_to_file(filename, winners);
    ifstream file(filename);
    cout << endl << file.rdbuf() << endl;  // displays leaderboard in console
    file.close();
}

// creates the maze winners file
void winners_file(const string filename) {
    fstream file(filename.c_str());
    if (file.is_open() && file.good() == true) { // checks if the file already exists
        file.close();
        return;
    }
    else {
        file.close();
        ofstream f{ filename };  // creates leaderboard file if it doesn't exist
        fstream file(filename.c_str());
        file << "Player          - Time\n";
        file << "----------------------\n";
        file.close();
    }
}

// add new winner to the winners file
void new_winner(const double final_time, const string filename, vector<NameAndTime>& winners) {
    stringstream winner_s, final_time_s;
    string winner = winner_name();

    ofstream file(filename, fstream::app);
    winner_s << left << setfill(' ') << setw(15) << winner;
    final_time_s << setfill(' ') << setw(4) << to_string((int)final_time);
    file << winner_s.str() << " - " << final_time_s.str() << "\n";
    file.close();
}

// asks and returns the winner's name
string winner_name() {
    string name;
    cout << endl << "Congratulations, you won!" << endl;
    cout << "What's your name (15 characters maximum)? ";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    while (true) {
        cin.clear();
        getline(cin, name, '\n');
        if (!cin.fail() && name.size() <= 15) {
            break;
        }
        else if (cin.fail() && cin.eof()) exit(0);
        cout << "Please enter a valid name. ";
    }
    return name;
}

// read winners file and create the winners vector
void file_to_vectors(const string filename, vector<NameAndTime>& winners) {
    const int NAME_BEG = 0, NAME_SIZE = 15, TIME_BEG = 18, TIME_SIZE = 4;
    string line, name, time;
    NameAndTime winner;
    ifstream file(filename);

    file.seekg(ios::beg);
    for (int i = 0; i < 2; i++)
        file.ignore(numeric_limits<streamsize>::max(), '\n');
    while (true) {
        file.clear();
        getline(file, line, '\n');
        if (file.eof()) break;
        winner.name = line.substr(NAME_BEG, NAME_SIZE);
        winner.time = stoi(line.substr(TIME_BEG, TIME_SIZE));
        winners.push_back(winner);
    }
    file.close();
}

// order winners vector by time and name
void sort_winners(vector<NameAndTime>& winners) {
    bool not_done = true;
    while (not_done) {
        not_done = false;
        for (int i = 0; i < winners.size() - 1; i++) {
            string name = cmp_names(winners.at(i).name, winners.at(i + 1).name);
            if (winners.at(i).time > winners.at(i + 1).time) {
                not_done = true;
                switch_pos(winners, i);
            }
            if (winners.at(i).time == winners.at(i + 1).time && winners.at(i).name != name) {
                not_done = true;
                switch_pos(winners, i);
            }
        }
    }
}

// erase duplicate names from winners vector
void del_duplicate(vector<NameAndTime>& winners) {
    int i = 0;
    while (i < winners.size()) {
        for (int j = i + 1; j <= winners.size() - 1; j++) {
            if (winners.at(i).name == winners.at(j).name) {
                if (winners.at(i).time > winners.at(j).time) {
                    winners.erase(winners.begin() + i);
                }
                else {
                    winners.erase(winners.begin() + j);
                }
            }
        }
        i += 1;
    }
}

// update file with new winners vector
void vectors_to_file(const string filename, vector<NameAndTime>& winners) {
    ofstream file(filename, ofstream::trunc);
    file << "Player          - Time\n";
    file << "----------------------\n";
    for (int i = 0; i < winners.size(); i++) {
        stringstream winner_s, time_s;
        winner_s << left << setfill(' ') << setw(15) << winners.at(i).name;
        time_s << setfill(' ') << setw(4) << to_string(winners.at(i).time);
        file << winner_s.str() << " - " << time_s.str() << "\n";
    }
    file.close();
}


//-------------------------------SORTING SUBFUNCTIONS------------------------------------

// compare two strings and return the first in ascii order
string cmp_names(const string name1,const string name2) {
    for (unsigned int i = 0; i < name1.size(); i++) {
        if (int(name1[i]) < int(name2[i]))
            return name1;
        else if (int(name1[i]) > int(name2[i]))
            return name2;
    }
    return name2;
}

// switch positions in winners vector
void switch_pos(vector<NameAndTime>& winners, int i) {
    NameAndTime w = winners.at(i);
    winners.at(i) = winners.at(i + 1);
    winners.at(i + 1) = w;
}

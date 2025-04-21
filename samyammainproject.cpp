#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <conio.h>
#include <thread>
#include <chrono>
#include <windows.h>

using namespace std;

const int HEIGHT = 10;
const int WIDTH = 10;

void setConsoleColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void animatedText(const string& text, int delay = 30) {
    for (char c : text) {
        cout << c << flush;
        this_thread::sleep_for(chrono::milliseconds(delay));
    }
}

// Updated SAMYAM Banner
void showSAMYAMBanner() {
    setConsoleColor(11); // Set color to bright cyan
    cout << R"(
  SSSSS  AAAAA  M   M  Y   Y  AAAAA  M   M
 S       A   A  MM MM   Y Y   A   A  MM MM
  SSS    AAAAA  M M M    Y    AAAAA  M M M
     S   A   A  M   M    Y    A   A  M   M
  SSSS   A   A  M   M    Y    A   A  M   M

   )" << endl;
    setConsoleColor(15); // Reset color
}

class MazeGame {
private:
    char maze[HEIGHT][WIDTH];
    int playerX, playerY;
    int enemyX, enemyY;
    int score;
    bool hasPower;
    string username;
    string filePath;
    int currentLevel;

public:
    MazeGame(const string& user, int level) : playerX(1), playerY(1), enemyX(3), enemyY(3), score(0), hasPower(false), username(user), currentLevel(level) {
        filePath = "C:\\Users\\Samyam\\OneDrive\\Desktop\\samyampaudyal cpp\\level" + to_string(currentLevel) + ".txt";
        try {
            loadMaze(filePath);
        } catch (const runtime_error& e) {
            cout << e.what() << endl;
        }
        loadProgress(); // Load progress if available
    }

    void loadMaze(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            throw runtime_error("Error: Could not open file " + filename);
        }

        string line;
        for (int i = 0; i < HEIGHT; i++) {
            getline(file, line);
            for (int j = 0; j < WIDTH; j++) {
                maze[i][j] = line[j];
            }
        }
        file.close();
    }

    void movePlayer(int dx, int dy) {
        if (maze[playerY + dy][playerX + dx] != '#') {
            playerX += dx;
            playerY += dy;
            checkPowerUp();
        }
    }

    void checkPowerUp() {
        if (maze[playerY][playerX] == 'U') {  // 'U' represents the power-up (ULT)
            hasPower = true;
            score += 5;
            maze[playerY][playerX] = '.';  // Clear the power-up from the maze
        }
    }

    void moveEnemy() {
        int randomMove = rand() % 4;
        if (randomMove == 0 && maze[enemyY - 1][enemyX] != '#') enemyY--;
        else if (randomMove == 1 && maze[enemyY + 1][enemyX] != '#') enemyY++;
        else if (randomMove == 2 && maze[enemyY][enemyX - 1] != '#') enemyX--;
        else if (randomMove == 3 && maze[enemyY][enemyX + 1] != '#') enemyX++;
    }

    bool checkExit() {
        if (maze[playerY][playerX] == 'E') {
            cout << "You have reached the exit!" << endl;
            score += 10;
            cout << '\a';
            return true;
        }
        return false;
    }

    bool checkGameOver() {
        if (playerX == enemyX && playerY == enemyY) {
            if (hasPower) {
                hasPower = false;
                cout << "You were hit but survived!" << endl;
                return false;
            }
            cout << "Game Over! You collided with the enemy!" << endl;
            cout << '\a';  // Beep on collision
            return true;
        }
        return false;
    }

    void saveProgress() {
        ofstream file(username + "_progress.txt");
        file << playerX << " " << playerY << " " << enemyX << " " << enemyY << " " << score << " " << hasPower << endl;
        file.close();
    }

    void loadProgress() {
        ifstream file(username + "_progress.txt");
        if (file.is_open()) {
            file >> playerX >> playerY >> enemyX >> enemyY >> score >> hasPower;
            file.close();
            cout << "Progress loaded!" << endl;
        } else {
            cout << "No saved progress found, starting a new game..." << endl;
        }
    }

    void levelChange() {
        if (checkExit()) {
            currentLevel++;
            if (currentLevel > 5) {
                cout << "Congratulations! You've completed all levels!" << endl;
                exit(0);
            }
            filePath = "C:\\Users\\Samyam\\OneDrive\\Desktop\\samyampaudyal cpp\\level" + to_string(currentLevel) + ".txt";
            loadMaze(filePath);
        }
    }

    void displayMaze() {
        system("cls");
        for (int i = 0; i < HEIGHT; i++) {
            for (int j = 0; j < WIDTH; j++) {
                if (i == playerY && j == playerX) {
                    cout << 'P';  // Player symbol
                } else if (i == enemyY && j == enemyX) {
                    cout << 'X';  // Enemy symbol
                } else {
                    cout << maze[i][j];
                }
            }
            cout << endl;
        }
        cout << "Score: " << score << (hasPower ? " | ULT Active!" : "") << endl;  // Display power status
    }

    void playGame() {
        while (true) {
            displayMaze();
            if (_kbhit()) {
                char direction = _getch();
                if (direction == 'w') movePlayer(0, -1);
                else if (direction == 's') movePlayer(0, 1);
                else if (direction == 'a') movePlayer(-1, 0);
                else if (direction == 'd') movePlayer(1, 0);
                else if (direction == 'g') {
                    saveProgress();
                    cout << "Progress saved!" << endl;
                    continue;
                }
            }

            moveEnemy();
            if (checkGameOver()) {
                char restartChoice;
                cout << "Do you want to restart? (y/n): ";
                cin >> restartChoice;
                if (restartChoice == 'y') {
                    MazeGame game(username, currentLevel);  // Restart the game
                    game.playGame();
                } else {
                    break;  // Exit the game
                }
            }
            levelChange();
            this_thread::sleep_for(chrono::milliseconds(200));
        }
    }
};

bool registerUser() {
    string username, password, passwordConfirm;

    cout << "Enter a username: ";
    cin >> username;

    // Check if username already exists
    ifstream file("users.txt");
    string existingUser, existingPass;
    while (file >> existingUser >> existingPass) {
        if (existingUser == username) {
            cout << "Username already exists. Please choose a different one.\n";
            return false;
        }
    }
    file.close();

    // Enter password
    cout << "Enter a password: ";
    cin >> password;

    // Confirm password
    cout << "Confirm your password: ";
    cin >> passwordConfirm;

    if (password != passwordConfirm) {
        cout << "Passwords do not match. Please try again.\n";
        return false;
    }

    // Store the new user credentials
    ofstream outFile("users.txt", ios::app);
    outFile << username << " " << password << endl;
    outFile.close();

    cout << "Registration successful! Please login to continue.\n";
    return true;
}

bool loginUser(string& username, string& password) {
    cout << "Enter your username: ";
    cin >> username;

    cout << "Enter your password: ";
    cin >> password;

    ifstream file("users.txt");
    string storedUser, storedPass;
    while (file >> storedUser >> storedPass) {
        if (storedUser == username && storedPass == password) {
            return true;  // Successful login
        }
    }
    file.close();
    cout << "Invalid credentials. Please try again.\n";
    return false;
}

int main() {
    srand(static_cast<unsigned>(time(0)));
    string username, password;

    // Displaying the SAMYAM banner
    showSAMYAMBanner();

    // Displaying an animated welcome text
    animatedText("Welcome to Samyam's Maze Game!\n", 40);
    animatedText("=====================================\n", 20);

    int choice;
    while (true) {
        setConsoleColor(14); // Set color to yellow
        cout << "\n[1] Register" << endl;
        cout << "[2] Login" << endl;
        cout << "[3] Exit" << endl;
        setConsoleColor(15); // Reset color

        cout << "\nChoose an option: ";
        cin >> choice;

        if (choice == 1) {
            if (registerUser()) break;
        } else if (choice == 2) {
            if (loginUser(username, password)) break;
        } else if (choice == 3) {
            animatedText("Exiting game... See you next time!\n");
            return 0;
        } else {
            cout << "Invalid choice. Try again.\n";
        }
    }

    MazeGame game(username, 1);  // Start the game at level 1
    game.playGame();

    return 0;
}

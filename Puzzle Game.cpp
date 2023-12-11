//PUZZLE GAME
//Here are 4 Tasks:
//Task 1: Using H1 Greedy Search 
//Task 2: Using H1 A* Search
//Task 3: Using H2 greedy Search 
//Task 4: Using H2 A* Search


#include <iostream>
#include <vector>
#include <queue>
#include <unordered_set>

using namespace std;

// Structure to represent a state of the puzzle
struct PuzzleState {
    vector<vector<int>> board;
    int heuristicValue;
    int pathCost; // g(n)

    PuzzleState(const vector<vector<int>>& initialBoard, int pathCost) : board(initialBoard), heuristicValue(0),
                                                                         pathCost(pathCost) {
    }

    // Overloading the < operator for priority queue
    bool operator<(const PuzzleState& other) const {
        return (pathCost + heuristicValue) > (other.pathCost + other.heuristicValue);
    }

    pair<int, int> find_n(int n) const {
        // Find the row and column of n in the board
        const int size = this->board.size();
        for (int i = 0; i < size; ++i) {
            for (int j = 0; j < size; j++) {
                if (board[i][j] == n) {
                    return {i, j};
                }
            }
        }
        return {-1, -1};
    }

    // Check if two puzzle states are equal
    bool operator==(const PuzzleState& other) const {
        return board == other.board;
    }

    // Hash function for unordered_set
    struct Hash {
        size_t operator()(const PuzzleState& state) const {
            size_t hashValue = 0;
            for (const auto& row: state.board) {
                for (int value: row) {
                    hashValue ^= std::hash<int>{}(value) + 0x9e3779b9 + (hashValue << 6) + (hashValue >> 2);
                }
            }
            return hashValue;
        }
    };
};

//Task 1: Using H1 Greedy Search
// Calculate the Manhattan Distance heuristic
int calculateManhattanDistance(PuzzleState& source, PuzzleState& goal) {
    int distance = 0;
    const int size = source.board.size();

    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            if (source.board[i][j] == 0) continue;
            pair<int, int> source_position = source.find_n(source.board[i][j]);
            pair<int, int> goal_position = goal.find_n(source.board[i][j]);
            // Add the distance between the current position and the goal position
            distance += abs(source_position.first - goal_position.first) + abs(
                source_position.second - goal_position.second);
        }
    }

    return distance;
}

// Calculate the Misplaced Tiles heuristic
int calculateMisplacedTiles(PuzzleState& source, PuzzleState& goal) {
    int misplaced = 0;
    int size = source.board.size();

    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            if (source.board[i][j] == 0) {
                continue;
            }
            auto source_position = source.find_n(source.board[i][j]);
            auto goal_position = goal.find_n(source.board[i][j]);
            if (source_position != goal_position) {
                misplaced += 1;
            }
        }
    }

    return misplaced;
}

// Function to print the puzzle board
void printBoard(const vector<vector<int>>& board) {
    for (const auto& row: board) {
        for (int value: row) {
            cout << value << " ";
        }
        cout << endl;
    }
    cout << endl;
}

// Function to validate the input state and goal state for the 8-Puzzle
bool isValidState(const vector<vector<int>>& state) {
    int size = state.size();
    vector<int> values(size * size, 0);

    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            int value = state[i][j];
            if (value < 0 || value >= size * size || values[value] != 0) {
                return false;
            }
            values[value] = 1;
        }
    }

    return true;
}

bool isValidPuzzle(const PuzzleState& a, const PuzzleState& b) {
    // Check if the puzzle is valid by making sure each element in a.board is in b
    const pair<int, int> invalid_position = {-1, -1};
    for (const auto& i: a.board) {
        for (auto j: i) {
            if (invalid_position == b.find_n(j)) {
                return false;
            }
        }
    }
    return true;
}
//Task 2: Using H1 A* Search
// Function to perform A* search
void aStarSearch(const vector<vector<int>>& initialBoard, const vector<vector<int>>& goalBoard, bool useManhattan,
                 bool useMisplaced) {
    int size = initialBoard.size();
    PuzzleState initialState(initialBoard, 0);
    PuzzleState goalState(goalBoard, 0);
    if (not isValidPuzzle(initialState, goalState)) {
        cout << "Invalid initial state or goal state. Please check the values." << endl;
        return;
    }

    priority_queue<PuzzleState> frontier;
    unordered_set<PuzzleState, PuzzleState::Hash> visited;

    frontier.push(initialState);
    int iter = 0;
    while (!frontier.empty()) {
        PuzzleState current = frontier.top();
        frontier.pop();
        iter += 1;
        // cout <<"Current State Cost (Path Cost + Heuristic = Total): " << current.pathCost << " + " << current.heuristicValue <<" = " << current.pathCost + current.heuristicValue << endl;

        if (current == goalState) {
            cout << "Goal state reached!" << endl;
            cout << "Total Cost (f(n)): " << current.pathCost + current.heuristicValue << endl;
            cout << "Total iters: " << iter << endl;
            printBoard(current.board);
            return;
        }


        visited.insert(current);

        // Find the position of the empty space (0)
        pair<int, int> zeroPosition = current.find_n(0);
        int zeroRow = zeroPosition.first;
        int zeroCol = zeroPosition.second;

        // Define possible moves: up, down, left, right
        vector<pair<int, int>> moves = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

        for (const auto& move: moves) {
            int newRow = zeroRow + move.first;
            int newCol = zeroCol + move.second;

            // Check if the new position is within the bounds
            if (newRow >= 0 && newRow < size && newCol >= 0 && newCol < size) {
                // Create a new state after the move
                PuzzleState nextState = current;
                swap(nextState.board[zeroRow][zeroCol], nextState.board[newRow][newCol]);

                // Update the path cost
                nextState.pathCost += 1;
                nextState.heuristicValue = 0;
                // Calculate the heuristic value based on user's choice
                if (useManhattan) {
                    nextState.heuristicValue += calculateManhattanDistance(nextState, goalState);
                }

                if (useMisplaced) {
                    nextState.heuristicValue += calculateMisplacedTiles(nextState, goalState);
                }

                // Check if the new state has not been visited
                if (visited.find(nextState) == visited.end()) {
                    frontier.push(nextState);
                }
            }
        }
    }
    cout << "No solution found." << endl;
}
//Task 3: Using H2 greedy Search 
// Function to perform A* search
void greedySearch(const vector<vector<int>>& initialBoard, const vector<vector<int>>& goalBoard, bool useManhattan,
                  bool useMisplaced) {
    int size = initialBoard.size();
    PuzzleState initialState(initialBoard, 0);
    PuzzleState goalState(goalBoard, 0);
    if (not isValidPuzzle(initialState, goalState)) {
        cout << "Invalid initial state or goal state. Please check the values." << endl;
        return;
    }

    priority_queue<PuzzleState> frontier;
    unordered_set<PuzzleState, PuzzleState::Hash> visited;

    frontier.push(initialState);
    int iter = 0;
    while (!frontier.empty()) {
        PuzzleState current = frontier.top();
        frontier.pop();
        iter += 1;
        // cout <<"Current State Cost (Path Cost + Heuristic = Total): " << current.pathCost << " + " << current.heuristicValue <<" = " << current.pathCost + current.heuristicValue << endl;

        if (current == goalState) {
            cout << "Goal state reached!" << endl;
            cout << "Total Cost (f(n)): " << current.pathCost + current.heuristicValue << endl;
            cout << "Total iters: " << iter << endl;
            printBoard(current.board);
            return;
        }


        visited.insert(current);
        
//Task 4: Using H2 A* Search
        // Find the position of the empty space (0)
        pair<int, int> zeroPosition = current.find_n(0);
        int zeroRow = zeroPosition.first;
        int zeroCol = zeroPosition.second;

        // Define possible moves: up, down, left, right
        vector<pair<int, int>> moves = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

        for (const auto& move: moves) {
            int newRow = zeroRow + move.first;
            int newCol = zeroCol + move.second;

            // Check if the new position is within the bounds
            if (newRow >= 0 && newRow < size && newCol >= 0 && newCol < size) {
                // Create a new state after the move
                PuzzleState nextState = current;
                swap(nextState.board[zeroRow][zeroCol], nextState.board[newRow][newCol]);


                nextState.heuristicValue = 0;
                // Calculate the heuristic value based on user's choice
                if (useManhattan) {
                    nextState.heuristicValue += calculateManhattanDistance(nextState, goalState);
                }

                if (useMisplaced) {
                    nextState.heuristicValue += calculateMisplacedTiles(nextState, goalState);
                }

                // Check if the new state has not been visited
                if (visited.find(nextState) == visited.end()) {
                    frontier.push(nextState);
                }
            }
        }
    }
    cout << "No solution found." << endl;
}

vector<vector<int>> getDefaultIntialState() {
    return {{8, 0, 6}, {5, 4, 7}, {2, 3, 1}};
}

vector<vector<int>> getDefaultGoalState() {
    return {{0, 1, 2}, {3, 4, 5}, {6, 7, 8}};
}

void menu() {
    bool customPuzzle = 0, size = 3;
    system("cls");
        cout << "Default Initial State: \n";
        for (const auto i: getDefaultIntialState()) {
            for (const auto j: i) {
                cout << j << " ";
            }
            cout << endl;
        }
        cout << "Default Goal State: \n";
        for (const auto i: getDefaultGoalState()) {
            for (const auto j: i) {
                cout << j << " ";
            }
            cout << endl;
        }
        cout << "Want to input your own puzzle? (1 for yes, 0 for no): ";
        cin >> customPuzzle;
        if (customPuzzle) {
            cout << "Enter the Puzzle Size ( for 3x3 enter 3 ): ";
            cin >> size;
        }
        vector<vector<int>> initialBoard(size, vector<int>(size));
        vector<vector<int>> goalBoard(size, vector<int>(size));
        if (not customPuzzle) {
            // Intialize the default States for initial and goal
            initialBoard = getDefaultIntialState();
            goalBoard = getDefaultGoalState();
        } else {
            // Get the initial state of the puzzle from the user
            cout << "Enter the initial state of the puzzle (0 represents the empty space):" << endl;
            for (int i = 0; i < size; ++i) {
                for (int j = 0; j < size; ++j) {
                    cout << "Enter Value for Row: " << i + 1 << " Column: " << j + 1 << " : ";
                    cin >> initialBoard[i][j];
                }
            }

            cout << "Enter the goal state of the puzzle (0 represents the empty space):" << endl;
            for (int i = 0; i < size; ++i) {
                for (int j = 0; j < size; ++j) {
                    cout << "Enter Value for Row: " << i + 1 << " Column: " << j + 1 << " : ";
                    cin >> goalBoard[i][j];
                }
            }
        }

        // Ask the user for the heuristic choices
        bool useManhattan, useMisplaced;
        cout << "Select the Heuristics you want to use (one must be chosen)" << endl;
        cout << "Do you want to use Manhattan Distance heuristic? (1 for yes, 0 for no): ";
        cin >> useManhattan;
        cout << "Do you want to use Misplaced Tiles heuristic? (1 for yes, 0 for no): ";
        cin >> useMisplaced;

        // At least one heuristic must be selected
        if (!(useManhattan || useMisplaced)) {
            cout << "Please select at least one heuristic." << endl;
            return;
        }

        int algoChoice = 0;
        cout << "Select the algorithm you want to use (1 for A*, 2 for Greedy Best First Search): ";
        cin >> algoChoice;
        if (algoChoice == 1) {
            // Perform A* search
            aStarSearch(initialBoard, goalBoard, useManhattan, useMisplaced);
        } else if (algoChoice == 2) {
            // Perform Greedy Best First Search
            greedySearch(initialBoard, goalBoard, useManhattan, useMisplaced);
        } else {
            cout << "Invalid choice. Please select 1 for A* or 2 for Greedy Best First Search." << endl;
            return;
        }
}

int main() {
    bool keepRunning = 1;
    do {
        menu();
        cout << "Do you want to continue? (1 for yes, 0 for no): ";
        cin >> keepRunning;
    } while (keepRunning);

    return 0;
}

#include <iostream>
#include <string>
#include <list>

// CSC6033 Module07 Porject 07
// Alexander Medeiros 

class TM {
public:
    // Constructor to initialize the Turing Machine in the initial state
    TM() : currentState(I) {}

    // Method to reset the Turing Machine to its initial state
    void reset() {
        currentState = I; // Set current state to I
        tapeArray.clear(); // Clear the tape
        tapeArray.push_back(' ');
    }

    // Method to initialize the tape with the input string padded to handle situations where 1st binary is shorter than second binary
    void initializeTape(const std::string& input) {
        reset();
        std::string paddedInput = padWithZeros(input); //  Count the set of numbers until the divider, then pad with that many 0's
        for (char c : paddedInput) {
            tapeArray.push_back(c); // Add each character of the padded input string to the tape
        }
        tapeArray.push_back(' ');
        readHeadIndex = ++tapeArray.begin(); // Set the read head to the first character
    }

    // Method to count characters until 'a' and pad with that many 0s
    std::string padWithZeros(const std::string& input) {
        size_t pos = input.find('a');
        if (pos == std::string::npos) {
            std::cerr << "Invalid input: no 'a' found in the string." << std::endl;
            return input;
        }

        // Count the characters until 'a'
        size_t numZeros = pos;

        // Create a string with numZeros number of '0's
        std::string paddedString(numZeros, '0');

        // Add the original input to the padded string
        paddedString.append(input);

        return paddedString;
    }

    // Method to process a single character on the tape based on the current state
    void processChar() {
        char c = getCurrentTapeChar(); // Get the current character under the read head

        // Debugging Tools: Print the current state, read head position, and character under the read head
        std::cout << "Current State: " << stateToString(currentState)
                  << ", Read Head Position: " << std::distance(tapeArray.begin(), readHeadIndex)
                  << ", Char under Read Head: " << c << std::endl;

        // Switch case to handle transitions between states
        switch (currentState) {

            case I: // Initial state
                if (c == '0') {
                    writeToTape('0'); // Write the current character back to the tape
                    moveRight(); // Move the read head to the right
                    currentState = I; // Remain in state I
                } else if (c == '1') {
                    writeToTape('1');
                    moveRight();
                    currentState = I; // Remain in state I
                } else if (c == 'a') {
                    writeToTape('a');
                    moveRight();
                    currentState = J; // Transition to state J
                } else {
                    currentState = FailState; // Transition to fail state if input is outside the language alphabet
                }
                break;

            case J: // State J
                if (c == '0') {
                    writeToTape('0');
                    moveRight();
                    currentState = J; // Remain in state J
                } else if (c == '1') {
                    writeToTape('1');
                    moveRight();
                    currentState = K; // Transition to state K
                } else if (c == ' ') {
                    writeToTape(' ');
                    moveLeft();
                    currentState = O; // Transition to state O
                } else {
                    currentState = FailState; // Transition to fail state
                }
                break;

            case K: // State K
                if (c == '0') {
                    writeToTape('0');
                    moveRight();
                    currentState = K; // Remain in state K
                } else if (c == '1') {
                    writeToTape('1');
                    moveRight();
                    currentState = K; // Remain in state K
                } else if (c == ' ') { 
                    writeToTape(' ');
                    moveLeft();
                    currentState = L; // Transition to state L
                } else {
                    currentState = FailState; // Transition to fail state
                }
                break;

            case L: // State L
                if (c == '0') {
                    writeToTape('1');
                    moveLeft();
                    currentState = L; // Remain in state L
                } else if (c == '1') {
                    writeToTape('0');
                    moveLeft();
                    currentState = M; // Transition to state M
                } else {
                    currentState = FailState; // Transition to fail state
                }
                break;

            case M: // State M
                if (c == '0' || c == '1') {
                    moveLeft();
                    currentState = M; // Remain in state M
                } else if (c == 'a') {
                    moveLeft();
                    currentState = N; // Transition to state N
                } else {
                    currentState = FailState; // Transition to fail state
                }
                break;

            case N: // State N
                if (c == '0') {
                    writeToTape('1');
                    moveRight();
                    currentState = I; // Transition back to state I
                } else if (c == '1') {
                    writeToTape('0');
                    moveLeft();
                    currentState = N; // Remain in state N
                } else if (c == ' ') { 
                    writeToTape('1');
                    moveRight();
                    currentState = I; // Transition back to state I
                } else {
                    currentState = FailState; // Transition to fail state
                }
                break;

            case O: // State O
                if (c == '0') {
                    writeToTape(' ');
                    moveLeft();
                    currentState = O; // Remain in state O
                } else if (c == 'a') {
                    writeToTape(' ');
                    moveLeft();
                    currentState = P; // Transition to state P
                } else {
                    currentState = FailState; // Transition to fail state
                }
                break;

            case P: // State P
                if (c == '0' || c == '1') {
                    moveLeft();
                    currentState = P; // Remain in state P
                } else if (c == ' ') { 
                    moveRight();
                    currentState = FinalState; // Transition to final state (word accepted)
                } else {
                    currentState = FailState; // Transition to fail state (word rejected)
                }
                break;

            case FinalState:
                // Final state, end of processing
                break;

            case FailState: // Handle cases where input is outside of language alphabet
                std::cerr << "Error: Transitioned to FailState!" << std::endl;
                break;

            default: // Handle any undefined state transitions
                std::cerr << "Error: Unhandled state!" << std::endl;
                currentState = FailState; // Transition to FailState in case of error
                break;
        }

        printTape(); // Print the tape after each step for debugging
    }

    // Method to check if the string is accepted by the Turing Machine
    bool isAccepted() const {
        return currentState == FinalState;
    }

    // Method to process the entire string and determine acceptance
    bool processString(const std::string& str) {
        initializeTape(str); // Initialize the tape with the input string
        while (currentState != FinalState && currentState != FailState) {
            processChar(); // Process each character until reaching a final or fail state
        }
        return isAccepted(); // Return whether the string was accepted
    }

    // Method to print the content of the tape for debugging
    void printTape() const {
        std::cout << "Tape content: ";
        for (char c : tapeArray) {
            std::cout << c;
        }
        std::cout << std::endl;
    }

private:
    // Enumeration of possible states in the Turing Machine
    enum State { I, J, K, L, M, N, O, P, FinalState, FailState };
    State currentState; // Current state of the Turing Machine
    std::list<char> tapeArray; // The tape represented as a list of characters
    std::list<char>::iterator readHeadIndex; // Iterator representing the current position of the read head on the tape

    // Method to get the current character under the read head
    char getCurrentTapeChar() const {
        return *readHeadIndex;
    }

    // Method to write a character to the tape at the current read head position
    void writeToTape(char c) {
        *readHeadIndex = c;
    }

    // Method to move the read head to the right
    void moveRight() {
        ++readHeadIndex;
        if (readHeadIndex == tapeArray.end()) {
            tapeArray.push_back(' '); // Expand tape if needed
            --readHeadIndex; // Move back to the newly added element
        }
    }

    // Method to move the read head to the left
    void moveLeft() {
        if (readHeadIndex == tapeArray.begin()) {
            tapeArray.push_front(' '); // Expand tape if needed
        } else {
            --readHeadIndex;
        }
    }

    // Method to convert the current state to a string for debugging
    std::string stateToString(State s) const {
        switch (s) {
            case I: return "I";
            case J: return "J";
            case K: return "K";
            case L: return "L";
            case M: return "M";
            case N: return "N";
            case O: return "O";
            case P: return "P";
            case FinalState: return "FinalState";
            case FailState: return "FailState";
            default: return "Unknown";
        }
    }
};

int main() {
    TM tm; // Create Turing Machine instance
    std::string testString; // String for user input

    // Prompt the user to enter a string of two binary numbers separated by 'a'
    std::cout << "Enter two binary numbers separated by the character 'a'. For example: 1101a11\n";
    std::cin >> testString; // Take the string input from the user

    // Process the string and determine if it is accepted by the Turing Machine
    if (tm.processString(testString)) {
        std::cout << "The string is accepted by the TM. Word is valid." << std::endl;
    } else {
        std::cout << "The string is rejected by the TM. Not a valid word." << std::endl;
    }

    tm.printTape(); // Print the final tape configuration

    return 0;
}

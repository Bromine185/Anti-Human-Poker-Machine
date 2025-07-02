#include <iostream>
#include <string>
#include <vector>
#include <limits>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <cctype>

class PokerMLInterface {
private:
    // Game state variables
    std::string playerHand[2];
    double playerPot;
    std::vector<double> opponentPots;
    int numOpponents;
    double smallBlind;
    double bigBlind;

    // Hand state variables
    std::string communityCards[5];
    int communityCardCount;
    std::vector<double> opponentCalls;
    std::string currentRound;

    // Helper functions
    void clearInputStream() {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    bool isValidCard(const std::string& card) {
        if (card.length() != 2) return false;

        char rank = std::toupper(card[0]);
        char suit = std::toupper(card[1]);

        bool validRank = (rank >= '2' && rank <= '9') || 
                        rank == 'T' || rank == 'J' || 
                        rank == 'Q' || rank == 'K' || rank == 'A';

        bool validSuit = suit == 'H' || suit == 'D' || 
                        suit == 'C' || suit == 'S';

        return validRank && validSuit;
    }

    std::string getValidCard(const std::string& prompt) {
        std::string card;
        while (true) {
            std::cout << prompt;
            std::cin >> card;

            // Convert to uppercase for consistency
            std::transform(card.begin(), card.end(), card.begin(), ::toupper);

            if (isValidCard(card)) {
                return card;
            } else {
                std::cout << "Invalid card format. Use format like AH, KS, 2D, TC, etc.\n";
                clearInputStream();
            }
        }
    }

    double getValidAmount(const std::string& prompt, double minAmount = 0) {
        double amount;
        while (true) {
            std::cout << prompt;
            if (std::cin >> amount && amount >= minAmount) {
                return amount;
            } else {
                std::cout << "Invalid amount. Please enter a number >= " << minAmount << "\n";
                clearInputStream();
            }
        }
    }

    int getValidInteger(const std::string& prompt, int minVal, int maxVal) {
        int value;
        while (true) {
            std::cout << prompt;
            if (std::cin >> value && value >= minVal && value <= maxVal) {
                return value;
            } else {
                std::cout << "Invalid input. Please enter a number between " 
                         << minVal << " and " << maxVal << "\n";
                clearInputStream();
            }
        }
    }

    void displayGameState() {
        std::cout << "\n" << std::string(50, '=') << "\n";
        std::cout << "         CURRENT GAME STATE\n";
        std::cout << std::string(50, '=') << "\n";

        std::cout << "Your Hand: " << playerHand[0] << " " << playerHand[1] << "\n";
        std::cout << "Your Pot: $" << std::fixed << std::setprecision(2) << playerPot << "\n";

        std::cout << "\nOpponents:\n";
        for (int i = 0; i < numOpponents; i++) {
            std::cout << "  Player " << (i + 2) << ": $" << opponentPots[i] << "\n";
        }

        std::cout << "\nBlinds: $" << smallBlind << " / $" << bigBlind << "\n";

        if (communityCardCount > 0) {
            std::cout << "\nCommunity Cards: ";
            for (int i = 0; i < communityCardCount; i++) {
                std::cout << communityCards[i] << " ";
            }
            std::cout << "\n";
        }

        std::cout << std::string(50, '=') << "\n";
    }

public:
    PokerMLInterface() : communityCardCount(0) {}

    void setupGame() {
        std::cout << "\n" << std::string(60, '=') << "\n";
        std::cout << "           POKER ML ALGORITHM INTERFACE\n";
        std::cout << "                Texas Hold'em Style\n";
        std::cout << std::string(60, '=') << "\n\n";

        std::cout << "Setting up new game...\n\n";

        // Get player's initial hand
        std::cout << "Enter your hole cards:\n";
        playerHand[0] = getValidCard("  First card (e.g., AH): ");
        playerHand[1] = getValidCard("  Second card (e.g., KS): ");

        // Get player's pot
        playerPot = getValidAmount("\nEnter your current pot/stack: $");

        // Get number of opponents
        numOpponents = getValidInteger("\nEnter number of opponents (1-8): ", 1, 8);

        // Get opponent pots
        opponentPots.clear();
        std::cout << "\nEnter each opponent's pot:\n";
        for (int i = 0; i < numOpponents; i++) {
            std::string prompt = "  Player " + std::to_string(i + 2) + " pot: $";
            double pot = getValidAmount(prompt);
            opponentPots.push_back(pot);
        }

        // Get blinds
        std::cout << "\nEnter blind structure:\n";
        smallBlind = getValidAmount("  Small blind: $");
        bigBlind = getValidAmount("  Big blind: $");

        std::cout << "\nGame setup complete!\n";
        displayGameState();
    }

    void playHand() {
        std::cout << "\n" << std::string(50, '=') << "\n";
        std::cout << "              NEW HAND\n";
        std::cout << std::string(50, '=') << "\n";

        // Reset community cards
        communityCardCount = 0;
        opponentCalls.clear();

        // Get updated player information
        std::cout << "\nEnter your new hole cards:\n";
        playerHand[0] = getValidCard("  First card: ");
        playerHand[1] = getValidCard("  Second card: ");

        playerPot = getValidAmount("\nEnter your current pot: $");

        // Get opponent call amounts
        std::cout << "\nEnter opponent actions/calls:\n";
        for (int i = 0; i < numOpponents; i++) {
            std::string prompt = "  Player " + std::to_string(i + 2) + " call/bet amount: $";
            double call = getValidAmount(prompt);
            opponentCalls.push_back(call);
        }

        // Play through betting rounds
        playBettingRounds();
    }

    void playBettingRounds() {
        // Pre-flop
        currentRound = "Pre-flop";
        std::cout << "\n--- " << currentRound << " ---\n";
        displayGameState();
        MLDecision decision = getMLDecision();
        displayDecision(decision);

        if (decision.action == "FOLD") {
            std::cout << "\nHand ended.\n";
            return;
        }

        // Flop
        currentRound = "Flop";
        std::cout << "\n--- " << currentRound << " ---\n";
        std::cout << "Enter the flop (3 cards):\n";
        for (int i = 0; i < 3; i++) {
            std::string prompt = "  Card " + std::to_string(i + 1) + ": ";
            communityCards[i] = getValidCard(prompt);
        }
        communityCardCount = 3;

        displayGameState();
        decision = getMLDecision();
        displayDecision(decision);

        if (decision.action == "FOLD") {
            std::cout << "\nHand ended.\n";
            return;
        }

        // Turn
        currentRound = "Turn";
        std::cout << "\n--- " << currentRound << " ---\n";
        communityCards[3] = getValidCard("Enter the turn card: ");
        communityCardCount = 4;

        displayGameState();
        decision = getMLDecision();
        displayDecision(decision);

        if (decision.action == "FOLD") {
            std::cout << "\nHand ended.\n";
            return;
        }

        // River
        currentRound = "River";
        std::cout << "\n--- " << currentRound << " ---\n";
        communityCards[4] = getValidCard("Enter the river card: ");
        communityCardCount = 5;

        displayGameState();
        decision = getMLDecision();
        displayDecision(decision);

        std::cout << "\nHand complete!\n";
    }

    struct MLDecision {
        std::string action;  // "FOLD", "CALL", "RAISE"
        double amount;       // Amount to call or raise
        std::string reasoning;
    };

    MLDecision getMLDecision() {
        // This is where the actual ML algorithm would be called
        // For now, we'll simulate ML decision making

        std::cout << "\n[ML Algorithm Processing...]\n";
        std::cout << "- Analyzing hand strength...\n";
        std::cout << "- Evaluating opponent behavior...\n";
        std::cout << "- Calculating pot odds...\n";
        std::cout << "- Processing vision/tell data...\n";
        std::cout << "- Optimizing for opponent destruction strategy...\n";

        MLDecision decision;

        // Simulate decision logic (replace with actual ML algorithm)
        double randomFactor = (rand() % 100) / 100.0;
        double potOdds = bigBlind / playerPot;

        if (randomFactor < 0.2) {
            decision.action = "FOLD";
            decision.amount = 0;
            decision.reasoning = "Hand too weak for current pot odds";
        } else if (randomFactor < 0.6) {
            decision.action = "CALL";
            decision.amount = bigBlind;
            decision.reasoning = "Decent hand, calling to see next card";
        } else {
            decision.action = "RAISE";
            decision.amount = bigBlind * (2 + randomFactor);
            decision.reasoning = "Strong hand detected, raising for value/bluff";
        }

        return decision;
    }

    void displayDecision(const MLDecision& decision) {
        std::cout << "\n" << std::string(40, '-') << "\n";
        std::cout << "         ML ALGORITHM DECISION\n";
        std::cout << std::string(40, '-') << "\n";

        std::cout << "Action: " << decision.action << "\n";

        if (decision.action != "FOLD") {
            std::cout << "Amount: $" << std::fixed << std::setprecision(2) 
                     << decision.amount << "\n";
        }

        std::cout << "Reasoning: " << decision.reasoning << "\n";
        std::cout << std::string(40, '-') << "\n";
    }

    void displayMenu() {
        std::cout << "\n" << std::string(40, '=') << "\n";
        std::cout << "              MAIN MENU\n";
        std::cout << std::string(40, '=') << "\n";
        std::cout << "1. Setup New Game\n";
        std::cout << "2. Play Hand\n";
        std::cout << "3. View Current Game State\n";
        std::cout << "4. Exit\n";
        std::cout << std::string(40, '=') << "\n";
    }

    void run() {
        int choice;
        bool gameSetup = false;

        std::cout << "Welcome to the Poker ML Algorithm Interface!\n";

        while (true) {
            displayMenu();
            choice = getValidInteger("Enter your choice (1-4): ", 1, 4);

            switch (choice) {
                case 1:
                    setupGame();
                    gameSetup = true;
                    break;

                case 2:
                    if (!gameSetup) {
                        std::cout << "\nPlease setup a game first (Option 1).\n";
                    } else {
                        playHand();
                    }
                    break;

                case 3:
                    if (!gameSetup) {
                        std::cout << "\nNo game setup yet.\n";
                    } else {
                        displayGameState();
                    }
                    break;

                case 4:
                    std::cout << "\nThank you for using Poker ML Interface!\n";
                    return;
            }
        }
    }
};

int main() {
    PokerMLInterface interface;
    interface.run();
    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define DECK_SIZE 52
#define MAX_PLAYERS 4
#define MAX_TURNS 100
#define INITIAL_QUARTERS 3

// Define card structure
typedef struct {
    char value[6]; // "2" to "Ace"
    char suit[10]; // "Hearts", "Diamonds", etc.
} Card;

// Define player structure
typedef struct {
    int quarters;
    int wins;
    Card hand[3];
    int points;
    char strategy[20];
} Player;

// Function prototypes
void initialize_deck(Card deck[]);
void shuffle_deck(Card deck[], int size);
void deal_cards(Player players[], int num_players, Card deck[]);
int sum_points(Player *player);
int has31(Player *player);
void knock(Player players[], int num_players, int knocker_idx);
void wait_for_31(Player *player, Card *shuffled_deck, int *deck_size);
void knock_at_high_value(Player *player, Card *shuffled_deck, int *deck_size);
void indecisive_knock(Player *player, Card *shuffled_deck, int *deck_size);
void knock_first(Player *player);
char* game(Player players[], int num_players, Card deck[]);
void game_loop(int num_games, char strategies[][20], int num_strategies, Card deck[]);

int main() {
    srand(time(NULL)); // Seed random number generator

    // Define strategies
    char strategies[MAX_PLAYERS][20] = {
        "wait_for_31",
        "knock_at_high_value",
        "indecisive_knock",
        "knock_first"
    };
    int num_strategies = 4;

    // Initialize deck
    Card deck[DECK_SIZE];
    initialize_deck(deck);

    // Run game loop
    game_loop(1000, strategies, num_strategies, deck);

    return 0;
}

// Initialize the deck of cards
void initialize_deck(Card deck[]) {
    char *values[] = {"2", "3", "4", "5", "6", "7", "8", "9", "10", "Jack", "Queen", "King", "Ace"};
    char *suits[] = {"Hearts", "Diamonds", "Clubs", "Spades"};
    int index = 0;
    for (int i = 0; i < 13; i++) {
        for (int j = 0; j < 4; j++) {
            strcpy(deck[index].value, values[i]);
            strcpy(deck[index].suit, suits[j]);
            index++;
        }
    }
}

// Shuffle the deck
void shuffle_deck(Card deck[], int size) {
    for (int i = 0; i < size; i++) {
        int j = rand() % size;
        Card temp = deck[i];
        deck[i] = deck[j];
        deck[j] = temp;
    }
}

// Deal cards to players
void deal_cards(Player players[], int num_players, Card deck[]) {
    int card_index = 0;
    for (int i = 0; i < num_players; i++) {
        for (int j = 0; j < 3; j++) {
            players[i].hand[j] = deck[card_index++];
        }
    }
}

// Calculate the sum of points in a player's hand
int sum_points(Player *player) {
    int total = 0;
    char first_suit[10];
    strcpy(first_suit, player->hand[0].suit);

    // Check if all cards have the same suit
    for (int i = 1; i < 3; i++) {
        if (strcmp(player->hand[i].suit, first_suit) != 0) {
            return 0; // Suits don't match
        }
    }

    // Calculate points
    for (int i = 0; i < 3; i++) {
        if (strcmp(player->hand[i].value, "Jack") == 0 ||
            strcmp(player->hand[i].value, "Queen") == 0 ||
            strcmp(player->hand[i].value, "King") == 0) {
            total += 10;
        } else if (strcmp(player->hand[i].value, "Ace") == 0) {
            total += 11;
        } else {
            total += atoi(player->hand[i].value);
        }
    }
    return total;
}

// Check if a player has 31 points
int has31(Player *player) {
    return sum_points(player) == 31;
}

// Knock function
void knock(Player players[], int num_players, int knocker_idx) {
    int high_score = 0;
    for (int i = 0; i < num_players; i++) {
        int score = sum_points(&players[i]);
        if (score > high_score) {
            high_score = score;
        }
    }

    for (int i = 0; i < num_players; i++) {
        if (sum_points(&players[i]) < high_score) {
            players[i].quarters--;
        }
    }
}

// Strategy: Wait for 31
void wait_for_31(Player *player, Card *shuffled_deck, int *deck_size) {
    if (has31(player)) {
        return;
    }
    if (*deck_size > 0) {
        player->hand[2] = shuffled_deck[--(*deck_size)]; // Draw a card
    }
}

// Strategy: Knock at high value
void knock_at_high_value(Player *player, Card *shuffled_deck, int *deck_size) {
    if (sum_points(player) >= 25) {
        knock(&player, 1, 0);
    } else if (*deck_size > 0) {
        player->hand[2] = shuffled_deck[--(*deck_size)]; // Draw a card
    }
}

// Strategy: Indecisive knock
void indecisive_knock(Player *player, Card *shuffled_deck, int *deck_size) {
    if (rand() % 2 == 0 && sum_points(player) >= 20) {
        knock(&player, 1, 0);
    } else if (*deck_size > 0) {
        player->hand[2] = shuffled_deck[--(*deck_size)]; // Draw a card
    }
}

// Strategy: Knock first
void knock_first(Player *player) {
    knock(&player, 1, 0);
}

// Game function
char* game(Player players[], int num_players, Card deck[]) {
    shuffle_deck(deck, DECK_SIZE);
    deal_cards(players, num_players, deck);

    int deck_size = DECK_SIZE - (num_players * 3);
    Card *shuffled_deck = deck + (num_players * 3);

    for (int turn = 0; turn < MAX_TURNS; turn++) {
        for (int i = 0; i < num_players; i++) {
            if (players[i].quarters < 0) {
                continue; // Player is out
            }
            if (has31(&players[i])) {
                players[i].wins++;
                return players[i].strategy;
            }

            if (strcmp(players[i].strategy, "wait_for_31") == 0) {
                wait_for_31(&players[i], shuffled_deck, &deck_size);
            } else if (strcmp(players[i].strategy, "knock_at_high_value") == 0) {
                knock_at_high_value(&players[i], shuffled_deck, &deck_size);
            } else if (strcmp(players[i].strategy, "indecisive_knock") == 0) {
                indecisive_knock(&players[i], shuffled_deck, &deck_size);
            } else if (strcmp(players[i].strategy, "knock_first") == 0) {
                knock_first(&players[i]);
            }
        }
    }

    return NULL; // No winner
}

// Game loop function
void game_loop(int num_games, char strategies[][20], int num_strategies, Card deck[]) {
    int wins[MAX_PLAYERS] = {0};

    for (int game_num = 0; game_num < num_games; game_num++) {
        Player players[MAX_PLAYERS];
        for (int i = 0; i < num_strategies; i++) {
            strcpy(players[i].strategy, strategies[i]);
            players[i].quarters = INITIAL_QUARTERS;
            players[i].wins = 0;
        }

        char *winner = game(players, num_strategies, deck);
        if (winner != NULL) {
            for (int i = 0; i < num_strategies; i++) {
                if (strcmp(players[i].strategy, winner) == 0) {
                    wins[i]++;
                    break;
                }
            }
        }
    }

    // Print results
    printf("Game Results after %d games:\n", num_games);
    for (int i = 0; i < num_strategies; i++) {
        printf("%s: %d wins\n", strategies[i], wins[i]);
    }
}

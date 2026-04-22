#ifndef GAME_H
#define GAME_H

#include <string>
#include <stdexcept>
#include <vector>

struct Node;
struct GameState;
class Card;
class Pile;

/**
 * Skill card type
 * MULTIPLIER: Increases the score multiplier for the next score card
 * HEAD_BUTT: Moves the card at the bottom (tail) of the discard pile to the top (head) of the draw pile
 */
enum SkillCardType {
    MULTIPLIER,
    HEAD_BUTT,
};

/** 
 * Card base class
 * TODO: Add member variables and functions if needed
 */
class Card {
public:
    explicit Card() = default;
    virtual ~Card() = default;
    // Execute card effect, implemented by derived classes. You need to implement the specific effect of the current card type in each derived class
    virtual void play(GameState& game_state) = 0;
};

/** 
 * Score card class
 * TODO: Add member variables and functions as needed
 * You can modify the destructor if needed
 */
class ScoreCard: public Card {
private:
    int point_{};
public:
    explicit ScoreCard(int point);
    void play(GameState &game_state) override;
    ~ScoreCard() override = default;
};

/** 
 * Skill card class
 * TODO: Add member variables and functions as needed
 * You can modify the destructor if needed
 */
class SkillCard: public Card {
private:
    SkillCardType type_{};
public:
    explicit SkillCard(SkillCardType skill_card_type);
    void play(GameState &game_state) override;
    ~SkillCard() override = default;
};

/**
 * Power card class
 * TODO: Add member variables and functions as needed
 * You can modify the destructor if needed
 */
class PowerCard: public Card {
private:
    int plus_{};
public:
    explicit PowerCard(int plus_count);
    void play(GameState &game_state) override;
    ~PowerCard() override = default;
};

/** 
 * Linked list node structure
 * TODO: Add or modify member variables and functions as needed
 */
struct Node {
    Card* card{nullptr};
    Node* next{nullptr};
    explicit Node(Card* c=nullptr): card(c), next(nullptr) {}
};

/** 
 * Pile class
 * All three piles (draw pile, discard pile, hand pile) are represented using this class
 * TODO: Modify as needed, add member variables and functions
 */
class Pile {
private:
    int size_;
    Node* head_;
    Node* tail_;

public:
    friend void outShuffle(GameState&);
    friend void inShuffle(GameState&);
    friend void oddEvenShuffle(GameState&);
    friend class GameController;

    // Constructor and destructor declarations
    // Please implement them in the later part of the file
    Pile();
    ~Pile();

    // Return pile size
    int size() const {
        return size_;
    }

    // Check if pile is empty
    bool empty() const {
        return size_ == 0;
    }

    // Add a card to the end of the pile
    // This function is used to initialize the pile in main.cpp
    // Please implement this function in the later part of the file
    void appendCard(Card* card);
    // Remove and return card from head; returns nullptr if empty
    Card* popHead();
    // Remove and return card from tail; returns nullptr if empty
    Card* popTail();
    // Prepend card to head
    void prependCard(Card* card);
    // Append an existing Node* chain to tail, updating size, and clearing source
    void appendNode(Node* node);
    // Move entire pile to another pile preserving order at tail
    void moveAllTo(Pile& dest);
    // Internal helpers
    Node* head() const { return head_; }
    Node* tail() const { return tail_; }
    // Remove specific node by index (1-based), return Card*
    Card* removeAt(int index);
};

/**
 * Game state structure
 * Stores current game state information such as discard pile, draw pile, hand pile status, total score, current multiplier, etc.
 * TODO: Add member variables and functions as needed
 */
struct GameState {
    Pile hand_{};            // Hand pile
    Pile draw_pile_{};       // Draw pile
    Pile discard_pile_{};    // Discard pile
    long long total_score{0};
    long long add_bonus{0};
    int multiplier{1};
};

/** 
 * Shuffle function declarations
 * outShuffle: Shuffle the discard pile into the draw pile in order
 * inShuffle: Shuffle the discard pile into the draw pile in reverse order
 * oddEvenShuffle: Separate the discard pile by odd and even positions, with odd positions first and even positions last
 * Please implement these functions in the later part of this file
*/
void outShuffle(GameState &game_state);
void inShuffle(GameState &game_state);
void oddEvenShuffle(GameState &game_state);

/**
 * Game controller class
 * Used to handle game logic
 */
class GameController {
private:
    GameState game_state_;
    // Shuffle function pointer
    void (*shuffle_)(GameState&);
public:
    explicit GameController(int mode);

    // Draw 5 cards from the draw pile to the hand pile
    // This is a function declaration, please implement this function in the later part of the file
    void draw();

    // Play the card_to_play-th card in the hand pile (counting from 1)
    // This is a function declaration, please implement this function in the later part of the file
    void play(int card_to_play);

    // End the current turn, move all cards in the hand pile to the discard pile
    // This is a function declaration, please implement this function in the later part of the file
    void finish();

    // Shuffle in the specified manner
    // This is a function declaration, please implement this function in the later part of the file
    void shuffle();

    // Query current total score
    // This is a function declaration, please implement this function in the later part of the file
    int queryScore();

    // Query draw pile size
    int queryDrawPileSize() const {
        return game_state_.draw_pile_.size();
    }

    // Query hand pile size
    int queryHandSize() const {
        return game_state_.hand_.size();
    }

    // Query discard pile size
    int queryDiscardPileSize() const {
        return game_state_.discard_pile_.size();
    }

    // Get reference to the draw pile
    // Used to load cards in main.cpp
    Pile& drawPile() {
        return game_state_.draw_pile_;
    }
};

// ================= 洗牌函数实现 ===================

/**
 * Shuffle the discard pile into the draw pile in order
 * TODO: Implement this function
 */
void outShuffle(GameState& game_state) {
    // Move discard to draw preserving order (head..tail)
    if (game_state.discard_pile_.empty()) return;
    // Append all nodes in order
    game_state.discard_pile_.moveAllTo(game_state.draw_pile_);
}

/**
 * Shuffle the discard pile into the draw pile in reverse order
 * TODO: Implement this function
 */
void inShuffle(GameState& game_state) {
    if (game_state.discard_pile_.empty()) return;
    // Reverse discard into draw: we can pop from tail one by one.
    // Since Pile is singly linked, implement by collecting nodes then append in reverse.
    // Extract nodes to a vector of Card* to preserve reverse order, then append.
    // We'll implement by removing from head and storing, then append in reverse.
    std::vector<Card*> cards;
    cards.reserve(game_state.discard_pile_.size());
    Node* cur = game_state.discard_pile_.head_;
    while (cur) { cards.push_back(cur->card); cur = cur->next; }
    // clear discard
    // delete nodes but keep cards to move
    cur = game_state.discard_pile_.head_;
    while (cur) { Node* nxt = cur->next; delete cur; cur = nxt; }
    game_state.discard_pile_.head_ = game_state.discard_pile_.tail_ = nullptr;
    game_state.discard_pile_.size_ = 0;
    for (auto it = cards.rbegin(); it != cards.rend(); ++it) {
        game_state.draw_pile_.appendCard(*it);
    }
}

/**
 * Separate the discard pile by odd and even positions, with odd positions first and even positions last, and shuffle into the draw pile
 * TODO: Implement this function
 */
void oddEvenShuffle(GameState& game_state) {
    if (game_state.discard_pile_.empty()) return;
    std::vector<Card*> odd;
    std::vector<Card*> even;
    odd.reserve(game_state.discard_pile_.size());
    even.reserve(game_state.discard_pile_.size());
    Node* cur = game_state.discard_pile_.head_;
    int idx = 1;
    while (cur) {
        if (idx % 2 == 1) odd.push_back(cur->card); else even.push_back(cur->card);
        cur = cur->next; ++idx;
    }
    // clear discard nodes
    cur = game_state.discard_pile_.head_;
    while (cur) { Node* nxt = cur->next; delete cur; cur = nxt; }
    game_state.discard_pile_.head_ = game_state.discard_pile_.tail_ = nullptr;
    game_state.discard_pile_.size_ = 0;
    for (Card* c : odd) game_state.draw_pile_.appendCard(c);
    for (Card* c : even) game_state.draw_pile_.appendCard(c);
}
    
// ======================================================




// ================== Game State Structure Implementation ===================
// If you added functions to the GameState structure, you can implement them here





// ======================================================




// ================= Card Class Implementation ===========================

// === Score Card Class Implementation ===
ScoreCard::ScoreCard(int point) {
    point_ = point;
}

void ScoreCard::play(GameState &game_state) {
   long long gained = static_cast<long long>(point_) + game_state.add_bonus;
   gained *= game_state.multiplier;
   game_state.total_score += gained;
   game_state.multiplier = 1;
}
// ===================

// === Skill Card Class Implementation ===
SkillCard::SkillCard(SkillCardType skill_card_type) {
    type_ = skill_card_type;
}
void SkillCard::play(GameState &game_state) {
    if (type_ == SkillCardType::MULTIPLIER) {
        game_state.multiplier += 1;
    } else if (type_ == SkillCardType::HEAD_BUTT) {
        // move discard tail to draw head
        Card* c = game_state.discard_pile_.popTail();
        if (c) {
            game_state.draw_pile_.prependCard(c);
        }
    }
}
// ===================

// === Power Card Class Implementation ===

// === Power Card Class Implementation ===
PowerCard::PowerCard(int plus_count) {
    plus_ = plus_count;
}
void PowerCard::play(GameState &game_state) {
    game_state.add_bonus += plus_;
}
// ===================

// ======================================================




// ================= Pile Class Implementation ===========================
Pile::Pile() : size_(0), head_(nullptr), tail_(nullptr) {}
Pile::~Pile() {
    // delete all nodes but not cards (lifecycle managed externally)
    Node* cur = head_;
    while (cur) { Node* nxt = cur->next; delete cur; cur = nxt; }
    head_ = tail_ = nullptr; size_ = 0;
}

/**
  * Add a card to the end of the pile
  * TODO: Implement this function
  */
void Pile::appendCard(Card* card) {
    Node* node = new Node(card);
    if (!head_) {
        head_ = tail_ = node;
    } else {
        tail_->next = node;
        tail_ = node;
    }
    ++size_;
}

Card* Pile::popHead(){
    if (!head_) return nullptr;
    Node* node = head_;
    head_ = node->next;
    if (!head_) tail_ = nullptr;
    Card* c = node->card;
    delete node; --size_;
    return c;
}

Card* Pile::popTail(){
    if (!tail_) return nullptr;
    if (head_ == tail_) {
        Card* c = head_->card;
        delete head_;
        head_ = tail_ = nullptr;
        --size_;
        return c;
    }
    // find prev
    Node* prev = head_;
    while (prev->next != tail_) prev = prev->next;
    Card* c = tail_->card;
    delete tail_;
    tail_ = prev;
    tail_->next = nullptr;
    --size_;
    return c;
}

void Pile::prependCard(Card* card){
    Node* node = new Node(card);
    if (!head_) {
        head_ = tail_ = node;
    } else {
        node->next = head_;
        head_ = node;
    }
    ++size_;
}

void Pile::appendNode(Node* node){
    if (!node) return;
    // compute length and tail
    int cnt = 0; Node* cur = node; Node* t = nullptr;
    while (cur) { t = cur; ++cnt; cur = cur->next; }
    if (!head_) { head_ = node; tail_ = t; }
    else { tail_->next = node; tail_ = t; }
    size_ += cnt;
}

void Pile::moveAllTo(Pile& dest){
    if (!head_) return;
    dest.appendNode(head_);
    head_ = tail_ = nullptr; size_ = 0;
}

Card* Pile::removeAt(int index){
    if (index <= 0 || index > size_) throw std::runtime_error("Invalid Operation");
    Node* prev = nullptr; Node* cur = head_; int i = 1;
    while (i < index && cur) { prev = cur; cur = cur->next; ++i; }
    if (!cur) throw std::runtime_error("Invalid Operation");
    if (prev) prev->next = cur->next; else head_ = cur->next;
    if (!cur->next) tail_ = prev;
    Card* c = cur->card; delete cur; --size_;
    return c;
}

// TODO: Implement the functions you declared

// ======================================================




// ================= Game Controller Class Implementation ======================

/**
 * Constructor, initialize game state and shuffle mode
 * @param mode Shuffle mode, 1 for outShuffle, 2 for inShuffle, 3 for oddEvenShuffle
 * TODO: Implement this function
 */
GameController::GameController(int mode){
    if (mode == 1) shuffle_ = &outShuffle;
    else if (mode == 2) shuffle_ = &inShuffle;
    else if (mode == 3) shuffle_ = &oddEvenShuffle;
    else shuffle_ = &outShuffle;
}
/**
 * Draw 5 cards from the draw pile to the hand pile
 * TODO: Implement this function
 */
void GameController::draw() {
    int draw_count = 5;
    for (int k = 0; k < draw_count; ++k) {
        if (game_state_.draw_pile_.empty()) {
            // During drawing, if empty and discard not empty, shuffle
            if (!game_state_.discard_pile_.empty()) {
                shuffle_ (game_state_);
            }
        }
        if (game_state_.draw_pile_.empty()) break;
        Card* c = game_state_.draw_pile_.popHead();
        game_state_.hand_.appendCard(c);
    }
}

/**
 * Play the card_to_play-th card in the hand pile (counting from 1)
 * TODO: Implement this function
 * Reference implementation approach:
    * 1. Find the corresponding hand card node
    * 2. Remove the card from the hand pile
    * 3. Execute the card's effect
    * 4. If it's a score card or skill card, move it to the discard pile; if it's a power card, delete it
 */
void GameController::play(int card_to_play) {
    if (card_to_play <= 0 || card_to_play > game_state_.hand_.size())
        throw std::runtime_error("Invalid Operation");
    Card* c = game_state_.hand_.removeAt(card_to_play);
    // Identify type via dynamic_cast; we need members; try cast
    if (auto sc = dynamic_cast<ScoreCard*>(c)) {
        sc->play(game_state_);
        game_state_.discard_pile_.appendCard(c);
    } else if (auto sk = dynamic_cast<SkillCard*>(c)) {
        sk->play(game_state_);
        game_state_.discard_pile_.appendCard(c);
    } else if (auto pw = dynamic_cast<PowerCard*>(c)) {
        pw->play(game_state_);
        delete c; // destroy power card
    } else {
        // Unknown type
        delete c;
        throw std::runtime_error("Invalid Operation");
    }
}

/**
 * Shuffle in the specified manner
 * TODO: Implement this function
 */
void GameController::shuffle() {
    if (shuffle_) shuffle_(game_state_);
}

/**
 * End the current turn, move all cards in the hand pile to the discard pile
 * TODO: Implement this function
 */
void GameController::finish() {
    // Move all from hand to discard preserving order
    game_state_.hand_.moveAllTo(game_state_.discard_pile_);
}

/**
 * Query current total score
 * TODO: Implement this function
 */
int GameController::queryScore() {
    return static_cast<int>(game_state_.total_score);
}

#endif //GAME_H

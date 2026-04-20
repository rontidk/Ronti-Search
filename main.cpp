#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#define SIZE 256
using namespace std;

class Node {
public:
    char data;
    Node* children[SIZE];
    bool eof;
    int count;

    Node() : data('\0'), eof(false), count(0) {
        for (int i = 0; i < SIZE; i++) children[i] = nullptr;
    }
    Node(char val) : data(val), eof(false), count(0) {
        for (int i = 0; i < SIZE; i++) children[i] = nullptr;
    }
};

class Trie {
    Node* root;
public:
    Trie() { root = new Node(); }
    // insertion
    void insert(string word, int frequency) {
        Node* curr = root;
        for (int i = 0; i < (int)word.length(); i++) {
            int idx = word[i];
            if (curr->children[idx] == nullptr)
                curr->children[idx] = new Node(word[i]);
            curr = curr->children[idx];
        }
        curr->eof = true;
        curr->count = frequency;
    }
    // searching
    bool search(string word) {
        Node* curr = root;
        for (int i = 0; i < (int)word.length(); i++) {
            int idx = word[i];
            if (curr->children[idx] == nullptr) return false;
            curr = curr->children[idx];
        }
        return curr->eof;
    }
    // depth for search
    void DFS(Node* node, string current, vector<string>& results, int limit) {
        if (results.size() >= (size_t)limit) return;
        if (node->eof) results.push_back(current);
        for (int i = 0; i < SIZE; i++) {
            if (results.size() >= (size_t)limit) return;
            if (node->children[i] != nullptr)
                DFS(node->children[i], current + node->children[i]->data, results, limit);
        }
    }
    // auto completion
    vector<string> autoComplete(string prefix, int limit = 8) {
        vector<string> results;
        Node* curr = root;
        for (int i = 0; i < (int)prefix.length(); i++) {
            int idx = prefix[i];
            if (curr->children[idx] == nullptr) {
                return results;
            }
            curr = curr->children[idx];
        }
        DFS(curr, prefix, results, limit);
        return results;
    }
    // loading data from file
    void loadFromFile(string filename) {
        ifstream file(filename);
        if (!file.is_open()) return;
        string word;
        int frequency = 370000;
        while (getline(file, word)) {
            insert(word, frequency);
            frequency--;
        }
        file.close();
    }
    // additionalfeature to add custom words in file
    bool insertAndSave(string word, string filename) {
        if (word.empty()) return false;
        if (search(word)) return false;
        insert(word, 1);
        ofstream file(filename, ios::app);
        if (!file.is_open()) return false;
        file << "\n" << word;
        file.close();
        return true;
    }
};

int main() {
    // color palette retro Google inspired
    sf::Color bgColor(255, 255, 255);
    sf::Color headerBg(210, 227, 252);
    sf::Color borderColor(180, 180, 180);
    sf::Color rontiBlue(0, 0, 204);
    sf::Color rontiRed(204, 0, 0);
    sf::Color rontiGreen(0, 128, 0);
    sf::Color rontiYellow(230, 162, 0);
    sf::Color textBlack(34, 34, 34);
    sf::Color dimGray(128, 128, 128);
    sf::Color searchBoxBorder(100, 100, 100);
    sf::Color suggestionHighlight(230, 240, 255);
    sf::Color insertModeRed(180, 0, 0);
    // initializing the SFML window
    sf::RenderWindow window(sf::VideoMode(750, 620), "Ronti - Word Autocomplete");
    window.setFramerateLimit(60);
    // loads fonts from Windows system fonts
    sf::Font regularFont;
    if (!regularFont.loadFromFile("C:\\Windows\\Fonts\\arial.ttf")) return -1;
    sf::Font boldFont;
    boldFont.loadFromFile("C:\\Windows\\Fonts\\arialbd.ttf");
    // initialize trie and shows loading screen while dictionary is being loaded
    Trie trie;
    window.clear(bgColor);
    // loading screen draws Ronti logo letter by letter with Google colors
    sf::Text rLoad("R", boldFont, 72); rLoad.setFillColor(rontiBlue);   rLoad.setPosition(220, 200);
    sf::Text oLoad("o", boldFont, 72); oLoad.setFillColor(rontiRed);    oLoad.setPosition(272, 200);
    sf::Text nLoad("n", boldFont, 72); nLoad.setFillColor(rontiYellow); nLoad.setPosition(320, 200);
    sf::Text tLoad("t", boldFont, 72); tLoad.setFillColor(rontiBlue);   tLoad.setPosition(372, 200);
    sf::Text iLoad("i", boldFont, 72); iLoad.setFillColor(rontiGreen);  iLoad.setPosition(408, 200);
    sf::Text loadingMsg("Loading dictionary...", regularFont, 16);
    loadingMsg.setFillColor(dimGray);
    loadingMsg.setPosition(280, 300);
    window.draw(rLoad); window.draw(oLoad); window.draw(nLoad);
    window.draw(tLoad); window.draw(iLoad); window.draw(loadingMsg);
    window.display();
    // loads words from file into the trie
    trie.loadFromFile("words_alpha.txt");
    // UI state variables
    string searchPrefix = "";
    vector<string> suggestionList;
    bool cursorVisible = true;
    bool insertMode = false;
    string statusMsg = "";
    bool showStatusMsg = false;
    sf::Clock cursorBlinkClock;
    sf::Clock statusMsgClock;
    // main loop that runs until window is closed
    while (window.isOpen()) {
        // handles all window events
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::KeyPressed) {
                // F1 toggles between search and insert mode
                if (event.key.code == sf::Keyboard::F1) {
                    insertMode = !insertMode;
                    searchPrefix = "";
                    suggestionList.clear();
                    showStatusMsg = false;
                }
                // Enter in insert mode saves the typed word to file and trie
                if (event.key.code == sf::Keyboard::Enter && insertMode && !searchPrefix.empty()) {
                    bool savedSuccessfully = trie.insertAndSave(searchPrefix, "words_alpha.txt");
                    statusMsg = savedSuccessfully ? "\"" + searchPrefix + "\" added to dictionary!" : "\"" + searchPrefix + "\" already exists.";
                    showStatusMsg = true;
                    statusMsgClock.restart();
                    searchPrefix = "";
                }
            }
            if (event.type == sf::Event::TextEntered) {
                char typedChar = (char)event.text.unicode;
                // backspace removes last character
                if (typedChar == 8 && searchPrefix.size() > 0)
                    searchPrefix.pop_back();
                // only accepts printable ASCII characters
                else if (typedChar >= 32 && typedChar < 127)
                    searchPrefix += typedChar;
                // updates the suggestions live on every keystroke in search mode
                if (!insertMode)
                    suggestionList = searchPrefix.empty() ? vector<string>() : trie.autoComplete(searchPrefix);
            }
        }
        // hides status message after 2.5 seconds
        if (showStatusMsg && statusMsgClock.getElapsedTime().asSeconds() > 2.5f) {
            showStatusMsg = false;
        }
        // making the cursor blink every 0.5 seconds
        if (cursorBlinkClock.getElapsedTime().asSeconds() > 0.5f) {
            cursorVisible = !cursorVisible;
            cursorBlinkClock.restart();
        }
        // clear screen with white background
        window.clear(bgColor);
        // draws header background
        sf::RectangleShape headerBar(sf::Vector2f(750, 58));
        headerBar.setFillColor(headerBg);
        window.draw(headerBar);
        // draws header bottom border
        sf::RectangleShape headerBottomBorder(sf::Vector2f(750, 1));
        headerBottomBorder.setFillColor(borderColor);
        headerBottomBorder.setPosition(0, 58);
        window.draw(headerBottomBorder);
        // draws Ronti logo in header with colored letters
        float logoX = 20;
        sf::Text logoR("R", boldFont, 32); logoR.setFillColor(rontiBlue);   logoR.setPosition(logoX, 10); window.draw(logoR); logoX += 24;
        sf::Text logoO("o", boldFont, 32); logoO.setFillColor(rontiRed);    logoO.setPosition(logoX, 10); window.draw(logoO); logoX += 20;
        sf::Text logoN("n", boldFont, 32); logoN.setFillColor(rontiYellow); logoN.setPosition(logoX, 10); window.draw(logoN); logoX += 22;
        sf::Text logoT("t", boldFont, 32); logoT.setFillColor(rontiBlue);   logoT.setPosition(logoX, 10); window.draw(logoT); logoX += 16;
        sf::Text logoI("i", boldFont, 32); logoI.setFillColor(rontiGreen);  logoI.setPosition(logoX, 10); window.draw(logoI);
        // shows current mode in top right corner of header
        sf::Text modeIndicator(insertMode ? "[ INSERT MODE ]" : "[ SEARCH MODE ]", regularFont, 13);
        modeIndicator.setFillColor(insertMode ? insertModeRed : rontiBlue);
        modeIndicator.setPosition(530, 8);
        window.draw(modeIndicator);

        sf::Text toggleHint("F1 to toggle", regularFont, 11);
        toggleHint.setFillColor(dimGray);
        toggleHint.setPosition(556, 30);
        window.draw(toggleHint);

        sf::Text wordCountLabel("370,099 words loaded", regularFont, 11);
        wordCountLabel.setFillColor(dimGray);
        wordCountLabel.setPosition(530, 44);
        window.draw(wordCountLabel);
        // draws search input box
        sf::RectangleShape searchInputBox(sf::Vector2f(580, 38));
        searchInputBox.setFillColor(bgColor);
        searchInputBox.setOutlineColor(insertMode ? insertModeRed : searchBoxBorder);
        searchInputBox.setOutlineThickness(1.5f);
        searchInputBox.setPosition(85, 75);
        window.draw(searchInputBox);
        // draws magnifier circle icon
        sf::CircleShape magnifierCircle(9);
        magnifierCircle.setFillColor(bgColor);
        magnifierCircle.setOutlineColor(dimGray);
        magnifierCircle.setOutlineThickness(2);
        magnifierCircle.setPosition(92, 82);
        window.draw(magnifierCircle);
        // draws magnifier handle
        sf::RectangleShape magnifierHandle(sf::Vector2f(2, 8));
        magnifierHandle.setFillColor(dimGray);
        magnifierHandle.setRotation(45);
        magnifierHandle.setPosition(108, 96);
        window.draw(magnifierHandle);
        // draws typed text with blinking cursor
        string displayedInput = searchPrefix + (cursorVisible ? "|" : " ");
        sf::Text inputDisplay(displayedInput, regularFont, 20);
        inputDisplay.setFillColor(textBlack);
        inputDisplay.setPosition(115, 82);
        window.draw(inputDisplay);
        // horizontal divider below search box
        sf::RectangleShape sectionDivider(sf::Vector2f(750, 1));
        sectionDivider.setFillColor(borderColor);
        sectionDivider.setPosition(0, 125);
        window.draw(sectionDivider);
        // shows result count below divider in search mode
        if (!insertMode && !searchPrefix.empty()) {
            string resultCountStr = "About " + to_string(suggestionList.size()) + " results for \"" + searchPrefix + "\"";
            sf::Text resultCount(resultCountStr, regularFont, 12);
            resultCount.setFillColor(dimGray);
            resultCount.setPosition(85, 132);
            window.draw(resultCount);
        }
        // user interface for insert mode
        if (insertMode) {
            sf::Text insertHeading("Add a new word to dictionary", boldFont, 18);
            insertHeading.setFillColor(textBlack);
            insertHeading.setPosition(85, 145);
            window.draw(insertHeading);
            sf::Text insertInstructions("Type a word above and press ENTER to save it permanently.", regularFont, 14);
            insertInstructions.setFillColor(dimGray);
            insertInstructions.setPosition(85, 175);
            window.draw(insertInstructions);
            // to display success or error message after inserting
            if (showStatusMsg) {
                sf::Text statusDisplay(statusMsg, regularFont, 14);
                statusDisplay.setFillColor(statusMsg.find("added") != string::npos ? rontiGreen : rontiRed);
                statusDisplay.setPosition(85, 205);
                window.draw(statusDisplay);
            }
        }
        // user interface for search mode
        else {
            // empty state means no input yet
            if (searchPrefix.empty()) {
                sf::Text emptyHint("Start typing in the search box above...", regularFont, 16);
                emptyHint.setFillColor(dimGray);
                emptyHint.setPosition(85, 155);
                window.draw(emptyHint);

                sf::Text engineTagline("Trie-based Autocomplete Engine", regularFont, 13);
                engineTagline.setFillColor(dimGray);
                engineTagline.setPosition(85, 185);
                window.draw(engineTagline);
            }
            // no suggestions found for given prefix
            else if (suggestionList.empty()) {
                sf::Text noResultMsg("No results found for \"" + searchPrefix + "\"", regularFont, 16);
                noResultMsg.setFillColor(textBlack);
                noResultMsg.setPosition(85, 155);
                window.draw(noResultMsg);

                sf::Text noResultSubMsg("Try different keywords or add the word using F1 Insert Mode.", regularFont, 13);
                noResultSubMsg.setFillColor(dimGray);
                noResultSubMsg.setPosition(85, 180);
                window.draw(noResultSubMsg);
            }
            // draws suggestion list 
            else {
                for (int i = 0; i < (int)suggestionList.size(); i++) {
                    float rowY = 148 + i * 52;
                    // highlights first suggestion with light blue background
                    if (i == 0) {
                        sf::RectangleShape firstRowHighlight(sf::Vector2f(680, 44));
                        firstRowHighlight.setFillColor(suggestionHighlight);
                        firstRowHighlight.setPosition(0, rowY - 4);
                        window.draw(firstRowHighlight);
                    }
                    // suggestion word styled as blue hyperlink
                    sf::Text suggestionWord(suggestionList[i], boldFont, 20);
                    suggestionWord.setFillColor(rontiBlue);
                    suggestionWord.setPosition(85, rowY);
                    window.draw(suggestionWord);
                    // underline to mimic hyperlink style
                    float wordWidth = suggestionWord.getLocalBounds().width;
                    sf::RectangleShape wordUnderline(sf::Vector2f(wordWidth, 1));
                    wordUnderline.setFillColor(rontiBlue);
                    wordUnderline.setPosition(85, rowY + 24);
                    window.draw(wordUnderline);
                    sf::RectangleShape rowSeparator(sf::Vector2f(680, 1));
                    rowSeparator.setFillColor(sf::Color(220, 220, 220));
                    rowSeparator.setPosition(0, rowY + 48);
                    window.draw(rowSeparator);
                }
            }
        }
        // draws bottom bar with keyboard shortcuts
        sf::RectangleShape bottomBar(sf::Vector2f(750, 35));
        bottomBar.setFillColor(headerBg);
        bottomBar.setPosition(0, 585);
        window.draw(bottomBar);
        sf::RectangleShape bottomBarBorder(sf::Vector2f(750, 1));
        bottomBarBorder.setFillColor(borderColor);
        bottomBarBorder.setPosition(0, 585);
        window.draw(bottomBarBorder);
        sf::Text shortcutHints("BACKSPACE delete   |   F1 toggle insert/search   |   ENTER save word   |   ESC exit", regularFont, 11);
        shortcutHints.setFillColor(dimGray);
        shortcutHints.setPosition(100, 594);
        window.draw(shortcutHints);
        window.display();
        // pressing ESC key closes the window
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
            window.close();
    }
    return 0;
}


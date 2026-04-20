# Ronti - Word Autocomplete Engine

A real-time word autocomplete engine built in C++ using a 256-ary Trie data structure.
Supports 370,099 English words with live prefix search, DFS-based suggestion collection,
and a retro Google-style UI built with SFML.

# Demo
> Type a prefix → suggestions appear instantly

# Features
- Real-time autocomplete as you type
- 370,099 English words loaded from dictionary file
- DFS traversal to collect all words matching a prefix
- Top 8 suggestions displayed per search
- Frequency-ranked results (common words appear first)
- Insert Mode — add your own words and save them permanently
- Retro Google-style UI built with SFML
- Blinking cursor, live result count, search/insert mode toggle

# Data Structure
# Trie (256-ary)
Each node contains:
- `char data` — the character this node represents
- `Node* children[256]` — one slot per ASCII character
- `bool isEndOfWord` — marks if a complete word ends here
- `int frequency` — used for ranking suggestions

# Why Trie?
- Insert: O(L) where L = word length
- Search: O(L) where L = word length
- Prefix lookup: O(L) — far faster than scanning an array or BST

# DFS for Autocompletion
Once the prefix node is reached, DFS explores all reachable
paths collecting every word where `isEndOfWord == true`.

## How It Works

User types "ca"
→ autoComplete() walks: root → c → a
→ DFS() explores all children from 'a' node
→ Collects: "ca", "cab", "cabin", "cable", "can", "car", "cat" ...
→ Returns top 8 by frequency

# Controls

| Key         | Action                          |
|-------------|---------------------------------|
| Type        | Search / autocomplete           |
| Backspace   | Delete last character           |
| F1          | Toggle Search / Insert mode     |
| Enter       | Save new word (Insert mode only)|
| ESC         | Exit                            |

# Project Structure

├── dem.cpp              # Main source file
├── words_alpha.txt      # 370,099 word dictionary
├── README.md

# Setup & Run
# Requirements
- Visual Studio 2022
- SFML 2.6.0

# SFML Setup
1. Download SFML 2.6.0 from https://www.sfml-dev.org
2. In VS project properties set:
   - C/C++ → Additional Include Directories: `path\to\SFML\include`
   - Linker → Additional Library Directories: `path\to\SFML\lib`
   - Linker → Additional Dependencies: `sfml-graphics-d.lib`, `sfml-window-d.lib`, `sfml-system-d.lib`
3. Copy DLLs from `SFML\bin` into your project folder

# Run
1. Clone the repo
2. Place `words_alpha.txt` in the same folder as the executable
3. Build and run in Visual Studio

# Word Dictionary

Words loaded from `words_alpha.txt` — a free English word list
containing 370,099 lowercase words.

Source: https://github.com/dwyl/english-words

# Course Context
Built as a personal project during Data Structures and Algorithms course.

Concepts covered:
- Trie data structure
- Depth First Search (DFS)
- Recursive tree traversal
- File I/O in C++
- External library integration (SFML)


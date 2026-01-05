#include "../include/highscore.hpp"
#include <fstream>
#include <iostream>

HighscoreManager::HighscoreManager(const std::string &path): highscore(0), path(path) {
    load();
}

int HighscoreManager::getHighscore() const {
    return highscore;
}

void HighscoreManager::load() {
    std::ifstream in(path);

    if (!in) {
        highscore = 0;
        return;
    }

    int value = 0;
    if (in >> value) highscore = value;
    else highscore = 0;
}

bool HighscoreManager::saveIfHigher(int score) {
    if (score <= highscore) return false;

    std::ofstream out(path, std::ios::trunc);
    if (!out) return false; // failed to open file

    out << score << std::endl;
    highscore = score;
    return true;
}


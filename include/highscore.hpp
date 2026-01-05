#pragma once

#include <string>

class HighscoreManager {
public:
    explicit HighscoreManager(const std::string &path = "highscore.txt");
    int getHighscore() const;
    void load();
    bool saveIfHigher(int score);

private:
    int highscore;
    std::string path;
};


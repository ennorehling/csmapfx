#pragma once

#include <map>
#include <fx.h>

class Passwords {
private:
    struct Password {
        FXint turn;
        FXString password;
    };
    std::map<int, Password> m_data;
public:
    void erase(int factionId);
    void set(int factionId, int turn, const FXString& password);
    bool get(int factionId, FXString &out);

    void loadState(FXRegistry &reg);
    void saveState(FXRegistry &reg);
};

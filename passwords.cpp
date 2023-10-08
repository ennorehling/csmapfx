#include "passwords.h"
#include "fxhelper.h"

void Passwords::erase(int factionId) {
    m_data.erase(factionId);
}

void Passwords::set(int factionId, int turn, const FXString &password) {
    if (!password.empty()) {
        std::map<int, Password>::iterator iPassword = m_data.find(factionId);
        if (iPassword == m_data.end() || turn >= (*iPassword).second.turn) {
            m_data[factionId] = Password{ turn, password };
        }
    }
}

bool Passwords::get(int factionId, FXString &out) {
    std::map<int, Password>::iterator iPassword = m_data.find(factionId);
    if (iPassword == m_data.end()) return false;
    out = (*iPassword).second.password;
    return true;
}

void Passwords::loadState(FXRegistry &reg)
{
    FXString stream = reg.readStringEntry("settings", "passwords");
    FXint i = 0;
    for (FXint i = 0;;++i) {
        FXString item = stream.section(' ', i);
        if (item.empty()) break;
        FXint id = FXIntVal(item.section(':' , 0), 36);
        FXint turn = FXIntVal(item.section(':' , 1), 10);
        FXString password = item.section(':', 2);
        if (!password.empty()) {
            set(id, turn, password);
        }
    }
}

void Passwords::saveState(FXRegistry &reg)
{
    FXString stream;
    for (std::map<int, Password>::value_type &iPassword : m_data) {
        FXint factionId = iPassword.first;
        FXint turn = iPassword.second.turn;
        const FXString &password = iPassword.second.password;
        if (!stream.empty()) {
            stream += " ";
        }
        stream += FXStringValEx(factionId, 36) + ":" + FXStringVal(turn) + ":" + password;
    }
    reg.writeStringEntry("settings", "passwords", stream.text());
}

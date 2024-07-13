#ifndef MESSAGEHISTORY_H
#define MESSAGEHISTORY_H
#include <string>
#include <deque>
class MessageHistory
{
public:
    MessageHistory(int size);
    void push_message(std::string& message);
    std::string to_string();
private:
    std::deque<std::string> history;
    int history_size;
};

#endif // MESSAGEHISTORY_H

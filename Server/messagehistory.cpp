#include "messagehistory.h"

MessageHistory::MessageHistory(int size)
    : history_size(size)
{}

void MessageHistory::push_message(std::string& message)
{
    if(history.size() >= history_size)
        history.pop_front();
    history.push_back(message);
}

std::string MessageHistory::to_string()
{
    std::string stream;
    for(const auto& str : history) {
        stream += str + '\n';
    }
    return stream;
}


#include "messagehistory.h"

MessageHistory::MessageHistory(int size)
    : _history_size(size)
{}

void MessageHistory::push_message(std::string&& message)
{
    if(_history.size() >= _history_size)
        _history.pop_front();
    _history.push_back(std::move(message));
}

std::string MessageHistory::to_string()
{
    std::string stream;
    for(const auto& str : _history) {
        stream += str + '\n';
    }
    return stream;
}

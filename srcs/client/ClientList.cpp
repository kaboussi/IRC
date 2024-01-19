#include "ClientList.hpp"

std::map<std::string, Client*> ClientList::_clients;

void ClientList::add(Client* client) {
    _clients[client->getUserInfo().getNickname()] = client;
}

Client* ClientList::get(const std::string& nickname) {
    std::map<std::string, Client*>::iterator it = _clients.find(nickname);

    return (it == _clients.end()) ? NULL : it->second;
}

Client* ClientList::get(const size_t& index) {
    if (index >= _clients.size())
        throw std::out_of_range("index out of range.");
    std::map<std::string, Client*>::iterator it = _clients.begin();

    std::advance(it, index);
    return it->second;
}

void ClientList::remove(const std::string& nickname) {
    std::map<std::string, Client*>::iterator it = _clients.find(nickname);

    if (it == _clients.end())
        throw std::runtime_error("no such client.");
    _clients.erase(it);
}

size_t ClientList::numberOfClients() {
    return _clients.size();
}

bool ClientList::exist(const std::string& nickname) {
    return _clients.find(nickname) != _clients.end();
}

size_t ClientList::size() {
    return _clients.size();
}

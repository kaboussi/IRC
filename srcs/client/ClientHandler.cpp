//
// Created by Redouane Elkaboussi on 1/16/24.
//

#include "ClientHandler.hpp"
#include "../parser/Message.hpp"
#include "../server/Reactor.hpp"

/*
 * RFC 1459 #section-2.3
 *
 * 510 is the Max _message length
 * 512 is for the last trailing CRLF(\r\n)
 * 513 a byte extra for protection if exceeded the limits
 */

#define MAX_MSG_LEN 512

void ClientHandler::handleNewConnection(const pollfd& serverFd) {
    sockaddr_storage clientAddr;
    socklen_t        clientAddrLen = sizeof(clientAddr);
    int              clientSocket = accept(
        serverFd.fd, reinterpret_cast<sockaddr*>(&clientAddr), &clientAddrLen);

    if (clientSocket == -1) {
        std::cerr << "Failed to accept new connection" << std::endl;
        return;
    }

    Reactor::getInstance().addClient(new Client(clientAddr, clientSocket),
                                     clientSocket);
}

void ClientHandler::handleClientInput(Client*& pClient) {
    if (pClient->isDoneReading()) {
        return;
    }

    char buffer[MAX_MSG_LEN + 1];
    std::memset(buffer, 0, sizeof(buffer));

    int bytesRead =
        recv(pClient->getSockfd(), buffer, sizeof(buffer), MSG_DONTWAIT);

    if (bytesRead < 0) {
        handleReceiveError(pClient);
        return;
    } else if (bytesRead == 0) {
        std::cout << "Hangup\n";
        Reactor::getInstance().removeClient(pClient);
        return;
    } else if (bytesRead > MAX_MSG_LEN) {
        handleTooLongMessage(pClient);
        return;
    }
    pClient->appendMessage(buffer);

    if (hasEndOfMessage(pClient->getMessage())) {
        pClient->setIsDoneReading(true);
        Message msg_parser;

        try {
            msg_parser.parse(pClient->getMessage());
        } catch (const std::exception& e) {
            std::cout << e.what() << std::endl;
        }
    }
}

void ClientHandler::handleReceiveError(Client*& pClient) {
    Reactor::getInstance().removeClient(pClient);
}

void ClientHandler::handleTooLongMessage(Client*& pClient) {
    // TODO: Throw an exception or handle the case of a too long message
}

bool ClientHandler::hasEndOfMessage(const std::string& message) {
    return (message.find(CR_LF) != std::string::npos) ||
           (message.find(LF_) != std::string::npos);
}
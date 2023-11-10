# C++ Terminal Chat Application with Winsock2

## Overview

This is a simple client-server chat application written in C++ that operates in a terminal environment. The application utilizes the Winsock2 library for socket programming to establish communication between a client and a server.

## Features

- **Echoing messages:** Currently server can only echo messages back to sender. 

- **Future Features:**
  - **Direct Chats with Usernames:** Capability to have private conversations with specific users.
  - **Rooms Handling:** Support for group chats to facilitate communication among multiple users simultaneously.
  - **Usernames:** Users can set and display unique usernames.
  - **RSA Encryption:** Implementation of RSA encryption for secure communication.
  - **Linux Version:** Adaptation of the application to run on Linux platforms.
  - **Graphical User Interface (GUI):** A graphical interface for a more user-friendly experience.

## Prerequisites

- **Windows Operating System:** The application is currently developed for Windows using the Winsock2 library.
- **C++ Compiler:** Ensure you have a C++ compiler installed (e.g., MinGW for Windows).

## How to Build

1. Clone the repository:

    ```bash
    git clone https://github.com/jekuper/cpp_chat.git
    ```

2. Compile Server scripts:

    ```bash
    cd cpp_chat_client/cpp_chat_client/
    g++ server.cpp -o server
    ```

3. Compile Client scripts:

    ```bash
    cd cpp_chat_server/cpp_chat_server/
    g++ client.cpp -o client
    ```

## How to Run

### Server

1. Open a terminal and navigate to the project's server directory.

2. Run the server:

    ```bash
    ./server
    ```

### Client

1. Open a new terminal and navigate to the project's client directory.

2. Run the client:

    ```bash
    ./client
    ```

3. Follow the on-screen prompts to connect to the server and start chatting.

## Future Development

Contributions and feature requests are welcome. Here are some areas for potential enhancement:

- Implementing additional security measures.
- Improving error handling and input validation.
- Enhancing the user interface for both the server and client.
- Extending platform compatibility to include Linux.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

- Special thanks to the Winsock2 library for enabling network communication in the Windows environment.
- Inspiration and guidance from various online resources and tutorials.

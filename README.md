# C++ Terminal Chat Application with Winsock2

## Overview

This is a simple client-server chat application written in C++ that operates in a terminal environment. The application utilizes the Winsock2 library for socket programming to establish communication between a client and a server.

## Features

- **P2P chatting:** Clients can connect to a server and communicate with one other specified client.

- **Future Features:**
  - **Rooms Handling:** Support for group chats to facilitate communication among multiple users simultaneously.
  - **RSA Encryption:** Implementation of RSA encryption for secure communication.
  - **Linux Version:** Adaptation of the application to run on Linux platforms.
  - **Graphical User Interface (GUI):** A graphical interface for a more user-friendly experience.

## Prerequisites

- **Windows Operating System:** The application is currently developed for Windows using the Winsock2 library.

## How to Build
Assuming you have **MSVC C++ Compiler** and **msbuild** available. If not, then install Visual Studio.

1. Clone the repository:

    ```bash
    git clone https://github.com/jekuper/cpp_chat.git
    ```

2. Open a VS Developer Command Prompt in this directory

3. Compile Server scripts:

    ```bash
    cd cpp_chat_server
    msbuild cpp_chat_server.sln /p:Configuration=Release
    ```

4. Compile Client scripts:

    ```bash
    cd cpp_chat_client
    msbuild cpp_chat_client.sln /p:Configuration=Release
    ```
5. Run server:

    ```bash
    cd cpp_chat_server/x64/Release
    cpp_chat_server.exe
    ```
6. Run client:

    ```bash
    cd cpp_chat_client/x64/Release
    cpp_chat_client.exe
    ```

Refer to [client](cpp_chat_client/README.md) and [server](cpp_chat_server/README.md) documentations for detailed functionality.

## We ACCEPT new ideas!

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
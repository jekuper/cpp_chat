# Client Documentation

## Building the Solution File
To build the solution file, follow these steps:

1. Open a terminal.

2. Navigate to the root directory of the client application.

3. Run the following command:

```bash
msbuild cpp_chat_client.sln /p:Configuration=Release
```
## Command Line Arguments
The client application accepts the following command-line arguments:

### --name (Required)
* __Description:__ Specifies the desired username for the client.
* __Usage:__ ``--name <desired_username>``
* __Example:__ ``--name Alice``

### --target (Required)
* __Description:__ Specifies the target peer's IP address.
* __Usage:__ ``--target <target_ip>``
* __Example:__ ``--target 192.168.1.2``
### --server (Required)
* __Description:__ Specifies the server's IP address.
* __Usage:__ ``--server <server_ip>``
* __Example:__ ``--server 192.168.1.1``
## Running the Client
After building the solution, you can run the client with the specified command-line arguments. Here's an example:

```bash
cpp_chat_client.exe --name Alice --target 192.168.1.2 --server 192.168.1.1
```
Replace ``cpp_chat_client.exe`` with the actual name of your client executable.

## Troubleshooting
If you encounter any issues during the build or execution process, please refer to the documentation or contact us.

Thank you for using our terminal application! If you have any feedback or suggestions, feel free to reach out.
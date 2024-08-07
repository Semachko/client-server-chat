# Client-Server Chat

Date of creation - 07.2024

____

### Client-Server Chat - a desktop application with the ability to communicate over a local network via IP.
____

## Technologies and frameworks used:
- C++, Qt Creator, Boost.Asio

____
# **Starting the server**

![image](https://github.com/user-attachments/assets/3ec6a2d0-9ceb-41b1-88b1-b24af024222d)

### *For the beginning you need to start the server. After it doesn't matter anymore, you can just leave the server window running.*

____

# This includes: 

# **Connecting to the server**

![image](https://github.com/user-attachments/assets/80e7662a-efc1-4d19-a503-110f16767f13)

### *You need to enter the IP/port and unique username to connect. By default the server port is 44445*

____
# **Chat window**

<img src="https://github.com/user-attachments/assets/c3ae8b36-967e-451c-b8ec-37bda9454018"/>
<img src="https://github.com/user-attachments/assets/e91410af-34f6-440c-b86c-8a300d12bdf2"/>

### *After you are connected, the chat history will be loaded. From now on, you can freely communicate with chat users.*

____

# **Details:**

- Connections and data exchange are implemented through the **TCP protocol**. I/O operations are processed through asynchronous operations using **io_context** from the *Boost.Asio* library.
- To compile the project, the *Boost.Asio* library is required.

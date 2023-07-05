# robocin-ines

## Table of Contents

- [Development Environment](#development_environment)
- [Codemap](#codemap)


<a name="development_environment"></a>

## Development Environment

This repository is developed using Visual Studio Code's DevContainers. DevContainers provide a consistent and reproducible development environment for the project. It ensures that all developers working on the project have the same dependencies and configurations.

### Prerequisites

To use the DevContainer development environment, make sure you have the following prerequisites installed on your local machine:

- Visual Studio Code (VSCode)
- Docker

### Getting Started

To set up the development environment using DevContainers:

1. Clone this repository to your local machine.
2. Open the repository in VSCode.
3. If you have the "Remote - Containers" extension installed, it will detect the DevContainer configuration automatically. Click on the green icon at the bottom left corner of the VSCode window and select "Reopen in Container." If you don't have the extension, you will be prompted to install it.
4. VSCode will build the Docker image and set up the development container for you. This process may take a few minutes depending on your internet speed.
5. Once the DevContainer is ready, the repository will open in a new VSCode window within the container.
6. You can now edit, run, and test the code within the DevContainer environment.

<a name="codemap"></a>

## Codemap

- [`vision.py`](ines/vision_py/vision.py): acts as a mapper-retransmitter for vision UDP messages received from RoboCup SSL's Vision system. It receives the UDP messages, maps them in a RobôCIn's protocol and finally sends using ZeroMQ Inter-Process Communication (IPC) protocol.

- [`referee.py`](ines/referee_py/referee.py): acts as a retransmitter for referee UDP messages received from RoboCup SSL's AutoReferee system. It receives the UDP messages and retransmits them using the ZeroMQ Inter-Process Communication (IPC) protocol.


<a name="license"></a>

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.


Descript that the current repository contains separated clients that uses a common protocol (ZeroMQ ipc) to link in them in a microservice

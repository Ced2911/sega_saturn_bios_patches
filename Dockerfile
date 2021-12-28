FROM ced2911/saturntoolchain:latest

# fix vscode formatter
RUN apt update && apt install zlib1g-dev libtinfo5 -y

# build tools
RUN mkdir tools/patcher/build && cd tools/patcher/build && cmake .. && make
# 定义编译器
CXX = g++

# 定义编译选项
CXXFLAGS = -Wall -g -I./include
LDFLAGS = -lpthread

# 定义目标文件目录
BIN_DIR = build/bin
OBJ_DIR = build/obj

# 定义目标文件
SERVER_TARGET = $(BIN_DIR)/server
CLIENT_TARGET = $(BIN_DIR)/client

# 定义源文件
SRCS = Server.cpp EchoServer.cpp TcpServer.cpp Acceptor.cpp Connection.cpp Buffer.cpp Eventloop.cpp Epoll.cpp Channel.cpp Socket.cpp InetAddress.cpp ThreadPool.cpp Logger.cpp TimeStamp.cpp
CLIENT_SRC = Client.cpp Buffer.cpp

# 定义对象文件
OBJS = $(SRCS:%.cpp=$(OBJ_DIR)/%.o)
CLIENT_OBJ = $(CLIENT_SRC:%.cpp=$(OBJ_DIR)/%.o)

# 创建目标目录
$(shell mkdir -p $(BIN_DIR) $(OBJ_DIR))

# 默认目标
all: $(SERVER_TARGET) $(CLIENT_TARGET)

# 链接服务器目标文件
$(SERVER_TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# 链接客户端目标文件
$(CLIENT_TARGET): $(CLIENT_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

# 编译源文件到对象文件
$(OBJ_DIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 清理生成的文件
clean:
	rm -rf $(BIN_DIR) $(OBJ_DIR)

# 定义伪目标（不实际存在的文件名）
.PHONY: all clean

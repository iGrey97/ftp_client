CC := gcc

TARGET = ftp_client
SRCS := $(shell find  -name "*.c") 

#SRCS = $(wildcard *.c)
OBJS = $(patsubst %c, %o, $(SRCS))
# -I指定头文件目录
#INCLUDE = -I./include
# -L指定库文件目录，-l指定静态库名字(去掉文件名中的lib前缀和.a后缀)
#LIB = -L./libs -ltomcrypt
# 开启编译warning和设置优化等级
CFLAGS = -Wall -g -O3


.PHONY:all clean
#伪目标，表示不把这个位置的all、clean，当成需要生成的文件（避免要编译生成的文件也有all、clean同名的）
all: $(TARGET)
#all 后面可跟要生成的多个可执行文件，all是伪目标，所以后面的多个真正的目标都会生成，这个方法用来生成多个目标
# 链接时候指定库文件目录及库文件名
$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LIB)
 
# 编译时候指定头文件目录
%o:%c
	$(CC) -c $^ $(INCLUDE) $(CFLAGS) 

clean:
	rm -rf $(OBJS) $(TARGET)


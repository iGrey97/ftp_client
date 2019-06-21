CC := gcc

TARGET = ftp_client
SRCS := $(shell find  -name "*.c") 

#SRCS = $(wildcard *.c)
OBJS = $(patsubst %c, %o, $(SRCS))
# -Iָ��ͷ�ļ�Ŀ¼
#INCLUDE = -I./include
# -Lָ�����ļ�Ŀ¼��-lָ����̬������(ȥ���ļ����е�libǰ׺��.a��׺)
#LIB = -L./libs -ltomcrypt
# ��������warning�������Ż��ȼ�
CFLAGS = -Wall -g -O3


.PHONY:all clean
#αĿ�꣬��ʾ�������λ�õ�all��clean��������Ҫ���ɵ��ļ�������Ҫ�������ɵ��ļ�Ҳ��all��cleanͬ���ģ�
all: $(TARGET)
#all ����ɸ�Ҫ���ɵĶ����ִ���ļ���all��αĿ�꣬���Ժ���Ķ��������Ŀ�궼�����ɣ���������������ɶ��Ŀ��
# ����ʱ��ָ�����ļ�Ŀ¼�����ļ���
$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LIB)
 
# ����ʱ��ָ��ͷ�ļ�Ŀ¼
%o:%c
	$(CC) -c $^ $(INCLUDE) $(CFLAGS) 

clean:
	rm -rf $(OBJS) $(TARGET)


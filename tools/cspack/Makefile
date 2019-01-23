
srcs = $(wildcard src/*.c)
objs = $(patsubst %.c,%.o,$(srcs))
target = cspack

heads = src

CFLAGS = -O0 -g -I$(heads) -Wall
LDFLAGS = -static -lc


all : $(target)

$(target) : $(objs)
	@gcc $(CFLAGS) $(LDFLAGS) $(objs) -o $(target) 


clean :
	@rm -rf $(objs)

$(objs) : $(srcs)

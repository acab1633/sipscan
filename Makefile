all: scan.c ip4_range.c link.c message.c opts.c request.c response.c siplib.c
	gcc scan.c ip4_range.c link.c message.c opts.c request.c response.c siplib.c -lpthread -o optscan -Wall

client: performConnection.c prolog.c
	clang -c performConnection.c -Wall -Werror
	clang -c network.c -Wall -Werror
	clang -c prolog.c -Wall -Werror
	clang -o client performConnection.o prolog.o network.o -Wall -Werror

# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: brettleclerc <brettleclerc@student.42.f    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/06/23 16:24:43 by lkukhale          #+#    #+#              #
#    Updated: 2024/06/25 12:05:55 by brettlecler      ###   ########.fr        #
#                                                                              #
# **************************************************************************** #



NAME = webserv

SRC = ${addsuffix .cpp, ./src/main ./src/Server ./src/Utils ./src/Config ./src/ConfigBase ./src/ServerConfig \
						./src/ServerRoutesConfig ./src/Cluster ./src/Response ./src/Response-Process ./src/HeaderUtil ./src/Cgi}
HEADERS = -I ./include/
CC = c++
RM = rm -f
CPPFLAGS = -Wall -Wextra -Werror -g -std=c++98 
OBJ = ${SRC:.cpp=.o}


all: ${NAME}

.cpp.o:
	@${CC} ${CPPFLAGS} -o $@ -c $< ${HEADERS}

${NAME}: ${OBJ}
	${CC} ${CPPFLAGS} ${OBJ} ${HEADERS} -o ${NAME}

clean:
	${RM} ${OBJ}

fclean: clean
	${RM} ${NAME}

re: fclean ${NAME}

.PHONY: all clean fclean re
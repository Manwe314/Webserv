# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: lkukhale <lkukhale@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/06/23 16:24:43 by lkukhale          #+#    #+#              #
#    Updated: 2024/06/11 20:58:27 by lkukhale         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #



NAME = webserv

SRC = ${addsuffix .cpp, ./src/main ./src/Server ./src/Utils ./src/Config ./src/ConfigBase ./src/ServerConfig \
						./src/ServerRoutesConfig ./src/Cluster ./src/Response ./src/Response-Process ./src/HeaderUtil}
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
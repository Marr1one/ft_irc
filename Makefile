# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: esouhail <esouhail@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/02/22 23:44:43 by marwan            #+#    #+#              #
#    Updated: 2026/03/18 23:35:52 by esouhail         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = ircserv

SRCDIR = src
OBJDIR = build
INCDIR = include
SERVERDIR = $(SRCDIR)/server
CLIENTDIR = $(SRCDIR)/client
CHANNELDIR = $(SRCDIR)/channel
MESSAGEDIR = $(SRCDIR)/message
COMMANDDIR = $(SERVERDIR)/commands
SERVERINCDIR = $(INCDIR)/server
CLIENTINCDIR = $(INCDIR)/client
CHANNELINCDIR = $(INCDIR)/channel
MESSAGEINCDIR = $(INCDIR)/message

SRCS = $(SRCDIR)/main.cpp \
	   $(SERVERDIR)/Server.cpp \
	   $(SERVERDIR)/BlackjackGame.cpp \
	   $(SERVERDIR)/ServerNetwork.cpp \
	   $(SERVERDIR)/ServerLoop.cpp \
	   $(SERVERDIR)/CommandHandler.cpp \
	   $(MESSAGEDIR)/Parser.cpp \
	   $(COMMANDDIR)/Pass.cpp \
	   $(COMMANDDIR)/Nick.cpp \
	   $(COMMANDDIR)/User.cpp \
	   $(COMMANDDIR)/Join.cpp \
	   $(COMMANDDIR)/Privmsg.cpp \
	   $(COMMANDDIR)/Blackjack.cpp \
	   $(COMMANDDIR)/Part.cpp \
	   $(COMMANDDIR)/Quit.cpp \
	   $(COMMANDDIR)/Kick.cpp \
	   $(COMMANDDIR)/Invite.cpp \
	   $(COMMANDDIR)/Topic.cpp \
	   $(COMMANDDIR)/Mode.cpp \
	   $(COMMANDDIR)/Cap.cpp \
	   $(COMMANDDIR)/Ping.cpp \
	   $(CLIENTDIR)/Client.cpp \
	   $(CHANNELDIR)/Channel.cpp

CXX = c++
CXXFLAGS = -Wall -Werror -Wextra -std=c++98
INCFLAGS = -I $(INCDIR) \
		   -I $(SERVERINCDIR) \
		   -I $(CLIENTINCDIR) \
		   -I $(CHANNELINCDIR) \
		   -I $(MESSAGEINCDIR)

OBJS = $(SRCS:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

all : $(NAME)

$(NAME) : $(OBJS)
	$(CXX) $(CXXFLAGS) $(INCFLAGS) $(OBJS) -o $(NAME)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(INCFLAGS) -c $< -o $@

clean : 
	rm -rf $(OBJDIR)

fclean : clean
	rm -f $(NAME)

re : fclean $(NAME)

.PHONY : re clean all fclean

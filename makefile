OBJDIR = obj
BINDIR = bin
OBJS	=  $(addprefix $(OBJDIR)/, main.o room.o flappy_bird.o import.o libWeb.o textField.o tetris.o snake.o asteroid.o shooter.o hashage.o fullpath.o communFunctions.o leaderboard.o demineur.o )
SOURCE	= launcher.c
HEADER	= room.h flappy_bird.h tetris.h snake.h asteroid.h shooter.h demineur.h fullpath.h leaderboard.h
OUT	= Nineteen


UNAME := $(shell uname)

CC = gcc


FLAGS	 = -g -c -Wall
LFLAGS	 =  -lm -lcurl -lcurl -L/usr/local/opt/openssl/lib -lcrypto -lassimp
LSDL = `sdl2-config --cflags --libs` -lSDL2 -lSDL2_ttf -lSDL2_image -lSDL2_mixer


ifeq ($(UNAME), Linux)
FFLAGS = -lGL -lGLU -lglut
endif

ifeq ($(UNAME), Darwin)
FFLAGS = -framework OpenGL -framework GLUT
endif

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(BINDIR)/$(OUT) $(LFLAGS) $(LSDL) $(FFLAGS)

$(OBJDIR)/room.o: room/room.c $(OBJDIR)/import.o
	$(CC) $(FLAGS) $(LSDL) -o $@ $<

$(OBJDIR)/main.o: main.c
	$(CC) $(FLAGS) $(LSDL) -o $@ $<

$(OBJDIR)/flappy_bird.o : games/3_flappy_bird/flappy_bird.c
	$(CC) $(FLAGS) $(LSDL) -o $@ $<

$(OBJDIR)/shooter.o : games/4_shooter/shooter.c
	$(CC) $(FLAGS) $(LSDL) -o $@ $<

$(OBJDIR)/demineur.o : games/6_demineur/demineur.c
	$(CC) $(FLAGS) $(LSDL) -o $@ $<

$(OBJDIR)/tetris.o : games/5_tetris/tetris.c
	$(CC) $(FLAGS) $(LSDL) -o $@ $<

$(OBJDIR)/snake.o : games/2_snake/snake.c
	$(CC) $(FLAGS) $(LSDL) -o $@ $<

$(OBJDIR)/asteroid.o : games/7_asteroid/asteroid.c
	$(CC) $(FLAGS) $(LSDL) -o $@ $<

$(OBJDIR)/leaderboard.o : room/leaderboard/leaderboard.c
	$(CC) -o $@ $< $(FLAGS)

$(OBJDIR)/import.o : room/import.c
	$(CC) -o $@ $< $(FLAGS)

$(OBJDIR)/libWeb.o : include/libWeb.c
	$(CC) -o $@ $< $(FLAGS)

$(OBJDIR)/hashage.o : include/hashage.c
	$(CC) -o $@ $< $(FLAGS)

$(OBJDIR)/textField.o : include/textField.c
	$(CC) -o $@ $< $(FLAGS)

$(OBJDIR)/fullpath.o : include/fullpath.c
	$(CC) -o $@ $< $(FLAGS)

$(OBJDIR)/communFunctions.o : include/communFunctions.c
	$(CC) -o $@ $< $(FLAGS)

clean:
	rm -f $(OBJS) ./$(OUT)

run: $(OUT)
	./$(OUT)

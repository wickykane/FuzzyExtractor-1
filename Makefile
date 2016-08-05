OBJS=main.o minutiae_template.o circle.o
BIN=fuzzy_extractor
LIBS = -lopencv_core -lopencv_highgui -lopencv_imgproc
FLAGS= -ggdb

$(BIN): $(OBJS)
	g++ $(OBJS) -o $(BIN) $(LIBS) $(FLAGS)

%.o: %.cpp %.h
	g++ -c $< -o $@ $(FLAGS)
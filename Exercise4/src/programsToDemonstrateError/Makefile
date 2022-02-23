# build an executable
TARGET1=serverSolution
TARGET2=clientSolution
all:
	gcc serverSolution.c clientServerProtocol.c -o $(TARGET1) -lpthread
	gcc clientSolution.c -o $(TARGET2)

clean: 
	rm $(TARGET1)
	rm $(TARGET2)

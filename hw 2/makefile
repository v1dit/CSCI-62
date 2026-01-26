a.out: user.o network.o social_network.o
	g++ user.o network.o social_network.o

user.o: user.cpp user.h
	g++ -c user.cpp

network.o: network.cpp network.h user.h
	g++ -c network.cpp

social_network.o: social_network.cpp network.h user.h
	g++ -c social_network.cpp

clean:
	rm -f *.o a.out

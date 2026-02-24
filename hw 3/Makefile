a.out: user.o network.o post.o social_network.o
	g++ -std=c++11 user.o network.o post.o social_network.o -o a.out

user.o: user.cpp user.h
	g++ -std=c++11 -c user.cpp

network.o: network.cpp network.h user.h post.h
	g++ -std=c++11 -c network.cpp

post.o: post.cpp post.h
	g++ -std=c++11 -c post.cpp

social_network.o: social_network.cpp network.h user.h post.h
	g++ -std=c++11 -c social_network.cpp

clean:
	rm -f *.o a.out

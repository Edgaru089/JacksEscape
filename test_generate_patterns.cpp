// g++ test_generate_patterns.cpp -lsfml-graphics -lsfml-window -lsfml-system -lX11 -lGL

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>

#include <SFML/Graphics.hpp>

using namespace std;
using namespace sf;


uint8_t pattern[8];
int     x = 0, y = 0;

void outpattern() {
	cout << "\"";
	for (int i = 0; i < 8; i++)
		printf("\\x%02x", pattern[i]);
	cout << "\"" << endl;
}

void setbit(int x, int y, bool value) {
	if (x < 0 || x > 7 || y < 0 || y > 7)
		return;
	uint8_t &target = pattern[y];
	if (value) { // set to 1
		uint8_t mask = 1 << (7 - x);
		target |= mask;
	} else { // set to 0
		uint8_t mask = ~(1 << (7 - x));
		target &= mask;
	}
}

bool getbit(int x, int y) {
	if (x < 0 || x > 7 || y < 0 || y > 7)
		return false;
	uint8_t &target = pattern[y];
	uint8_t  mask   = 1 << (7 - x);
	return (target & mask) != 0;
}

int main() {
	memset(pattern, -1, sizeof(pattern));

	RenderWindow win(VideoMode(400, 400), "Generator");
	win.setVerticalSyncEnabled(true);
	win.setFramerateLimit(120);

	outpattern();
	setbit(0, 0, false);
	outpattern();


	while (win.isOpen()) {
		Event e;
		while (win.pollEvent(e)) {
			if (e.type == Event::KeyPressed) {
				switch (e.key.code) {
					case Keyboard::Up:
						y--;
						break;
					case Keyboard::Down:
						y++;
						break;
					case Keyboard::Left:
						x--;
						break;
					case Keyboard::Right:
						x++;
						break;
				}
				setbit(x, y, false);
				outpattern();
			} else if (e.type == Event::Closed)
				win.close();
		}

		win.clear();

		for (int i = 0; i < 8; i++)
			for (int j = 0; j < 8; j++) {
				if (getbit(i, j)) {
					RectangleShape rect(Vector2f(50, 50));
					rect.setPosition(i * 50, j * 50);
					rect.setFillColor(Color::White);
					win.draw(rect);
				}
			}

		win.display();
	}
}

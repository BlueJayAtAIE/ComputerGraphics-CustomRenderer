#pragma once

class context
{
	// Forwards declaration: I am delacring a type and using it here.
	struct GLFWwindow *window;

public:
	bool init(int width, int height, const char *title);
	void tick();
	void term();
	void clear();

	bool shouldClose() const;
};
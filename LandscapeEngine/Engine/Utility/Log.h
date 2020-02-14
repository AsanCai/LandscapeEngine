#pragma once

namespace LandscapeEngine {
	/* 0 = print errors to stdout
   1 = print errors to log.txt */
	#define ERROR_LOG 0


	void logInit();
	void log(const char* message, ...);
	void logOpenglError();
	void logOpenglClearErrors();
}
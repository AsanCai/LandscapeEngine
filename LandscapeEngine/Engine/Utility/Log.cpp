#include <Utility/Log.h>

#include <glad/glad.h>

#include <stdio.h>
#include <stdarg.h>
#include <atltime.h>

namespace LandscapeEngine{
	void logInit() {
		if (ERROR_LOG == 1) {
			FILE* file;
			errno_t error;
			error = fopen_s(&file, "../log.txt", "a");
			if (error != 0) {
				fprintf(stdout, "Could not open log.txt for appending!");
				return;
			}

			CString t = CTime::GetCurrentTime().Format("%Y-%m-%d %H:%M");
			fprintf(file, "\n\n[%s]\n", t);
			fclose(file);
		}
	}

	// should be able to splicing all parameters
	void log(const char* message, ...) {
		va_list args;

		va_start(args, message);

		if (ERROR_LOG == 1) {
			FILE* file;
			errno_t error;
			error = fopen_s(&file, "../log.txt", "a");
			if (error != 0) {
				fprintf(stdout, "Could not open log.txt for appending!");
				return;
			}

			vfprintf(file, message, args);
			fclose(file);
		}
		else {
			vfprintf(stderr, message, args);
		}

		va_end(args);
	}

	void logOpenglError() {
		int e = glGetError();
		if (e != 0) {
			printf("OpenGL Error: 0x%x\n\n", e);
		}
	}

	void logOpenglClearErrors() {
		while (glGetError() != 0);
	}
}
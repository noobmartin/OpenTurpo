#ifndef LOG_MANAGER_h
#define LOG_MANAGER_h

#define MICROS_TO_SECONDS 1/1000000
class logger {
public:
	logger();
	~logger();

	void logMessage(char const *message);
	void logMessage(const char *message, float value);
	void logMessage(const char *message, int value);

private:
};

#endif

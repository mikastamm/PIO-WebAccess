#ifndef ARGPARSER_H
#define ARGPARSER_H

#include <Arduino.h>

class ArgParser {
public:
    ArgParser(const char *input);
    ~ArgParser();

    bool getInt(const char *key, int &outValue);
    bool getFloat(const char *key, float &outValue);
    bool getString(const char *key, const char **outValue);

private:
    struct Node {
        char *key;
        char *value;
        Node *next;
    } *head = nullptr, *tail = nullptr;

    void addKeyValuePair(const char *segment);
    char *strdup(const char *str);
    bool streq(const char *s1, const char *s2);
    bool toInt(const char* str, int& outValue);
    bool toFloat(const char *str, float &outValue);
};

#endif // ARGPARSER_H

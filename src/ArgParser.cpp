// ArgParser.cpp
#include "ArgParser.h"
#include <Arduino.h>

char *ArgParser::strdup(const char *str)
{
    if (str == nullptr)
    {
        return nullptr;
    }
    size_t len = strlen(str);
    char *copy = new char[len + 1];
    strcpy(copy, str);
    return copy;
}

bool ArgParser::streq(const char *s1, const char *s2)
{
    return strcmp(s1, s2) == 0;
}
bool ArgParser::toInt(const char* str, int& outValue) {
  int value = 0;
  bool hasDigits = false;

  // Check if the string is negative.
  bool isNegative = (*str == '-');
  if (isNegative) {
    ++str;
  }

  while (*str) {
    if (*str < '0' || *str > '9') {
      // It's not a number; handle the error if no digits were found
      return hasDigits;
    }
    value = value * 10 + (*str - '0');
    ++str;
    hasDigits = true;
  }

  // Apply the negative sign if necessary.
  outValue = isNegative ? -value : value;
  return hasDigits;
}

bool ArgParser::toFloat(const char *str, float &outValue)
{
    if (str == nullptr || *str == '\0' || *str == '+' || *str == '-')
    {
        // Empty string or only sign is not a valid number
        return false;
    }
    
    float value = 0.0f;
    float factor = 0.1f;
    bool decimalPoint = false;
    bool hasDigits = false;

    // Check if the string is negative.
    bool isNegative = (*str == '-');
    if (isNegative || *str == '+')
    {
        ++str; // Skip the sign
    }

    while (*str)
    {
        if (*str >= '0' && *str <= '9')
        {
            hasDigits = true;
            if (decimalPoint)
            {
                value += (*str - '0') * factor;
                factor *= 0.1f;
            }
            else
            {
                value = value * 10.0f + (*str - '0');
            }
        }
        else if (*str == '.' && !decimalPoint)
        {
            decimalPoint = true;
        }
        else
        {
            // Invalid character encountered
            return false;
        }
        ++str;
    }

    if (!hasDigits)
    {
        // String contains only a decimal point or signs but no digits
        return false;
    }

    outValue = isNegative ? -value : value;
    return true;
}

ArgParser::ArgParser(const char *input)
{
    char *segment = strdup(input);
    char *segmentStart = segment;
    char *ptr = segment;

    while (*ptr)
    {
        if (*ptr == ';')
        {
            *ptr = '\0';
            addKeyValuePair(segmentStart);
            segmentStart = ptr + 1;
        }
        ptr++;
    }
    // Handle the last key-value pair if it's not followed by a semicolon
    if (segmentStart != ptr)
    {
        addKeyValuePair(segmentStart);
    }
    delete[] segment;
}

ArgParser::~ArgParser()
{
    Node *current = head;
    while (current != nullptr)
    {
        Node *toDelete = current;
        current = current->next;
        delete[] toDelete->key;
        delete[] toDelete->value;
        delete toDelete;
    }
}

void ArgParser::addKeyValuePair(const char *segment)
{
    // Find the location of the equal sign
    const char *equalSign = strchr(segment, '=');
    if (!equalSign)
    {
        // If there's no equal sign, we have a malformed entry; skip it
        return;
    }

    // Allocate and copy the key part
    int keyLen = equalSign - segment;
    char *key = new char[keyLen + 1];
    strncpy(key, segment, keyLen);
    key[keyLen] = '\0'; // Null-terminate the key

    // Allocate and copy the value part
    const char *valueStart = equalSign + 1;
    int valueLen = strlen(valueStart);
    char *value = new char[valueLen + 1];
    strcpy(value, valueStart); // The value is already null-terminated

    // Create a new node
    Node *newNode = new Node{key, value, nullptr};

    // Append to the linked list
    if (!head)
    {
        head = newNode;
    }
    else
    {
        tail->next = newNode;
    }
    tail = newNode;
}

bool ArgParser::getFloat(const char *key, float &outValue)
{
    for (Node *current = head; current != nullptr; current = current->next)
    {
        if (streq(current->key, key))
        {
            float value;
            if (toFloat(current->value, value))
            {
                outValue = value;
                return true; // Success
            }
            break; // Key was found, but conversion failed
        }
    }
    return false; // Key not found or conversion failed
}

bool ArgParser::getInt(const char *key, int &outValue)
{
    for (Node *current = head; current != nullptr; current = current->next)
    {
        if (streq(current->key, key))
        {
            int value;
            if (toInt(current->value, value))
            {
                outValue = value;
                return true; // Success
            }
            break; // Key was found, but conversion failed
        }
    }
    return false; // Key not found or conversion failed
}


bool ArgParser::getString(const char *key, const char **outValue)
{
    for (Node *current = head; current != nullptr; current = current->next)
    {
        if (streq(current->key, key))
        {
            *outValue = current->value;
            return true; // Success
        }
    }
    return false; // Key not found
}

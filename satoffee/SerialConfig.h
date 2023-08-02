#pragma once

struct KeyValue
{
    String key;
    String value;
};

void configOverSerialPort();
void executeConfig();
void executeCommand(String commandName, String commandData);
void removeFile(String path);
void appendToFile(String path, String data);
void readFile(String path);
KeyValue extractKeyValue(String s);
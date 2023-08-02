#include <Arduino.h>
#include "FS.h"
#include "FFat.h"
#include "SerialConfig.h"

void configOverSerialPort()
{
    executeConfig();
}

void executeConfig()
{

    while (true)
    {
        if (Serial.available() == 0)
            continue;
        String data = Serial.readStringUntil('\n');
        Serial.println("received: " + data);
        KeyValue kv = extractKeyValue(data);
        String commandName = kv.key;
        if (commandName == "/config-done")
        {
            Serial.println("/config-done");
            ESP.restart();
        }
        executeCommand(commandName, kv.value);
    }
}

void executeCommand(String commandName, String commandData)
{
    Serial.println("executeCommand: " + commandName + " > " + commandData);
    KeyValue kv = extractKeyValue(commandData);
    String path = kv.key;
    String data = kv.value;

    if (commandName == "/hello")
    {
        // https://patorjk.com/software/taag/#p=display&f=Small%20Slant&t=SATOFFEE%0A
        Serial.println("   _______ __________  ________________");
        Serial.println("  / __/ _ /_  __/ __ \\/ __/ __/ __/ __/");
        Serial.println(" _\\ \\/ __ |/ / / /_/ / _// _// _// _/  ");
        Serial.println("/___/_/ |_/_/  \\____/_/ /_/ /___/___/  ");
        return;
    }
    if (commandName == "/file-remove")
    {
        return removeFile(path);
    }
    if (commandName == "/file-append")
    {
        return appendToFile(path, data);
    }

    if (commandName == "/file-read")
    {
        return readFile(path);
    }

    Serial.println("- Unknown command");
}

void removeFile(String path)
{
    Serial.println("- Remove file: " + path);
    FFat.remove("/" + path);
}

void appendToFile(String path, String data)
{
    Serial.println("- Append to file: " + path);
    File file = FFat.open("/" + path, FILE_APPEND);
    if (!file)
    {
        file = FFat.open("/" + path, FILE_WRITE);
    }
    if (file)
    {
        file.println(data);
        file.close();
    }
}

void readFile(String path)
{
    Serial.println("- Read file: " + path);
    File file = FFat.open("/" + path, "r");
    if (file)
    {
        while (file.available())
        {
            String line = file.readStringUntil('\n');
            Serial.println("/file-read " + line);
        }
        file.close();
    }
    else
    {
        Serial.println("- Failed to open file for reading");
    }
    Serial.println("- Read file done");
}

KeyValue extractKeyValue(String s)
{
    int spacePos = s.indexOf(" ");
    String key = s.substring(0, spacePos);
    if (spacePos == -1)
    {
        return {key, ""};
    }
    String value = s.substring(spacePos + 1, s.length());
    return {key, value};
}
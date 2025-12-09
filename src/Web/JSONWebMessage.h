#pragma once
#include <cstring>
#include <string>
#include "WebMessage.h"

class WebMessageJSON {

public:

    // ======================================================
    //  PARSE JSON → WebMessage   (Zero allocation)
    // ======================================================
    static bool parse(const char* data, size_t len, WebMessage& msg)
    {
        msg = {};
        msg.type = WebMessageType::Command; // default fallback

        const char* p = data;
        const char* end = data + len;

        skipWS(p);
        if (*p != '{') return false;
        p++;

        while (p < end)
        {
            skipWS(p);
            if (*p == '}') break;

            // Parse key
            std::string key;
            if (!parseJSONString(p, key)) return false;

            skipWS(p);
            if (*p != ':') return false;
            p++;

            skipWS(p);

            // -------------------------
            // type
            // -------------------------
            if (key == "type")
            {
                std::string typeStr;
                if (!parseJSONString(p, typeStr)) return false;

                if (typeStr == "set")
                    msg.type = WebMessageType::SetParameter;
                else if (typeStr == "get")
                    msg.type = WebMessageType::GetParameter;
                else if (typeStr == "cmd")
                    msg.type = WebMessageType::Command;
                else if (typeStr == "param") 
                    msg.type = WebMessageType::Param;
                else
                    return false;
            }

            // -------------------------
            // ID
            // -------------------------
            else if (key == "id")
            {
                if (!parseJSONString(p, msg.paramID))
                    return false;
            }

            // -------------------------
            // VALUE
            // -------------------------
            else if (key == "value")
            {
                if (!parseJSONFloat(p, msg.value))
                    return false;
            }

            // -------------------------
            // COMMAND
            // -------------------------
            else if (key == "command")
            {
                if (!parseJSONString(p, msg.command))
                    return false;
            }

            // -------------------------
            // ARGS  (raw JSON string)
            // -------------------------
            else if (key == "args")
            {
                if (!parseJSONString(p, msg.args))
                    return false;
            }

            // Skip to next
            skipWS(p);
            if (*p == ',') p++;
            else if (*p == '}') break;
        }

        return true;
    }


    // ======================================================
    //  SERIALIZE WebMessage → JSON
    // ======================================================
    static std::string serialize(const WebMessage& msg)
    {
        std::string json = "{";

        // type
        json += "\"type\":\"";
        switch (msg.type)
        {
            case WebMessageType::SetParameter:  json += "set"; break;
            case WebMessageType::GetParameter:  json += "get"; break;
            case WebMessageType::Command:       json += "cmd"; break;
            case WebMessageType::Param:         json += "param"; break;
        }
        json += "\"";

        // parameter ID
        if (!msg.paramID.empty())
        {
            json += ",\"id\":\"";
            json += msg.paramID;
            json += "\"";
        }

        // numeric value
        if (msg.type == WebMessageType::SetParameter)
        {
            json += ",\"value\":";
            json += std::to_string(msg.value);
        }

        // command name
        if (msg.type == WebMessageType::Command && !msg.command.empty())
        {
            json += ",\"command\":\"";
            json += msg.command;
            json += "\"";
        }

        // args (raw JSON string)
        if (!msg.args.empty())
        {
            json += ",\"args\":\"";
            json += msg.args;
            json += "\"";
        }

        json += "}";
        return json;
    }


private:

    // ======================================================
    //  Internal helpers (also zero-alloc)
    // ======================================================

    static inline void skipWS(const char*& p)
    {
        while (*p == ' ' || *p == '\n' || *p == '\r' || *p == '\t')
            p++;
    }

    static bool parseJSONString(const char*& p, std::string& out)
    {
        skipWS(p);
        if (*p != '"') return false;
        p++; // skip "

        const char* start = p;

        while (*p && *p != '"')
            p++;

        if (*p != '"') return false;

        out.assign(start, p - start);
        p++; // skip "
        return true;
    }

    static bool parseJSONFloat(const char*& p, float& out)
    {
        skipWS(p);
        char* endPtr;
        out = strtof(p, &endPtr);
        if (p == endPtr) return false;
        p = endPtr;
        return true;
    }
};

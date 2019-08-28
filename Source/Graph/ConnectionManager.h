#pragma once


#ifndef CONNECTIONMANAGER_H_INCLUDED
#define CONNECTIONMANAGER_H_INCLUDED

#include "Connection.h"

namespace APG {

// TODO experimental connection manager alternative
template <typename Type>
class ConnectionManager
{
public:
    ConnectionManager () :
        width (20),
        data (new Type[width])
    {
    }

    ~ConnectionManager ()
    {
        if (data != nullptr)
        {
            delete[] data;
            data = nullptr;
        }
    }

    void addConnection (Connection newConnection)
    {

    }

    size_t index (int x, int y) const { return x + width * y; }

private:
    Type* data {nullptr};
    size_t width {0};
};

}

#endif

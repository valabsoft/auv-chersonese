#ifndef SEVROVDATA_H
#define SEVROVDATA_H


#include <QByteArray>
#include <QString>

class SevROVData
{
public:
    SevROVData();

    std::string toIntString(int8_t value);
    std::string toFloatString(float value, int precision = 2);

    virtual void Initialize() = 0;
    virtual QByteArray toByteArray() = 0;
    virtual void printDebugInfo() = 0;
};

#endif // SEVROVDATA_H

#ifndef RTags_h
#define RTags_h

#include <string>
#include <stdlib.h>
#include <QtCore>
#include <leveldb/db.h>
#include "Path.h"
#include <clang-c/Index.h>

class CursorKey;
namespace RTags {
bool parseLocation(const std::string &string,
                   std::string &file, unsigned &line, unsigned &col);
Path findRtagsDb();
class LevelDBScope
{
public:
    LevelDBScope(leveldb::DB *d)
        : db(d)
    {}
    ~LevelDBScope()
    {
        delete db;
    }
private:
    leveldb::DB *db;
};

QByteArray kindToString(CXCursorKind kind);
const char *completionChunkKindToString(int kind);
bool locationFromString(const QByteArray &string, Path *path = 0, int *line = 0, int *column = 0);
static inline QByteArray eatString(CXString string)
{
    const QByteArray ret = clang_getCString(string);
    clang_disposeString(string);
    return ret;
}

static inline bool isValidCursor(CXCursor cursor)
{
    CXCursorKind kind = clang_getCursorKind(cursor);
    if (!clang_isInvalid(kind)) {
        CXSourceLocation loc = clang_getCursorLocation(cursor);
        CXFile file;
        unsigned int line, col, off;
        clang_getInstantiationLocation(loc, &file, &line, &col, &off);
        CXString filename = clang_getFileName(file);
        const char* str = clang_getCString(filename);
        if (!str || !strcmp(str, "")) {
            clang_disposeString(filename);
            return false;
        }
        clang_disposeString(filename);
        return true;
    }
    return false;
}


static inline void removeWhitespace(QByteArray &ba)
{
    int size = ba.size();
    int i = 0;
    while (i < size) {
        if (ba.at(i) == ' ') {
            ba.remove(i, 1);
            --size;
        } else {
            ++i;
        }
    }
}

bool cursorDefinitionFor(const CursorKey& d, const CursorKey &c);
QDebug operator<<(QDebug dbg, CXCursor cursor);
}

#endif

#include "icfile.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

ICFile::ICFile(const QString& filename):
    fileName_(filename)
{
}

bool ICFile::ICWrite(const QByteArray &toWrite)
{
#ifndef Q_WS_WIN32
    int fd = open(QString(fileName_ + ".bak").toUtf8(),
                  O_WRONLY | O_CREAT | O_TRUNC,
                  S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH);
    if(fd < 0) return false;

    int ret = 0;
    int writeSize = toWrite.size();
    while( writeSize > 0)
    {
        ret = write(fd, toWrite.constData() + ret, writeSize);
        writeSize -= ret;
    }
    if(ret < 0)
    {
        close(fd);
        return false;
    }
    fsync(fd);
    close(fd);
    rename(QString(fileName_ + ".bak").toUtf8(), fileName_.toUtf8());
#else
    FILE *pfile=fopen(QString(fileName_).toUtf8(),"w");
    int ret = 0;
    int writeSize = toWrite.size();
    while( writeSize > 0)
    {
        ret = fwrite(toWrite.constData() + ret, 1,writeSize,pfile);
        writeSize -= ret;
    }
    if(ret < 0)
    {
        fclose(pfile);
        return false;
    }
    fclose(pfile);
    //rename(QString(fileName_ + ".bak").toUtf8(), fileName_.toUtf8());

#endif
    return true;
}

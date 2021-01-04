//
// Created by GOD on 2017/4/20.
//

#include <logUtils.h>

void LogString(int pro, const char *szFormat, ...)
{
    __android_log_print(pro, "JuBiter", "%s", szFormat);
}

// log.h 中说明单次最大日志是1023 个字符
void LogBinary(const char* msg, const unsigned char *pBuf, unsigned long ulSize)
{
    LogString(ANDROID_LOG_INFO, msg);

    // Show 16 bytes each line.
    char ss[4096] = {0};

    unsigned long ulLines = ulSize / 16;
    unsigned long ulCharInLastLine = 0;
    if(0 != ulSize % 16)
    {
        ulCharInLastLine = ulSize - ulLines * 16;
    }

    unsigned long i = 0;
    unsigned long j = 0;
    for(i = 0; i < ulLines; ++i)
    {
        memset(ss, 0, sizeof(ss));
        sprintf(&ss[strlen(ss)], "    ");

        // 显示16进制
        for(j = 0; j < 16; ++j)
        {
            sprintf(&ss[strlen(ss)], "%02X ", (unsigned short) pBuf[16 * i + j]);

            if(7 == j)
            {
                sprintf(&ss[strlen(ss)], "- ");
            }
        }

        sprintf(&ss[strlen(ss)], "    ");

        // 显示字符
        for(j = 0; j < 16; ++j)
        {
            if(isprint(pBuf[16 * i + j]))
            {
                sprintf(&ss[strlen(ss)], "%c", (char) pBuf[16 * i + j]);
            }
            else
            {
                sprintf(&ss[strlen(ss)], ".");
            }
        }
        LogString(ANDROID_LOG_INFO, ss);
    }

    if(0 != ulCharInLastLine)
    {
        memset(ss, 0, sizeof(ss));
        sprintf(ss, "    ");

        for(j = 0; j < ulCharInLastLine; ++j)
        {
            sprintf(&ss[strlen(ss)], "%02X ", (unsigned short) pBuf[16 * i + j]);

            if(j == 7 && ulCharInLastLine > 8)
            {
                sprintf(&ss[strlen(ss)], "- ");
            }
        }

        unsigned long k = 0;

        k += ((16 - ulCharInLastLine) * 3);

        if(ulCharInLastLine <= 8)
        {
            k += 2;
        }

        for(unsigned long kk = 0; kk < k; ++kk)
        {
            sprintf(&ss[strlen(ss)], " ");
        }

        sprintf(&ss[strlen(ss)], "    ");

        for(j = 0; j < ulCharInLastLine; ++j)
        {
            if(isprint(pBuf[16 * i + j]))
            {
                sprintf(&ss[strlen(ss)], "%c", (char) pBuf[16 * i + j]);
            }
            else
            {
                sprintf(&ss[strlen(ss)], ".");
            }
        }

        LogString(ANDROID_LOG_INFO, ss);
    }
}

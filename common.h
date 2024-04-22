#ifndef COMMON_H
#define COMMON_H
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <stack>
#include <string>
#include <map>
#include <vector>
#include <chrono>
#include <stack>

#ifdef INCLUDE_QT
#include <QDebug>
#include <QTime>
#include <QElapsedTimer>
#include <qfile.h>
#include <qtextstream.h>

#endif

#ifdef _MSC_VER
#define STR(R) #R
#define STRVALUE(R) STR(R)
#define TDEBUG(...)            qDebug(__VA_ARGS__)
#endif

#ifdef _MSC_VER
#define STRTOK(aptr,delm,savP) strtok_s(aptr,delm,&savP);
#else
#define STRTOK(aptr,delm,savP) strtok_r(aptr,delm,&savP);
#endif


#endif // COMMON_H

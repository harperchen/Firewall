#include "qlinkget.h"

QLinkGet::QLinkGet()
{

}

void QLinkGet::run()
{
    while(true){
        QString link = chrDev->copyLinks();
        emit newLinks(link);
        sleep(1);
    }
}

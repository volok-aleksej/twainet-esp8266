#include "utils.h"

twnstd::vector<String> getSubstrings(const String& data, const String& delimit)
{
    twnstd::vector<String> params;
    int startindex = 0;
    while(data.length()) {
        int index = data.indexOf(delimit, startindex);
        if(index == -1) {
            params.push_back(data.substring(startindex));
            break;
        } else {
            params.push_back(data.substring(startindex, index));
            startindex = index + 1;
        }
    }
    return params;
}
#ifndef AUTO_COMPLETE_HELPER_H
#define AUTO_COMPLETE_HELPER_H

#include <WString.h>
#include "vector.h"

class AutoCompleteHelper
{
public:
    twnstd::vector<String> autoCompleteHelper(const String& word, const twnstd::vector<String>& words)
    {
        twnstd::vector<String> usewords;
        for(int i = 0; i < words.length(); i++) {
            if(word.length() <= const_cast<twnstd::vector<String>&>(words)[i].length() &&
                memcmp(word.c_str(), const_cast<twnstd::vector<String>&>(words)[i].c_str(), word.length()) == 0) {
                usewords.push_back(const_cast<twnstd::vector<String>&>(words)[i]);
            }
        }

        return usewords;
    }
};

#endif/*AUTO_COMPLETE_HELPER_H*/

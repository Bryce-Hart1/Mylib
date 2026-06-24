#include <cstddef>
#include <cstdint>
#include <vector>
#include <array>
#include <exception>
#include <string>
#include <string_view>
#include <algorithm>
#include <cctype>
#include <map>
#include <optional>
#pragma once

namespace bstd{

/**
* Autocorrect V1 Bryce Hart
* @attention a autocorrect filter that can be applied to any string.
* This is meant to be a compromise between symspell to allow for smaller 
* dictionary sizes, while also being highly accurate. 
* 
*/
class autoCorrectFilter{
    using u8 = uint8_t;
    using usize = std::size_t; 
    
    private:
    //vars
    std::size_t _sensitivity;
    std::map<std::size_t, std::vector<std::string_view>> _dictionary;//holds size of string (key) to vector of strings

        // Bryce Hart  last changed: 6/24/2026 by: Claude
        // Returns the QWERTY key-distance between two letters via the lookup grid.
        // (Fixed: the a/b bounds checks were nested with mismatched braces, which
        // left a control path with no return; un-nested them and removed an unused
        // local so every path either throws on bad input or returns a distance.)
        u8 findLetterDistance(char in, char d_in) const{

            constexpr u8 ascii_a = 97;
            const int a = std::tolower(static_cast<unsigned char>(in)) - ascii_a;
            const int b = std::tolower(static_cast<unsigned char>(d_in)) - ascii_a;

            constexpr std::array<std::array<u8, 26>, 26> lookupGrid{{
            //      a b c d e f g h i j k l m n o p q r s t u v w x y z
            /* a */{0,5,3,2,2,3,4,5,7,6,7,8,7,6,8,9,1,3,1,4,6,4,1,2,5,1},
            /* b */{5,0,2,3,4,2,1,1,3,2,3,4,2,1,4,5,5,3,4,3,3,1,4,3,2,4},
            /* c */{3,2,0,1,3,1,2,3,5,4,5,6,4,3,6,7,4,2,2,3,4,1,3,1,3,2},
            /* d */{2,3,1,0,1,1,2,3,5,4,5,6,5,4,6,7,3,1,1,2,4,2,2,1,3,2},
            /* e */{2,4,3,1,0,2,3,3,5,4,5,6,5,4,6,7,2,1,1,2,4,3,1,2,3,3},
            /* f */{3,2,1,1,2,0,1,2,4,3,4,5,4,3,5,6,3,1,2,1,3,1,3,2,2,3},
            /* g */{4,1,2,2,3,1,0,1,3,2,3,4,3,2,4,5,4,2,3,1,2,1,3,3,1,4},
            /* h */{5,1,3,3,3,2,1,0,2,1,2,3,2,1,3,4,5,3,4,2,1,2,4,4,1,5},
            /* i */{7,3,5,5,5,4,3,2,0,1,1,2,2,3,1,2,7,4,6,3,1,4,6,6,2,7},
            /* j */{6,2,4,4,4,3,2,1,1,0,1,2,1,1,2,3,6,3,5,3,1,3,5,5,2,6},
            /* k */{7,3,5,5,5,4,3,2,1,1,0,1,1,2,1,2,7,4,6,3,2,4,6,6,3,7},
            /* l */{8,4,6,6,6,5,4,3,2,2,1,0,2,3,1,1,8,5,7,4,3,5,7,7,3,8},
            /* m */{7,2,4,5,5,4,3,2,2,1,1,2,0,1,3,3,7,4,6,4,3,3,6,5,3,6},
            /* n */{6,1,3,4,4,3,2,1,3,1,2,3,1,0,3,4,6,4,5,3,2,2,5,4,3,5},
            /* o */{8,4,6,6,6,5,4,3,1,2,1,1,3,3,0,1,8,5,7,4,2,5,7,7,3,8},
            /* p */{9,5,7,7,7,6,5,4,2,3,2,1,3,4,1,0,9,6,8,5,3,6,8,8,4,9},
            /* q */{1,5,4,3,2,3,4,5,7,6,7,8,7,6,8,9,0,3,2,4,6,4,1,3,5,3},
            /* r */{3,3,2,1,1,1,2,3,4,3,4,5,4,4,5,6,3,0,2,1,3,3,2,3,2,3},
            /* s */{1,4,2,1,1,2,3,4,6,5,6,7,6,5,7,8,2,2,0,3,5,3,1,1,4,1},
            /* t */{4,3,3,2,2,1,1,2,3,3,3,4,4,3,4,5,4,1,3,0,2,2,3,3,1,4},
            /* u */{6,3,4,4,4,3,2,1,1,1,2,3,3,2,2,3,6,3,5,2,0,3,5,5,1,6},
            /* v */{4,1,1,2,3,1,1,2,4,3,4,5,3,2,5,6,4,3,3,2,3,0,4,2,3,3},
            /* w */{1,4,3,2,1,3,3,4,6,5,6,7,6,5,7,8,1,2,1,3,5,4,0,3,4,2},
            /* x */{2,3,1,1,2,2,3,4,6,5,6,7,5,4,7,8,3,3,1,3,5,2,3,0,4,1},
            /* y */{5,2,3,3,3,2,1,1,2,2,3,3,3,3,3,4,5,2,4,1,1,3,4,4,0,5},
            /* z */{1,4,2,2,3,3,4,5,7,6,7,8,6,5,8,9,3,3,1,4,6,3,2,1,5,0},
            }};

            if(a < 0 || a > 25)[[unlikely]]{
                throw std::runtime_error("index " + std::to_string(a) + " of lookup grid is out of bounds");
            }
            if(b < 0 || b > 25)[[unlikely]]{
                throw std::runtime_error("index " + std::to_string(b) + " of lookup grid is out of bounds");
            }
            return lookupGrid[a][b];
        }
        

        //helper for main fix function, finds distance for every word
        // Bryce Hart  last changed: 6/24/2026 by: Claude
        // Sums the positional QWERTY key-distance between the input and a candidate
        // word. (Fixed: the old loop walked `input` while indexing `wordToCompare`
        // by the same counter, reading out of bounds whenever the two differed in
        // length. Now we only compare the overlapping prefix and charge a flat
        // penalty of one per extra/missing letter — the positional metric can't
        // realign shifted characters, so a length gap is treated as that many edits.)
        std::optional<usize> fixHelper(std::string_view input, std::string_view wordToCompare) const {

        //take each letter and preform operation
        const usize overlap = std::min(input.size(), wordToCompare.size());
        const usize lengthGap = (input.size() > wordToCompare.size())
                                  ? input.size() - wordToCompare.size()
                                  : wordToCompare.size() - input.size();

        usize totalDistance = lengthGap; //one unit per extra/missing letter
        for(usize ind = 0; ind < overlap; ind++){
            totalDistance += findLetterDistance(input[ind], wordToCompare[ind]);
        }
        //if totaldistance is greater then sensitivity, dont return, we dont even want to consider
        if(totalDistance > _sensitivity){
            return std::nullopt;
        }
        return totalDistance;
        }


    public:
        inline autoCorrectFilter(usize sensitivity, std::vector<std::string_view> dictionary){
            // Bryce Hart  last changed: 6/24/2026 by: Claude
            // Store the sensitivity. (Fixed: the ctor parameter was never assigned to
            // _sensitivity, leaving it uninitialized. It went unnoticed while the
            // candidate pool was always empty, but once fix() builds a real pool the
            // garbage value blows up maxFind and spins the search loop forever.)
            _sensitivity = sensitivity;
            //put all of them into the buckets of length
            for (std::string_view str : dictionary){
                _dictionary[str.length()].push_back(str);
            }
}

    //attempts to fix a word for the sensitivity provided
    std::string fix(std::string_view str) const{
        //init min range, max range
        const usize strLen = str.length();

        // Bryce Hart  last changed: 6/24/2026 by: Claude
        // Length window to search: words within +/- sensitivity of the input
        // length. (Fixed: `strLen - _sensitivity < 0` can never be true for an
        // unsigned type and would underflow, and both branches declared a throwaway
        // local `minFind` that went out of scope unused. Guard the lower bound
        // against underflow and keep a single usable minFind.)
        const usize minFind = (strLen > _sensitivity) ? (strLen - _sensitivity) : 0;
        const usize maxFind = (strLen + _sensitivity);

        // Bryce Hart  last changed: 6/24/2026 by: Claude
        // Build the candidate pool. (Fixed: the original loop body was empty, so
        // the pool was always empty and fix() returned the input unchanged. The
        // dictionary is bucketed by word length, so pull every bucket whose length
        // falls inside the window instead of scanning the whole dictionary.)
        std::vector<std::string_view> pool;
        for(usize len = minFind; len <= maxFind; len++){
            const auto bucket = _dictionary.find(len);
            if(bucket != _dictionary.end()){
                pool.insert(pool.end(), bucket->second.begin(), bucket->second.end());
            }
        }
        std::string bestMatch{str};
        usize bestMatchDistance = SIZE_MAX; //if its the og word it'll get discarded anyway
        for(std::string_view word : pool){
            std::optional<usize> count = fixHelper(str, word);
            if(count && *count < bestMatchDistance){
                bestMatch = word;
                bestMatchDistance = *count;
            }
        }
        return bestMatch;
    }




};





} //bstd
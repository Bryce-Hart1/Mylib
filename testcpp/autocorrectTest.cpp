#include "../cppstd/autoCorrect.hpp" 



// Accuracy harness for bstd::autoCorrectFilter.
//
// Idea: a correction is "right" iff fix(misspelling) == intended word.
// Cases are grouped by ERROR TYPE so the report shows not just *how* accurate
// the filter is, but *which kinds of typos* it can and cannot handle.
//
// Build: g++ -std=c++17 -O2 test_accuracy.cpp -o test_accuracy
#include "autocorrect.hpp"
#include <iostream>
#include <iomanip>
#include <string>
#include <string_view>
#include <vector>
#include <map>
#include <sstream>

// ----- Dictionary -------------------------------------------------------
// Backed by string literals (static storage), so the string_views the filter
// stores never dangle. This sidesteps the lifetime trap for the test only.
static const std::vector<std::string_view> DICT = {
    "the","be","to","of","and","a","in","that","have","it","for","not","on",
    "with","he","as","you","do","at","this","but","his","by","from","they",
    "we","say","her","she","or","an","will","my","one","all","would","there",
    "their","what","so","up","out","if","about","who","get","which","go","me",
    "when","make","can","like","time","no","just","him","know","take","people",
    "into","year","your","good","some","could","them","see","other","than",
    "then","now","look","only","come","its","over","think","also","back",
    "after","use","two","how","our","work","first","well","way","even","new",
    "want","because","any","these","give","day","most","us","hello","world",
    "where","much","through","before","great","little","world","place","right",
    "small","large","every","found","under","never","while","might","close",
    "house","water","words","write","number","sound","field","power","light",
};

// ----- Test cases -------------------------------------------------------
enum class Cat { Correct, Substitution, Transposition, Insertion, Deletion };

const char* catName(Cat c) {
    switch (c) {
        case Cat::Correct:       return "Correct (no error)";
        case Cat::Substitution:  return "Substitution (adj key)";
        case Cat::Transposition: return "Transposition";
        case Cat::Insertion:     return "Insertion (extra char)";
        case Cat::Deletion:      return "Deletion (missing char)";
    }
    return "?";
}

struct Case { std::string input; std::string expected; Cat cat; };

static const std::vector<Case> CASES = {
    // --- correctly spelled words: must be returned unchanged ---
    {"hello","hello",Cat::Correct}, {"world","world",Cat::Correct},
    {"people","people",Cat::Correct}, {"about","about",Cat::Correct},
    {"there","there",Cat::Correct}, {"their","their",Cat::Correct},
    {"because","because",Cat::Correct}, {"which","which",Cat::Correct},
    {"would","would",Cat::Correct}, {"first","first",Cat::Correct},

    // --- substitution: one letter swapped for a QWERTY neighbor, same length ---
    {"hellp","hello",Cat::Substitution},     // o->p
    {"worle","world",Cat::Substitution},     // d->e
    {"abiut","about",Cat::Substitution},     // o->i
    {"rhere","there",Cat::Substitution},     // t->r
    {"rheir","their",Cat::Substitution},     // t->r
    {"becausr","because",Cat::Substitution}, // e->r
    {"qhich","which",Cat::Substitution},     // w->q
    {"wpuld","would",Cat::Substitution},     // o->p
    {"peopke","people",Cat::Substitution},   // l->k
    {"firat","first",Cat::Substitution},     // s->a

    // --- transposition: two adjacent letters swapped, same length ---
    {"hlelo","hello",Cat::Transposition},
    {"wrold","world",Cat::Transposition},
    {"abuot","about",Cat::Transposition},
    {"thier","their",Cat::Transposition},
    {"frist","first",Cat::Transposition},

    // --- insertion: one extra letter (length + 1) ---
    {"helllo","hello",Cat::Insertion},
    {"worrld","world",Cat::Insertion},
    {"abouut","about",Cat::Insertion},
    {"theere","there",Cat::Insertion},
    {"peopple","people",Cat::Insertion},

    // --- deletion: one missing letter (length - 1) ---
    {"helo","hello",Cat::Deletion},
    {"wrld","world",Cat::Deletion},
    {"abut","about",Cat::Deletion},
    {"ther","there",Cat::Deletion},
    {"peple","people",Cat::Deletion},
};

int main() {
    const std::vector<std::size_t> sensitivities = {1,2,3,4,5,6,8,10,15};

    // ---- 1. Accuracy sweep: category x sensitivity ----
    std::cout << "ACCURACY BY CATEGORY AND SENSITIVITY  (cell = correct / total)\n";
    std::cout << std::string(96, '-') << "\n";
    std::cout << std::left << std::setw(26) << "category";
    for (auto s : sensitivities) std::cout << std::right << std::setw(7) << ("s=" + std::to_string(s));
    std::cout << "\n" << std::string(96, '-') << "\n";

    std::vector<Cat> cats = {Cat::Correct, Cat::Substitution, Cat::Transposition,
                             Cat::Insertion, Cat::Deletion};

    // remember best overall sensitivity as we go
    std::size_t bestS = sensitivities.front();
    double bestOverall = -1.0;

    // pre-count totals per category
    std::map<Cat,int> totalPerCat;
    for (const auto& c : CASES) totalPerCat[c.cat]++;

    for (Cat cat : cats) {
        std::cout << std::left << std::setw(26) << catName(cat);
        for (auto s : sensitivities) {
            bstd::autoCorrectFilter filter(s, DICT);
            int correct = 0;
            for (const auto& c : CASES) {
                if (c.cat != cat) continue;
                if (filter.fix(c.input) == c.expected) correct++;
            }
            std::cout << std::right << std::setw(7)
                      << (std::to_string(correct) + "/" + std::to_string(totalPerCat[cat]));
        }
        std::cout << "\n";
    }

    std::cout << std::string(96, '-') << "\n";
    std::cout << std::left << std::setw(26) << "OVERALL accuracy";
    for (auto s : sensitivities) {
        bstd::autoCorrectFilter filter(s, DICT);
        int correct = 0;
        for (const auto& c : CASES)
            if (filter.fix(c.input) == c.expected) correct++;
        double pct = 100.0 * correct / CASES.size();
        if (pct > bestOverall) { bestOverall = pct; bestS = s; }
        std::ostringstream cell; cell << std::fixed << std::setprecision(0) << pct << "%";
        std::cout << std::right << std::setw(7) << cell.str();
    }
    std::cout << "\n\n";

    // ---- 2. Per-case detail at the best sensitivity ----
    std::cout << "PER-CASE RESULTS at best sensitivity (s=" << bestS
              << ", overall " << std::fixed << std::setprecision(0) << bestOverall << "%)\n";
    std::cout << std::string(96, '-') << "\n";
    std::cout << std::left << std::setw(26) << "category"
              << std::setw(12) << "input" << std::setw(12) << "expected"
              << std::setw(12) << "got" << "result\n";
    std::cout << std::string(96, '-') << "\n";

    bstd::autoCorrectFilter filter(bestS, DICT);
    int pass = 0;
    for (const auto& c : CASES) {
        std::string got = filter.fix(c.input);
        bool ok = (got == c.expected);
        pass += ok;
        std::cout << std::left << std::setw(26) << catName(c.cat)
                  << std::setw(12) << c.input << std::setw(12) << c.expected
                  << std::setw(12) << got << (ok ? "PASS" : "FAIL") << "\n";
    }
    std::cout << std::string(96, '-') << "\n";
    std::cout << pass << "/" << CASES.size() << " cases corrected at s=" << bestS << "\n\n";

    // ---- 3. Regression gate ----
    // The positional metric only supports equal-length comparisons, so it can
    // never fix insertion/deletion errors. We therefore gate ONLY the categories
    // the design is meant to handle. This fails on a real regression (e.g. the
    // ind++ bug coming back) without complaining about known limitations.
    constexpr std::size_t REF_S = 4;            // reference sensitivity for the gate
    struct Gate { Cat cat; double minPct; };
    const std::vector<Gate> gates = {
        {Cat::Correct,      100.0},  // correctly-spelled words must survive untouched
        {Cat::Substitution, 100.0},  // adjacent-key typos are the design's core job
    };

    std::cout << "REGRESSION GATE (s=" << REF_S << ")\n";
    std::cout << std::string(96, '-') << "\n";
    bstd::autoCorrectFilter gateFilter(REF_S, DICT);
    bool gatePassed = true;
    for (const auto& g : gates) {
        int correct = 0, total = 0;
        for (const auto& c : CASES) {
            if (c.cat != g.cat) continue;
            ++total;
            if (gateFilter.fix(c.input) == c.expected) ++correct;
        }
        double pct = 100.0 * correct / total;
        bool ok = pct + 1e-9 >= g.minPct;
        gatePassed &= ok;
        std::cout << std::left << std::setw(26) << catName(g.cat)
                  << "required >= " << std::setw(5) << (std::to_string((int)g.minPct) + "%")
                  << " got " << std::setw(5) << (std::to_string((int)pct) + "%")
                  << (ok ? "  PASS" : "  *** FAIL ***") << "\n";
    }
    std::cout << std::string(96, '-') << "\n";
    std::cout << "GATE " << (gatePassed ? "PASSED" : "FAILED") << "\n";

    return gatePassed ? 0 : 1;
}
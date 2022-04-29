#include "Parser.h"
#include <getopt.h>

int main(int argc, char **argv) {

    //possible command-line OPTIONS
    struct option search_engine_options[] = {
            {"stats", no_argument, nullptr, 's'},
            {"query", required_argument, nullptr, 'q'}
    };
    if (argc == 1) {
        return 1;
    }
    Parser parser;
    std::string search_term = argv[1];
    Porter2Stemmer::trim(search_term);
    Porter2Stemmer::stem(search_term);
    parser.parse(argv[2]);
    std::vector<AvlTree<std::string, Article *>> pairs = parser.build_AVL_trees();
    for (auto &pair: pairs) {
        auto n = pair.search(search_term);
        if (n == nullptr) continue;
        for (auto &i: *n)
            std::cout << i->title << '\n';
    }
//    std::vector<Article *> matches = parser.search_article_trees(search_term, pairs);
//    std::cout << matches.size();
//    for (Article *article: matches) std::cout << article->id << '\n';
    return 0;
}

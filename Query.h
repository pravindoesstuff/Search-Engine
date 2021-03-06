/**
 * @Author(s):      Pravin and Kassi
 * @filename:       Query.h
 * @date:           04-06-2022
 * @description:    Implementation of our boolean query processor
 */

#ifndef INC_22S_FINAL_PROJ_QUERY_H
#define INC_22S_FINAL_PROJ_QUERY_H

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <set>
#include "Article.h"
#include "AvlTree.h"
#include "porter2_stemmer.h"
#include "HashMap.h"
#include <unordered_set>
#include <chrono>

class Query {

private:
    std::vector<std::string> and_keywords;
    std::vector<std::string> or_keywords;
    std::vector<std::string> not_words;
    std::string organization;
    std::string person;

    double query_processing_time = 0;
public:
    explicit Query(const std::string &query);

    std::set<Article *> get_elements(const AvlTree<std::string, Article *> &article_tree,
                                     const HashMap <std::string, std::set<Article *>> &person_map,
                                     const HashMap <std::string, std::set<Article *>> &orgs_map);

    double get_query_processing_time() { return query_processing_time; }

    int frequency(const std::vector<std::string> &tokens);
};

struct ArticlePair {
    Article *article;
    int weight;
    bool operator<(const ArticlePair &pair) {
        return weight < pair.weight;
    }
};


#endif //INC_22S_FINAL_PROJ_QUERY_H

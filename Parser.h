/**
 * @Author(s):      Pravin and Kassi
 * @filename:       Parser.h
 * @date:           04-05-2022
 * @description:    This class is the parser of the project
 *                  It is responsible for:
 *                      - reading and processing JSON files asynchronously
 *                      - Storing processed JSON file (Article) into std::deque
 */

#ifndef INC_22S_FINAL_PROJ_PARSER_H
#define INC_22S_FINAL_PROJ_PARSER_H

#include <filesystem>
#include <fstream>
#include <iostream>
#include <thread>

#include <future>
#include <vector>
#include <exception>
#include <string>
#include <sstream>
#include <algorithm>
#include <unordered_set>
#include <set>
#include "HashMap.h"

#include "Article.h"
#include "rapidjson/document.h"
#include "thread_pool.h"
#include "porter2_stemmer.h"
#include "AvlTree.h"

//stop word list borrowed from: https://www.webconfs.com/stop-words.php
static std::unordered_set<std::string> stop_words = {
        "a", "able", "about", "above", "abroad", "according", "accordingly",
        "across", "actually",
        "adj", "after", "afterwards", "again", "against", "ago", "ahead",
        "ain't", "all", "allow",
        "allows", "almost", "alone", "along", "alongside", "already",
        "also", "although", "always",
        "am", "amid", "amidst", "among", "amongst", "an", "and", "another",
        "any", "anybody", "anyhow",
        "anyone", "anything", "anyway", "anyways", "anywhere", "apart",
        "appear", "appreciate",
        "appropriate", "are", "aren't", "around", "as", "a's", "aside",
        "ask", "asking", "associated",
        "at", "available", "away", "awfully", "back", "backward",
        "backwards", "be", "became",
        "because", "become", "becomes", "becoming", "been", "before",
        "beforehand", "begin", "behind",
        "being", "believe", "below", "beside", "besides", "best", "better",
        "between", "beyond", "both",
        "brief", "but", "by", "came", "can", "cannot", "cant", "can't",
        "caption", "cause", "causes",
        "certain", "certainly", "changes", "clearly", "c'mon", "co", "co.",
        "com", "come", "comes",
        "concerning", "consequently", "consider", "considering", "contain",
        "containing", "contains",
        "corresponding", "could", "couldn't", "course", "c's", "currently",
        "dare", "daren't",
        "definitely", "described", "despite", "did", "didn't", "different",
        "directly", "do", "does",
        "doesn't", "doing", "done", "don't", "down", "downwards", "during",
        "each", "edu", "eg",
        "eight", "eighty", "either", "else", "elsewhere", "end", "ending",
        "enough", "entirely",
        "especially", "et", "etc", "even", "ever", "evermore", "every",
        "everybody", "everyone",
        "everything", "everywhere", "ex", "exactly", "example", "except",
        "fairly", "far", "farther",
        "few", "fewer", "fifth", "first", "five", "followed", "following",
        "follows", "for", "forever",
        "former", "formerly", "forth", "forward", "found", "four", "from",
        "further", "furthermore",
        "get", "gets", "getting", "given", "gives", "go", "goes", "going",
        "gone", "got", "gotten",
        "greetings", "had", "hadn't", "half", "happens", "hardly", "has",
        "hasn't", "have", "haven't",
        "having", "he", "he'd", "he'll", "hello", "help", "hence", "her",
        "here", "hereafter", "hereby",
        "herein", "here's", "hereupon", "hers", "herself", "he's", "hi",
        "him", "himself", "his",
        "hither", "hopefully", "how", "howbeit", "however", "hundred",
        "i'd", "ie", "if", "ignored",
        "i'll", "i'm", "immediate", "in", "inasmuch", "inc", "inc.",
        "indeed", "indicate", "indicated",
        "indicates", "inner", "inside", "insofar", "instead", "into",
        "inward", "is", "isn't", "it",
        "it'd", "it'll", "its", "it's", "itself", "i've", "just", "k",
        "keep", "keeps", "kept", "know",
        "known", "knows", "last", "lately", "later", "latter", "latterly",
        "least", "less", "lest",
        "let", "let's", "like", "liked", "likely", "likewise", "little",
        "look", "looking", "looks",
        "low", "lower", "ltd", "made", "mainly", "make", "makes", "many",
        "may", "maybe", "mayn't",
        "me", "mean", "meantime", "meanwhile", "merely", "might",
        "mightn't", "mine", "minus", "miss",
        "more", "moreover", "most", "mostly", "mr", "mrs", "much", "must",
        "mustn't", "my", "myself",
        "name", "namely", "nd", "near", "nearly", "necessary", "need",
        "needn't", "needs", "neither",
        "never", "neverf", "neverless", "nevertheless", "new", "next",
        "nine", "ninety", "no", "nobody",
        "non", "none", "nonetheless", "noone", "no-one", "nor", "normally",
        "not", "nothing",
        "notwithstanding", "novel", "now", "nowhere", "obviously", "of",
        "off", "often", "oh", "ok",
        "okay", "old", "on", "once", "one", "ones", "one's", "only",
        "onto", "opposite", "or", "other",
        "others", "otherwise", "ought", "oughtn't", "our", "ours",
        "ourselves", "out", "outside",
        "over", "overall", "own", "particular", "particularly", "past",
        "per", "perhaps", "placed",
        "please", "plus", "possible", "presumably", "probably", "provided",
        "provides", "que", "quite",
        "qv", "rather", "rd", "re", "really", "reasonably", "recent",
        "recently", "regarding",
        "regardless", "regards", "relatively", "respectively", "right",
        "round", "said", "same", "saw",
        "say", "saying", "says", "second", "secondly", "see", "seeing",
        "seem", "seemed", "seeming",
        "seems", "seen", "self", "selves", "sensible", "sent", "serious",
        "seriously", "seven",
        "several", "shall", "shan't", "she", "she'd", "she'll", "she's",
        "should", "shouldn't",
        "since", "six", "so", "some", "somebody", "someday", "somehow",
        "someone", "something",
        "sometime", "sometimes", "somewhat", "somewhere", "soon", "sorry",
        "specified", "specify",
        "specifying", "still", "sub", "such", "sup", "sure", "take",
        "taken", "taking", "tell", "tends",
        "th", "than", "thank", "thanks", "thanx", "that", "that'll",
        "thats", "that's", "that've",
        "the", "their", "theirs", "them", "themselves", "then", "thence",
        "there", "thereafter",
        "thereby", "there'd", "therefore", "therein", "there'll",
        "there're", "theres", "there's",
        "thereupon", "there've", "these", "they", "they'd", "they'll",
        "they're", "they've", "thing",
        "things", "think", "third", "thirty", "this", "thorough",
        "thoroughly", "those", "though",
        "three", "through", "throughout", "thru", "thus", "till", "to",
        "together", "too", "took",
        "toward", "towards", "tried", "tries", "truly", "try", "trying",
        "t's", "twice", "two", "un",
        "under", "underneath", "undoing", "unfortunately", "unless",
        "unlike", "unlikely", "until",
        "unto", "up", "upon", "upwards", "us", "use", "used", "useful",
        "uses", "using", "usually",
        "v", "value", "various", "versus", "very", "via", "viz", "vs",
        "want", "wants", "was",
        "wasn't", "way", "we", "we'd", "welcome", "well", "we'll", "went",
        "were", "we're", "weren't",
        "we've", "what", "whatever", "what'll", "what's", "what've",
        "when", "whence", "whenever",
        "where", "whereafter", "whereas", "whereby", "wherein", "where's",
        "whereupon", "wherever",
        "whether", "which", "whichever", "while", "whilst", "whither",
        "who", "who'd", "whoever",
        "whole", "who'll", "whom", "whomever", "who's", "whose", "why",
        "will", "willing", "wish",
        "with", "within", "without", "wonder", "won't", "would",
        "wouldn't", "yes", "yet", "you",
        "you'd", "you'll", "your", "you're", "yours", "yourself",
        "yourselves", "you've", "zero"
};


class Parser {
private:
    /// \description Parser::parse will move futures of AVL trees into this vector
    std::vector<std::future<Article *>> future_queue;
    ThreadPool thread_pool;

    /// \param json_file    -> Path to JSON file within the filesystem
    /// \return AVL tree    -> A small AVL tree :)
    /// \description        -> Reads, parses, extracts, and process data (persons, organizations,
    ///                     text) from raw JSON file and populate a small AVL tree
    Article *parse_json(const std::filesystem::directory_entry &json_file);

public:
    ///
    /// \param root_folder_path            -> Path the kaggle folder (data set folder)
    /// \description                       -> asynchronously call "parse_folder" on EACH folder within
    ///                                     "root_folder_path" and store "Future" variables within the Parser::future_queue
    void parse(const std::filesystem::path &root_folder_path);

    std::set<std::string> k1, k2;
    /// \return set of AVL trees    -> AVL trees returns by each threads
    /// \description                -> "Move all variables from Parser::future_queue into the Parser::articles,
    ///                             where they can be accessed. Optimally, this should only be called once and
    ///                             should be called before accessing Parser::articles
    AvlTree<std::string, Article *> build_AVL_tree();

    HashMap<std::string, std::set<Article*>> orgs_map, person_map;
};


#endif //INC_22S_FINAL_PROJ_PARSER_H

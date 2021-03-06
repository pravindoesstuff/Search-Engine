#include "Parser.h"


Article *Parser::parse_json(const std::filesystem::directory_entry &json_file) {

    //1- Open stream to file
    std::ifstream file(json_file.path());

    //2- Read the file into "json_string"
    std::string json_string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    //3- Close the stream
    file.close();

    //4- Parsing "json_string" into "JSON_document" using rapidjson
    rapidjson::Document JSON_document;
    JSON_document.Parse(json_string.c_str());


    //5- Store "persons name" into Article
    Article *article = new Article();
    auto &entities = JSON_document["entities"]; //an element of the JSON file containing arrays
    auto &persons = entities["persons"]; //an array
    for (const auto &person: persons.GetArray()) {
        //add person_map names to article vector named "persons"
        article->persons.emplace_back(person["name"].GetString());
    }

    //6- Store "organizations name" into Article
    auto &organizations = entities["organizations"]; //an array
    for (const auto &organization: organizations.GetArray()) {
        //add organization names to article vector named "organizations"
        article->organizations.emplace_back(organization["name"].GetString());
    }

    //7- Tokenize, lowercase, and stemming
    article->text = JSON_document["text"].GetString();
    std::istringstream ss(article->text);
    std::string token;
    std::unordered_set<std::string> used_tokens;
    HashMap<std::string, std::string> stem_cache(50);
    while (std::getline(ss, token, ' ')) {
        //if stop-word, ignore.
        if (stop_words.find(token) != stop_words.end()) {
            continue;
        }
        //punctuation removal
        char *token_ptr = token.data();

        // Remove all characters that are not alphabetic from token
        for (const char c: token) {
            if (isalpha(c)) {
                *token_ptr++ = c;
            }
        }

        // If the token_ptr hasn't changed, then no alphabetic characters were found, so we skip this token
        if (token.c_str() == token_ptr) {
            continue;
        }

        // Truncate punctuation that may appear at the end of the character
        token.resize(token_ptr - token.c_str());

        // Convert all alphabetic characters to lowercase
        for (char &c: token) {
            c = (char) tolower(c);
        }

        std::string stemmed = token;
        auto stem = stem_cache.find(token);
        if (stem != nullptr) {
            stemmed = *stem;
        } else {
            stemmed = token;
            Porter2Stemmer::stem(stemmed);
            stem_cache.insert({token, stemmed});
        }

        //SECOND STOP WORD CHECK... because some NON stop words, when stemmed result in stop words
        if (stop_words.find(stemmed) != stop_words.end()) {
            continue;
        }

        //add token to list of tokens in the article.
        if (used_tokens.find(stemmed) != used_tokens.end()) {
            continue;
        } else {
            used_tokens.insert(stemmed);
        }
        article->tokens.emplace_back(stemmed);
    }

    //8- Store ID
    article->id = JSON_document["uuid"].GetString();

    //9- Get Title
    article->title = JSON_document["title"].GetString();

    return article;
}

void Parser::parse(const std::filesystem::path &root_folder_path) {
    auto data_dir = std::filesystem::directory_iterator(root_folder_path);
    for (const auto &element: data_dir) {
        if (element.is_directory()) {
            // Recursively enter the next directory
            parse(element.path());
        } else if (element.path().extension() == ".json") {
            //1- Submit parse_json routine to pool and move it into the future_queue
            future_queue.push_back(thread_pool.enqueue(&Parser::parse_json, this, element));
        }
    }
}



AvlTree<std::string, Article *> Parser::build_AVL_tree() {
    AvlTree<std::string, Article *> article_tree;

    //set the total number of article indexed
    article_tree.set_total_articles(future_queue.size());
    for (std::future<Article *> &future_article: future_queue) {
        Article *article = future_article.get();
        //add article tokens to article_tree total tokens
        article_tree.add_tokens(article->tokens.size());
        k1.insert(article->persons.cbegin(), article->persons.cend());
        k2.insert(article->organizations.cbegin(), article->organizations.cend());
        goto persons;
        std::async([this, article] () {
            for (std::string &person: article->persons) {
                auto person_set = person_map.find(person);
                if (person_set != nullptr) {
                    person_set->insert(article);
                } else {
                    person_map.insert({person, {article}});
                }
            }
        }).wait();
        std::async([this, article] () {
            for (std::string &organization: article->organizations) {
                auto orgs_set = orgs_map.find(organization);
                if (orgs_set != nullptr) {
                    orgs_set->insert(article);
                } else {
                    orgs_map.insert({organization, {article}});
                }
            }
        }).wait(); persons:
        for (std::string &token: article->tokens) {
            article_tree.insert(token, article);
        }
        //t1.wait();
        //t2.wait();
        article->tokens.clear();
    }
    return article_tree;
}

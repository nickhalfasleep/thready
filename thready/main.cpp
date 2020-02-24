
#include <iostream>
#include <vector>
#include <mutex>
#include <thread>
#include <string>
#include <regex>
#include <unordered_set>
#include <random>
class UrlGenerator {
public:
  UrlGenerator() {
    num_rands = 0;
  };
  
  std::vector<std::string> getUrls(std::string url_in) {
    std::vector<std::string> answer;
    if (num_rands < 1200) {
      for (size_t j = 0; j < 5; j++) {
        int rand_val = std::rand();
        answer.push_back("http://gully.org/nick/" + std::to_string(rand_val));
        count_mx.lock();
        num_rands++;
        count_mx.unlock();
      }
      std::chrono::milliseconds timespan(150);
      std::this_thread::sleep_for(timespan);
    }
    return answer;
  }
                     
private:
  std::mutex count_mx;
  size_t num_rands;
};

class Solution {
public:
  std::vector<std::string> crawl(std::string startUrl, UrlGenerator& urlGen) {
    std::unordered_set<std::string> visited;
    std::regex e = std::regex("http[s]?://([a-z0-9.-]{1,63})[/]?.*");
    std::vector<std::string> answer;
    std::smatch sm;
    regex_match(startUrl, sm, e);
    std::string host = sm[1];
    visited.insert(startUrl);
    answer.push_back(startUrl);
    std::thread t(&Solution::searcher, this, startUrl, std::ref(urlGen), std::ref(visited),
             std::ref(host), std::ref(e), std::ref(answer));
    t.join();
    return answer;
  }
  
private:
  std::mutex writer_mtx;
  void searcher(const std::string url, UrlGenerator& htmlParser,
                std::unordered_set<std::string>& visited, const std::string& hostname,
                std::regex& reggie, std::vector<std::string>& answer) {
    std::smatch smx;
    std::vector<std::string> urls = htmlParser.getUrls(url);
    std::vector<std::thread> threads;
    for (auto url : urls) {
      writer_mtx.lock();
      if (visited.find(url) == visited.end() && regex_match(url, smx, reggie) && hostname ==
          smx[1]) {
        visited.insert(url);
        answer.push_back(url);
        threads.push_back(std::thread(&Solution::searcher, this, url, std::ref(htmlParser),
                                 std::ref(visited), std::ref(hostname), std::ref(reggie), std::ref(answer)));
      }
      writer_mtx.unlock();
    }
    for (auto& thread : threads) {
      thread.join();
    }
  }  
};

int main(int argc, const char * argv[]) {
  // insert code here...
  std::cout << "Hello, World!\n";
  
  UrlGenerator parsee;
  Solution bob;
  auto results = bob.crawl("http://gully.org", parsee);
  
  for (auto url : results) {
    std::cout << "\n" << url;
  }
  return 0;
}

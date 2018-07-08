#include <bitset>
#include <cassert>

constexpr size_t kNumPossibleChars = (1lu << (CHAR_BIT * sizeof(char)));

constexpr int kDepInvalid = std::numeric_limits<int>::min();
constexpr int kDepCycle = std::numeric_limits<int>::max();

class Solution 
{
    
public:

    
    string alienOrder(vector<string>& words) 
    {
        // Initialize
        dependency_order.fill( kDepInvalid );
        must_appear_after.fill( std::unordered_set<char>() );
        chars_forgotten.reset();
        
        // Step 0: mark every character seen in chars_forgotten set
        for (size_t i = 0; i < words.size(); ++i)
        {
            for (char c : words[i])
            {
                chars_forgotten[c] = true;
            }
        }
        
        // Step 1: detect all dependencies
        // Also unmark characters in chars_forgotten seen in the dependency set,
        // what's left in chars_forgotten are characters that has no dependency info
        // but still needs to be included in the output.    
        for (size_t i = 1; i < words.size(); ++i)
        {
            add_dependencies(words[i-1], words[i]);
        }        
        
        // Step 2: Traverse forward to assign dependency numbers
        for (size_t i = 0; i < kNumPossibleChars; ++i)
        {
            if (must_appear_after[i].empty())
            {
                continue;
            }
            
            bool cycle = start_assign_dependency_number(i);
            
            if (cycle)
            {
                //std::cout << "Cycle!!\n";
                return std::string();
            }
        }
        
        // Step 3: Sort letters by dependency!
        std::vector<std::set<char>> result;
        for (size_t i = 0; i < dependency_order.size(); ++i)
        {
            int dep = dependency_order[i];

            if (dep != kDepInvalid && dep != kDepCycle)
            {
                assert(dep >= 0);
                if (dep >= static_cast<int>(result.size()))
                {
                    result.resize(dep + 1);
                }
                result[dep].insert(i);
            }            
        }
        
        // Step 4: Build Final Output
        std::string final_result;
        for (auto iter = result.rbegin(); iter != result.rend(); ++iter)
        {
            for (char c: *iter)
            {
                final_result.push_back(c);
            }
        }
        for (size_t i = 0; i < kNumPossibleChars; ++i)
        {
            if (chars_forgotten[i])
            {
                final_result.push_back(i);
            }
        }
        
        return std::move(final_result);
    }
    
private:
    
    bool start_assign_dependency_number(char c)
    {
        std::bitset<kNumPossibleChars> discovered;
        
        recurse_assign_dependency_number(c, discovered);
        
        bool cycle = (dependency_order[c] == kDepCycle);
        return cycle;
    }
    
    void recurse_assign_dependency_number(char c, std::bitset<kNumPossibleChars> & discovered)
    {   
        // Cycle or reconvergent path!
        //std::cout << "Into " << c << std::endl;
        
        if (discovered[c])
        {
            //std::cout << "Exit " << c << " Discovered" << std::endl;
            
            // Discovering a node that's undone, meaning we're in a cycle!
            if (dependency_order[c] == kDepInvalid)
            {
                dependency_order[c] = kDepCycle;
            }
            return;
        }
        
        discovered[c] = true;
        
        if (must_appear_after[c].empty())
        {
            // It's a leaf!
            dependency_order[c] = 0;
            //std::cout << "Exit " << c << " Leaf" << std::endl;
            return;
        }
        
        // Deduce solution from children and also propagate cycle upstrea
        int dep = kDepInvalid;
        for (char child : must_appear_after[c])
        {
            recurse_assign_dependency_number(child, discovered);
            
            assert(dependency_order[child] != kDepInvalid);

            if (dependency_order[child] != kDepCycle)
            {
                dep = std::max(dep, dependency_order[child] + 1);
            }
            else
            {
                // Child has cycle below. Propagate it upwards.
                dep = kDepCycle;
            }
        }
        // Commit only after children are done
        assert(dep != kDepInvalid);
        dependency_order[c] = dep;
        
        //std::cout << "Exit " << c << " Complete " << dependency_order[c] << std::endl;
    }
    
    void add_dependencies(const string & a, const string & b)
    {
        const size_t num_to_compare = std::min(a.size(), b.size());
        
        for (size_t i = 0; i < num_to_compare; ++i)
        {
            if (a[i] != b[i])
            {
                // First different character!
                chars_forgotten[a[i]] = false;
                chars_forgotten[b[i]] = false;
  
                //std::cout << a[i] << ' ' << b[i] << std::endl;
                must_appear_after[a[i]].insert(b[i]);
                
                break;
            }
        }
    }
    
    std::array<std::unordered_set<char>, kNumPossibleChars> must_appear_after;
    std::bitset<kNumPossibleChars> chars_forgotten;
    std::array<int, kNumPossibleChars> dependency_order;
};

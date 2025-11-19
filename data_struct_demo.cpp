
#include <bits/stdc++.h>
#include <shared_mutex>
#include <thread>
#include <chrono>

using namespace std;
using Clock = chrono::high_resolution_clock;

struct DataStruct {
    vector<int> fields; 
    vector<shared_mutex> locks; 
    DataStruct(size_t m = 2) : fields(m, 0), locks(m) {}

    int get(size_t idx) {
        shared_lock<shared_mutex> lk(locks[idx]); 
        return fields[idx];
    }

    void set(size_t idx, int val) {
        unique_lock<shared_mutex> lk(locks[idx]);
        fields[idx] = val;
    }

    string to_string_all() {
    
        shared_lock<shared_mutex> lk0(locks[0]);
        shared_lock<shared_mutex> lk1(locks[1]);
 
        ostringstream oss;
        oss << "Field0=" << fields[0] << "; Field1=" << fields[1];
        return oss.str();
    }
};


struct Cmd {
    enum Type { READ, WRITE, STRING } type;
    int idx;
    int val;
    Cmd(Type t=STRING, int i=0, int v=0) : type(t), idx(i), val(v) {}
};

vector<Cmd> parse_file_to_cmds(const string &filename) {
    ifstream in(filename);
    if (!in) {
        cerr << "Cannot open file: " << filename << "\n";
        return {};
    }
    vector<Cmd> res;
    string op;
    while (in >> op) {
        if (op == "read") {
            int idx; in >> idx;
            res.emplace_back(Cmd::READ, idx, 0);
        } else if (op == "write") {
            int idx, val; in >> idx >> val;
            res.emplace_back(Cmd::WRITE, idx, val);
        } else if (op == "string") {
            res.emplace_back(Cmd::STRING, 0, 0);
        } else {
         
            string rest; getline(in, rest);
        }
    }
    return res;
}


void execute_commands(DataStruct &ds, const vector<Cmd> &cmds) {
    for (const auto &c : cmds) {
        if (c.type == Cmd::READ) {
            volatile int v = ds.get(c.idx); (void)v;
        } else if (c.type == Cmd::WRITE) {
            ds.set(c.idx, c.val);
        } else { 
            string s = ds.to_string_all();
            (void)s;
        }
    }
}

int main(int argc, char** argv) {
    if (argc < 3) {
        cout << "Usage:\n  " << argv[0] << " mode threads file1 [file2 file3]\n";
        cout << "mode: single | multi\n";
        cout << "threads: number of threads to spawn (1,2,3). For single mode, use 1\n";
        return 0;
    }

    string mode = argv[1];
    int threads = stoi(argv[2]);
    vector<string> files;
    for (int i = 0; i < threads; ++i) {
        if (3 + i < argc) files.push_back(argv[3 + i]);
        else files.push_back(""); 
    }

    
    vector<vector<Cmd>> all_cmds;
    for (int i = 0; i < threads; ++i) {
        if (files[i].empty()) {
            cerr << "Missing filename for thread " << i << "\n";
            return 1;
        }
        auto cmds = parse_file_to_cmds(files[i]);
        all_cmds.push_back(move(cmds));
        cout << "Thread " << i << " commands: " << all_cmds.back().size() << "\n";
    }

    DataStruct ds(2);

    vector<thread> ths;
    auto t0 = Clock::now();
    for (int i = 0; i < threads; ++i) {
        ths.emplace_back([&ds, &all_cmds, i]() {
            execute_commands(ds, all_cmds[i]);
        });
    }
    for (auto &t : ths) t.join();
    auto t1 = Clock::now();

    auto duration = chrono::duration_cast<chrono::microseconds>(t1 - t0).count();
    cout << "Execution time (microseconds): " << duration << "\n";
    return 0;
}
